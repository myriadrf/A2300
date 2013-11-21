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
#include <time.h>

#include <stdexcept>
#include <vector>

#include <Dci/DciUtils.h>
#include <A2300/ConfigDevice.h>

using namespace A2300;

/******************************************************************
 * global variables
 *****************************************************************/

#define WCA_COMPONENT_INDEX 4

typedef enum {
  e_doingWrite,
  e_doingRead,
  e_rw_invalid
} rwType;

typedef enum {
  e_Firmware = WCACOMP_FLASH,
  e_RFProfile = -1,
  e_FPGADirect = WCACOMP_FPGA,
  e_tt_invalid = -1
} targetType;

static const size_t s_ctFrames = 16;
static const size_t s_sizeFrame = 8192;
static byte s_Frames[s_ctFrames*s_sizeFrame];
static rwType s_rwType = e_rw_invalid;
static targetType s_targetType = e_tt_invalid;
static char* s_fileName = NULL;
static FILE* s_fileStream = NULL;
static TransportDci* s_ptd = NULL;

/******************************************************************
 * Forward Declarations.
 *****************************************************************/

//Define BIT client for data transfer.
static Dci_BitClient s_BITClient;

static Dci_BitOperationMgr s_bitmgr;

//Define BIT handlers.
static byte OnBitInitiateSourceTransfer(Dci_BitOperation* pbop);
static byte OnBitInitiateTargetTransfer(Dci_BitOperation* pbop);
static int  OnBitGetFrameData(Dci_BitOperation* pbop, byte* buff, uint16 ctBytes);
static int  OnBitSetFrameData(Dci_BitOperation* pbop, byte* buff, uint16 ctBytes);
static void OnBitTransferComplete(Dci_BitOperation* pbop, byte idStatus);

class A2300Update
{

public:

	A2300Update( rwType rw_type, targetType target_type,
		     char* file_name, FILE* bit_file);

	int Run( );

private:

	ConfigDevice  m_config;
	unsigned int  m_ctPackets;
	unsigned int  m_ctData;
	bool 		  m_bRunning;
	rwType		  m_rwType;
        targetType        m_targetType;
	char*		  m_fileName;
	FILE*		  m_fileStream;
};

void WriteHeader( );
void ParseOptions(int argc, char** argv);

/******************************************************************
 * Main Program Entry Point.
 *****************************************************************/

int main(int argc, char** argv)
{
	//ASR-2300 Product information.
	WriteHeader( );
	ParseOptions( argc, argv);
	A2300Update bitUtil( s_rwType, s_targetType, s_fileName, s_fileStream);
	return bitUtil.Run( );
}

/**
 * <summary>
 * Writes the application header information to the standard output.
 * </summary>
 */

