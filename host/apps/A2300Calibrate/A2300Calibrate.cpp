/** Name: A2300Calibrate.cpp
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
#include <Windows.h>
#endif

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <stdexcept>
#include <vector>

#include <A2300/ConfigDevice.h>

//Include sleep functionality and homogenize api.
#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN 
	#include<windows.h>
	#define SLEEP_SEC(a)  Sleep((a)*1000)
#else
	#include <unistd.h>
	#define SLEEP_SEC(a) sleep((a))
#endif 

using namespace A2300;

/******************************************************************
 * Static Data
 *****************************************************************/

static ConfigDevice s_cfgDevice;

/******************************************************************
 * Forward Declarations.
 *****************************************************************/

static int Run ();
static void WriteHeader ();
static void DumpDeviceInformation ();

// Support functions
static int  DoCalibrate ();

/******************************************************************
 * Functions.
 *****************************************************************/

/**
 * <summary>
 * Main Program Entry Point.
 * </summary>
 */
int main(int /* argc */, const char** /* argv */)
{
	WriteHeader();
	// ignore all arguments
	return Run();
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

	// Calibrate the device
	retval = DoCalibrate ();

	// Detach from device
	s_cfgDevice.Detach();

	return retval;
}

/**
 * <summary>
 * Calibrates the ASR-2300.
 * </summary>
 */

