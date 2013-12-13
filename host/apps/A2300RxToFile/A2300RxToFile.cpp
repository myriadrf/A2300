/* Name: A2300RxToFile.cpp
*
* Copyright(c) 2013 Loctronix Corporation
* http://www.loctronix.com
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN 
	#include<windows.h>
	#define SLEEP_SEC(a)  Sleep((a)*1000)
#else
	#include <unistd.h>
	#define SLEEP_SEC(a) sleep((a))
#endif 

#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdexcept>
#include <string>
#include <vector>

#include <A2300/ConfigDevice.h>

using namespace A2300;

/******************************************************************
 * Static Data
 *****************************************************************/

// const values

const static size_t cs_BytesPerSample = 4;
const static size_t cs_ctFrames = 16;
const static size_t cs_sizeFrame = 8192;

// default to: RF0, PCS Band, 9 dB Gain, 1950 MHz, 2.5 MHz bandwidth,
// 2 MHz sampling, infinite # samples (0)

static byte s_idComponent = WCACOMP_RF0;
static byte s_rxProfile = RX0DPE_PcsExt;
static byte s_rxGain = 3;
static uint32 s_rxFreq = 1950000;
static byte s_rxBandwidth = 13;
static uint16 s_hostSampRate = 8;
static size_t s_numSamples = 0;
static size_t s_numBytesToCollect = 0;
static size_t s_numBytesCollected = 0;

static char* s_fileName = NULL;
static FILE* s_fileStream = NULL;

static ConfigDevice s_config;
static ulong s_ctPackets = 0;
static ulong s_ctData = 0;
static bool s_bRunning = false;

/******************************************************************
 * Forward Declarations.
 *****************************************************************/

static int Run ();

// Program configuration routines
static void WriteHeader ();
static void PrintUsage ();
static int ParseOptions (int argc, const char** argv);
static void DumpDeviceInformation ();

template < typename T >
static bool GetArgumentNumber(int argc, const char** argv, const char* propertyName, int& currentArgNumber, T& outVar);

// Support functions
static int DoRxToFile ();
static void OnFrameReady (void* arg, BulkDataPort::TransferContext* pctxt);

/******************************************************************
 * Functions.
 *****************************************************************/

/**
 * <summary>
 * Main Program Entry Point.
 * </summary>
 */

int main(int argc, const char** argv) {
	int retval = 0;
	WriteHeader();
	retval = ParseOptions(argc, argv);
	if( retval == 0)
	{
		// Open the specified file for writing.
		s_fileStream = fopen(s_fileName, "wb");
		if (!s_fileStream) {
			printf("\nError: Provided filename ('%s') cannot be "
					"accessed.\n", s_fileName);
			PrintUsage();
			return -3;
		}

		// Now run the read operation.
		retval = Run();

		//close the file -- note this was opened in ParseOptions,
		if (fclose(s_fileStream) != 0) {
			printf("\nError %d closing file '%s'.\n", errno, s_fileName);
		}
	}
	return retval;
}

/**
 * <summary>
 * Primary entry point for running this executable.
 * </summary>
 */

static int Run() {

	int retval = 0;

	try {
		// Find the list of addresses at the specified VID/PID.
		printf("\n"
				"Enumerating ASR-2300 devices...\n"
				"--------------------------------------\n");

		int addr = s_config.Attach();

		printf("Attached to ASR-2300 at address = %d\n", addr);

	} catch (std::runtime_error& re) {
		printf("Error:  %s\n", re.what());
		return -8;
	}

	// Dump the device information
	DumpDeviceInformation();

	// Execute the bulk data transfer program
	retval = DoRxToFile ();

	s_config.Detach();
	return retval;
}

/**
 * Function configures the ASR-2300 to receive a specified frequency
 * and then streams the data to the host via BulkDataPort interface.
 */

