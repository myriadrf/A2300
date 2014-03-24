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

#if defined(LINUX) || defined(APPLE)
#include <pthread.h>
#include <signal.h>
#include <termios.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#include <stdexcept>
#include <string>
#include <vector>

#include <A2300/ConfigDevice.h>
#include <A2300/ConfigDduc.h>
#include <System/ArgParser.hpp>

using namespace A2300;

/******************************************************************
* Declarations
*****************************************************************/
struct RxConfig {
	const char* szPath;
	const char* szDescr;
	byte		epRx;
	byte		idRf;
	byte		idDdc;
	byte        idPath;
}; 

struct UserParams {
	std::string sfile;
	int idxConfig;
	byte gain;
	double freq;
	double bandwidth;
	double samprate;
	size_t numsamples;
};

/******************************************************************
* Static Data
*****************************************************************/

ARGPARSER_BEGIN_MAP( s_argmap ) 
ARGPARSER_PARAM( "filename", "", "Output file name.")
ARGPARSER_VAR( "path", ArgParser::Entry::DOUBLE, "pcs",
			  "Receiver path to use. Choose from \"gpsant\", \"gpsext\", \"pcs\","
			  "\"wideband0\", \"uhf\", \"ism_ant\", \"ism_ext\", \"wideband1\"")
			  ARGPARSER_VAR( "freq", ArgParser::Entry::DOUBLE, "1970", "Receiver center frequency in MHz. Valid range is between 300 MHz to 3.8 MHz")
			  ARGPARSER_VAR( "gain", ArgParser::Entry::LONG,   "9",    "Receiver gain in dB. Valid range is between 0 dB to 60 dB")
			  ARGPARSER_VAR( "bw",   ArgParser::Entry::DOUBLE, "5",    "Receiver front-end bandwidth in MHz. Valid range is 1.5 MHz to 28 MHz.")
			  ARGPARSER_VAR( "rate", ArgParser::Entry::DOUBLE, "2",    "Host sample rate in MS/s. Valid range is 3.90625 kS/s to 32 MS/s.")
			  ARGPARSER_VAR( "num",  ArgParser::Entry::LONG,   "0",    "Number of samples to collect.  Default (0) is infinite.")
			  ARGPARSER_END_MAP

			  static const RxConfig s_config[] = {
				  { "gpsant",		"GPS L1 Internal Antenna",	0x88,  WCACOMP_RF0, WCACOMP_DSP_DDC0, RX0DPE_GpsL1Int },
				  { "gpsext",		"GPS L1 External Input",	0x88,  WCACOMP_RF0, WCACOMP_DSP_DDC0, RX0DPE_GpsL1Ext },
				  { "pcs",		"PCS Input",				0x88,  WCACOMP_RF0, WCACOMP_DSP_DDC0, RX0DPE_PcsExt },
				  { "wideband0",  "RF0 Wideband Input",		0x88,  WCACOMP_RF0, WCACOMP_DSP_DDC0, RX0DPE_Wideband },
				  { "uhf",		"UHF External Input",		0x89,  WCACOMP_RF1, WCACOMP_DSP_DDC1, RX1DPE_UhfExt },
				  { "ism_ant",	"ISM 2.4 GHz Antenna",		0x89,  WCACOMP_RF1, WCACOMP_DSP_DDC1, RX1DPE_IsmInt },
				  { "ism_ext",	"ISM 2.4 GHz Ext. Input",	0x89,  WCACOMP_RF1, WCACOMP_DSP_DDC1, RX1DPE_IsmExt },
				  { "wideband1",	"RF1 Wideband Input",		0x89,  WCACOMP_RF1, WCACOMP_DSP_DDC1, RX1DPE_Wideband }
};
const static int COUNT_CONFIG = 8;
static UserParams s_params = { "", 2, 9, 1970.0, 5.0, 2.0, 0};
static ArgParser s_args;

const static size_t cs_BytesPerSample = 4;
const static size_t cs_ctFrames = 16;
const static size_t cs_sizeFrame = 8192;

static size_t s_numBytesToCollect = 0;
static size_t s_numBytesCollected = 0;

static FILE*		s_fileStream = NULL;
static ConfigDevice s_cfgDevice;
static ulong		s_ctPackets = 0;
static ulong		s_ctData = 0;
static bool			s_bRunning = false;
static bool			s_bKeyHit = false;

