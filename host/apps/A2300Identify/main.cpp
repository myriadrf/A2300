// Name: main.cpp
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

#ifdef WIN32
#include <Windows.h>
#endif

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdexcept>
#include <vector>

#include <A2300/UsbDevice.h>
#include <A2300/BulkDataPort.h>
#include "Dci/DciMsg.h"
#include "Dci/DciConversation.h"
#include "Dci/InfrastructureMsgs.h"
#include "Dci/WcaMsgs.h"
#include "Dci/StandardMsgs.h"

// Defines.
#define DCI_MESSAGE_BUFF_COUNT 10

using namespace A2300;

const int A2300_PID = A2300_PRODUCT_ID;
const int A2300_VID = A2300_VENDOR_ID;

//*************************************************************************
// Forward Declarations.
//*************************************************************************

void RunTest(int vector_id, int product_id);

void IdentifyDevice(int address);
void QueryProperties(A2300::BulkDataPort& pPort, int addr);

bool WaitForReceivedMsg( A2300::BulkDataPort& pPort, int addr, int msecWait);
void WriteHeader( );

//*************************************************************************
// Main Program Entry Point.
//*************************************************************************
int main(int argc, char** argv)
{
	//ASR-2300 Product information.
	WriteHeader();

	//Testing...
	RunTest(A2300_VID, A2300_PID);
	return(0);
}

/**
 * Test to Open a USB device, bind to it, and access information on the
 * ASR-2300
 */
void RunTest(int vendor_id, int product_id)
{
	// Find the list of addresses at the specified VID/PID.
	printf("\n");
	printf (" Enumerating ASR-2300 devices...\n");	 
	printf ("--------------------------------------\n");

	// Get a list of all attached devices.
	std::vector<int> addrs;
	int ctDevices = A2300::UsbDevice::FindAttached(addrs, vendor_id, product_id);
	if (ctDevices == 0)
	{
		printf(" No devices found.\n\n");
		return;
	}

	// Display devices and properties.
	for( int i = 0; i<ctDevices; i++)
	{
		int addr = addrs.at(i);
		if( addr >= 0 )
		{
			//Query specific device and return properties.
			IdentifyDevice(addr);
		}
	}
}


//*************************************************************************
// Processing Functions.
//*************************************************************************

/**
 * <summary>
 * Writes the application header information to the standard output.
 * </summary>
 */
void WriteHeader( )
{
	printf("*********************************************************************\n"
		   "* ASR-2300 Identification and Version Information Tool \n"
		   "*********************************************************************\n"
		   "*\n"
		   "* This software example provided by Loctronix Corporation (2013) \n"
		   "* www.loctronix.com\n" 
		   "*********************************************************************\n");
}

/**
* Routine waits for a response from the devices and displays the information.
* This routine follows the DCI protocol to send/receive infromation to the ASR-2300.
*/
bool WaitForReceivedMsg( A2300::BulkDataPort& port, int msecWait)
{
	byte buff[DCI_MAX_MSGSIZE];

	//Read the next DCI message from the port.
	int ctRead = port.Read( buff, DCI_MAX_MSGSIZE, msecWait);
	if( ctRead == LIBUSB_ERROR_TIMEOUT)
	{
		printf("ERR - Timeout, no response from device.\n");
		return false;
	}
	else
	{
		printf("ERR - Port read operation returned error code: %d\n", ctRead);
		return false;
	}

	//Process the received DCI message.
	bool bHandled = false;
	switch( Dci_Hdr_MessageId( (Dci_Hdr *)buff) )
	{
	case 0x0200:		// Handle device identification Messages.
		{
			Dci_IdentifyDevice* pid = (Dci_IdentifyDevice*) buff;
			printf("  ID:\t\t\t%s\n  S/N:\t\t\t%s\n  Model:\t\t%s\n", pid->DeviceId, pid->SerialNumber, pid->Model);
			bHandled = true;
		}
		break;
	case 0x2002:		// Handle version Messages.
		{
			Dci_VersionInfo* pinfo = (Dci_VersionInfo*) buff;
			printf("  CPU Ver:\t\t%04d.%04d.%04d Rev:%04d\n", pinfo->VerMajor, pinfo->VerMinor, pinfo->VerMaintenance, pinfo->Revision);
			printf("  FX3 Ver:\t\t-Not implemented-\n");
			bHandled = true;
		}
		break;
	case 0x2101:		// Property Response
		{
			Dci_TypedProperties* ptp = (Dci_TypedProperties*) buff;
			int ctProps = ptp->ctProperties;
			Dci_Property *pProp = Dci_TypedProperties_GetProperties(ptp);

			for(int i=0; i< ctProps; i++)
			{
				switch(pProp[i].idprop)
				{
				case(0x1):			// ID
					printf("  FPGA ID:\t\t%04X\n", pProp[i].value.vUint16);
					break;
				case(0x2):			// Version
					printf("  FPGA Version:\t\t%04X\n", pProp[i].value.vUint16);
					break;
				default:
					printf("  Unknown property ID: %d\n", pProp[i].idprop);
					break;
				}
			}
			bHandled = true;
		}
		break;
	case Dci_MessageError_Id:	// Unknown.
		{
			Dci_MessageError* perr = (Dci_MessageError*) buff;
			printf( "  Unrecognized message: category=%02X, id=%02X\n", perr->UnrecognizedCategoryId, perr->UnrecognizedTypeId);
			bHandled = true;
		}
		break;
	case Dci_DebugMsg_Id:
		{
			Dci_DebugMsg* pdbg= (Dci_DebugMsg*) buff;
			const pcstr szLogType[] = { "ERR ", "WARN", "INFO", "DEBUG"};
			printf( "  %s ( src=%02X): %s\n", szLogType[ pdbg->status], pdbg->src, Dci_DebugMsg_Message(pdbg));
			break;
		}
	default: //Handle All other messages.
		{
			Dci_Hdr* phdr = (Dci_Hdr *)buff;
			printf( "Received unhandled message: category=%02X, id=%02X\n", phdr->idCategory, phdr->idType);
		}
		break;
	}
	return(bHandled);
}