static int DoRxToFile ()
{
	const static char* s_rx0_pathsDefault[] = {"Disabled", "GPS L1 Int.", "GPS L1 Ext.", "PCS Band Ext.", "Wideband"};
	const static char* s_rx1_pathsDefault[] = {"Disabled", "UHF Ext.", "ISM Int.", "ISM Ext.", "Wideband"};

	//Select endpoints, components, and path definitions based on which RF front-end was specified.
	bool  bIsRf0   = (s_idComponent == WCACOMP_RF0);
	byte  epRx     = (bIsRf0) ? 0x88 : 0x98;
	byte  idDdc    = (bIsRf0) ? WCACOMP_DSP_DDC0 : WCACOMP_DSP_DDC1;
	pcstr* szPaths = (bIsRf0) ? s_rx0_pathsDefault : s_rx1_pathsDefault;
	float fSampFreq = 32.0e6f / (s_hostSampRate / 2);

	double dt = ((double) s_numSamples) / ((double) fSampFreq);
	double framesPerSecond = fSampFreq*cs_BytesPerSample / cs_sizeFrame;
	ulong totalFrames = (ulong)ceil(framesPerSecond*dt);

	TransportDci& td = s_config.Dci0Transport();  //Get the dci interface so we can send messages directly.

	//Display some interesting information about the configuration.
	printf("\n");
	printf("Opening Data Port...\n");
	printf("--------------------------------------\n");
	printf("RF Component:  %s (id=%02Xh, ep=%02Xh)\n", (bIsRf0)? "RF0" : "RF1", s_idComponent, epRx);
	printf("RX Profile:    %s (id=%02Xh)\n", szPaths[s_rxProfile & 0xF], s_rxProfile);
	printf("RX Gain:       %d dB\n", s_rxGain*3);
	printf("RX Frequency:  %0.3f MHz\n", s_rxFreq/1.0e6f);
	printf("RX Bandwidth:  ?? MHz (id=%02Xh)\n", s_rxBandwidth);
	printf("Host Rate:     %0.3f MHz\n", fSampFreq/1.0e6f);
	if (s_numSamples == 0) {
	  printf("Duration:      infinite samples\n");
	} else {
	  printf("Duration:      %ld samples, %0.02lf sec, %lu frames\n", s_numSamples, dt, totalFrames);
	}

	// 1) Bind the Bulk Data Port to the specified endpoint.
	BulkDataPort portData(epRx, PortBase::EP_UNDEF);
	UsbDevice& device  = s_config.Device();
	if( device.BindPort( &portData) != 0)
	{
		printf("Error Binding to Wca Ports.");
		return -5;
	}

	//2) Reset the DDC Component so no data is in cache.
	td.SetProperty<byte>(idDdc, DSP_DDUC_CTRL, DSP_DDUC_CTRL_RESET);
	td.SetProperty<byte>(idDdc, DSP_DDUC_CTRL, DSP_DDUC_CTRL_DISABLED);

	//3) Configure the RF Front-end profile, gain, frequency, and Bandwidth.
    //   NOTE: this will get moved to the ConfigRf object soon.  This is the
	//         brute force way of doing it.
	td.SetProperty<byte>(s_idComponent,   RFPROP_RXPATH, s_rxProfile );
	td.SetProperty<byte>(s_idComponent,   RFPROP_RXGAIN, s_rxGain);
	td.SetProperty<uint32>(s_idComponent, RFPROP_RXFREQ, s_rxFreq);
	td.SetProperty<uint32>(s_idComponent, RFPROP_RXBANDWIDTH, s_rxBandwidth);

	//4) Open the port, attach callback function to receive data as it comes in.
	portData.Open();
	portData.ReadTransfer() = BulkDataPort::TransferEvent
	  (NULL, &OnFrameReady);

	//5) Initialize the buffers and queue for processing.
	s_bRunning = true;
	BulkDataPort::TransferContextList listContext;

	byte frames[cs_ctFrames*cs_sizeFrame];

	for( size_t nn = 0; nn < cs_ctFrames; ++nn)
	{
		BulkDataPort::TransferContext* pctxt = portData.CreateReadTransferContext(frames + (nn*cs_sizeFrame), cs_sizeFrame);
		pctxt->Submit();
		listContext.push_back( pctxt);
	}

	//6) Configure the DDC and Enable bulk data transfer on the device.
	//   NOTE:  DDC functionality will move to the ConfigDDUC class at some point.
	td.SetProperty<uint16>(idDdc, DSP_DDUC_SAMPRATE, s_hostSampRate);
	//TODO CONFIGURE IF Frequency.
	td.SetProperty<byte>(idDdc, DSP_DDUC_CTRL, DSP_DDUC_CTRL_ENABLED);

	//7) Process until finished

	printf("\n--> Starting Run\n");

	s_numBytesToCollect = s_numSamples * cs_BytesPerSample;
	s_numBytesCollected = 0;
	int retval = 0;
	while ((retval == 0) &&
	       (s_numBytesCollected < s_numBytesToCollect))
	{
#if defined(HAVE_LIBUSB)
		retval= device.PollAsynchronousEvents();
#elif defined(WIN32)
		//Windows implementation does not have a PollAsynchronous Data method.
		//Instead, we wait for events on each port.
		BulkDataPort::TransferContext& ctxt = portData.WaitForReadTransferEvent( 1000);
		retval = ctxt.status;
#endif
	}

	// check for USB error
	if( retval != 0) printf("USB error occurred transfer stopped.\n");

	//8) Reset the DDC Port
	td.SetProperty<byte>(idDdc, DSP_DDUC_CTRL, DSP_DDUC_CTRL_RESET);
	td.SetProperty<byte>(idDdc, DSP_DDUC_CTRL, DSP_DDUC_CTRL_DISABLED);

	//9) Print results
	printf("\n--> Completed Run: Packets = %ld, Bytes = %ld\n", s_ctPackets, s_ctData);

	//10) Disable the RF Communications
	td.SetProperty<byte>(s_idComponent,  RFPROP_RXPATH, bIsRf0 ? (byte)RX0DPE_Disabled : (byte)RX1DPE_Disabled );

	//11) Cancel all transfer operations.
	BulkDataPort::TransferContextList::iterator iter;
	for( iter = listContext.begin(); iter != listContext.end(); iter++)
		(*iter)->Cancel();

	//Wait for the transfer cancellations to complete.
	time_t ttCur, ttEnd;
	time(&ttCur);
	ttEnd = ttCur + 2;

#ifdef HAVE_LIBUSB
	//We must provide events in LIBUSB.
	int completed = 0;
	while( !completed && (ttCur < ttEnd))
	{
		device.PollAsynchronousEvents(0.010, completed);
		time(&ttCur);
	}
#endif

	//12) Destroy all the transfer contexts.
	for( iter = listContext.begin(); iter != listContext.end(); iter++)
		(*iter)->Destroy(); // All done.


	//11) Close the Data Port
	portData.Close();

	return 0;
}