/******************************************************************
* Forward Declarations.
*****************************************************************/

static int Run ();
static int ValidateArgs();
static void WriteHeader ();
static void PrintUsage ();
static void DumpDeviceInformation ();

// Support functions
static int  DoRxToFile ();
static void OnFrameReady (void* arg, BulkDataPort::TransferContext* pctxt);
static void* KeyThreadFunc (void* arg);
static bool setCbreak (struct termios& saveTermios);
static bool unsetCbreak (struct termios& savedTermios);

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

	s_args.AddMapEntries( s_argmap);
	retval = s_args.Parse( argc, argv);

	if( retval > 0)
	{
		//Validate Arguments.
		retval = ValidateArgs();
		if( retval) {
			PrintUsage();
			return retval;
		}

		// Open the specified file for writing.
		s_fileStream = fopen(s_params.sfile.c_str(), "wb");
		if (!s_fileStream) {
			printf("\nError: Provided filename ('%s') cannot be "
				"accessed.\n", s_params.sfile.c_str());
			PrintUsage();
			return -3;
		}

		// Now run the read operation.
		retval = Run();

		//close the file -- note this was opened in ParseOptions,
		if (fclose(s_fileStream) != 0) {
			printf("\nError %d closing file '%s'.\n", errno, s_params.sfile.c_str());
		}
	}
	else //Arguments were not right.
	{
		PrintUsage();
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

		int addr = s_cfgDevice.Attach();

		printf("Attached to ASR-2300 at address = %d\n", addr);

	} catch (std::runtime_error& re) {
		printf("Error:  %s\n", re.what());
		return -8;
	}

	// Dump the device information
	DumpDeviceInformation();

	// Execute the bulk data transfer program
	retval = DoRxToFile ();

	s_cfgDevice.Detach();
	return retval;
}

/**
* <summary>
* Function configures the ASR-2300 to receive a specified frequency
* and then streams the data to the host via BulkDataPort interface.
* </summary>
*/

