#include "DataStreamLogger.h"

#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Threading/Thread.hpp>

using namespace std;


/******************************************************************
* Declarations
*****************************************************************/

ARGPARSER_BEGIN_MAP( s_argsLogger ) 
	ARGPARSER_PARAM( "filename", "", "Binary data output file name.")
	ARGPARSER_VAR( "path", ArgParser::Entry::DOUBLE, "pcs",
				  "Receiver path to use. Choose from \"gpsant\", \"gpsext\", \"pcs\","
				  "\"wideband0\", \"widebandlf0\", \"uhf\", \"ism_ant\", \"ism_ext\", \"wideband1\", \"widebandlf1\" ")
	ARGPARSER_VAR( "freq", ArgParser::Entry::DOUBLE, "1970", "Receiver center frequency in MHz. Valid range is between 300 MHz to 3.8 MHz")
	ARGPARSER_VAR( "gain", ArgParser::Entry::LONG,   "9",    "Receiver gain in dB. Valid range is between 0 dB to 60 dB")
	ARGPARSER_VAR( "bw",   ArgParser::Entry::DOUBLE, "5",    "Receiver front-end bandwidth in MHz. Valid range is 1.5 MHz to 28 MHz.")
	ARGPARSER_VAR( "rate", ArgParser::Entry::DOUBLE, "2",    "Host sample rate in MS/s. Valid range is 3.90625 kS/s to 32 MS/s.")
	ARGPARSER_SWITCH("speedtest", false, "Switch test I/O speed for overflow, does not write to disk.")
ARGPARSER_END_MAP

/**
* Define standard structure for RF path definitions.
*/
struct RfPathDef {
	const char* szPath;
	const char* szDescr;
	byte		epRx;
	byte		idRf;
	byte		idDdc;
	byte        idPath;
}; 
static const RfPathDef s_pathDef[] = {
  { "gpsant",	"GPS L1 Internal Antenna",	0x88,  WCACOMP_RF0, WCACOMP_DSP_DDC0, RX0DPE_GpsL1Int },
  { "gpsext",	"GPS L1 External Input",	0x88,  WCACOMP_RF0, WCACOMP_DSP_DDC0, RX0DPE_GpsL1Ext },
  { "pcs",		"PCS Input",				0x88,  WCACOMP_RF0, WCACOMP_DSP_DDC0, RX0DPE_PcsExt },
  { "wideband0","RF0 Wideband Input",		0x88,  WCACOMP_RF0, WCACOMP_DSP_DDC0, RX0DPE_Wideband },
  { "widebandlf0","RF0 Wideband Low Freq. Input",		0x88,  WCACOMP_RF0, WCACOMP_DSP_DDC0, RX0DPE_WidebandLowFreq },
  { "uhf",		"UHF External Input",		0x89,  WCACOMP_RF1, WCACOMP_DSP_DDC1, RX1DPE_UhfExt },
  { "ism_ant",	"ISM 2.4 GHz Antenna",		0x89,  WCACOMP_RF1, WCACOMP_DSP_DDC1, RX1DPE_IsmInt },
  { "ism_ext",	"ISM 2.4 GHz Ext. Input",	0x89,  WCACOMP_RF1, WCACOMP_DSP_DDC1, RX1DPE_IsmExt },
  { "wideband1","RF1 Wideband Input",		0x89,  WCACOMP_RF1, WCACOMP_DSP_DDC1, RX1DPE_Wideband },
  { "widebandlf1","RF1 Wideband Low Freq. Input",		0x89,  WCACOMP_RF1, WCACOMP_DSP_DDC1, RX1DPE_WidebandLowFreq }

};
const static int COUNT_PATHDEFS = 10;

/******************************************************************
* DataStreamLogger Implementation.
*****************************************************************/

DataStreamLogger::DataStreamLogger(size_t bytesPerSample, size_t sizeFrame, size_t iMaxFrames, char chTick)
: m_idxRfPath(-1), m_byteGain(0), m_dFreqRf(0),
	m_dBandwidth( 28), m_dSampRate(2), 
	m_pDevice(NULL), m_pRf(NULL), m_BytesPerSample(bytesPerSample),
	m_sizeFrame(sizeFrame), m_iMaxBuffs(iMaxFrames), m_ctFramesProcessed(0),
	m_totalFramesToProcess(0), m_framesPerSec(0), m_chTick(chTick), m_pbuff(NULL), m_file(NULL),
	m_bIsRunning(false), m_bSpeedTest(false)
{
}

DataStreamLogger::~DataStreamLogger(void)
{
	Stop();
	Terminate();
}


void DataStreamLogger::RegisterArgs( string sId, ArgParser& args)
{
	m_sId = sId;
	args.AddMapEntries(s_argsLogger,"global", m_sId.c_str());

}

