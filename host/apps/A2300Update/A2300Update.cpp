// Name: A2300Update.cpp
//
// Copyright(c) 2013 Loctronix Corporation
// http://www.loctronix.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <stdexcept>
#include <vector>

#include <Dci/DciUtils.h>
#include <A2300/ConfigDevice.h>

using namespace A2300;

/******************************************************************
 * global variables
 *****************************************************************/

#ifndef WCA_COMPONENT_INDEX
// temporarily, until it is defined elsewhere
#define WCA_COMPONENT_INDEX ((byte)4)
#warning "WCA_COMPONENT_INDEX not yet defined."
#else
#warning "WCA_COMPONENT_INDEX has been defined elsewhere; please remove this warning section."
#endif



/******************************************************************
 * Type Declarations.
 *****************************************************************/

typedef enum _opType {
	e_UpdateFirmware,
	e_DownloadToFlash,
	e_DownloadToFpga,
	e_DownloadToRfProfiles,
	e_UploadFromFlash,
	e_UploadFromRfProfiles
} opType;

typedef enum _transferDir {
	e_Download,
	e_Upload
} transferDir;
/**
* Handler called when component is to initiate transfer.
*/
typedef int (*OpHandlerFnc)();

typedef struct _opConfig {
	opType op;
	transferDir dir;
	pcstr szName;
	byte idTargetComponent;
	OpHandlerFnc  fncOpHandler;
	pcstr szDescriptionFormat;
} opConfig;



/******************************************************************
 * Forward Declarations.
 *****************************************************************/

static int Run();

//Operational control functions.
static int DoUpdateFirmware();
static int DoBitTransferFlash();
static int DoBitTransfer();

//Program configuration routines
static void WriteHeader();
static void PrintUsage();
static int ParseOptions(int argc, char** argv);
static bool IsArgumentName( pcstr arg, pcstr szName, size_t minChars);
static void DumpDeviceInformation();

//BitUtil support functions.
static byte OnBitInitiateSourceTransfer(Dci_BitOperation* pbop);
static byte OnBitInitiateTargetTransfer(Dci_BitOperation* pbop);
static int OnBitGetFrameData(Dci_BitOperation* pbop, byte* buff, uint16 ctBytes);
static int OnBitSetFrameData(Dci_BitOperation* pbop, byte* buff, uint16 ctBytes);
static void OnBitTransferComplete(Dci_BitOperation* pbop, byte idStatus);
static int OnSendMessage(byte* pmsg, int len, bool bAckRequired,	Dci_Context* pctxt );
/******************************************************************
 * Static Data
 *****************************************************************/

//Define supported operations.
static const opConfig s_aops[] = {
		{e_UpdateFirmware, 			e_Download, "firmware", WCACOMP_FLASH,		DoUpdateFirmware, 	"Updating ASR-2300 firmware with file %s\n"},
		{e_DownloadToFlash, 		e_Download, "flash", 	WCACOMP_FLASH,		DoBitTransferFlash, "Downloading %s to ASR-2300 flash\n"},
		{e_DownloadToFpga, 			e_Download, "fpga", 	WCACOMP_FPGA,		DoBitTransfer, 		"Downloading %s to ASR-2300 FPGA\n"},
		{e_DownloadToRfProfiles, 	e_Download, "profiles", WCACOMP_RFPROFILES, DoBitTransfer,		"Downloading %s to ASR-2300 RF Profiles NVM\n"},
		{e_UploadFromFlash, 		e_Upload, 	"flash", 	WCACOMP_FLASH, 		DoBitTransfer,		"Uploading ASR-2300 Flash to %s\n" },
		{e_UploadFromRfProfiles, 	e_Upload, 	"profiles", WCACOMP_RFPROFILES, DoBitTransfer,		"Uploading ASR-2300 RF Profiles to %s\n" }
};
#define COUNT_OPS  6

static ConfigDevice s_config;
static Dci_BitClient s_BITClient;
static Dci_BitOperationMgr s_bitmgr;