/**
 * Open USB Device and Query for ASR-2300 Identification Information.
 */
void IdentifyDevice(int usbAddress)
{
	//Create a Driver and PortA2300_PID
	A2300::UsbDevice drv;
	A2300::BulkDataPort port(A2300_DciIdc0_EpIn, A2300_DciIdc0_EpOut );

	// Initialize (and Open) the USB driver.
	if( drv.Initialize(usbAddress) != 0)
	{
		printf("Error Initializing A2300 Device.");
		return;
	}

	// Start USB driver activity (open port if not already opened).
	if( drv.Start() != 0)
	{
		printf("Error Initializing A2300 Device.");
		return;
	}

	// Bind the USB driver to a port.
	if( drv.BindPort( &port) != 0)
	{
		printf("Error Binding to DCI IDC0 data port.");
		drv.Stop();
		drv.Terminate();
		return;
	}

	//Open Port so we can communicate.
	port.Open();

	// Test Port Interface by reading properties.
	QueryProperties( port, usbAddress );

	// Test a stream.
	//uhd::stream_args_t stream_args("fc32");
	// uhd::rx_streamer::sptr rx_stream = a2300->get_rx_stream(stream_args);

	// Unitialize port.
	port.Close();			//Done with USB
	drv.Stop();
	drv.Terminate();
}

/**
 * Display Device Properties 
 */
void QueryProperties(A2300::BulkDataPort& port, int usbAddress)
{
	// Message response timeout (msec)
	const int timeOutMs = 100;

	// Declare a buffer to hold messages.
	byte buff[DCI_MAX_MSGSIZE];

	try
	{
		printf( " [USB Address: %d] \n", usbAddress);
		//Create a DCI Conversation, Assumes little endian.
		Dci_Conversation conv;
		Dci_Conversation_Init( &conv, 0);


		//Query Identify Device.
		int len = Dci_IdentifyDeviceQuery_Init(buff);
		Dci_Conversation_PrepareMessageHdr( &conv, (Dci_Hdr*) buff, false);
		port.Write( buff, len, 0);
		if( !WaitForReceivedMsg( port, timeOutMs ) )
		{
			printf( " Device identification: no response received.\n");
		}

		// Query Version
		len = Dci_VersionInfoQuery_Init(buff);
		Dci_Conversation_PrepareMessageHdr( &conv, (Dci_Hdr*) buff, false);
		port.Write( buff, len, 0);
		if( !WaitForReceivedMsg( port, timeOutMs ) )
		{
			printf( " Version identification: no response received.\n");
		}

		// Request FPGA firmware ID & version information.
		byte ids[] = {0x1,0x2};
		len = Dci_TypedPropertiesQuery_Init(buff, DCI_MAX_MSGSIZE, 0x00, 2, ids );
		Dci_Conversation_PrepareMessageHdr( &conv, (Dci_Hdr*) buff, false);
		port.Write( buff, len, 0);
		if( !WaitForReceivedMsg( port, timeOutMs ) )
		{
			printf( " FPGA ID & Version: no response received.\n");
		}		
	}
	catch(std::exception &e)
	{
		printf("exception %s\n", e.what());
	}

	printf( "\n"); 
}
