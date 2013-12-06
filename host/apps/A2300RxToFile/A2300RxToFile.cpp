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

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <stdexcept>
#include <vector>

#include <A2300/ConfigDevice.h>

using namespace A2300;

/******************************************************************
 * Static Data
 *****************************************************************/

static const size_t  s_ctFrames = 16;
static const size_t  s_sizeFrame = 8192;
static byte s_Frames[s_ctFrames*s_sizeFrame];

// default to: RF0, PCS Band, 9 dB Gain, 1950 MHz, 2.5 MHz bandwidth,
// 2 MHz sampling, infinite # samples (0)

static byte s_idComponent = WCACOMP_RF0;
static byte s_rxProfile = RX0DPE_PcsExt;
static byte s_rxGain = 3;
static uint32 s_rxFreq = 1950000;
static byte s_rxBandwidth = 13;
static uint16 s_hostSampRate = 8;
static size_t s_numSamples = 0;

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
static int ParseOptions (int argc, char** argv);
static bool IsArgumentName (pcstr arg, pcstr szName, size_t minChars);
static void DumpDeviceInformation ();

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

int main(int argc, char** argv) {
	int retval = 0;
	WriteHeader();
	retval = ParseOptions(argc, argv);
	if( retval == 0)
	{
		// Open the specified file for writing.
		s_fileStream = fopen(s_fileName, "w");
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
	float fSampFreq = 32.0e6f / s_hostSampRate/2;
	double dt = 10.0;
	double framesPerSecond = fSampFreq*4 / s_sizeFrame; //4 bytes per sample
	ulong totalFrames = (ulong)ceil(framesPerSecond*dt);

	TransportDci& td = s_config.Dci0Transport();  //Get the dci interface so we can send messages directly.

	//Display some interesting information about the configuration.
	printf("\n");
	printf("Opening Data Port...\n");
	printf("--------------------------------------\n");
	printf("RF Component:  %s (id=%02Xh, ep=%02Xh)\n", (bIsRf0)? "RF0" : "RF1", s_idComponent, epRx);
	printf("RX Profile:    %s (id=%02Xh)\n", szPaths[s_rxProfile & 0xF], s_rxProfile);
	printf("RX Gain:       %d dB\n", s_rxGain*3);
	printf("RX Frequency:  %0.3f MHz\n", s_rxFreq/1000.0F);
	printf("RX Bandwidth:  ?? MHz (id=%02Xh)\n", s_rxBandwidth);
	printf("Host Rate:     %0.3f MHz\n", fSampFreq/1.0e6f);
	printf("Duration:      %0.02lf sec (%lu frames)\n", dt, totalFrames);

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
	for( size_t i = 0; i < s_ctFrames; i++)
	{
		BulkDataPort::TransferContext* pctxt = portData.CreateReadTransferContext(
				s_Frames + i*s_sizeFrame, s_sizeFrame);
		pctxt->Submit();
		listContext.push_back( pctxt);
	}

	//6) Configure the DDC and Enable bulk data transfer on the device.
	//   NOTE:  DDC functionality will move to the ConfigDDUC class at some point.
	td.SetProperty<uint16>(idDdc, DSP_DDUC_SAMPRATE, s_hostSampRate);
	//TODO CONFIGURE IF Frequency.
	td.SetProperty<byte>(idDdc, DSP_DDUC_CTRL, DSP_DDUC_CTRL_ENABLED);

	//7) Process for 10 seconds.

	printf("--> Starting Run\n");


	int retval = 0;
	while( retval == 0 && s_ctPackets < totalFrames)
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

	if( retval != 0) printf("USB error occurred transfer stopped.\n");

	//8) Reset the DDC Port
	td.SetProperty<byte>(idDdc, DSP_DDUC_CTRL, DSP_DDUC_CTRL_RESET);
	td.SetProperty<byte>(idDdc, DSP_DDUC_CTRL, DSP_DDUC_CTRL_DISABLED);

	//9) Set Path Profile to Disabled.
	printf("\n\n--> Completed Run:  Packets = %ld, Bytes = %ld\n", s_ctPackets, s_ctData);
	fflush(stdout);

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

static void OnFrameReady (void* arg, BulkDataPort::TransferContext* pctxt)
{
	//Save data to disk.
	if( pctxt->status == 0)
	{
		s_ctPackets++;
		s_ctData += pctxt->nActualLength;
		if( (s_ctPackets % 488) == 0)
		{
			putc('.', stdout);
			fflush(stdout);
		}

		//Resubmit.
		pctxt->Submit();

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
	printf("\nUsage for A2300Update:\n\n"
			"  A2300Update w[rite] [fi[rmware]|p[rofile]|fl[ash]|f[pga]] file\n"
			"    write (download) file to firmware, RF profiles, flash, or directly to FPGA.\n\n"
			"  A2300Update r[ead] p[rofile]|fl[ash]] data\n"
			"    read (upload) Rf profile or flash data from device to file.\n\n"
			"NOTE: Distinct sub-words are allowed; for example:\n"
			"    A2300Update w p file\n"
			"  means the same as\n"
			"    A2300Update write profile file\n\n"
			"WRITING firmware will cause the device to reprogram.  Be sure you do this operation carefully, it can brick the device\n\n"
			"NOTE: Text is case sensitive; just use lowercase. \n");
}

/**
 * <summary>
 * Parses the privided command line string.
 * </summary>
 */

static int ParseOptions(int argc, char** argv) {
#if 0
	int i;

	if (argc < 4) {
		printf("\nError: Too few arguments: Got %d, expecting 4.\n", argc);
		PrintUsage();
	}

	//Parse the Transfer direction.
	transferDir dir;
	if 	( IsArgumentName( argv[1], "write", 1)) {
		dir = e_Download;
	}	else if ( IsArgumentName( argv[1], "read", 1))  {
		dir = e_Upload;
	}	else    {
		printf("\nError: Unknown second argument: '%s'\n", argv[1]);
		printf("  Must be either 'read' or 'write'.\n");
		PrintUsage();
		return -1;
	}

	//Parse the Operation Mode.
	for( i = 0; i < COUNT_OPS; i++) {
		if( dir == s_aops[i].dir && IsArgumentName( argv[2], s_aops[i].szName, 2) )
		{
			s_pOp = s_aops + i;
			break; // we found it.
		}
	}

	if( s_pOp == NULL)	{
		printf("\nError: Unknown third argument: '%s'\n", argv[2]);
		printf("	Must be one of 'firmware', 'profile', or 'flash'.\n");
		PrintUsage();
		return -2;
	}
#endif

	// File name to write to.
	s_fileName = argv[3];

	return 0;
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
