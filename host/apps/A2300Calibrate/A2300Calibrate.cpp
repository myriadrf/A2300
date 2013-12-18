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

  // Set the RF Profile component into cache mode
  memset(buff, 0, sizeof(buff));
  msgSize = Dci_ExecuteAction_Init (buff, DCI_MAX_MSGSIZE, WCACOMP_RFPROFILES, RFP_ACTION_CACHEDATA, 0, NULL);
  ctSent = tDci.SendMsg (buff, (size_t) msgSize, true);
  // get ack, and verify
  memset(buff, 0, sizeof(buff));
  ctAck = tDci.ReceiveMsg (buff, DCI_MAX_MSGSIZE);
  if ((ctAck <= 0) || (ctSent != msgSize)) {
    printf ("Warning: Unable to set device into cache mode (%d, %d).\n",
	    ctSent, ctAck);
  }

  for (size_t nn = 0; nn < numRxComponents; ++nn) {

    // calibrate profiles for this Rx component
    struct _RxInfo& tRxInfo = rxInfo[nn];

    // Reset Top-Level Calibration
    memset(buff, 0, sizeof(buff));
    msgSize = Dci_ExecuteAction_Init (buff, DCI_MAX_MSGSIZE, RFACTION_RESETTOPCALIB, 0, 0, NULL);
    ctSent = tDci.SendMsg (buff, (size_t) msgSize, true);
    // get ack, and verify
    memset(buff, 0, sizeof(buff));
    ctAck = tDci.ReceiveMsg (buff, DCI_MAX_MSGSIZE);
    if ((ctAck <= 0) || (ctSent != msgSize)) {
      printf ("Warning: Unable to Reset Top-Level Calibration for Rx component #%ld (%d, %d).\n", nn, ctSent, ctAck);
    }

    // Calibrate Top-Level
    memset(buff, 0, sizeof(buff));
    msgSize = Dci_ExecuteAction_Init (buff, DCI_MAX_MSGSIZE, RFACTION_TOPCALIBRATE, 0, 0, NULL);
    ctSent = tDci.SendMsg (buff, (size_t) msgSize, true);
    // get ack, and verify
    memset(buff, 0, sizeof(buff));
    ctAck = tDci.ReceiveMsg (buff, DCI_MAX_MSGSIZE);
    if ((ctAck <= 0) || (ctSent != msgSize)) {
      printf ("Warning: Unable to Calibrate Top-Level for Rx component #%ld  (%d, %d).\n", nn, ctSent, ctAck);
    }

    ConfigRf& cRf = tRxInfo.cRf;

    // calibrate all Rx paths, one at a time
    for (size_t mm = 0; mm < tRxInfo.numPaths; ++mm) {

      // Set RX Path to Calibrate
      struct _rxPath& thisRxPath = tRxInfo.rxPaths[mm];
      cRf.RxPath(thisRxPath.Value);

      // Reset RX Calibration for this path
      memset(buff, 0, sizeof(buff));
      msgSize = Dci_ExecuteAction_Init (buff, DCI_MAX_MSGSIZE, RFACTION_RESETRXCALIB, 0, 0, NULL);
      ctSent = tDci.SendMsg (buff, (size_t) msgSize, true);
      // get ack, and verify
      memset(buff, 0, sizeof(buff));
      ctAck = tDci.ReceiveMsg (buff, DCI_MAX_MSGSIZE);
      if ((ctAck <= 0) || (ctSent != msgSize)) {
	printf ("Warning: Unable to Reset RX Calibration for Rx%ld path %s (%d, %d).\n", nn, thisRxPath.Name, ctSent, ctAck);
      }

      // Calibrate this path
      memset(buff, 0, sizeof(buff));
      msgSize = Dci_ExecuteAction_Init (buff, DCI_MAX_MSGSIZE, RFACTION_SAVERXPROFILE, 0, 0, NULL);
      ctSent = tDci.SendMsg (buff, (size_t) msgSize, true);
      // get ack, and verify
      memset(buff, 0, sizeof(buff));
      ctAck = tDci.ReceiveMsg (buff, DCI_MAX_MSGSIZE);
      if ((ctAck <= 0) || (ctSent != msgSize)) {
	printf ("Warning: Unable to Calibrate Rx%ld path %s (%d, %d).\n", nn, thisRxPath.Name, ctSent, ctAck);
      }
    }
  }

  // Save Cached Changes
  memset(buff, 0, sizeof(buff));
  msgSize = Dci_ExecuteAction_Init (buff, DCI_MAX_MSGSIZE, WCACOMP_RFPROFILES, RFP_ACTION_SAVECHANGES, 0, NULL);
  ctSent = tDci.SendMsg (buff, (size_t) msgSize, true);
  // get ack, and verify
  memset(buff, 0, sizeof(buff));
  ctAck = tDci.ReceiveMsg (buff, DCI_MAX_MSGSIZE);
  if ((ctAck <= 0) || (ctSent != msgSize)) {
    printf ("Warning: Unable to set device into cache mode (%d, %d).\n",
	    ctSent, ctAck);
  }

  return 0;

}

#if 0
Note most of them use the DCI WCA Execute Action Message (Action Msg).
 
1.       Set the RF Profile component into cache mode.  Send Action Msg to component WCACOMP_RFPROFILES [0x94] action ID = 0.
2.       For each RF Component ( 0x81, 0x82) Do the following:
a.       Reset Top-Level Calibration: Send Action Msg (id=0x87)
b.      Calibrate Top-Level: Send Action Msg( id =0x84)
c.       For each RX RF Profile
 i.      Set RX Profile Path Property (propid=0x0D),  see ASR-2300 Comm Interface Specs for allowed values or  A2300InterfaceDefs.h
 ii.      Reset RX Calibration: Send Action Msg (id=0x88)
 iii.      Calibrate Top-Level: Send Action Msg ( id =0x85)
 iv.      Wait for Log Message Response, Info is ok, Err means it didn’t work.
3.       Save Cached Changes by sending Action Msg to component 0x94 action ID=0x02.
#endif

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
