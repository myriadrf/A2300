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
#include <list>

namespace A2300
{
	//*************************************************************************
	//* PortBase Implementation.
	//*************************************************************************
	class UsbPort;

	/**
	 * Template class implements simple event / delegate  to provide callback
	 * capability to object members
	 */
	template<typename R, typename A1>
	class Event
	{
	public:
	    typedef R (*Function)(void*, A1);

	    Event() : _obj(NULL), _func(&Event<R,A1>::DumpFunc){}
	    Event(void* o, Function f) : _obj(o), _func(f) {}

	    Event( const Event<R, A1>& rhs) : _obj( rhs._obj), _func(rhs._func){}

	    R operator()(A1 a1) const
	    {
	        return (*_func)(_obj, a1);
	    }

	private:
	   static R DumpFunc( void*, A1) { /*NOP*/ }

	private:
	    void* _obj;
	    Function _func;
	};

	/**
	 * Delegate wraps  pointers to member s.  Note required if
	 *  is static.
	 */
	template<typename R, class T, typename A1, R (T::*Func)(A1)>
	R Delegate(void* o, A1 a1)
	{
	    return (static_cast<T*>(o)->*Func)(a1);

	}

	//*************************************************************************
	//* BulkDataPort Binding Implementation.
	//*************************************************************************
	/**
	* Implements binding of a port to a USB Bulk Data Endpoint.
	*/
	class BulkDataPort : public PortBase
	{
	public:
		class TransferContext
		{
			friend class BulkDataPort;
		public:
			BulkDataPort* pSrc;
			byte* bufFrame;
			size_t nFrameSize;
			size_t nActualLength;
			int status;
			bool bCompleted;

#ifdef HAVE_LIBUSB
			TransferContext() : pSrc(NULL), bufFrame(NULL), nFrameSize(0),
					nActualLength(0), status(0), bCompleted( false), lut(NULL){}
			~TransferContext()
			{
				if( lut) libusb_free_transfer( lut);
			}

			inline int Submit( int size)
			{
				lut->length = size;
				return libusb_submit_transfer(lut);
			}

			inline int Submit( )
			{
				lut->length = nFrameSize;
				return libusb_submit_transfer(lut);
			}

		protected:
			libusb_transfer *lut;

#elif defined(WIN32)
		//TODO
#endif

		};
		typedef Event<void, TransferContext*> TransferEvent;
		typedef std::list<TransferContext*>   TransferContextList;

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

		TransferContext*	CreateReadTransferContext(byte* bufFrame, size_t sizeFrame);
		TransferContext*	CreateWriteTransferContext(byte* bufFrame, size_t sizeFrame);

		/**
		 * Sets the Asynchronous read callback handler.
		 */
		TransferEvent& ReadTransfer() { return m_evtRead;}


		/**
		 * Sets the Asynchronous write callback handler.
		 */
		TransferEvent& WriteTransfer() { return m_evtWrite;}


		/**
		* Synchronous read the specified number of bytes from the port.
		*/
		int Read( byte * pdata, int ctBytes, int msecTimeout );


		/**
		 * Writes the specified number bytes to the port.
		 */
		int Write(byte * pdata, int ctBytes, int msecTimeout );


	protected:
		virtual void* OnGetInterface() { return this;}

	private:
		int OnInit();

		// reference to UsbDriver instance mostly used for logging
		uint				m_timeout;

#if defined(HAVE_LIBUSB)
		TransferEvent m_evtRead;
		TransferEvent m_evtWrite;
		TransferContextList m_listReadContexts;
		TransferContextList m_listWriteContexts;
		/**
		*  Checks if the given end point is available
		*/
		bool EndPointAvailable(byte epid);
		libusb_device_handle* 	m_pDevHandle;

		static void  LibusbAsyncReadCallback(libusb_transfer *lut);
		static void  LibusbAsyncWriteCallback(libusb_transfer *lut);

#elif defined(WIN32)
		CCyUSBDevice * 		m_pCypressDevice;
		CCyBulkEndPoint* 	m_pEndPointIn;
		CCyBulkEndPoint* 	m_pEndPointOut;
#endif
	};
}

#endif /* A2300_BULK_DATAPORT_H */