/**
 * <summary>
 * Callback issued when data is ready.
 * </summary>
 */

static void OnFrameReady (void* /* arg */, BulkDataPort::TransferContext* pctxt)
{
	// if the status is OK (0)
	if (pctxt->status == 0)
	{
	  ++s_ctPackets;
	  if (s_numSamples == 0) {
	    // Save data to disk
	    size_t nWritten = fwrite (pctxt->bufFrame, 1, pctxt->nActualLength, s_fileStream);
	    if (nWritten != pctxt->nActualLength) {
	      printf ("\nWarning: Some data failed to write to the selected file ('%s').\n", s_fileName);
	      printf ("  Ignoring and hoping for the best.\n");
	    }

	    // Print progress '.'
	    if (s_numSamples == 0) {
	      if ((s_ctPackets % 488) == 0)
		{
		  putc('.', stdout);
		  fflush(stdout);
		}
	    }
	  } else {

	    size_t nToWrite = s_numBytesToCollect - s_numBytesCollected;
	    if (nToWrite > pctxt->nActualLength) {
	      nToWrite = pctxt->nActualLength;
	    }

	    // Save data to disk
	    size_t nWritten = fwrite (pctxt->bufFrame, 1, nToWrite, s_fileStream);
	    if (nWritten != nToWrite) {
	      printf ("\nWarning: Some data failed to write to the selected file ('%s').\n", s_fileName);
	      printf ("  Ignoring and hoping for the best.\n");
	    }

	    size_t oldNumBytesCollected = s_numBytesCollected;
	    s_numBytesCollected += nWritten;
	    s_ctData += pctxt->nActualLength;

	    // Resubmit
	    pctxt->Submit();

	    // 60 dots is complete; figure out how far along and if
	    // another dot needs to be printed.

	    float dotByteLen = ((float) s_numBytesToCollect) / ((float) 60);
	    if ((size_t)(((float) oldNumBytesCollected) / dotByteLen) !=
		(size_t)(((float) s_numBytesCollected) / dotByteLen))
	      {
		putc('.', stdout);
		fflush(stdout);
	      }
	  }
	}
}