static int DoCalibrate ()
{
  byte buff[DCI_MAX_MSGSIZE];
  int msgSize, ctSent, ctAck;
  TransportDci& tDci = s_cfgDevice.Dci0Transport ();

  struct _rxPath {
    byte Value;
    const char* Name;
  };

  struct _rxPath rx0Paths[] = {
    {(byte) RX0DPE_GpsL1Int, "GpsL1Int"},
    {(byte) RX0DPE_GpsL1Ext, "GpsL1Ext"},
    {(byte) RX0DPE_PcsExt, "PcsExt"},
    {(byte) RX0DPE_Wideband, "Wideband"}
  };
  const size_t numRx0Paths = sizeof(rx0Paths) / sizeof(struct _rxPath);

  struct _rxPath rx1Paths[] = {
    {(byte) RX1DPE_UhfExt, "UhfExt"},
    {(byte) RX1DPE_IsmInt, "IsmInt"},
    {(byte) RX1DPE_IsmExt, "IsmExt"},
    {(byte) RX1DPE_Wideband, "Wideband"}
  };
  const size_t numRx1Paths = sizeof(rx1Paths) / sizeof(struct _rxPath);

  struct _RxInfo {
    ConfigRf& cRf;
    byte numPaths;
    struct _rxPath* rxPaths;
  };

  struct _RxInfo rxInfo[] = {
    {s_cfgDevice.RF0(), (byte) numRx0Paths, rx0Paths},
    {s_cfgDevice.RF1(), (byte) numRx1Paths, rx1Paths}
  };
  const size_t numRxComponents = sizeof(rxInfo) / sizeof(struct _RxInfo);

  // disable both Tx and Rx paths
  s_cfgDevice.RF0().TxPath(TX0DPE_Disabled);
  s_cfgDevice.RF1().TxPath(TX1DPE_Disabled);
  s_cfgDevice.RF0().RxPath(RX0DPE_Disabled);
  s_cfgDevice.RF1().RxPath(RX1DPE_Disabled);

  SLEEP_SEC(1);

  // Set the RF Profile component into cache mode
  printf ("Setting the RF Profile component into cache mode ...\n");
  memset(buff, 0, sizeof(buff));
  msgSize = Dci_ExecuteAction_Init (buff, DCI_MAX_MSGSIZE, WCACOMP_RFPROFILES, RFP_ACTION_CACHEDATA, 0, NULL);
  ctSent = tDci.SendMsg (buff, (size_t) msgSize, true);
  // get ack, and verify
  memset(buff, 0, sizeof(buff));
  ctAck = tDci.ReceiveMsg (buff, DCI_MAX_MSGSIZE);
  if ((ctAck <= 0) || (ctSent != msgSize)) {
    printf ("Warning: Unable to set device into cache mode (%d, %d).\n",
	    ctSent, ctAck);
    printf ("  Proceeding anyway.");
  }
  // print received ack info
  if (Dci_Hdr_MessageId((Dci_Hdr*) buff) == Dci_DebugMsg_Id) {
    Dci_Hdr* pMsg = (Dci_Hdr*) buff;
    Dci_DebugMsg* plog = (Dci_DebugMsg*)( pMsg);
    std::string smsg = TransportDci::DebugMsgToString( plog);
    puts( smsg.c_str());
    putc( '\n', stdout);
  }
  SLEEP_SEC(1);
  printf ("Done.\n");

  for (byte nn = 0; nn < (byte) numRxComponents; ++nn) {

    // calibrate profiles for this Rx component
    struct _RxInfo& tRxInfo = rxInfo[nn];

    // Reset Top-Level Calibration
    printf ("Resetting Top-Level Calibration for Rx component %d ...\n", (nn+1));

    memset(buff, 0, sizeof(buff));
    msgSize = Dci_ExecuteAction_Init (buff, DCI_MAX_MSGSIZE, tRxInfo.cRf.componentId(), RFACTION_RESETTOPCALIB, 0, NULL);
    ctSent = tDci.SendMsg (buff, (size_t) msgSize, true);
    // get ack, and verify
    memset(buff, 0, sizeof(buff));
    ctAck = tDci.ReceiveMsg (buff, DCI_MAX_MSGSIZE);
    if ((ctAck <= 0) || (ctSent != msgSize)) {
      printf ("Warning: Unable to Reset Top-Level Calibration for Rx component #%d (%d, %d).\n", nn, ctSent, ctAck);
      printf ("  Proceeding anyway.");
    }
    // print received ack info
    if (Dci_Hdr_MessageId((Dci_Hdr*) buff) == Dci_DebugMsg_Id) {
      Dci_Hdr* pMsg = (Dci_Hdr*) buff;
      Dci_DebugMsg* plog = (Dci_DebugMsg*)( pMsg);
      std::string smsg = TransportDci::DebugMsgToString( plog);
      puts( smsg.c_str());
      putc( '\n', stdout);
    }
    SLEEP_SEC(1);
    printf ("Done.\n");

    // Calibrate Top-Level
    printf ("Calibration Top-Level for Rx component %d ...\n", (nn+1));
    memset(buff, 0, sizeof(buff));
    msgSize = Dci_ExecuteAction_Init (buff, DCI_MAX_MSGSIZE, tRxInfo.cRf.componentId(), RFACTION_TOPCALIBRATE, 0, NULL);
    ctSent = tDci.SendMsg (buff, (size_t) msgSize, true);
    // get ack, and verify
    memset(buff, 0, sizeof(buff));
    ctAck = tDci.ReceiveMsg (buff, DCI_MAX_MSGSIZE);
    if ((ctAck <= 0) || (ctSent != msgSize)) {
      printf ("Warning: Unable to Calibrate Top-Level for Rx component #%d  (%d, %d).\n", nn, ctSent, ctAck);
      printf ("  Proceeding anyway.");
    }
    // print received ack info
    if (Dci_Hdr_MessageId((Dci_Hdr*) buff) == Dci_DebugMsg_Id) {
      Dci_Hdr* pMsg = (Dci_Hdr*) buff;
      Dci_DebugMsg* plog = (Dci_DebugMsg*)( pMsg);
      std::string smsg = TransportDci::DebugMsgToString( plog);
      puts( smsg.c_str());
      putc( '\n', stdout);
    }
    SLEEP_SEC(1);
    printf ("Done.\n");

    ConfigRf& cRf = tRxInfo.cRf;

    // calibrate all Rx paths, one at a time
    for (byte mm = 0; mm < tRxInfo.numPaths; ++mm) {

      // Set RX Path to Calibrate
      struct _rxPath& thisRxPath = tRxInfo.rxPaths[mm];
      cRf.RxPath(thisRxPath.Value);

      // Reset RX Calibration for this path
      printf ("Resetting Calibration for Rx component %d path %d (%s) ...\n", (nn+1), (mm+1), thisRxPath.Name);
      memset(buff, 0, sizeof(buff));
      msgSize = Dci_ExecuteAction_Init (buff, DCI_MAX_MSGSIZE, tRxInfo.cRf.componentId(), RFACTION_RESETRXCALIB, 0, NULL);
      ctSent = tDci.SendMsg (buff, (size_t) msgSize, true);
      // get ack, and verify
      memset(buff, 0, sizeof(buff));
      ctAck = tDci.ReceiveMsg (buff, DCI_MAX_MSGSIZE);
      if ((ctAck <= 0) || (ctSent != msgSize)) {
	printf ("Warning: Unable to Reset RX Calibration for Rx%d path %s (%d, %d).\n", nn, thisRxPath.Name, ctSent, ctAck);
	printf ("  Proceeding anyway.");
      }
      // print received ack info
      if (Dci_Hdr_MessageId((Dci_Hdr*) buff) == Dci_DebugMsg_Id) {
	Dci_Hdr* pMsg = (Dci_Hdr*) buff;
	Dci_DebugMsg* plog = (Dci_DebugMsg*)( pMsg);
	std::string smsg = TransportDci::DebugMsgToString( plog);
	puts( smsg.c_str());
	putc( '\n', stdout);
      }
      SLEEP_SEC(1);
      printf ("Done.\n");

      // Calibrate this path
      printf ("Calibrating Rx component %d path %d (%s) ...\n", (nn+1), (mm+1), thisRxPath.Name);
      memset(buff, 0, sizeof(buff));
      msgSize = Dci_ExecuteAction_Init (buff, DCI_MAX_MSGSIZE, tRxInfo.cRf.componentId(), RFACTION_SAVERXPROFILE, 0, NULL);
      ctSent = tDci.SendMsg (buff, (size_t) msgSize, true);
      // get ack, and verify
      memset(buff, 0, sizeof(buff));
      ctAck = tDci.ReceiveMsg (buff, DCI_MAX_MSGSIZE);
      if ((ctAck <= 0) || (ctSent != msgSize)) {
	printf ("Warning: Unable to Calibrate Rx%d path %s (%d, %d).\n", nn, thisRxPath.Name, ctSent, ctAck);
	printf ("  Proceeding anyway.");
      }
      // print received ack info
      if (Dci_Hdr_MessageId((Dci_Hdr*) buff) == Dci_DebugMsg_Id) {
	Dci_Hdr* pMsg = (Dci_Hdr*) buff;
	Dci_DebugMsg* plog = (Dci_DebugMsg*)( pMsg);
	std::string smsg = TransportDci::DebugMsgToString( plog);
	puts( smsg.c_str());
	putc( '\n', stdout);
      }
      SLEEP_SEC(1);
      printf ("Done.\n");
    }
  }

  // Save Cached Changes
  printf ("Saving all cached calibrations ...\n");
  memset(buff, 0, sizeof(buff));
  msgSize = Dci_ExecuteAction_Init (buff, DCI_MAX_MSGSIZE, WCACOMP_RFPROFILES, RFP_ACTION_SAVECHANGES, 0, NULL);
  ctSent = tDci.SendMsg (buff, (size_t) msgSize, true);
  // get ack, and verify
  memset(buff, 0, sizeof(buff));
  ctAck = tDci.ReceiveMsg (buff, DCI_MAX_MSGSIZE);
  if ((ctAck <= 0) || (ctSent != msgSize)) {
    printf ("Warning: Unable to set device into cache mode (%d, %d).\n",
	    ctSent, ctAck);
    printf ("  Proceeding anyway.");
  }
  // print received ack info
  if (Dci_Hdr_MessageId((Dci_Hdr*) buff) == Dci_DebugMsg_Id) {
    Dci_Hdr* pMsg = (Dci_Hdr*) buff;
    Dci_DebugMsg* plog = (Dci_DebugMsg*)( pMsg);
    std::string smsg = TransportDci::DebugMsgToString( plog);
    puts( smsg.c_str());
    putc( '\n', stdout);
  }
  printf ("Done.\n");

  // disable both Tx and Rx paths
  s_cfgDevice.RF0().TxPath(TX0DPE_Disabled);
  s_cfgDevice.RF1().TxPath(TX1DPE_Disabled);
  s_cfgDevice.RF0().RxPath(RX0DPE_Disabled);
  s_cfgDevice.RF1().RxPath(RX1DPE_Disabled);

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
		"* ASR-2300 Calibrate\n"
		"*********************************************************************\n"
		"* This software example provided by Loctronix Corporation (2013) \n"
		"* www.loctronix.com\n"
		"*********************************************************************\n");
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
