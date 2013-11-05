// Name: RxPortToFile.cpp
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

////////////////////////////////////////////////////////////////////////
// Use architecture defines to determine the implementation
////////////////////////////////////////////////////////////////////////
#if defined(LINUX)
    #define A2300_HRT_USE_CLOCK_GETTIME
    #include <ctime>
#elif defined(APPLE)
    #define A2300_HRT_USE_MACH_ABSOLUTE_TIME
    #include <mach/mach_time.h>
#else
    #error "Unknown system; not sure how to get time."
#endif

#include <stdio.h>
#include <math.h>
// #include <time.h>
#include <stdexcept>
#include <vector>

#include <A2300/UsbDevice.h>
#include <A2300/BulkDataPort.h>
#include "DciProperty.h"
#include <A2300/A2300InterfaceDefs.h>
#include <A2300/A2300_Defs.h>

using namespace A2300;

const int A2300_PID = A2300_PRODUCT_ID;
const int A2300_VID = A2300_VENDOR_ID;

Dci_Conversation s_conv;
const size_t  s_ctFrames = 16;
const size_t  s_sizeFrame = 8192;
byte  		  s_Frames[s_ctFrames*s_sizeFrame];


//*************************************************************************
// Forward Declarations.
//*************************************************************************
using namespace A2300;

class RxPortToFile
{

public:
	RxPortToFile() : m_ctPackets(0), m_ctData(0) {}
	int DoRxPortToFile( UsbDevice& device, BulkDataPort& portDci);
	void OnFrameReady( BulkDataPort::TransferContext* pctxt);

private:
	unsigned int  m_ctPackets;
	unsigned int  m_ctData;
};

void WriteHeader( );



//*************************************************************************
// Main Program Entry Point.
//*************************************************************************
int main(int argc, char** argv)
{
	//ASR-2300 Product information.
	WriteHeader();

	Dci_Conversation_Init( &s_conv, 0);


	// Find the list of addresses at the specified VID/PID.
	printf("\n");
	printf (" Enumerating ASR-2300 devices...\n");
	printf ("--------------------------------------\n");

	// Get a list of all attached devices.
	std::vector<int> addrs;
	int ctDevices = UsbDevice::FindAttached(addrs, A2300_VID, A2300_PID);
	if (ctDevices == 0)
	{
		printf(" No devices found.\n\n");
		return -1;
	}

	int addr = addrs.at(0);


	//Create a Driver and PortA2300_PID
	UsbDevice device;
	BulkDataPort portDci(A2300_DciIdc0_EpIn, A2300_DciIdc0_EpOut );

	// Initialize (and Open) the USB driver.
	if( device.Initialize(addr) != 0)
	{
		printf("Error Initializing A2300 Device.");
		return -2;
	}

	// Start USB driver activity (open portDci if not already opened).
	if( device.Start() != 0)
	{
		printf("Error Initializing A2300 Device.");
		return -3;
	}

	// Bind the USB driver to a portDci.
	if( device.BindPort( &portDci) != 0)
	{
		printf("Error Binding to DCI IDC0 data portDci.");
		device.Stop();
		device.Terminate();
		return -4;
	}

	//Open portDci so we can communicate.
	portDci.Open();

	//Get the Version information of the FPGA.
	uint16 id, ver;
	DciProperty dspProps(WCACOMP_HALDEFAULT, &portDci, A2300_WAIT_TIME, &s_conv);
	dspProps.GetProperty<uint16>(0x01, id);
	dspProps.GetProperty<uint16>(0x02, ver);
	printf( "FPGA Logic ID/Ver: %04X-%02X.%02X\n\n", id, (ver>>8), ver&0x00ff);


	RxPortToFile rtpf;
	int retval = 0;
	retval = rtpf.DoRxPortToFile( device, portDci);


	// Unitialize portDci.
	portDci.Close();			//Done with USB
	device.Stop();
	device.Terminate();

	return(retval);
}

/**
 *
 */
