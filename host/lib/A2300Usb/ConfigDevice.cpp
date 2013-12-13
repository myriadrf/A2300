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
namespace A2300 {


ConfigDevice::ConfigDevice()
	: m_bCreated(false), m_pDevice( NULL),
	  m_dci0(A2300_DciIdc0_EpIn, A2300_DciIdc0_EpOut),
	  m_timeoutDefault(A2300_WAIT_TIME ),
	  m_rf0( WCACOMP_RF0, "RF0", this),
	  m_rf1( WCACOMP_RF1, "RF1", this)
	{}


ConfigDevice::~ConfigDevice()
{
	Detach();
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

std::string ConfigDevice::FirmwareVersion( int /*idWhich*/)
{
	byte buff[DCI_MAX_MSGSIZE];
	char sbuff[256];
	TransportDci& dt = m_dci0.transport;
	int len = Dci_VersionInfoQuery_Init(buff);

	if( dt.SendMsg( buff, (size_t) len, false) > 0)
	{
		int len = dt.ReceiveMsg( buff, DCI_MAX_MSGSIZE);
		Dci_VersionInfo* pinfo = (Dci_VersionInfo*) buff;
		if( len > 0 && Dci_Hdr_MatchesId1(&(pinfo->hdr), Dci_VersionInfo_Id))
		{
			sprintf(sbuff, "%02d.%02d.%02d - %04d",
					(int) pinfo->VerMajor, (int) pinfo->VerMinor,
					(int) pinfo->VerMaintenance, (int) pinfo->Revision);
			return std::string( sbuff);
		}
	}
	return std::string("-N/A-");
}

uint16 ConfigDevice::FpgaId()
{
	TransportDci& dt = m_dci0.transport;

	uint16 value;
	if( dt.GetProperty<uint16>(WCACOMP_HALDEFAULT, 0x01, value) == 0)
		return value;
	else
		return 0;
}
uint16 ConfigDevice::FpgaVersion()
{
	TransportDci& dt = m_dci0.transport;

	uint16 value;
	if( dt.GetProperty<uint16>(WCACOMP_HALDEFAULT, 0x02, value) == 0)
		return value;
	else
		return 0;

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
