// Name: BulkDataPort.h
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

#ifndef A2300_BULK_DATAPORT_H
#define A2300_BULK_DATAPORT_H

#include <A2300/UsbDevice.h>

namespace A2300
{
	//*************************************************************************
	//* PortBase Implementation.
	//*************************************************************************
	class UsbPort;

	//*************************************************************************
	//* BulkDataPort Binding Implementation.
	//*************************************************************************
	/**
	* Implements binding of a port to a USB Bulk Data Endpoint.
	*/
	class BulkDataPort : public PortBase
	{
	public:
		// Constructor.
		BulkDataPort( byte epidIn, byte epidOut);
		virtual ~BulkDataPort();

		/**
		* Open the port
		*/
		void Open( );

		/**
		* Closes the port
		*/
		void Close( );

		/**
		* Read the specified number of bytes from the port.
		*/
		int Read( byte * pdata, int ctBytes, int msecTimeout );

		/**
		* Writes the specified number bytes to the port.
		*/
		int Write( byte * pdata, int ctBytes, int msecTimeout );
	protected:
		virtual void* OnGetInterface() { return this;}

	private:
		int OnInit();

		// reference to UsbDriver instance mostly used for logging
		uint				m_timeout;

#if defined(HAVE_LIBUSB)
		/**
		*  Checks if the given end point is available
		*/
		bool EndPointAvailable(byte epid);
		libusb_device_handle* 	m_pDevHandle;

#elif defined(WIN32)
		CCyUSBDevice * 		m_pCypressDevice;
		CCyBulkEndPoint* 	m_pEndPointIn;
		CCyBulkEndPoint* 	m_pEndPointOut;
#endif
	};
}

#endif /* A2300_BULK_DATAPORT_H */
