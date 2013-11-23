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
	int DoRxPortToFile(
			byte idComponent, byte rxProfile,
			byte rxGain, uint32 rxFreq,
			byte rxBandwidth, uint16 hostSampRate  );
	void OnFrameReady( BulkDataPort::TransferContext* pctxt);


private:
	ConfigDevice  	m_config;
	ulong  			m_ctPackets;
	ulong  			m_ctData;
	bool 		 	m_bRunning;
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

	//Execute the bulk data transfer program (default to PCS Band, 9 dB Gain, 1950 MHz, 2.5 MHz bandwidth, 2 MHz sampling).
	//TODO make configurable from command line.
	int retval = DoRxPortToFile(WCACOMP_RF0, RX0DPE_PcsExt, 3, 1950000, 13, 8 );

	m_config.Detach();

	return(retval);
}

/**
 * Function configures the ASR-2300 to receive a specified frequency and then streams the data to the host
 * via BulkDataPort interface.
 */
int RxPortToFile::DoRxPortToFile(
		byte idComponent, byte rxProfile,
		byte rxGain, uint32 rxFreq,
		byte rxBandwidth, uint16 hostSampRate  )
{
	const static char* s_rx0_pathsDefault[] = {"Disabled", "GPS L1 Int.", "GPS L1 Ext.", "PCS Band Ext.", "Wideband"};
	const static char* s_rx1_pathsDefault[] = {"Disabled", "UHF Ext.", "ISM Int.", "ISM Ext.", "Wideband"};

	//Select endpoints, components, and path definitions based on which RF front-end was specified.
	bool  bIsRf0   = (idComponent == WCACOMP_RF0);
	byte  epRx     = (bIsRf0) ? 0x88 : 0x98;
	byte  idDdc    = (bIsRf0) ? WCACOMP_DSP_DDC0 : WCACOMP_DSP_DDC1;
	pcstr* szPaths = (bIsRf0) ? s_rx0_pathsDefault : s_rx1_pathsDefault;
	float fSampFreq = 32.0e6f / hostSampRate/2;
	double dt = 10.0;
	double framesPerSecond = fSampFreq*4 / s_sizeFrame; //4 bytes per sample
	ulong totalFrames = (ulong)ceil(framesPerSecond*dt);

	TransportDci& td = m_config.Dci0Transport();  //Get the dci interface so we can send messages directly.

	//Display some interesting information about the configuration.
	printf("\n");
	printf(" Opening Data Port...\n");
	printf("--------------------------------------\n");
	printf("RF Component:  %s (id=%02Xh, ep=%02Xh)\n", (bIsRf0)? "RF0" : "RF1", idComponent, epRx);
	printf("RX Profile:    %s (id=%02Xh)\n", szPaths[rxProfile & 0xF], rxProfile);
	printf("RX Gain:       %d dB\n", rxGain*3);
	printf("RX Frequency:  %0.3f MHz\n", rxFreq/1000.0F);
	printf("RX Bandwidth:  -- MHz (id=%02Xh)\n", rxBandwidth);
	printf("Host Rate:     %0.3f MHz\n", fSampFreq/1.0e6f);
	printf("Duration:      %lf (%lu frames)\n", dt, totalFrames);

	// 1) Bind the Bulk Data Port to the specified endpoint.
	BulkDataPort portData(epRx, PortBase::EP_UNDEF);
	UsbDevice& device  = m_config.Device();
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
	td.SetProperty<byte>(idComponent,   RFPROP_RXPATH, rxProfile );
	td.SetProperty<byte>(idComponent,   RFPROP_RXGAIN, rxGain);
	td.SetProperty<uint32>(idComponent, RFPROP_RXFREQ, rxFreq);
	td.SetProperty<uint32>(idComponent, RFPROP_RXBANDWIDTH, rxBandwidth);

	//4) Open the port, attach callback function to receive data as it comes in.
	portData.Open();
	portData.ReadTransfer() = BulkDataPort::TransferEvent( static_cast<void*>(this),
			&Delegate<void, RxPortToFile, BulkDataPort::TransferContext*, &RxPortToFile::OnFrameReady>);

	//5) Initialize the buffers and queue for processing.
	m_bRunning = true;
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
	td.SetProperty<uint16>(idDdc, DSP_DDUC_SAMPRATE, hostSampRate);
	//TODO CONFIGURE IF Frequency.
	td.SetProperty<byte>(idDdc, DSP_DDUC_CTRL, DSP_DDUC_CTRL_ENABLED);


	//7) Process for 10 seconds.

	printf("--> Starting Run\n");


	int retval = 0;
	while( retval == 0 && m_ctPackets < totalFrames)
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
	printf("\n\n--> Completed Run:  Packets = %ld, Bytes = %ld\n", m_ctPackets, m_ctData);
	fflush(stdout);

	//10) Disable the RF Communications
	td.SetProperty<byte>(idComponent,  RFPROP_RXPATH, bIsRf0 ? (byte)RX0DPE_Disabled : (byte)RX1DPE_Disabled );

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






