/**
 * <summary>
 * Writes the application header information to the standard output.
 * </summary>
 */

static void WriteHeader() {
	printf(
		"*********************************************************************\n"
		"* ASR-2300 RxToFile\n"
		"*********************************************************************\n"
		"* This software example provided by Loctronix Corporation (2013) \n"
		"* www.loctronix.com\n"
		"*********************************************************************\n");
}

/**
 * <summary>
 * Prints usage of this program, on command line parsing error.
 * </summary>
 */
static void PrintUsage() {
	printf( "\nUsage for A2300RxToFile:\n\n"
		"  A2300RxToFile [options] output_filename\n\n"
		"Options [default]:\n"
		"    -h        This usage message.\n"
		"    -c #      Rx component number, 0 or 1 [0].\n"
		"    -A name   antenna name:\n"
		"      Rx0 : GpsL1Int, GpsL1Ext, PcsExt, Wideband [PcsExt]\n"
		"      Rx1 : UhfExt, IsmInt, IsmExt, Wideband [Wideband]\n"
		"    -g #      gain in dB [9]\n"
		"    -f #      center frequency [1950 MHz]\n"
		"    -b #      bandwidth [2.5 MHz]\n"
		"    -s #      sample rate [2 MS/s]\n"
		"    -N #      number of samples to collect [inf]\n");
}

/**
 * <summary>
 * Helper function makes argument number conversion more generic.
 * </summary>
 */

// number of bits in a 'char'
#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

template < typename T >
void GetMinMax (T& /* arg */, double& MIN_VAL, double& MAX_VAL, bool isSigned)
{
  unsigned char numBits = sizeof(T)*CHAR_BIT;
  if (isSigned) {
    --numBits;
    MIN_VAL = (double) (((unsigned long long) 1) << numBits);
    MAX_VAL = MIN_VAL - ((double) 1);
    MIN_VAL *= ((double)(-1));
  } else {
    MIN_VAL = (double) 0;
    if (numBits == (sizeof(unsigned long long)*CHAR_BIT)) {
      MAX_VAL = (double)((unsigned long long) -1);
    } else {
      MAX_VAL = (double) ((((unsigned long long) 1) << numBits) -
			  ((unsigned long long) 1));
    }
  }
}

void GetMinMaxName (char arg, double& MIN_VAL, double& MAX_VAL, std::string& nameString)
{
  GetMinMax (arg, MIN_VAL, MAX_VAL, true);
  nameString = "char";
}

void GetMinMaxName (unsigned char arg, double& MIN_VAL, double& MAX_VAL, std::string& nameString)
{
  GetMinMax (arg, MIN_VAL, MAX_VAL, false);
  nameString = "unsigned char";
}

void GetMinMaxName (short arg, double& MIN_VAL, double& MAX_VAL, std::string& nameString)
{
  GetMinMax (arg, MIN_VAL, MAX_VAL, true);
  nameString = "short";
}

void GetMinMaxName (unsigned short arg, double& MIN_VAL, double& MAX_VAL, std::string& nameString)
{
  GetMinMax (arg, MIN_VAL, MAX_VAL, false);
  nameString = "unsigned short";
}