static const opConfig* s_pOp = NULL;
static char* s_fileName = NULL;
static FILE* s_fileStream = NULL;
static TransportDci* s_ptd = NULL;


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
		//Open the specified file for writing or reading.
		if( s_pOp->dir == e_Upload)	{
			s_fileStream = fopen(s_fileName, "w");
			if (!s_fileStream) {
				printf("\nError: Provided filename ('%s') cannot be "
						"accessed or created.\n", s_fileName);
				PrintUsage();
				return -3;
			}
		} else { //Download
			s_fileStream = fopen(s_fileName, "r");
			if (!s_fileStream) {
				printf("\nError: Provided filename ('%s') cannot be "
						"accessed for reading.\n", s_fileName);
				PrintUsage();
				return -4;
			}
		}


		//Now run the Update operation.
		retval = Run();

		//close the file -- note this was opened in ParseOptions,
		if (fclose(s_fileStream) != 0) {
			printf("\nError %d closing the BIT file '%s'.\n", errno, s_fileName);
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

	//Dump the device information and grab the
	//DCI transport interface.
	DumpDeviceInformation();
	s_ptd = &(s_config.Dci0Transport());


	//1) Tell the user what we are doing
	printf("--------------------------------------\n");
	printf( s_pOp->szDescriptionFormat, s_fileName);
	printf("--------------------------------------\n");

	//2) set up the BIT operations manager
	Dci_BitOperationMgrInit(&s_bitmgr, &OnSendMessage);

	//3) set up the BIT client and its callbacks.

	//NOTE: Make our component ID the same as the target component, so it knows
	//how to send the data back.  This worksaround a design issue with the BIT
	//structure.
	Dci_BitClient_Init(&s_BITClient, s_pOp->idTargetComponent);
	s_BITClient.fncInitiateSourceTransfer = &OnBitInitiateSourceTransfer;
	s_BITClient.fncInitiateTargetTransfer = &OnBitInitiateTargetTransfer;
	s_BITClient.fncGetFrameData = &OnBitGetFrameData;
	s_BITClient.fncSetFrameData = &OnBitSetFrameData;
	s_BITClient.fncTransferComplete = &OnBitTransferComplete;

	//4) Register the Client with the BIT operations manager.
	Dci_BitRegisterClient(&s_bitmgr, &s_BITClient);

	// Do the specified operation
	retval = (*(s_pOp->fncOpHandler))();

	s_config.Detach();
	return retval;
}

//*******************************************************
// Main Operation Handlers
//*******************************************************

static int DoUpdateFirmware()
{
	byte buff[MAX_MSG_SIZE];
	int outLen, inLen;
	int retval = 0;

	//1) Erase Flash and Download. NOTE, the name firmware will be
	//   added to the BIT info on target initiation.
	retval = DoBitTransferFlash();

	// 2) Tell device to update firmware.
	if( retval == 0) {
		printf("Updating firmware, device will reboot when complete...\n");
		fflush (stdout);

		memset(buff, 0, sizeof(buff));
		inLen = Dci_ExecuteAction_Init(buff, sizeof(buff), WCACOMP_MICRO,
				MICRO_UpdateFirmware, 0, NULL);
		outLen = s_ptd->SendMsg(buff, (size_t) inLen, false);

		if (outLen == inLen) {
			printf("done.\n");
		} else {
			printf("error; proceeding anyway.\n");
			retval = -7;
		}
	}
	return retval;
}
static int DoBitTransferFlash()
{
	byte buff[MAX_MSG_SIZE];
	int outLen, inLen;

	// Erase the flash, then wait 20
	// seconds; FIXME: hopefully this will be changed into an ACK (or
	// equivalent) in the near future; just sleep for now.
	printf("Erasing Flash ... ");
	fflush (stdout);
	memset(buff, 0, sizeof(buff));
	inLen = Dci_ExecuteAction_Init(buff, sizeof(buff), WCACOMP_FLASH,
			FLASH_ActionErase, 0, NULL);
	outLen = s_ptd->SendMsg(buff, (size_t) inLen, false);
	if (outLen == inLen) {
		printf("done.\n");
	} else {
		printf("error; proceeding anyway.\n");
	}
	printf("Sleeping for 20 seconds ... ");
	fflush(stdout);
	sleep(20);
	printf("done.\n");

	//Proceed with standard Bit Transfer.
	return DoBitTransfer();

}

/**
 * Operation Handler function implements message processing performing the specified BIT Transfer operation.
 */
