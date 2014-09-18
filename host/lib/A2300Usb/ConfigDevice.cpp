/** Name: ConfigDevice.cpp
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

#include <A2300/ConfigDevice.h>
#include <Dci/InfrastructureMsgs.h>
#include <Dci/StandardMsgs.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>

#define ENABLE DCI1 1


namespace A2300 {

ConfigDevice::ConfigDevice()
	: m_bCreated(false), m_pDevice( NULL),
	  m_dci0(A2300_DciIdc0_EpIn, A2300_DciIdc0_EpOut),
	  m_dci1(A2300_DciIdc1_EpIn, A2300_DciIdc1_EpOut),
	  m_timeoutDefault(A2300_WAIT_TIME ),
	  m_rf0( WCACOMP_RF0, "RF0", this),
	  m_rf1( WCACOMP_RF1, "RF1", this)
	{}

ConfigDevice::~ConfigDevice()
{
	Detach();
}

/**
 * Property gets the DCI Conversation ID 1 transport (asynchronous Data I/O).  Use this
 * transport to send/receive data with the ASR-2300.  This conversation should be used
 * in a way that supports multiple interactions simultaneously.  Recommend you do not block
 * (wait) for receives on this transport unless you are sure you are the only user.
 */
TransportDci& ConfigDevice::Dci1Transport()
{
	//Initialize using latebinding.
	if( !m_dci1.IsInitialized() && m_pDevice != NULL)
	{
		if( m_pDevice == NULL)
			std::runtime_error("Cannot provide DCI 1 Transport.  No Device attached.");

		m_dci1.Init( *m_pDevice, 1,m_timeoutDefault);
	}

	return m_dci1.transport;
}


std::string ConfigDevice::IdentifyDevice()
{
	byte buff[DCI_MAX_MSGSIZE];
	char sbuff[256];
	TransportDci& dt = m_dci0.transport;

	int len = Dci_IdentifyDeviceQuery_Init(buff);
	if( dt.SendMsg( buff, (size_t) len, false) > 0)
	{
		int len = dt.ReceiveMsg( buff, DCI_MAX_MSGSIZE);
		Dci_IdentifyDevice* pid = (Dci_IdentifyDevice*) buff;
		if( len > 0 && Dci_Hdr_MatchesId1(&(pid->hdr), Dci_IdentifyDevice_Id))
		{
			Dci_IdentifyDevice* pid = (Dci_IdentifyDevice*) buff;
			sprintf( sbuff, "ID: %s, S/N: %s, Model: %s",
					pid->DeviceId, pid->SerialNumber, pid->Model);
			return std::string( sbuff);
		}
	}
	return std::string("-N/A-");
}

std::string ConfigDevice::FirmwareVersion( )
{
	Dci_VersionInfo vi;
	if( FirmwareVersionRaw( &vi) )
	{
		char sbuff[256];
		sprintf(sbuff, "%02d.%02d.%02d - %04d",
				(int) vi.VerMajor, (int) vi.VerMinor,
				(int) vi.VerMaintenance, (int) vi.Revision);
		return std::string( sbuff);
	}
	else
		return std::string("-N/A-");
}

bool ConfigDevice::FirmwareVersionRaw(Dci_VersionInfo* pvi)
{
	byte buff[DCI_MAX_MSGSIZE];
	TransportDci& dt = m_dci0.transport;
	int len = Dci_VersionInfoQuery_Init(buff);

	if( dt.SendMsg( buff, (size_t) len, false) > 0)
	{
		int len = dt.ReceiveMsg( buff, DCI_MAX_MSGSIZE);
		Dci_VersionInfo* pinfo = (Dci_VersionInfo*) buff;
		if( len > 0 && Dci_Hdr_MatchesId1(&(pinfo->hdr), Dci_VersionInfo_Id))
		{
			memcpy( pvi, buff, sizeof( Dci_VersionInfo));
			return true;
		}
	}
	return false;
}

#define GET_UINT16PROP( comp, id, defaultval) \
	TransportDci& dt = m_dci0.transport; \
	uint16 value; \
	if( dt.GetProperty<uint16>(comp, id, value) == 0) \
		return value; \
	else \
		return defaultval; \


uint16 ConfigDevice::FpgaId()
{
	GET_UINT16PROP( WCACOMP_HALDEFAULT, 0x01, 0)
}