void GetMinMaxName (int arg, double& MIN_VAL, double& MAX_VAL, std::string& nameString)
{
  GetMinMax (arg, MIN_VAL, MAX_VAL, true);
  nameString = "int";
}

void GetMinMaxName (unsigned int arg, double& MIN_VAL, double& MAX_VAL, std::string& nameString)
{
  GetMinMax (arg, MIN_VAL, MAX_VAL, false);
  nameString = "unsigned int";
}

void GetMinMaxName (long arg, double& MIN_VAL, double& MAX_VAL, std::string& nameString)
{
  GetMinMax (arg, MIN_VAL, MAX_VAL, true);
  nameString = "int";
}

void GetMinMaxName (unsigned long arg, double& MIN_VAL, double& MAX_VAL, std::string& nameString)
{
  GetMinMax (arg, MIN_VAL, MAX_VAL, false);
  nameString = "unsigned long";
}

template < typename T >
static bool GetArgumentAsDouble(int argc, const char** argv, const char* propertyName, int& currentArgNumber, double& outValAsDouble)
{
  // convert to double

  // retrieve the argument string, checking to make sure there are enough

  bool rv = (++currentArgNumber < argc);
  if (rv) {
    // convert to a double, allowing for "0x" starting and "e+" ending
    char* endPtr = 0;
    outValAsDouble = strtod (argv[currentArgNumber], &endPtr);
    if (endPtr == argv[currentArgNumber]) {
      // failed to convert
      printf ("Error: Unable to convert the string for argument %s ('%s') into a number.\n", propertyName, argv[currentArgNumber]);
      PrintUsage();
      rv = false;
    }
  } else {
    printf ("Error: Too few arguments to determine %s.\n", propertyName);
    PrintUsage();
  }

  // check for MIN, MAX, and factional value

  if (rv) {

    // retrieve MIN, MAX, name string

    T arg = (T) 0;
    double MIN_VAL, MAX_VAL;
    std::string typeName;
    GetMinMaxName (arg, MIN_VAL, MAX_VAL, typeName);

    // convert to the number type, taking into account limits
    if (outValAsDouble > MAX_VAL) {
      printf ("Warning: Argument %s (%g) is larger than the maximum for converting to type '%s' (%g); truncating to max.\n", propertyName, outValAsDouble, typeName.c_str(), MAX_VAL);
      outValAsDouble = MAX_VAL;
    } else if (outValAsDouble < MIN_VAL) {
      printf ("Warning: Argument %s (%g) is more negative than the minimum for converting to type '%s' (%g); truncating to min.\n", propertyName, outValAsDouble, typeName.c_str(), MIN_VAL);
      outValAsDouble = MIN_VAL;
    } else if (((double)((T)outValAsDouble)) != outValAsDouble) {
      printf ("Warning: Argument %s (%g) contains fractional value; truncating to integer.\n", propertyName, outValAsDouble);
      outValAsDouble = (double)((T) outValAsDouble);
    }
  }

  return rv;
}

template < typename T >
static bool GetArgumentNumber(int argc, const char** argv, const char* propertyName, int& currentArgNumber, T& outVar)
{
  double outValAsDouble = (double) 0;
  bool rv = GetArgumentAsDouble<T>(argc, argv, propertyName, currentArgNumber, outValAsDouble);
  if (rv) {
    outVar = (T) outValAsDouble;
  }
  return rv;
}

/**
 * <summary>
 * Helper function makes argument matching more readable.
 * </summary>
 */

static bool IsArgumentName( pcstr arg, pcstr szName, size_t minChars)
{
	size_t lenArg = strlen(arg);
	size_t lenName = strlen(szName);
	return lenArg >= minChars && lenArg <= lenName
			&& (strncmp( arg, szName, lenArg) == 0);
}

/**
 * <summary>
 * Parses the privided command line string.
 * </summary>
 */

