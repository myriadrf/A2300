#include "DataStreamWriter.h"

#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



using namespace std;


/******************************************************************
* Declarations
*****************************************************************/

ARGPARSER_BEGIN_MAP( s_argsLogger ) 
//	ARGPARSER_PARAM( "filename", "", "Binary data output file name.")
	ARGPARSER_VAR( "path",		ArgParser::Entry::DOUBLE, "wideband0",	"Transmitter path to use. Choose from \"wideband0\", \"ism_int\", \"ism_ext\", \"wideband1\"")
	ARGPARSER_VAR( "freq",		ArgParser::Entry::DOUBLE, "2420",	"Receiver center frequency in MHz. Valid range is between 300 MHz to 3.8 MHz")
	ARGPARSER_VAR( "if-freq",	ArgParser::Entry::DOUBLE, "0",		"Digital Upconverter Frequency Offset in Hz")
	ARGPARSER_VAR( "gain",		ArgParser::Entry::LONG,   "9",		"Receiver gain in dB. Valid range is between 0 dB to 60 dB")
	ARGPARSER_VAR( "bw",		ArgParser::Entry::DOUBLE, "3",		"Receiver front-end bandwidth in MHz. Valid range is 1.5 MHz to 28 MHz.")
	ARGPARSER_VAR( "rate",		ArgParser::Entry::DOUBLE, "2",		"Host sample rate in MS/s. Valid range is 3.90625 kS/s to 32 MS/s.")
ARGPARSER_END_MAP

/**
* Define standard structure for RF path definitions.
*/
struct RfPathDef {
	const char* szPath;
	const char* szDescr;
	byte		epTx;
	byte		idRf;
	byte		idDuc;
	byte        idPath;
}; 
static const RfPathDef s_pathDef[] = {
  { "wideband0",	"RF0 Wideband Output",	A2300_Port0_Out,  WCACOMP_RF0, WCACOMP_DSP_DUC0, TX0DPE_Wideband },
  { "ism_int",		"ISM Internal Antenna",	A2300_Port1_Out,  WCACOMP_RF1, WCACOMP_DSP_DUC1, TX1DPE_IsmInt },
  { "ism_ext",		"PCS External Output",	A2300_Port1_Out,  WCACOMP_RF1, WCACOMP_DSP_DUC1, TX1DPE_IsmExt },
  { "wideband1",	"RF1 Wideband Output",	A2300_Port1_Out,  WCACOMP_RF1, WCACOMP_DSP_DUC1, TX1DPE_Wideband }
};
const static int COUNT_PATHDEFS = 4;




/******************************************************************
* DataStreamWriter Implementation.
*****************************************************************/

DataStreamWriter::DataStreamWriter(size_t bytesPerSample, size_t sizeFrame, size_t iMaxFrames, char chTick)
: m_idxRfPath(-1), m_byteGain(0), m_dFreqRf(0), m_dFreqIf(0),
	m_dBandwidth( 28), m_dSampRate(2), 
	m_pDevice(NULL), m_pRf(NULL), m_BytesPerSample(bytesPerSample),
	m_sizeFrame(sizeFrame), m_iMaxBuffs(iMaxFrames), m_ctFramesProcessed(0),
	m_totalFramesToProcess(0), m_framesPerSec(0), m_chTick(chTick), m_pbuff(NULL), m_file(NULL)
{
}

DataStreamWriter::~DataStreamWriter(void)
{
	Stop();
	Terminate();
}


void DataStreamWriter::RegisterArgs( string sId, ArgParser& args)
{
	m_sId = sId;
	args.AddMapEntries(s_argsLogger,"global", m_sId.c_str());

}