int RxPortToFile::DoRxPortToFile( UsbDevice& device, BulkDataPort& portDci)
{
	// Find the list of addresses at the specified VID/PID.
	printf("\n");
	printf (" Opening Data Port...\n");
	printf ("--------------------------------------\n");


	BulkDataPort portData(0x88, PortBase::EP_UNDEF);

	// Bind the USB driver to a portDci.
	if( device.BindPort( &portData) != 0)
	{
		printf("Error Binding to Wca Ports.");
		return -5;
	}


	//3) Path Profile
	DciProperty rfProps(WCACOMP_RF0, &portDci, A2300_WAIT_TIME, &s_conv);

//	//TODO REMOVE
	rfProps.SetProperty<byte>(6,4);
	rfProps.SetProperty<byte>(9,0);
	rfProps.SetProperty<uint32>(7,2400000);
	rfProps.SetProperty<byte>(14,0);
	rfProps.SetProperty<byte>(2,4);
	rfProps.SetProperty<byte>(5,0);
	rfProps.SetProperty<uint32>(3,2400000);
//




	rfProps.SetProperty<byte>(0x0D, 0x01);

	byte profile;
	rfProps.GetProperty<byte>(0x0D, profile);
	printf("Profile: %02X\n", profile);


	//Initialize the properties:

	//1) Host Data Rate
	DciProperty dspProps(WCACOMP_DSP_DDC0, &portDci, A2300_WAIT_TIME, &s_conv);
	dspProps.SetProperty<uint16>(DSP_DDUC_SAMPRATE, 2);
	dspProps.SetProperty<byte>(DSP_DDUC_CTRL, 1);

	//2) If Frequency


	//3) RF Frequency and Gain.
	rfProps.SetProperty<byte>(RFPROP_RXGAIN, 0x05);
	rfProps.SetProperty<uint32>(RFPROP_RXFREQ, 1575420);





	portData.Open();

	//Attach read callback
	portData.ReadTransfer() = BulkDataPort::TransferEvent( static_cast<void*>(this),
			&Delegate<void, RxPortToFile, BulkDataPort::TransferContext*, &RxPortToFile::OnFrameReady>);

	//Initialize the buffers and queue for processing.
	for( size_t i = 0; i < s_ctFrames; i++)
	{
		BulkDataPort::TransferContext* pctxt = portData.CreateReadTransferContext(
				s_Frames + i*s_sizeFrame, s_sizeFrame);

		pctxt->Submit();
	}

	//Process for 10 seconds.
	printf("Starting Run\n");

#if defined(A2300_HRT_USE_CLOCK_GETTIME)

	struct timespec tStart, tEnd;
	clock_gettime(CLOCK_REALTIME, &tStart);
	tEnd = tStart;
	int retval = 0;
	while( retval == 0 and (tEnd.tv_sec < (tStart.tv_sec + 10)))
	{
		retval= device.PollAsynchronousEvents();
		clock_gettime(CLOCK_REALTIME, &tEnd);
	}

#elif defined(A2300_HRT_USE_MACH_ABSOLUTE_TIME)

        mach_timebase_info_data_t timebase_info;
        mach_timebase_info(&timebase_info);
	typedef unsigned long long time_t;

	time_t mach_timebase_multiplier =
	  ((time_t) (1000000000UL) *
	   (time_t) (timebase_info.numer) /
	   (time_t) (timebase_info.denom));

	time_t tStart, tEnd;
	tStart = tEnd = time_t(mach_absolute_time()) * mach_timebase_multiplier;
	int retval = 0;
	while((retval == 0) && (tEnd < (tStart + 10)))
	{
		printf(".");
		fflush(stdout);
		retval= device.PollAsynchronousEvents();
		tEnd = time_t(mach_absolute_time()) * mach_timebase_multiplier;
	}

#endif
	//Set Path Profile to Disabled.
	printf("\nCompleted Run:  Packets = %d, Bytes = %d\n", m_ctPackets, m_ctData);
	fflush(stdout);
	rfProps.SetProperty<uint16>(0x0D, 0x00);



	portData.Close();



	return 0;
}

void RxPortToFile::OnFrameReady( BulkDataPort::TransferContext* pctxt)
{
	//Save data to disk.
	if( pctxt->status == 0)
	{
		m_ctPackets++;
		m_ctData += pctxt->nActualLength;
		if( (m_ctPackets % 488) == 0) putc('.', stdout);
	}
	//Resubmit.
	pctxt->Submit();
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
		   "* ASR-2300 RX portDci to File  \n"
		   "*********************************************************************\n"
		   "*\n"
		   "* This software example provided by Loctronix Corporation (2013) \n"
		   "* www.loctronix.com\n" 
		   "*********************************************************************\n");
}
