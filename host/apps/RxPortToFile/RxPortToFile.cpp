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

#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdexcept>
#include <vector>

#include <A2300/ConfigDevice.h>
using namespace A2300;


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
	RxPortToFile() : m_ctPackets(0), m_ctData(0), m_bRunning(false) {}
	int Run();
	int DoRxPortToFile( );
	void OnFrameReady( BulkDataPort::TransferContext* pctxt);


private:
	ConfigDevice  m_config;
	unsigned int  m_ctPackets;
	unsigned int  m_ctData;
	bool 		  m_bRunning;
};

void WriteHeader( );



//*************************************************************************
// Main Program Entry Point.
//*************************************************************************
int main(int /*argc*/, char** /*argv*/)
{
	//ASR-2300 Product information.
	WriteHeader();
	RxPortToFile rptf;
	return rptf.Run();
}

int RxPortToFile::Run()
{
	try
	{
		// Find the list of addresses at the specified VID/PID.
		printf("\n");
		printf (" Enumerating ASR-2300 devices...\n");
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

	//Execute the bulk data transfer program.
	int retval = DoRxPortToFile( );

	m_config.Detach();

	return(retval);
}

/**
 *
 */
int RxPortToFile::DoRxPortToFile( )
{
	// Find the list of addresses at the specified VID/PID.
	printf("\n");
	printf (" Opening Data Port...\n");
	printf ("--------------------------------------\n");

	BulkDataPort portData(0x88, PortBase::EP_UNDEF);

	UsbDevice& device  = m_config.Device();

	// Bind the USB driver to a portDci.
	if( device.BindPort( &portData) != 0)
	{
		printf("Error Binding to Wca Ports.");
		return -5;
	}

	TransportDci& td = m_config.Dci0Transport();

	//3) Path Profile

	//TODO REMOVE
//	td.SetProperty<byte>(WCACOMP_RF0, 6, 4);
//	td.SetProperty<byte>(WCACOMP_RF0, 9, 0);
//	td.SetProperty<uint32>(WCACOMP_RF0, 7, 2400000);
	td.SetProperty<byte>(WCACOMP_RF0, 14,0);
	td.SetProperty<byte>(WCACOMP_RF0, 2,4);
	td.SetProperty<byte>(WCACOMP_RF0, 5,0);
//	td.SetProperty<uint32>(WCACOMP_RF0, 3,2400000);


	td.SetProperty<byte>(WCACOMP_RF0, 0x0D, 0x01);

	byte profile;
	td.GetProperty<byte>(WCACOMP_RF0, 0x0D, profile);
	printf("Profile: %02X\n", profile);


	//Initialize the properties:

	//1) Host Data Rate
	td.SetProperty<uint16>(WCACOMP_DSP_DDC0, DSP_DDUC_SAMPRATE, 2);

	//2) If Frequency

	//3) RF Frequency and Gain.
	td.SetProperty<byte>(WCACOMP_DSP_DDC0, RFPROP_RXGAIN, 0x05);
	td.SetProperty<uint32>(WCACOMP_DSP_DDC0, RFPROP_RXFREQ, 1575420);


	//4) Open the port and prepare it for reading.
	portData.Open();

	//Attach read callback
	portData.ReadTransfer() = BulkDataPort::TransferEvent( static_cast<void*>(this),
			&Delegate<void, RxPortToFile, BulkDataPort::TransferContext*, &RxPortToFile::OnFrameReady>);

	//Initialize the buffers and queue for processing.
	m_bRunning = true;
	BulkDataPort::TransferContextList listContext;
	for( size_t i = 0; i < s_ctFrames; i++)
	{
		BulkDataPort::TransferContext* pctxt = portData.CreateReadTransferContext(
				s_Frames + i*s_sizeFrame, s_sizeFrame);

		pctxt->Submit();

		listContext.push_back( pctxt);
	}

	//5) Turn on the DDUC Port to pump data.
	td.SetProperty<byte>(WCACOMP_DSP_DDC0, DSP_DDUC_CTRL, 1);


	//6) Process for 10 seconds.
	printf("Starting Run\n");
	int retval = 0;
	time_t ttCur, ttEnd;
	time(&ttCur);
	ttEnd = ttCur + 10;

	while( retval == 0 && ttCur < ttEnd)
	{
#if defined(HAVE_LIBUSB)
		retval= device.PollAsynchronousEvents();
#elif defined(WIN32)
		//Windows implementation does not have a PollAsynchronous Data method.
		//Instead, we wait for events on each port.
		BulkDataPort::TransferContext& ctxt = portData.WaitForReadTransferEvent( 1000);
		retval = ctxt.status;
#endif
		time(&ttCur);
	}

	if( retval != 0) printf("USB error occurred transfer stopped.\n");

	//8) Turn off the DDC Port and reset
	td.SetProperty<byte>(WCACOMP_DSP_DDC0, DSP_DDUC_CTRL, 0x2);
	td.SetProperty<byte>(WCACOMP_DSP_DDC0, DSP_DDUC_CTRL, 0x0);

	//9) Set Path Profile to Disabled.
	printf("\n\nCompleted Run:  Packets = %d, Bytes = %d\n", m_ctPackets, m_ctData);
	fflush(stdout);

	//10) Disable the RF Communications
	td.SetProperty<uint16>(WCACOMP_RF0, 0x0D, 0x00);


	//11) Cancel all transfer operations.
	BulkDataPort::TransferContextList::iterator iter;
	for( iter = listContext.begin(); iter != listContext.end(); iter++)
		(*iter)->Cancel();

	//Wait for the transfer cancellations to complete.
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

void RxPortToFile::OnFrameReady( BulkDataPort::TransferContext* pctxt)
{
	//Save data to disk.
	if( pctxt->status == 0)
	{
		m_ctPackets++;
		m_ctData += pctxt->nActualLength;
		if( (m_ctPackets % 488) == 0)
		{
			putc('.', stdout);
			fflush(stdout);
		}

		//Resubmit.
		pctxt->Submit();

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
		   "* ASR-2300 RX Port to File  \n"
		   "*********************************************************************\n"
		   "*\n"
		   "* This software example provided by Loctronix Corporation (2013) \n"
		   "* www.loctronix.com\n" 
		   "*********************************************************************\n");
}






