int DataStreamWriter::Init( ArgParser& args, ConfigDevice* pDevice)
{
	m_pDevice   = pDevice;
	//m_sFilename	= args.GetString(m_sId + ".filename");


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
	m_dFreqIf    = args.GetDouble(m_sId + ".if-freq");
	m_byteGain   = (byte) args.GetLong(m_sId + ".gain");
	m_dBandwidth = args.GetDouble(m_sId + ".bw");
	m_dSampRate  = args.GetDouble(m_sId + ".rate");


	// Open the specified file for reading.
	//m_file = fopen(m_sFilename.c_str(), "rb");
	//if (!m_file) {
	//	printf("\nError: filename ('%s') cannot be "
	//		"accessed.\n", m_sFilename.c_str());
	//	return -2;
	//}

	//Initialize the USB Interface.
	const RfPathDef& rpath	= s_pathDef[m_idxRfPath];
	bool      bIsRf0	= rpath.idRf == WCACOMP_RF0;
	m_pPort				= new BulkDataPort(PortBase::EP_UNDEF, rpath.epTx);
	UsbDevice& device	= m_pDevice->Device();
	if( device.BindPort( m_pPort) != 0)
	{
		printf("Error binding to USB endpoint %02x (%s)", rpath.epTx, rpath.szPath);
		return -3;
	}

	//Clear out any DCI command messages.
	m_pDevice->Dci0Transport().ClearReceiveQueue();

	// Configure the RF Front-end profile, gain, frequency, and Bandwidth.
	m_pRf				= (bIsRf0) ? &(m_pDevice->RF0()) : &(m_pDevice->RF1());
	m_pRf->TxPath( rpath.idPath);
	m_byteGain			= m_pRf->TxGain( m_byteGain);
	m_dFreqRf			= m_pRf->TxFrequency( (uint32)(m_dFreqRf*1000 + 0.5))/1000.0;
	m_dBandwidth		= ConfigRf::BandwidthToMHz(m_pRf->TxBandwidth( ConfigRf::BandwidthFromMHz( m_dBandwidth )));

	// Reset the DDC Component so no data is in cache.
	m_pDduc				= new ConfigDduc(rpath.idDuc, m_sId + ".duc", m_pDevice, true);
	m_pDevice->Components().push_back(m_pDduc);
	m_pDduc->Reset();
	m_pDduc->Mode( ConfigDduc::Normal /*| ConfigDduc::SynchronizeReset*/);  //Synchronizes FIFOS with other streams.
	m_dFreqIf   = m_pDduc->FrequencyOffset( m_dFreqIf);
	m_dSampRate = m_pDduc->HostSamplingRate( m_dSampRate*1.0e6, true)/1.0e6;
	//TODO Set the baseband DDC offset.

	// Open USB Ports, attach callbacks,  and initialize Transfer Buffers Open the raw port, attach callback function to receive raw data as it comes in.
	m_pPort->Open(); 
	m_pPort->WriteTransfer() = BulkDataPort::TransferEvent(this, &DataStreamWriter::OnFrameSent);

	//Add buffers
	for( size_t nn = 0; nn < m_iMaxBuffs; ++nn)
	{
		m_listTxBuffers.push_back( new TransferBuffer( m_pPort, m_sizeFrame));
	}

	//Ready to go.
	return 0;
}

void DataStreamWriter::DisplayConfiguration()
{
	const RfPathDef& rpath	= s_pathDef[m_idxRfPath];
	bool      bIsRf0	= rpath.idRf == WCACOMP_RF0;

	printf("\n");
	printf("%s Configuration", m_sId.c_str());
	printf("--------------------------------------\n");
	printf("RF Component:  %s (id=%02Xh, ep=%02Xh)\n", (bIsRf0)? "RF0" : "RF1", rpath.idRf, rpath.epTx);
	printf("TX Profile:    %s (id=%02Xh)\n", rpath.szDescr, rpath.idPath);
	printf("TX Gain:       %d dB\n",	  m_byteGain);
	printf("TX Frequency:  %0.3f MHz\n",  m_dFreqRf);
	printf("TX Bandwidth:  %0.3lf MHz\n", m_dBandwidth);
	printf("Baseband IF:   %0.3lf Hz\n", m_dFreqIf);    
	printf("Host Rate:     %0.3lf MHz\n", m_dSampRate);

}

