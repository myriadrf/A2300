// Name: UsbDevice_libusb.cpp
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

#include <A2300/UsbDevice.h>

#include <time.h>

#include <stdexcept>
#include <string>
#include <vector>

/**
 * Initializes the USB access (Cypress)
 * Return with a DeviceHandle initialized (if found).
 */
int A2300::UsbDevice::InitializeUsb( )
{
	int rc = libusb_init(&m_pCtx);
	if( rc < 0 )
		return(rc);					// Error initializing libusb.
	libusb_set_debug(m_pCtx, 2);    // Verbosity Level.

	// Get a list of USB devices
	return OpenDevice(m_vendorId, m_productId, m_usbAddress);
}

/*
* Starts the device
*/
int A2300::UsbDevice::StartUsb()
{
	//Make sure initialize was called.
	if( m_pDevHandle == NULL )
	{
		if( OpenDevice(m_vendorId, m_productId, m_usbAddress) != 0)
			return(-1);
	}

	// Check if a kernel driver is attached to the interface. If it is
	// we need to detach it.
	int retval = libusb_kernel_driver_active(m_pDevHandle, 0);
	if (retval == 1)
	{
		if ((retval=libusb_detach_kernel_driver(m_pDevHandle, 0)))
		{
			throw std::runtime_error("libusb_detach_kernel_driver() failed: " + std::string(libusb_error_name(retval)));
		}
	}
	else if (retval != 0 )
	{
		throw std::runtime_error("libusb_kernel_driver_active() failed: " + std::string(libusb_error_name(retval)));
	}

	//claim interface for the device
	if( (retval=libusb_claim_interface(m_pDevHandle, 0)))
		throw std::runtime_error("libusb_claim_interface() failed." + std::string(libusb_error_name(retval)));

	return 0;
}


void A2300::UsbDevice::StopUsb()
{
	//release the claimed interface
	int retval = libusb_release_interface(m_pDevHandle, 0);

	if(retval!=0)
	{
		throw std::runtime_error("Failure in releasing device interface.");
	}
}

void A2300::UsbDevice::TerminateUsb()
{
	//close the opened device.
	libusb_close(m_pDevHandle);
	libusb_exit(m_pCtx);   // Must be called at the end of application.
}

/*
 * Open USB Device.
 */
int A2300::UsbDevice::OpenDevice(uint16 vid, uint16 pid, uint16 usbAddress)
{
	libusb_device** devList;
	int ctDevices = libusb_get_device_list(m_pCtx, &devList);
	if (ctDevices == 0) return -1; // no devices

	// Get the specified device from the device list
	libusb_device *pDevice;

	struct libusb_device_descriptor desc;
	uint8_t devAddress;

	// Discover requested device VID/PID and optional address.
	for (int i = 0; i < ctDevices; i++)
	{
		pDevice = devList[i];
		int rc = libusb_get_device_descriptor( pDevice, &desc );
		if (rc < 0) throw std::runtime_error("libusb_get_device_descriptor() failed: " + std::string(libusb_error_name(rc)));

		// First look at VID/PID
		if( desc.idVendor == vid && desc.idProduct == pid )
		{
			devAddress = libusb_get_device_address(pDevice);	// Get the address of the device on the bus.

			// Then look at the device port address (a small integer)
			if( usbAddress == A2300_ADDR_UNDEF || usbAddress == (uint16)devAddress)
			{
				// Open the device
				rc=libusb_open(pDevice, &m_pDevHandle);
				if( rc == LIBUSB_ERROR_ACCESS )
					throw std::runtime_error("libusb_open() failed (no write access): " + std::string(libusb_error_name(rc)));
				else if( rc )
					throw std::runtime_error("libusb_open() failed: " + std::string(libusb_error_name(rc)));

				m_usbAddress = (uint16)devAddress;
				m_vendorId = desc.idVendor;
				m_productId = desc.idProduct;
				// Leave with device open.
				return(0);
			}
		}
	}
	return(-2);
}

/**
* Get address information for specified VID/PID
*/
int A2300::UsbDevice::FindAttachedUsb(std::vector<int>& addrVect, int vid, int pid)
{
	int usbAddress;

	libusb_context *pCtx;
	int rc = libusb_init(&pCtx);
	if( rc < 0 )
		return(0);

	// Get a list of USB devices
	libusb_device** devList;
	int ctDevices = libusb_get_device_list(pCtx, &devList);
	if (ctDevices == 0) return -1; // no devices	libusb_device** devList;

	// find  device count
	int ctDeviceFound = 0;
	for (int i = 0; i < ctDevices; i++)
	{
		libusb_device *pDevice = devList[i];

		struct libusb_device_descriptor desc;
		int retval = libusb_get_device_descriptor( pDevice, &desc );
		if (retval < 0) throw std::runtime_error("libusb_get_device_descriptor() failed: " + std::string(libusb_error_name(retval)));

		if( desc.idVendor == vid && desc.idProduct == pid )
		{
			usbAddress = libusb_get_device_address(pDevice);	// Get the address of the device on the bus.
			addrVect.push_back( usbAddress );
			ctDeviceFound++;
		}
	}
	libusb_exit(pCtx);
	return ctDeviceFound;
}

int A2300::UsbDevice::PollAsynchronousEvents( double timeout, int& completed)
{
	timeval tv;
	tv.tv_sec = (time_t)(timeout);
	timeout -= (double)(tv.tv_sec);
	tv.tv_usec = (suseconds_t)(timeout*((double)(1.0e6)));

	return libusb_handle_events_timeout_completed(m_pCtx, &tv, &completed);
}