static int DoRxToFile ()
{
	const RxConfig& cfg    = s_config[s_params.idxConfig];
	bool      bIsRf0 = cfg.idRf == WCACOMP_RF0;

	double dSampRateHz = s_params.samprate * 1.0e6;
	double dt = ((double) s_params.numsamples) / dSampRateHz;
	double framesPerSecond = dSampRateHz*cs_BytesPerSample / cs_sizeFrame;
	ulong totalFrames = (ulong)ceil(framesPerSecond*dt);

	// 1) Bind the Bulk Data Port to the specified endpoint.
	BulkDataPort portData(cfg.epRx, PortBase::EP_UNDEF);
	UsbDevice& device  = s_cfgDevice.Device();
	if( device.BindPort( &portData) != 0)
	{
		printf("Error Binding to Wca Ports.");
		return -5;
	}

	//2) Reset the DDC Component so no data is in cache.
	ConfigDduc ddc( cfg.idDdc, "ddc", &s_cfgDevice, true);
	s_cfgDevice.Components().push_back(&ddc);

	//3) Configure the RF Front-end profile, gain, frequency, and Bandwidth.
	ConfigRf& rf = (bIsRf0) ? s_cfgDevice.RF0() : s_cfgDevice.RF1();
	rf.RxPath( cfg.idPath);
	s_params.gain		= rf.RxGain( s_params.gain);
	s_params.freq		= rf.RxFrequency( (uint32) (s_params.freq*1000 + 0.5))/1000.0;
	s_params.bandwidth	= ConfigRf::BandwidthToMHz(rf.RxBandwidth( ConfigRf::BandwidthFromMHz( s_params.bandwidth)));

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
	s_params.samprate = ddc.HostSamplingRate( s_params.samprate*1.0e6, true)/1.0e6;
	//TODO CONFIGURE IF Frequency.

	//7) Display Configuration actually used.
	printf("\n");
	printf("Opening Data Port...\n");
	printf("--------------------------------------\n");
	printf("RF Component:  %s (id=%02Xh, ep=%02Xh)\n", (bIsRf0)? "RF0" : "RF1", cfg.idRf, cfg.epRx);
	printf("RX Profile:    %s (id=%02Xh)\n", cfg.szDescr, cfg.idPath);
	printf("RX Gain:       %d dB\n",	  s_params.gain);
	printf("RX Frequency:  %0.3f MHz\n",  s_params.freq);
	printf("RX Bandwidth:  %0.3lf MHz\n", s_params.bandwidth);
	printf("Host Rate:     %0.3lf MHz\n", s_params.samprate);
	if (s_params.numsamples == 0) 
	{
		printf("Duration:      infinite samples\n");
	} 
	else 
	{
		printf("Duration:      %ld samples, %0.02lf sec, %lu frames\n", s_params.numsamples, dt, totalFrames);
	}

	//8) Process until finished
	printf("\n--> Starting Run\n");
	int retval = 0;
	ddc.Enable(true);

	// start a separate thread waiting on any keyboard entry
#if defined(LINUX) || defined(APPLE)
	pthread_t keyThread;
	if (pthread_create (&keyThread, NULL, KeyThreadFunc, NULL) != 0) {
		printf ("Error creating key entry thread; use ^C to exit data connection if necessary.\n");
	}
#endif

	if (s_params.numsamples != 0) 
	{

		printf("Wait for finish, or hit any key to stop data collection ...\n");

		s_numBytesToCollect = s_params.numsamples * cs_BytesPerSample;
		s_numBytesCollected = 0;

		while ((retval == 0) && (!s_bKeyHit) &&
			(s_numBytesCollected < s_numBytesToCollect))
		{
#if defined(HAVE_LIBUSB)
			retval= device.PollAsynchronousEvents();
#elif defined(WIN32)
			//Windows implementation does not have a PollAsynchronous Data method.
			//Instead, we wait for events on each port.
			BulkDataPort::TransferContext* pctxt = portData.WaitForReadTransferEvent( 1000);
			retval = pctxt->status;
#endif
		}
	} 
	else 
	{
		printf("Hit any key to stop data collection ...\n");

		while ((retval == 0) && (!s_bKeyHit))
		{
#if defined(HAVE_LIBUSB)
			retval= device.PollAsynchronousEvents();
#elif defined(WIN32)
			//Windows implementation does not have a PollAsynchronous Data method.
			//Instead, we wait for events on each port.
			BulkDataPort::TransferContext* pctxt = portData.WaitForReadTransferEvent( 1000);
			retval = pctxt->status;
#endif
		}
	}

	// check for USB error
	if( retval != 0) printf("USB error occurred transfer stopped.\n");

	//9) Reset the DDC Port (disables the data pump)
	ddc.Reset();

	//Done with DDC.
	s_cfgDevice.Components().remove( &ddc);

	//10) Print results
	printf("\n--> Completed Run: Packets = %ld, Bytes = %ld\n", s_ctPackets, s_ctData);

	//11) Disable the RF Communications
	rf.RxPath( bIsRf0 ? (byte)RX0DPE_Disabled : (byte)RX1DPE_Disabled );

	//12) Cancel all transfer operations.
	BulkDataPort::TransferContextList::iterator iter;
	for( iter = listContext.begin(); iter != listContext.end(); iter++)
		(*iter)->Cancel(1000);

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

	//13) Destroy all the transfer contexts.
	for( iter = listContext.begin(); iter != listContext.end(); iter++)
		(*iter)->Destroy(); // All done.

	//14) Close the Data Port
	portData.Close();

	// join the keyboard entry thread
#if defined(LINUX) || defined(APPLE)
	if (!s_bKeyHit) {
		pthread_kill (keyThread, SIGINT);
	}
	pthread_join (keyThread, NULL);
#endif

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
		// common functionality
		++s_ctPackets;
		s_ctData += pctxt->nActualLength;
		pctxt->Submit();

		if (s_params.numsamples == 0) {
			// Save data to disk
			size_t nWritten = fwrite (pctxt->bufFrame, 1, pctxt->nActualLength, s_fileStream);
			if (nWritten != pctxt->nActualLength) {
				printf ("\nWarning: Some data failed to write to the selected file ('%s').\n", s_params.sfile.c_str());
				printf ("  Ignoring and hoping for the best.\n");
			}

			// Print progress '.'
			if ((s_ctPackets % 488) == 0)
			{
				putc('.', stdout);
				fflush(stdout);
			}
		} else {

			size_t nToWrite = s_numBytesToCollect - s_numBytesCollected;
			if (nToWrite > pctxt->nActualLength) {
				nToWrite = pctxt->nActualLength;
			}

			// Save data to disk
			size_t nWritten = fwrite (pctxt->bufFrame, 1, nToWrite, s_fileStream);
			if (nWritten != nToWrite) {
				printf ("\nWarning: Some data failed to write to the selected file ('%s').\n", s_params.sfile.c_str());
				printf ("  Ignoring and hoping for the best.\n");
			}

			size_t oldNumBytesCollected = s_numBytesCollected;
			s_numBytesCollected += nWritten;

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

static int ValidateArgs()
{
	//Get configuration path.
	std::string sPath = s_args.GetString("path");
	int idxConfig = 0;
	for( ; idxConfig < COUNT_CONFIG; idxConfig++)
	{
		if( sPath.compare( s_config[ idxConfig].szPath) == 0)
			break;
	}
	if( idxConfig == COUNT_CONFIG)
	{
		printf( "Error - Invalid path variable %s\n\n", sPath.c_str());
		return -1;
	}

	s_params.sfile = s_args.GetString("filename");
	s_params.idxConfig = idxConfig;
	s_params.freq = s_args.GetDouble("freq");
	s_params.gain = (byte) s_args.GetLong("gain");
	s_params.bandwidth = s_args.GetDouble("bw");
	s_params.samprate = s_args.GetDouble("rate");
	s_params.numsamples = (size_t) s_args.GetLong("num");

	//TODO Add range validation.  Rather extend ArgParser to do it.	
	return 0;
}

/**
* <summary>
* Writes the application header information to the standard output.
* </summary>
*/

static void  WriteHeader() {
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
		"  A2300RxToFile filename [[<var>=<value>] ...]\n\n");

	s_args.WriteDescriptions();
}

/**
* <summary>
* Print out Device information.
* </summary>
*/

static void DumpDeviceInformation ()
{
	std::string sId = s_cfgDevice.IdentifyDevice();
	std::string sVer = s_cfgDevice.FirmwareVersion(0);
	uint16 idFpga = s_cfgDevice.FpgaId();
	uint16 verFpga = s_cfgDevice.FpgaVersion();
	int iVer = (verFpga >> 8);
	int iRev = (verFpga & 0x00ff);

	printf("\n");
	printf("Identity:    %s\n", sId.c_str());
	printf("FW Ver:      %s\n", sVer.c_str());
	printf("FPGA ID-Ver: %04X-%02X.%02X\n\n", idFpga, iVer, iRev);
}

/**
* <summary>
* Thread to get key hit, unbuffered but blocking, to stop data collection
* </summary>
*/

static void* KeyThreadFunc (void* /* arg */)
{
#if defined(LINUX) || defined(APPLE)
	struct termios saveTermios;
	setCbreak (saveTermios);
	s_bKeyHit = false;
	getchar();
	unsetCbreak (saveTermios);
	s_bKeyHit = true;
#endif
	return NULL;
}

/**
* <summary>
* Set terminal character mode to unbuffered, read at will; returns
* true on success, false on failure (for any reason).
* </summary>
*/

static bool setCbreak (struct termios& saveTermios)
{
#if defined(LINUX) || defined(APPLE)
	// Remove line buffering from stdin
	if (setvbuf (stdin, NULL, _IONBF, 0) != 0) {
		return false;
	}

	// Store previous terminal settings
	if (tcgetattr (0, &saveTermios)) {
		return false;
	}

	// Modify terminal settings, using a copy of the previous settings
	struct termios buf = saveTermios;
	buf.c_lflag &= ~(ECHO|ICANON);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	// Set new terminal settings
	if (tcsetattr (0, TCSAFLUSH, &buf) < 0) {
		return false;
	}
#endif
	return true;
}

static bool unsetCbreak (struct termios& savedTermios)
{
#if defined(LINUX) || defined(APPLE)
	// Return line buffering to stdin
	if (setvbuf (stdin, NULL, _IOLBF, 0) != 0) {
		return false;
	}

	// Return original terminal settings
	if (tcsetattr (0, TCSAFLUSH, &savedTermios) < 0) {
		return false;
	}
#endif
	return true;
}