int DataStreamWriter::Start(size_t msecDur, bool bIsMaster)
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

	//Initialize all buffers in the list and submit.
	//Clear stored TX buffers.
	while( !m_listTxBuffers.empty())
	{
		TransferBuffer* pbuff = m_listTxBuffers.front();
		m_listTxBuffers.pop_front();

		size_t ctSamples = m_sizeFrame/2;

		int16* ptr = ((int16*)pbuff->pbuffer);
		int16* pend   = ptr + ctSamples;
	
		//Fill the buffer with a constant.
		while( ptr < pend)
		{
			*ptr = 8000; ptr++;
			*ptr = -5000; ptr++;
		}

		//Send the data to the ASR-2300
		pbuff->Submit( (int)  m_sizeFrame);
	}




	return 0;
}

int DataStreamWriter::CheckStatus()
{
	return !(m_ctFramesProcessed <=  m_totalFramesToProcess) && m_totalFramesToProcess > 0;
}

void DataStreamWriter::Stop()
{
	if( m_pPort!= NULL && m_pDduc != NULL)
	{
		m_pDduc->Reset(); // Reset disables the channel so no need to call Enable(0).


		bool bIsRf0	= s_pathDef[m_idxRfPath].idRf == WCACOMP_RF0;

		//Clear out any DCI command messages.
		m_pDevice->Dci0Transport().ClearReceiveQueue();

		// Disable the RF Front-end.
		m_pRf->TxPath( bIsRf0 ? (byte)TX0DPE_Disabled : (byte)TX1DPE_Disabled );

	}
}
void DataStreamWriter::Terminate()
{
	if( m_pPort == NULL || m_pDduc == NULL)
		return;

	//bool bIsRf0	= s_pathDef[m_idxRfPath].idRf == WCACOMP_RF0;

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

	//Clear current TX buffers transferring.
	BulkDataPort::TransferContext* pctxt;
	while( (pctxt = m_pPort->CancelWriteTransfer(1000))!= NULL)
	{
		//Delete the smart buffer, which will clean
		//up the context.
		delete (TransferBuffer*)(pctxt->pObj);
	}

	//Clear stored TX buffers.
	while( !m_listTxBuffers.empty())
	{
		delete m_listTxBuffers.front();
		m_listTxBuffers.pop_front();
	}

	// Close the Data Port
	m_pPort->Close();

	delete m_pPort; m_pPort = NULL;
	delete m_pDduc; m_pDduc = NULL;
	delete m_pbuff; m_pbuff = NULL;

	////close the file -- note this was opened in ParseOptions,
	//if (fclose(m_file) != 0) {
	//	printf("\nError %d closing file '%s'.\n", errno, m_sFilename.c_str());
	//}
}


void DataStreamWriter::OnFrameSent (void* pObj, BulkDataPort::TransferContext* pctxt)
{
	DataStreamWriter* pThis = static_cast<DataStreamWriter*>(pObj);

	// if the status is OK (0)
	if( pctxt->status == 0 && !pThis->CheckStatus())
	{
		if( pThis->GetDataAndSubmit( static_cast<TransferBuffer*>(pctxt->pObj)))
		{
			pThis->m_ctFramesProcessed ++;

			if( pThis->m_ctFramesProcessed % pThis->m_framesPerSec == 0)
			{
				putc( pThis->m_chTick, stdout);
				fflush(stdout);
			}
		}
	}
	else
	{
		printf("Failed to send buffer transfer not successful\n");
		pThis->m_listTxBuffers.push_back( static_cast<TransferBuffer*>(pctxt->pObj));
	}
}

bool   DataStreamWriter::GetDataAndSubmit( TransferBuffer* pBuff)
{
	//// Save data to disk
	//size_t nWritten = fwrite (pctxt->bufFrame, 1, pctxt->nActualLength, pThis->m_file);
	//if (nWritten != pctxt->nActualLength)
	//{
	//	printf ("\nWarning: Some data failed to write to the selected file ('%s').\n", pThis->m_sFilename.c_str());
	//}
	pBuff->Submit( (int) m_sizeFrame);
	return true;

}