uint16 ConfigDevice::FpgaVersion()
{
	GET_UINT16PROP( WCACOMP_HALDEFAULT, 0x02, 0)
}

uint16 ConfigDevice::RfProfileId()
{
	GET_UINT16PROP( WCACOMP_RFPROFILES, RFP_PROP_ID, 0xFFFF)
}
uint16 ConfigDevice::RfProfileVersion()
{
	GET_UINT16PROP( WCACOMP_RFPROFILES, RFP_PROP_VERSION, 0xFFFF)
}

uint16 ConfigDevice::RfProfileRevision()
{
	GET_UINT16PROP( WCACOMP_RFPROFILES, RFP_PROP_REVISION, 0xFFFF)
}


/** 
* Queries hardware for the currently list of RF Profiles available.
*/
void ConfigDevice::GetRfDescriptorList( RfProfileDescriptorList& list)
{
	#define DESCR_RECSIZE 18
	#define MAX_RECSPERMSG 14
	byte buff[DCI_MAX_MSGSIZE];
	TransportDci& dt = m_dci0.transport; 

	//Request the path data descriptors.
	int len = Dci_TypedDataRecordQuery_Init(buff, RFP_TYPEDATA_PATHDESCRIP ,0);
	if( dt.SendMsg( buff, (size_t) len, false) > 0)
	{
		int len,  ctRecords = MAX_RECSPERMSG;
		while( ctRecords >= MAX_RECSPERMSG && (len = dt.ReceiveMsg( buff, DCI_MAX_MSGSIZE)) > 0)
		{
			Dci_TypedDataRecord* ptdr = (Dci_TypedDataRecord*) buff;

			if( len > 0 && Dci_Hdr_MatchesId1(&(ptdr->hdr), Dci_TypedDataRecord_Id))
			{
				byte* pdata = Dci_TypedDataRecord_GetData(ptdr);
				ctRecords = ptdr->lenData / DESCR_RECSIZE;
				for( int i = 0; i < ctRecords; i++, pdata+=DESCR_RECSIZE)
				{	
					RfProfileDescriptor descr;
					descr.id = pdata[0];
					memcpy( descr.descr, pdata+1, 17);
					descr.descr[17] = '\0';
					list.push_back( descr);
				}
			}
		}
	}
}

/**
 * Synchronizes the internal RF Lime interfaces to eliminate interchannel bias
 * resulting in filters and various counters not be reset at the same time.  This
 * is only needed when channels must be synchronized.
 */
void ConfigDevice::SynchRfState()
{
	TransportDci& dt = m_dci0.transport;
	dt.SetProperty<byte>( WCACOMP_FPGA, 0x00, 0x10); 
	dt.SetProperty<byte>( WCACOMP_FPGA, 0x00, 0x00);
}

/**
 * Attach ASR-2300 device by finding the device specified by address, if not defined,
 * the first device found is selected.  Override to use specialized VID, PID values.
 * throws runtime error if not able to create and attach to ASR-2300 device.
 */
int ConfigDevice::Attach( int addr, int vid, int pid )
{

	if( addr == FindFirstDevice)
	{
		std::vector<int> addrs;
		int ctDevices = UsbDevice::FindAttached(addrs, vid, pid);
		if (ctDevices == 0)
		{
			throw std::runtime_error("No attached ASR-2300 devices found.");
		}

		addr = addrs.at(0);
	}

	m_pDevice = new UsbDevice();
	m_bCreated = true;

	// Initialize (and Open) the USB driver.
	if( m_pDevice->Initialize(addr) != 0)
	{
		delete m_pDevice;
		m_pDevice = NULL;
		m_bCreated = false;
		throw std::runtime_error("Could not initialize ASR-2300 USB Device interface");
	}

	// Start USB driver activity (open portDci if not already opened).
	if( m_pDevice->Start() != 0)
	{
		m_pDevice->Terminate();
		delete m_pDevice;
		m_pDevice = NULL;
		m_bCreated = false;
		throw std::runtime_error("Could not start the ASR-2300 USB Device interface.");
	}

	//Attach now like an externally provided device interface.
	Attach( m_pDevice);
	return addr;
}

/**
 * Attach to existing ASR-2300 device.   Device assumed to be initialized and started
 * prior to attach.
 */
