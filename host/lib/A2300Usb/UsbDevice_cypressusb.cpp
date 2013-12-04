/** Name: UsbDevice_cypressUsb.cpp
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

#include <A2300/UsbDevice.h>
#include <vector>

/**
 * Initializes the USB access (Cypress)
 */
int A2300::UsbDevice::InitializeUsb( )
{
	m_pCypressDevice = new CCyUSBDevice(NULL); // Create an instance of CCyUSBDevice
	if( !m_pCypressDevice )
		return(-1);

	// Initialize the address for this device.
	int rc = OpenDevice(m_vendorId, m_productId, m_usbAddress);
	if( rc != 0)
	{
		delete m_pCypressDevice;
		m_pCypressDevice = NULL;
	}
	return(rc);
}

/*
 * Find a device for the requested VID/PID.
 */
int A2300::UsbDevice::OpenDevice(uint16 vid, uint16 pid, uint16 usbAddress)
{
	bool deviceFound = false;

	int ctDevices = (int)m_pCypressDevice->DeviceCount();
	if( !ctDevices )
	{
		return(-1);
	}

	for (int i = 0; i < ctDevices && !deviceFound; i++)
	{
		m_pCypressDevice->Open((UCHAR)i); // Open automatically, calls Close() if necessary

		if (m_pCypressDevice->VendorID == vid &&
			m_pCypressDevice->ProductID == pid)
		{
			if((usbAddress == A2300_ADDR_UNDEF) || (usbAddress == m_pCypressDevice->USBAddress))
			{
				//Save Address for later.
				m_vendorId = vid;
				m_productId = pid;
				m_usbAddress =  m_pCypressDevice->USBAddress;
				return(0);
			}
			m_pCypressDevice->Close();
		}
	}
	// Failed to find the device if you got here.
	return(-1);
}


/*
* Starts the device
*/
int A2300::UsbDevice::StartUsb()
{	
	//Make sure initialize was called.
	if( m_pCypressDevice == NULL )
		return(-1);

	// Verify that we have a USB connection for the specified (vid,pid,addr).
	int rc = 0;
	if( !m_pCypressDevice->IsOpen() )
	{
		rc = OpenDevice(m_vendorId, m_productId, m_usbAddress);
	}
	return (rc);
}


void A2300::UsbDevice::StopUsb()
{
	if( m_pCypressDevice != NULL )
	{
		m_pCypressDevice->Close();
	}
}

void A2300::UsbDevice::TerminateUsb()
{
	if( m_pCypressDevice != NULL )
	{
		delete m_pCypressDevice;
		m_pCypressDevice = NULL;
	}
}

/**
* Get address information for specified VID/PID
*/
int A2300::UsbDevice::FindAttachedUsb(std::vector<int>& addrVect, int vid, int pid)
{
	CCyUSBDevice *pUsbDevice = new CCyUSBDevice(NULL);
	if( !pUsbDevice )
		return(0);

	int deviceCount = (int)pUsbDevice->DeviceCount();
	if (deviceCount == 0) return 0; // no devices

	// Return the number of devices found at the requested location.
	int ctDeviceFound = 0;

	// Get ASR-2300 device count
	for (int i = 0; i < deviceCount; i++)
	{
		pUsbDevice->Open((UCHAR)i);

		if (pUsbDevice->VendorID == (uint16)vid &&
			pUsbDevice->ProductID == (uint16)pid)
		{
			addrVect.push_back( pUsbDevice->USBAddress );
			++ctDeviceFound;
		}

		pUsbDevice->Close();
	}

	delete(pUsbDevice);

	return(ctDeviceFound);
}