static int ParseOptions(int argc, const char** argv) {
	if (argc < 2) {
		printf("\nError: Too few arguments: Got %d, expecting at least 2.\n", argc);
		PrintUsage();
		return -1;
	}

	int t_arg = 0;
	char* antenna_flag_entry = NULL;

	while (++t_arg < argc) {

	  // get this argument string
	  const char* t_argv = argv[t_arg];

	  // see if this argument starts with '-'
	  if (t_argv[0] != '-') {
	    // no; is it the last one?
	    if (++t_arg == argc) {
	      // yes; set filename
	      s_fileName = const_cast<char*>(t_argv);
	      break;
	    } else {
	      // no; print an error
	      printf ("Error: unknown flag '%s'.\n", t_argv);
	      PrintUsage();
	      return -1;
	    }
	  }

	  // make sure the string length is 2
	  if (strlen (t_argv) != 2) {
	    // no: print error and exit
	    printf ("Error: unknown flag '%s'.\n", t_argv);
	    PrintUsage();
	    return -1;
	  }

	  // swtich on the flag letter
	  switch (t_argv[1]) {
	  case 'A':
	    // antenna name; next is string; depends on which Rx# is
	    // selected, if any; process this after all other flags
	    if (++t_arg < argc) {
	      antenna_flag_entry = const_cast<char*>(argv[t_arg]);
	    } else {
	      printf ("Error: No antenna name specified with -A flag.\n");
	      PrintUsage();
	      return -1;
	    }
	    break;

	  case 'b':
	    {
	      // bandwidth; next is #
	      byte rxBandwidth = 0;
	      if (!GetArgumentNumber(argc, argv, "bandwidth", t_arg, rxBandwidth)) {
		return -1;
	      }
	      if (rxBandwidth == 0) {
		printf ("Error: Specified bandwidth (%d) is 0; must be positive.\n", rxBandwidth);
		PrintUsage();
		return -1;
	      }
	      s_rxBandwidth = rxBandwidth;
	      printf ("selected bandwidth: %d\n", s_rxBandwidth);
	    }
	    break;

	  case 'c':
	    {
	      // Rx component #, as int 0 or 1
	      byte idComponent = 0xff;
	      if (!GetArgumentNumber(argc, argv, "Rx component #", t_arg, idComponent)) {
		return -1;
	      }
	      if (idComponent == 0) {
		s_idComponent = WCACOMP_RF0;
	      } else if (idComponent == 1) {
		s_idComponent = WCACOMP_RF1;
	      } else {
		printf ("Error: Unknown Rx component #%d; expecting 0 or 1.\n", idComponent);
		PrintUsage();
		return -1;
	      }
	      printf ("selected id component: %d\n", idComponent);
	    }
	    break;

	  case 'f':
	    {
	      // center frequency; next is # as int
	      uint32 rxFreq = 0;
	      if (!GetArgumentNumber(argc, argv, "center frequency", t_arg, rxFreq)) {
		return -1;
	      }
	      if (rxFreq <= 0) {
		printf ("Error: Specified center frequency (%d) is <= 0; must be positive.\n", rxFreq);
		PrintUsage();
		return -1;
	      }
	      s_rxFreq = rxFreq;
	      printf ("selected center frequency: %d\n", rxFreq);
	    }
	    break;

	  case 'g':
	    {
	      // gain in dB; next is # as int
	      byte rxGain = 0;
	      if (!GetArgumentNumber(argc, argv, "gain in dB", t_arg, rxGain)) {
		return -1;
	      }
	      if (rxGain <= 0) {
		printf ("Error: Specified gain in dB (%d) is out of range; expecting an integer in [,].\n", rxGain);
		PrintUsage();
		return -1;
	      }
	      s_rxGain = rxGain;
	      printf ("selected gain: %d\n", rxGain);
	    }
	    break;

	  case 'h':
	    {
	      PrintUsage();
	      return -1;
	    }
	    break;

	  case 'N':
	    {
	      // number of samples to collect; next is # as int (non-negative; 0 means infinite)
	      size_t numSamples = 0;
	      if (!GetArgumentNumber(argc, argv, "number of samples to collect", t_arg, numSamples)) {
		return -1;
	      }
	      s_numSamples = numSamples;
	      printf ("selected number of samples: %ld\n", numSamples);
	    }
	    break;

	  case 's':
	    {
	      // sample rate; next is # as int (positive)
	      uint16 hostSampRate = 0;
	      if (!GetArgumentNumber(argc, argv, "sample rate", t_arg, hostSampRate)) {
		return -1;
	      }
	      if (hostSampRate == 0) {
		printf ("Error: Specified sample rate (%d) is 0, must be positive.\n", hostSampRate);
		PrintUsage();
		return -1;
	      }
	      s_hostSampRate = hostSampRate;
	      printf ("selected sample rate: %d\n", hostSampRate);
	    }
	    break;

	  default:
	    printf ("Unknown flag '%c'.\n", t_argv[1]);
	    PrintUsage();
	    return -1;
	  }
	}

	// was a filename specified?
	if (!s_fileName) {
	  printf ("Error: No output filename specified.\n");
	  PrintUsage();
	  return -1;
	}

	// see if the antenna flag entry was set
	if (antenna_flag_entry) {
	  // yes: process depending on which Rx #
	  if (s_idComponent == WCACOMP_RF0) {
	    // Rx0 names: GpsL1Int, GpsL1Ext, PcsExt, Wideband [PcsExt]
	    if (       IsArgumentName(antenna_flag_entry, "GpsL1Int", 6)) {
	      s_rxProfile = RX0DPE_GpsL1Int;
	    } else if (IsArgumentName(antenna_flag_entry, "GpsL1Ext", 6)) {
	      s_rxProfile = RX0DPE_GpsL1Ext;
	    } else if (IsArgumentName(antenna_flag_entry, "PcsExt", 1)) {
	      s_rxProfile = RX0DPE_PcsExt;
	    } else if (IsArgumentName(antenna_flag_entry, "Wideband", 1)) {
	      s_rxProfile = RX0DPE_Wideband;
	    } else {
	      printf ("Error: Unknown antenna name '%s' on Rx0.\n",
		      antenna_flag_entry);
	      PrintUsage();
	      return -1;
	    }
	    printf ("selected Rx0 antenna: %s\n", antenna_flag_entry);
	  } else {
	    // Rx1 names: UhfExt, IsmInt, IsmExt, Wideband [Wideband]
	    if (       IsArgumentName(antenna_flag_entry, "UhfExt", 1)) {
	      s_rxProfile = RX1DPE_UhfExt;
	    } else if (IsArgumentName(antenna_flag_entry, "IsmInt", 4)) {
	      s_rxProfile = RX1DPE_IsmInt;
	    } else if (IsArgumentName(antenna_flag_entry, "IsmExt", 4)) {
	      s_rxProfile = RX1DPE_IsmExt;
	    } else if (IsArgumentName(antenna_flag_entry, "Wideband", 1)) {
	      s_rxProfile = RX1DPE_Wideband;
	    } else {
	      printf ("Error: Unknown antenna name '%s' on Rx1.\n",
		      antenna_flag_entry);
	      PrintUsage();
	      return -1;
	    }
	    printf ("selected Rx1 antenna: %s\n", antenna_flag_entry);
	  }
	}

	printf ("selected filename '%s'\n", s_fileName);

	return 0;
}

/**
 * <summary>
 * Print out Device information.
 * </summary>
 */

static void DumpDeviceInformation ()
{
	std::string sId = s_config.IdentifyDevice();
	std::string sVer = s_config.FirmwareVersion(0);
	uint16 idFpga = s_config.FpgaId();
	uint16 verFpga = s_config.FpgaVersion();
	int iVer = (verFpga >> 8);
	int iRev = (verFpga & 0x00ff);

	printf("\n");
	printf("Identity:    %s\n", sId.c_str());
	printf("FW Ver:      %s\n", sVer.c_str());
	printf("FPGA ID-Ver: %04X-%02X.%02X\n\n", idFpga, iVer, iRev);
}