void WriteHeader( )
{
	printf("*********************************************************************\n"
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

void PrintUsage( )
{
	printf("\nUsage for A2300Update:\n\n");
	printf("  A2300Update w[rite] [fi[rmware]|p[rofile]|fl[ash]] file\n");
	printf("    write file to [firmware|RF profile|FPGA flash]\n\n");
	printf("  A2300Update r[ead] [fi[rmware]|p[rofile]|fl[ash]] file\n");
	printf("    read from [firmware|RF profile|FPGA flash] to file\n\n");
	printf("NOTE: Distinct sub-words are allowed; for example:\n");
	printf("    A2300Update w p file\n");
	printf("  means the same as\n");
	printf("    A2300Update write profile file\n\n");
	printf("NOTE: Text is case sensitive; just use lowercase.\n");
	exit(-1);
}

/**
 * <summary>
 * Parses the privided command line string.
 * </summary>
 */

void ParseOptions(int argc, char** argv)
{
	if (argc < 4) {
		printf("\nError: Too few arguments: Got %d, expecting 4.\n", argc);
		PrintUsage();
	}

	s_rwType = e_rw_invalid;
	if ((strlen(argv[1]) <= strlen("write")) &&
			(strncmp (argv[1], "write", strlen(argv[1])) == 0)) {
		s_rwType = e_doingWrite;
	} else if ((strlen(argv[1]) <= strlen("read")) &&
			 (strncmp (argv[1], "read", strlen(argv[1])) == 0)) {
		s_rwType = e_doingRead;
	} else {
		printf("\nError: Unknown second argument: '%s'\n", argv[1]);
		printf("  Must be either 'read' or 'write'.\n");
		PrintUsage();
	}

	s_targetType = e_tt_invalid;
	if ((strlen(argv[2]) <= strlen("firmware")) &&
			(strncmp (argv[2], "firmware", strlen(argv[2])) == 0)) {
		s_targetType = e_Firmware;
	} else if ((strlen(argv[2]) <= strlen("profile")) &&
			 (strncmp (argv[2], "profile", strlen(argv[2])) == 0)) {
		s_targetType = e_RFProfile;
	} else if ((strlen(argv[2]) <= strlen("flash")) &&
			 (strncmp (argv[2], "flash", strlen(argv[2])) == 0)) {
		s_targetType = e_FPGADirect;
	} else {
		printf("\nError: Unknown third argument: '%s'\n", argv[2]);
	  printf("	Must be one of 'firmware', 'profile', or 'flash'.\n");
		PrintUsage();
	}

	s_fileName = argv[3];

	// try to open argv[3] for read or write
	if (s_rwType == e_doingRead) {
		s_fileStream = fopen(s_fileName, "w");
		if (!s_fileStream) {
			printf("\nError: Provided filename ('%s') cannot be "
			 "accessed or created.\n", s_fileName);
			PrintUsage();
		}
	} else {
		s_fileStream = fopen(s_fileName, "r");
		if (!s_fileStream) {
			printf("\nError: Provided filename ('%s') cannot be "
			 "accessed for reading.\n", s_fileName);
			PrintUsage();
		}
	}
}

/**
* Sends a DCI message using the current message conversation context.
*/

int opManagerSendMessage(  byte* pmsg, int len, bool bAckRequired, Dci_Context* pctxt)
{
  return (s_ptd->SendMsg(pmsg, len, bAckRequired));
}

A2300Update::A2300Update(rwType rw_type, targetType target_type, char* file_name, FILE* file_stream)
: m_ctPackets(0), m_ctData(0), m_bRunning(false), m_rwType(rw_type), m_targetType(target_type), m_fileName(file_name), m_fileStream(file_stream) {}

/**
 * <summary>
 * Primary entry point for running this executable.
 * </summary>
 */

int A2300Update::Run()
{
	try
	{
		// Find the list of addresses at the specified VID/PID.
		printf("\n");
		printf ("Enumerating ASR-2300 devices...\n");
		printf ("--------------------------------------\n");

		int addr = m_config.Attach();

		printf("Attached to ASR-2300 at address = %d\n", addr);

	}
	catch( std::runtime_error& re)
	{
		printf("Error:  %s\n", re.what() );
		return -1;
	}

	//Print out Device information
	std::string sId 		= m_config.IdentifyDevice();
	std::string sVer 		= m_config.FirmwareVersion(0);
	uint16	    idFpga 		= m_config.FpgaId();
	uint16 		verFpga 	= m_config.FpgaVersion();
	int  		iVer = (verFpga>>8);
	int	 		iRev = (verFpga& 0x00ff);

	printf("\n");
	printf( "Identity:    %s\n", sId.c_str());
	printf( "FW Ver:      %s\n", sVer.c_str());
	printf( "FPGA ID-Ver: %04X-%02X.%02X\n\n", idFpga, iVer, iRev);

	printf("--------------------------------------\n");
	printf("Starting transfer ");
	if (m_rwType == e_doingWrite) {
	  printf("from file '%s' to device ", m_fileName);
	} else {
	  printf("from device ");
	}
	if (m_targetType == e_Firmware) {
	  printf("firmware");
	} else if (m_targetType == e_RFProfile) {
	  printf("RF profile");
	} else {
	  printf("FPGA flash");
	}

	if (m_rwType == e_doingRead) {
	  printf(" to file '%s'", m_fileName);
	}
        printf(".\n");
	printf("--------------------------------------\n");

	//set up the BIT operations manager

	Dci_BitOperationMgrInit( &s_bitmgr, &opManagerSendMessage);

	//set up the BIT client

	Dci_BitClient_Init( &s_BITClient, WCACOMP_FLASH);
	s_BITClient.fncInitiateSourceTransfer 	= &OnBitInitiateSourceTransfer;
	s_BITClient.fncInitiateTargetTransfer 	= &OnBitInitiateTargetTransfer;
	s_BITClient.fncGetFrameData 				= &OnBitGetFrameData;
	s_BITClient.fncSetFrameData 				= &OnBitSetFrameData;
	s_BITClient.fncTransferComplete 			= &OnBitTransferComplete;

	Dci_BitRegisterClient( &s_bitmgr, &s_BITClient);

	// initiate BIT operation

	// initiate target transfer

	s_ptd = &(m_config.Dci0Transport());

	// FPGA and Flash, not profile
	if () {
	  buff = buffer[MAX_SIZE];
	  len = Dci_ExecuteAction_Init (buff, WCACOMP_FLASH, FLASH_ActionErase,  0, NULL);
	s_ptd->SendMessage(buff, len);
	sleep(10);
	}

	Dci_Context ctxt;
	bzero(&ctxt, sizeof(ctxt));
	ctxt.pConv = s_ptd->Conversation();

	byte idStatus = BSE_OperationNotAvailable;

	if (s_rwType == e_doingWrite) {
	  idStatus = Dci_BitInitiateTargetTransfer(&s_bitmgr, &s_BITClient, s_targetType, 1, 0, &ctxt);
	  // flags: 1 == save; 0 means don't save

	  if (idStatus != BSE_InitiatingTransfer) {
	    printf("Error initiating target transfer:\n");
	    if (idStatus == BSE_OperationNotAvailable) {
	      printf("  Operation not available.\n");
	    } else {
	      printf("  Read error.\n");
	    }
	  }

	} else {
	  s_BITClient.idComponent = s_targetType;
	  Dci_BitRequestSourceTransfer(&s_bitmgr, &s_BITClient, s_targetType, 0, 0, &ctxt);
	}

	// enter while loop to process messages while BIT operation complates

	int nread = 0;
	byte buff[MAX_MSG_SIZE];
	bzero(&ctxt, sizeof(ctxt));
	bool bContinue = true;
	int cntLoop = 0;

	while(cntLoop < 2) {
	  nread = s_ptd->ReceiveMsg( buff, MAX_MSG_SIZE);
	  if (nread > 0) {

	    Dci_Hdr* pMsg = (Dci_Hdr*) buff;

	    // Prepare the context and send received message off for
	    // processing.
	    ctxt.pMsg = pMsg;
	    ctxt.lenMsg = nread;
	    ctxt.pConv = s_ptd->Conversation();
	    ctxt.bHandled = false;
	    ctxt.idMessage = Dci_Hdr_MessageId( pMsg);
	    ctxt.idComponent = 0xFF;

	    //If WCA Message grab the component ID to help WCA based
	    //message processing.

	    if( pMsg->idCategory == 0x21)
	      ctxt.idComponent = ((byte*)pMsg)[WCA_COMPONENT_INDEX];

	    if (!Dci_BitProcessDciMsg(&s_bitmgr, &ctxt)) {
	      printf("Unhandled Dci message: %04X.\n", ctxt.idMessage);
	    }

	    if (s_bitmgr.aBitOps[0].state == DCI_BOS_IDLE) {
	      break;
	    }

	    cntLoop = 0;
	  } else {
	    ++cntLoop;
	  }
	}

	m_config.Detach();

	//close the file
	if (fclose(m_fileStream) != 0) {
	  printf( "\nError %d closing the BIT file '%s'.\n",
		  errno, m_fileName);
	}

	// if firmware, need to send action message to upgrade
	if () {
	  buff = buffer[MAX_SIZE];
	  len = Dci_ExecuteAction_Init (buff, WCACOMP_FLASH, MICRO_UPDATE_FIRMWARE,  0, NULL);
	  s_ptd->SendMessage(buff, len);
	  sleep(10);
	}

	return(0);
}

//*******************************************************
// BIT Operation handlers
//*******************************************************

byte OnBitInitiateSourceTransfer(Dci_BitOperation* pbop)
{
  fseek(s_fileStream, 0, SEEK_END);
  long fLen = ftell(s_fileStream);
  rewind(s_fileStream);

  Dci_BinaryImageTransfer* pbiti = &(pbop->bitinfo);

  // already initialized to empty string, so copy only if needed
  if (s_targetType == e_Firmware) {
    memcpy( pbiti->szName, "firmware", 32);
  } else if (s_targetType == e_RFProfile) {
    memcpy( pbiti->szName, s_fileName, 32);
  }

  // already initialized to empty string, so copy only if needed
  // memcpy( pbiti->szDescription, s_szDescription, 32);

  pbiti->sizeImg   = fLen;
  pbiti->sizeFrame = 256;
  pbiti->ctFrames  = (fLen + 255) / 256;

  return BSE_InitiatingTransfer;
}

byte OnBitInitiateTargetTransfer(Dci_BitOperation* pbop)
{
  //Nothing todo we are ready to go.
  return BSE_InitiatingTransfer;
}

int OnBitGetFrameData(Dci_BitOperation* pbop, byte* buff, uint16 ctBytes  )
{
  // read ctBytes bytes from file and push into buffer
  size_t nread = fread(buff, 1, (size_t) ctBytes, s_fileStream);
  return (nread);
}

int OnBitSetFrameData(Dci_BitOperation* pbop, byte* buff, uint16 ctBytes )
{
  // write ctBytes bytes to file from buffer
  size_t nwritten = fwrite(buff, 1, (size_t) ctBytes, s_fileStream);
  return (nwritten);
}

void OnBitTransferComplete(Dci_BitOperation* pbop, byte idStatus)
{
  if( idStatus == BSE_TransferComplete ) {
    printf ("Transfer complete.");
  } else {
    printf ("Transfer failed: status is %d.", idStatus);
  }
}