int DataStreamLogger::Init( ArgParser& args, ConfigDevice* pDevice)
{
	m_pDevice   = pDevice;
	m_sFilename	= args.GetString(m_sId + ".filename");


	//Get RF path definition.
	std::string sPath = args.GetString(m_sId + ".path");
	int idxConfig = 0;
	for( ; idxConfig < COUNT_PATHDEFS; idxConfig++)
	{
		if( sPath.compare( s_pathDef[ idxConfig].szPath) == 0)
			break;
	}
	if( idxConfig == COUNT_PATHDEFS)
	{
		printf( "Error - Invalid path variable %s\n\n", sPath.c_str());
		return -1;
	}

	m_idxRfPath  = idxConfig;
	m_dFreqRf    = args.GetDouble(m_sId + ".freq");
	m_byteGain   = (byte) args.GetLong(m_sId + ".gain");
	m_dBandwidth = args.GetDouble(m_sId + ".bw");
	m_dSampRate  = args.GetDouble(m_sId + ".rate");
	m_bSpeedTest = args.GetBool(m_sId + ".speedtest");


	// Open the specified file for writing.
	m_file = fopen(m_sFilename.c_str(), "wb");
	if (!m_file) {
		printf("\nError: filename ('%s') cannot be "
			"accessed.\n", m_sFilename.c_str());
		return -2;
	}

	//Initialize the USB Interface.
	const RfPathDef& rpath	= s_pathDef[m_idxRfPath];
	bool      bIsRf0	= rpath.idRf == WCACOMP_RF0;
	m_pPort				= new BulkDataPort(rpath.epRx, PortBase::EP_UNDEF);
	UsbDevice& device	= m_pDevice->Device();
	if( device.BindPort( m_pPort) != 0)
	{
		printf("Error binding to USB endpoint %02x (%s)", rpath.epRx, rpath.szPath);
		return -3;
	}

	//Launch the write thread
	m_bIsRunning = true;
	A2300::Threading::Thread::ThreadCreate(true,&WriteDataThreadFunc, this);

	//Clear out any DCI command messages.
	m_pDevice->Dci0Transport().ClearReceiveQueue();

	// Configure the RF Front-end profile, gain, frequency, and Bandwidth.
	m_pRf				= (bIsRf0) ? &(m_pDevice->RF0()) : &(m_pDevice->RF1());
	m_pRf->RxPath( rpath.idPath);
	m_byteGain			= m_pRf->RxGain( m_byteGain);
	m_dFreqRf			= m_pRf->RxFrequency( (uint32)(m_dFreqRf*1000 + 0.5))/1000.0;
	m_dBandwidth		= ConfigRf::BandwidthToMHz(m_pRf->RxBandwidth( ConfigRf::BandwidthFromMHz( m_dBandwidth )));

	// Reset the DDC Component so no data is in cache.
	m_pDduc				= new ConfigDduc(rpath.idDdc, m_sId + ".ddc", m_pDevice, true);
	m_pDevice->Components().push_back(m_pDduc);
	m_pDduc->Reset();
	m_pDduc->Mode( ConfigDduc::Normal | ConfigDduc::SynchronizeReset);  //Synchronizes FIFOS with other streams.
	m_dSampRate = m_pDduc->HostSamplingRate( m_dSampRate*1.0e6, true)/1.0e6;
	//TODO Set the baseband DDC offset.

	// Open USB Ports, attach callbacks,  and initialize Transfer Buffers Open the raw port, attach callback function to receive raw data as it comes in.
	m_pPort->Open(); 
	m_pPort->ReadTransfer() = BulkDataPort::TransferEvent(this, &DataStreamLogger::OnFrameReady);

	//Add buffers
	m_pbuff = new byte[m_iMaxBuffs*m_sizeFrame];
	for( size_t nn = 0; nn < m_iMaxBuffs; ++nn)
	{
		BulkDataPort::TransferContext* pctxtRaw = m_pPort->CreateReadTransferContext(m_pbuff + (nn*m_sizeFrame), m_sizeFrame);
		pctxtRaw->Submit();
		m_listContext.push_back( pctxtRaw);
	}



	//Ready to go.
	return 0;
}

