/** Name: UsbDevice_core.cpp
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
#include <float.h>

//*****************************************************************************
//UsbDevice Implementation.
//*****************************************************************************
// Constructor.
A2300::UsbDevice::UsbDevice(int vid, int pid):
	m_vendorId((uint16)vid), m_productId((uint16)pid)
   ,m_usbAddress(A2300_ADDR_UNDEF)
#if defined(WIN32)
	,m_pCypressDevice(NULL)
#elif defined(HAVE_LIBUSB)
    ,m_pDevHandle(0)
	,m_pCtx(NULL)
#endif
{ 
}

// Destructor.
A2300::UsbDevice::~UsbDevice()
{
#if defined(WIN32)
	if( m_pCypressDevice )
		delete m_pCypressDevice;
#elif defined(HAVE_LIBUSB)
#endif
}

/**
 * <summary>
 * Bind a port to the device.
 * </summary>
 */
int A2300::UsbDevice::BindPort(PortBase* pPort)
{
	return pPort->Init(this);
}

/**
 * <summary>
 * Initializes the Driver.
 * Return:  0 for successful start, < 0 for error, > 0 for warning (not started).
 * </summary>
 */
int A2300::UsbDevice::Initialize(int addr)
{ 
	m_usbAddress = (uint16) addr;
	//Do Platform specific initialization
	return InitializeUsb();
}

/**
 * <summary>
 * Terminates subsystem operations and permanently releases all resources.  Cannot
 * be started again unless initialized.  Likely must instantiate new object after termination.
 * </summary>
 */
void A2300::UsbDevice::Terminate( )
{
	TerminateUsb();
}

/**
 * <summary>
 * Starts the subsystem after being initialized.
 * Return:  0 for successful start, < 0 for error, > 0 for warning (not started).
 * </summary>
 */
 int A2300::UsbDevice::Start()
 {
	 //Call platform specific USB Start.
	 int iRetVal = StartUsb();

	//TODO Perform other start up activities.

	 return iRetVal;
}

/**
 * <summary>
 * Stops a subsystem but leaves in an initialized State.
 * </summary>
 */
void A2300::UsbDevice::Stop( )
{
	// Perform any other stopping before we stop the device.
	StopUsb();
}

/**
 * <summary>
 * Determine the number of devices attached at current VID/PID.
 * </summary>
 */
int A2300::UsbDevice::FindAttached(std::vector<int>& addrVect, int vid, int pid)
{
	return FindAttachedUsb(addrVect, vid, pid);
}