void ConfigDevice::Attach( UsbDevice* pdevice)
{
	//Bind DCI IDC0 so we can communicate with the device.
	m_dci0.Init( *pdevice, 0, m_timeoutDefault);
#ifdef ENABLE_IDC1
	m_dci1.Init( *pdevice, 1, m_timeoutDefault);
#endif

}

/**
 * Detachs configuration object from ASR-2300 does appropriate cleanup.  If profived
 * external UsbDevice object, stop and termination operations are left for the calling software.
 */
void ConfigDevice::Detach()
{
	//Terminate all components and internal transports.
	m_dci0.Term();

	if( m_bCreated && m_pDevice != NULL)
	{
		m_pDevice->Stop();
		m_pDevice->Terminate();
		delete m_pDevice;
		m_bCreated = false;
	}

	m_pDevice = NULL;
}

/**
 * Initializes device to the current configuration state.
 */
void ConfigDevice::Synchronize()
{
	m_rf0.Synch();
	m_rf1.Synch();

	ConfigComponentList::iterator iter = m_listComponents.begin();

	for( ; iter != m_listComponents.end(); iter++)
	{
		(*iter)->Synch();
	}
}

/**
 * Resets the device to its default state.
 */
void ConfigDevice::Reset()
{
	m_rf0.Reset();
	m_rf1.Reset();

	ConfigComponentList::iterator iter = m_listComponents.begin();

	for( ; iter != m_listComponents.end(); iter++)
	{
		(*iter)->Reset();
	}

}


/**
* Routine dumps ASR-2300 version data to the specified FILE*
*/
void ConfigDevice::DumpVersionInfo( FILE* fp)
{
	//Clear out any previous messages.
	Dci0Transport().ClearReceiveQueue();

	//Print out Device information
	std::string sId 		= IdentifyDevice();
	std::string sVer 		= FirmwareVersion();
	uint16	    idFpga 		= FpgaId();
	uint16 		verFpga 	= FpgaVersion();
	uint16		idProfile   = this->RfProfileId();
	uint16      verProfile  = this->RfProfileVersion();
	uint16      revProfile  = this->RfProfileRevision();

	int  		iVer = (verFpga>>8);
	int	 		iRev = (verFpga& 0x00ff);

	fprintf(fp, "[USB Address: %d] \n", this->m_pDevice->GetUsbAddress());
	fprintf(fp, "  Identity:               %s\n", sId.c_str());
	fprintf(fp, "  CPU FW Ver:             %s\n", sVer.c_str());
	fprintf(fp, "  FX3 FW Ver:	           %s\n", "-Not implemented-");
	fprintf(fp, "  FPGA ID-Ver:            %04X - %X.%X\n", idFpga, iVer, iRev);
	fprintf(fp, "  RF Profile ID-Ver-Rev:  %04X - %X.%X-%X\n\n", idProfile, (verProfile >> 0x8), (verProfile & 0x00FF), revProfile);

}
/**
* Dumps RF Profile descriptors to the specified file pointer.
*/
void ConfigDevice::DumpRfProfilesDescriptors( RfProfileDescriptorList& listDescrs, FILE* fp)
{
	const char* szCh[] = {"RF0 TX", "RF0 RX", "RF1 TX", "RF1 RX"};
	RfProfileDescriptorList::iterator iter = listDescrs.begin();

	fprintf( fp, "  RF PATH DEFINITIONS: \n");
	fprintf( fp, "    ID  | Channel | Description\n");
	fprintf( fp, "    -----------------------------------------\n");

	for( ; iter != listDescrs.end(); iter++)
	{
		RfProfileDescriptor& pd = (*iter);
		byte ch = (byte) (pd.id >> 6);
		fprintf( fp, "    %02Xh | %6s  | %s\n", pd.id, szCh[ch], pd.descr);
	}

}


void ConfigDevice::DciCtrl::Init( UsbDevice& device, int idc, double timeout)
{
	// Bind the USB driver to a portDci.
	int retval = device.BindPort( &port);
	if( retval == 0)
	{
		//Open the port
		port.Open();

		Dci_Conversation_Init( &conversation, (byte) idc);

		//Initialize transport to simplify drudgery of communicating.
		transport.Init( &port, timeout, &conversation);
	}
}

void ConfigDevice::DciCtrl::Term()
{
	transport.Term();
	port.Close();

}

} /* namespace A2300 */