int DoBitTransfer()
{
	byte buff[MAX_MSG_SIZE];

	Dci_Context ctxt;
	memset(&ctxt, 0, sizeof(ctxt));
	ctxt.pConv = s_ptd->Conversation();

	byte idStatus = BSE_OperationNotAvailable;

	if (s_pOp->dir == e_Download) {
		idStatus = Dci_BitInitiateTargetTransfer(&s_bitmgr, &s_BITClient,
				s_pOp->idTargetComponent, 1, 0, &ctxt);
		// flags: 1 == save; 0 means don't save

		if (idStatus != BSE_InitiatingTransfer) {
			printf("Error initiating target transfer:\n");
			if (idStatus == BSE_OperationNotAvailable) {
				printf("  Operation not available.\n");
				return -5;
			} else {
				printf("  Read error.\n");
				return -6;
			}
		}
	} else {
		Dci_BitRequestSourceTransfer(&s_bitmgr, &s_BITClient,
				s_pOp->idTargetComponent, 0, 0, &ctxt);
	}

	// enter while loop to process messages while BIT operation complates
	int nread = 0;
	int cntLoop = 0;

	while (cntLoop < 2) {
		memset(buff, 0, sizeof(buff));
		nread = s_ptd->ReceiveMsg(buff, MAX_MSG_SIZE);
		if (nread > 0) {

			Dci_Hdr* pMsg = (Dci_Hdr*) buff;

			// Prepare the context and send received message off for
			// processing.
			memset(&ctxt, 0, sizeof(ctxt));
			ctxt.pMsg = pMsg;
			ctxt.lenMsg = nread;
			ctxt.pConv = s_ptd->Conversation();
			ctxt.bHandled = false;
			ctxt.idMessage = Dci_Hdr_MessageId(pMsg);
			ctxt.idComponent = 0xFF;

			//If WCA Message grab the component ID to help WCA
			// based message processing.

			if (pMsg->idCategory == 0x21)
				ctxt.idComponent = ((byte*) pMsg)[WCA_COMPONENT_INDEX ];

			if (!Dci_BitProcessDciMsg(&s_bitmgr, &ctxt)) {
				printf("Unhandled Dci message: %04X.\n", ctxt.idMessage);
			}

			// should go idle when finished
			if (s_bitmgr.aBitOps[0].state == DCI_BOS_IDLE) {
				break;
			}

			// got something; reset loop counter
			cntLoop = 0;

		} else {
			// got nothing; increment loop counter towards failure
			++cntLoop;
		}
	}


	return 0;
}

/**
 * <summary>
 * Writes the application header information to the standard output.
 * </summary>
 */

static void WriteHeader() {
	printf(
			"*********************************************************************\n"
					"* ASR-2300 Update\n"
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


	//Open the file for the operation.
	s_fileName = argv[3];

	return 0;
}

/**
 * Helper function makes argument matching more readable.
 */
bool IsArgumentName( pcstr arg, pcstr szName, size_t minChars)
{
	size_t lenArg = strlen(arg);
	size_t lenName = strlen(szName);
	return lenArg >= minChars && lenArg <= lenName
			&& (strncmp( arg, szName, lenArg) == 0);
}



static void DumpDeviceInformation()
{
	//Print out Device information

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

//*******************************************************
// BIT Util Operation handlers
//*******************************************************

static byte OnBitInitiateSourceTransfer(Dci_BitOperation* pbop) {

	Dci_BinaryImageTransfer* pbiti = &(pbop->bitinfo);

	//Get the length of the file to send.
	fseek(s_fileStream, 0, SEEK_END);
	long fLen = ftell(s_fileStream);
	rewind(s_fileStream);


	// If updating firmware mark the biti with the "firmware"
	// name so it can be recognized in device as firmware.
	if (s_pOp->op == e_UpdateFirmware) {
		memcpy(pbiti->szName, "firmware", 32);
	}

	//Update size, number of frames, etc.
	pbiti->sizeImg = (uint32) fLen;
	pbiti->sizeFrame = 256;
	pbiti->ctFrames = (((uint32) fLen) + 255) / 256;

	//ready to go.
	return BSE_InitiatingTransfer;
}

static byte OnBitInitiateTargetTransfer(Dci_BitOperation* /* pbop */) {
	//Nothing todo we are ready to go.
	return BSE_InitiatingTransfer;
}

static int OnBitGetFrameData(Dci_BitOperation*  pbop, byte* buff,
		uint16 ctBytes) {
	// read ctBytes bytes from file and push into buffer
	size_t nread = fread(buff, 1, (size_t) ctBytes, s_fileStream);

	if( (pbop->idFrame %100) == 0) {
		printf(".");
		fflush(stdout);
	}

	return ((int) nread);
}

static int OnBitSetFrameData(Dci_BitOperation* pbop, byte* buff,
		uint16 ctBytes) {
	// write ctBytes bytes to file from buffer
	size_t nwritten = fwrite(buff, 1, (size_t) ctBytes, s_fileStream);

	if( (pbop->idFrame %100) == 0) {
		printf(".");
		fflush(stdout);
	}

	return ((int) nwritten);
}

static void OnBitTransferComplete(Dci_BitOperation* /* pbop */, byte idStatus) {
	if (idStatus == BSE_TransferComplete) {
		printf("\nTransfer complete.");
	} else {
		printf("\nTransfer failed: status is %d.", idStatus);
	}
}

/**
 * Sends a DCI message using the current message conversation context.
 */
static int OnSendMessage(byte* pmsg, int len, bool bAckRequired,
		Dci_Context* /* pctxt */) {
	return (s_ptd->SendMsg(pmsg, (size_t) len, bAckRequired));
}