void DataStreamLogger::DisplayConfiguration()
{
	const RfPathDef& rpath	= s_pathDef[m_idxRfPath];
	bool      bIsRf0	= rpath.idRf == WCACOMP_RF0;

	printf("\n");
	printf("%s Configuration", m_sId.c_str());
	printf("--------------------------------------\n");
	printf("RF Component:  %s (id=%02Xh, ep=%02Xh)\n", (bIsRf0)? "RF0" : "RF1", rpath.idRf, rpath.epRx);
	printf("RX Profile:    %s (id=%02Xh)\n", rpath.szDescr, rpath.idPath);
	printf("RX Gain:       %d dB\n",	  m_byteGain);
	printf("RX Frequency:  %0.3f MHz\n",  m_dFreqRf);
	printf("RX Bandwidth:  %0.3lf MHz\n", m_dBandwidth);
	printf("Host Rate:     %0.3lf MHz\n", m_dSampRate);
	if( m_bSpeedTest)
		printf("** Speed Test Mode Enabled\n");

}

int DataStreamLogger::Start(size_t msecDur, bool bIsMaster)
{
	//Calculate the total frames.
	double sampsPerFrame = (double) (m_sizeFrame / m_BytesPerSample);
	double framesPerSec = m_dSampRate*1e6 / sampsPerFrame;
	m_totalFramesToProcess = (size_t)( double(msecDur/1000)*framesPerSec);
	m_framesPerSec = (size_t) (framesPerSec);

	//Only enable if master channel.
	if( bIsMaster)
	{
		m_pDduc->Clear();
		m_pDduc->Enable(true);
	}
	return 0;
}

int DataStreamLogger::CheckStatus()
{
	return !(m_ctFramesProcessed <=  m_totalFramesToProcess) ;
}

void DataStreamLogger::Stop()
{
	
	m_bIsRunning = false;
	if( m_pPort!= NULL && m_pDduc != NULL)
	{
		m_pDduc->Reset(); // Reset disables the channel so no need to call Enable(0).


		bool bIsRf0	= s_pathDef[m_idxRfPath].idRf == WCACOMP_RF0;

		//Clear out any DCI command messages.
		m_pDevice->Dci0Transport().ClearReceiveQueue();

		// Configure the RF Front-end profile, gain, frequency, and Bandwidth.
		m_pRf->RxPath( bIsRf0 ? (byte)RX0DPE_Disabled : (byte)RX1DPE_Disabled );

	}
}
void DataStreamLogger::Terminate()
{
	if( m_pPort == NULL || m_pDduc == NULL)
		return;

	//Wait for the transfer cancellations to complete.
	time_t ttCur, ttEnd;
	time(&ttCur);
	ttEnd = ttCur + 2;

#ifdef HAVE_LIBUSB
	//We must provide events in LIBUSB.
	int completed = 0;
	while( !completed && (ttCur < ttEnd))
	{
		m_pDevice->Device().PollAsynchronousEvents(0.010, completed);
		time(&ttCur);
	}
#endif

	BulkDataPort::TransferContextList::iterator iter;
	for( iter = m_listContext.begin(); iter != m_listContext.end(); iter++)
		(*iter)->Cancel(1000);

	// Close the Data Port
	m_pPort->Close();

	// Destroy all the transfer contexts.
	for( iter = m_listContext.begin(); iter != m_listContext.end(); iter++)
		(*iter)->Destroy(); // All done.
	m_listContext.clear();

	delete m_pPort; m_pPort = NULL;
	delete m_pDduc; m_pDduc = NULL;
	delete m_pbuff; m_pbuff = NULL;

	//close the file -- note this was opened in ParseOptions,
	if (fclose(m_file) != 0) {
		printf("\nError %d closing file '%s'.\n", errno, m_sFilename.c_str());
	}
}


void DataStreamLogger::OnFrameReady (void* pObj, BulkDataPort::TransferContext* pctxt)
{

	DataStreamLogger* pThis = static_cast<DataStreamLogger*>(pObj);
	pThis->m_queueFramesToWrite.Push( pctxt);
}


void* DataStreamLogger::WriteDataThreadFunc(void* arg)
{
	DataStreamLogger* pThis = static_cast<DataStreamLogger*>(arg);

	while(pThis->m_bIsRunning)
	{
		BulkDataPort::TransferContext* pctxt;

		if( pThis->m_queueFramesToWrite.Pop(&pctxt,100))
		{
			if( pctxt->status == 0 && !pThis->CheckStatus()  && !pThis->m_bSpeedTest )
			{
				// Save data to disk
				size_t nWritten = fwrite (pctxt->bufFrame, 1, pctxt->nActualLength, pThis->m_file);
				if (nWritten != pctxt->nActualLength)
				{
					printf ("\nWarning: Some data failed to write to the selected file ('%s').\n", pThis->m_sFilename.c_str());
				}

				pThis->m_ctFramesProcessed ++;
				if( pThis->m_ctFramesProcessed % pThis->m_framesPerSec == 0)
				{
					putc( pThis->m_chTick, stdout);
					fflush(stdout);
				}
			}

			//Resubmit.
			pctxt->Submit();
		}
	}
	return NULL;

}
