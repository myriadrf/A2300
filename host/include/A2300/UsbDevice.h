/** Name: UsbDevice.h
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

#ifndef A2300_USBDEVICE_H_
#define A2300_USBDEVICE_H_

#include <A2300/libusb_ext.h>
#include <System/DataTypes.h>
#include <vector>			// Used in FindAttached().


#define A2300_VENDOR_ID	  (0x1D50)  //Openmoko.com VID
#define A2300_PRODUCT_ID  (0x608B)	//Registered ASR-2300 PID.
#define A2300_ADDR_UNDEF  (0xffff)

#define A2300_DciIdc0_EpIn 0x82
#define A2300_DciIdc0_EpOut 0x1
#define A2300_DciIdc1_EpIn 0x84
#define A2300_DciIdc1_EpOut 0x3

namespace A2300
{
	class PortBase;
	//*************************************************************************
	//* A2300 USB Driver Implementation.
	//*************************************************************************
	/**
	* Lighweight USB Driver Implementation for the Asr-2300
	* series of devices.
	*/
	class UsbDevice
	{
		friend class PortBase;
	public:
		UsbDevice(int vid = A2300_VENDOR_ID, int pid = A2300_PRODUCT_ID);
		virtual ~UsbDevice();

	public:
		/**
		* <summary>
		* Initializes the driver, User can specify specific address or driver
		* will search (0xffff) for first available device.
		* Return:  0 for successful start, < 0 for error, > 0 for warning (not started).
		*
		* </summary>
		*/
		virtual int Initialize( int addr=A2300_ADDR_UNDEF );
		/**
		* <summary>
		* Starts the driver after being initialized.
		* Return:  0 for successful start, < 0 for error, > 0 for warning (not started).
		* </summary>
		*/
		virtual int Start();  // Startup USB Device.

		/**
		* <summary>
		* Stops a driver but leaves in an initialized State.
		* </summary>
		*/
		virtual void Stop( );  // StopDevice().
		/**
		* <summary>
		* Terminates driver operations and permanently releases all resources.  Cannot
		* be started again unless initialized.  Likely must instantiate new object after termination.
		* </summary>
		*/
		virtual void Terminate( );

		virtual int BindPort(PortBase* pPort);

#if defined(HAVE_LIBUSB)
		/**
		 * Polls ansynchronous events.
		 */

		inline int PollAsynchronousEvents()
		{
			return libusb_handle_events_completed( m_pCtx, NULL);
		}

		int PollAsynchronousEvents( double timeout, int& completed);
#endif

    protected:
#if defined(WIN32)
		// Cypress usb library support
		CCyUSBDevice * CyUSBDevice() { return m_pCypressDevice; }
#elif defined(HAVE_LIBUSB)
		//Access to underlying LIB USB Support.
		libusb_device_handle* 	DeviceHandle() { return m_pDevHandle;}
#endif

	private:
		// This is the maximum number of USB Addresses attached to a single VID/PID.
		static const int MAX_DEVICES = 32;

		/**
		* Helper function for control data transfer.
		*/
		int ControlDataTransfer( uint16 direction, byte reqCode, uint16 value, uint16 index, uint8* buf,  uint16 bytes, uint32 timeout);

		/**
		* Initializes usb device/library
		*/
		int InitializeUsb(  );

		/**
		* Starts the device
		*/
		int StartUsb();

		/**
		* Stops the device
		*/
		void StopUsb();

		/*
		* Terminate the device
		*/
		void TerminateUsb();

		/*
		 * Open a device at the specified VID/PID.
		 */
		int OpenDevice(uint16 vid, uint16 pid, uint16 addr);

		/*
		 * Find available addresses.
		 */
		static int FindAttachedUsb(std::vector<int>& addrVect, int vid, int pid);


	// Accessors.
	public:
		int GetUsbAddress() const { return (int)m_usbAddress; }
		//int SetBulkInTransfer(uint8* data, uint32 length);
		//int SetBulkOutTransfer(uint8* data, uint32 length);
		// Find a list of addresses for the specified VID/PID.
		static int FindAttached(std::vector<int>& addrVect,
						int vid = A2300_VENDOR_ID, int pid = A2300_PRODUCT_ID);

	private:
		uint16 		m_vendorId;
		uint16 		m_productId;
		uint16		m_usbAddress;

#ifdef HAVE_LIBUSB
		// Libusb specific
		libusb_device_handle* 	m_pDevHandle;
		libusb_context * 		m_pCtx;

#elif defined(WIN32)
		CCyUSBDevice * 			m_pCypressDevice;
#endif

	};

	/**
	* Base class for all ports supported by the USB driver. PortBse 
	* is a generic I/O interface to the USB Device.  Derived classes
	* provide specific I/O types: e.g. BulkDataPort (see below).
	*/
	class PortBase
	{
		// Allow access to Init().
		friend class UsbDevice;
	public:
		static const byte EP_UNDEF = 0;
		PortBase();
		PortBase( byte epidIn, byte epidOut);

		UsbDevice* driver()  const { return m_pUsbDevice;}
		byte	   epidIn()  const { return m_epidIn;}
		byte	   epidOut() const { return m_epidOut;}

	protected:
		bool IsInitialized() { return m_pUsbDevice != NULL; }
#ifdef HAVE_LIBUSB
		//Access to underlying LIB USB Support.
		libusb_device_handle* 	DeviceHandle() { return m_pUsbDevice->DeviceHandle();}
#elif defined(WIN32)
		// Cypress usb library support
		CCyUSBDevice * CyUSBDevice() { return m_pUsbDevice->CyUSBDevice(); }

#endif

		virtual int OnInit() = 0;
	private:
		/**
		* Private routine only called by UsbDevice.
		*/
		virtual int Init( UsbDevice *pDriver);

		// reference to UsbDevice instance mostly used for logging
		UsbDevice* 	m_pUsbDevice;
		// End Points In and Out.
		byte 		m_epidIn;
		byte 		m_epidOut;
	};
}

#endif /* A2300_USBDEVICE_H_ */

