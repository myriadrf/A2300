/** Name: BulkDataPort.h
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

#ifndef A2300_BULK_DATAPORT_H
#define A2300_BULK_DATAPORT_H

#include <A2300/UsbDevice.h>
#include <list>
#include <stdlib.h>

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

			/**
			* Pointer to user defined object.
			*/
			void* pObj;

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
			  lut->length = (int) nFrameSize;
				return libusb_submit_transfer(lut);
			}

			inline int Cancel()
			{
				return libusb_cancel_transfer(lut);
			}

			inline void Destroy()
			{
				delete this;
			}

		protected:
			libusb_transfer *lut;

#elif defined(WIN32)
		public:
			TransferContext( BulkDataPort* psrc, CCyBulkEndPoint* pEndpoint, byte* bframe,int nframesize)
				: pSrc(psrc),  bufFrame(bframe), nFrameSize(nframesize),
					nActualLength(0), status(0), bCompleted( false), 
					pep(pEndpoint)
			{
				//Create an event to monitor.
				overlap.hEvent = CreateEvent( NULL, false, false, NULL);
			}
			
			~TransferContext()
			{
				if( overlap.hEvent != NULL) 
					CloseHandle(overlap.hEvent);
			}

			inline ULONG Submit( int size)
			{
				context = pep->BeginDataXfer( bufFrame, size, &overlap);
				return ( pep->NtStatus || pep->UsbdStatus )? pep->NtStatus : 0;
			}

			inline ULONG Submit( )
			{
				return Submit( nFrameSize);
			}

			inline bool Cancel()
			{
                pep->Abort();
                if (pep->LastError == ERROR_IO_PENDING)
                    WaitForSingleObject(overlap.hEvent,2000);
				
				LONG size = (LONG) nFrameSize;
				bool bResult = pep->FinishDataXfer(bufFrame, size, &overlap, context);
				return bResult;
			}

			inline void Destroy()
			{
				delete this;
			}
		protected:
			bool WaitForTransfer(int msecTimeout)
			{
				status = 0;
				bCompleted = pep->WaitForXfer(&overlap, msecTimeout);
				if( !bCompleted) status = -1;
				return bCompleted;
			}

			void FinishTransfer()
			{
				LONG size;
				bCompleted  = pep->FinishDataXfer(bufFrame, size, &overlap, context);
				nActualLength = size;
				status = (bCompleted) ? 0 : -2;
			}

			CCyBulkEndPoint* pep;
			OVERLAPPED	  overlap;
			PUCHAR		  context;
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

#ifdef WIN32
		/**
		* Win32 interface, waits for a read transfer event and initiates callbacks 
		*/
		TransferContext&  WaitForReadTransferEvent(int msecTimeout);

		/**
		* Win32 interface, waits for a write transfer event and initiates callbacks 
		*/
		TransferContext&  WaitForWriteTransferEvent(int msecTimeout);
#endif
	protected:
		virtual void* OnGetInterface() { return this;}

	private:
		int OnInit();

		// reference to UsbDriver instance mostly used for logging
		uint				m_timeout;
		TransferEvent m_evtRead;
		TransferEvent m_evtWrite;
#ifdef HAVE_LIBUSB		
		/**
		*  Checks if the given end point is available
		*/
		bool EndPointAvailable(byte epid);
		libusb_device_handle* 	m_pDevHandle;

		static void  LibusbAsyncReadCallback(libusb_transfer *lut);
		static void  LibusbAsyncWriteCallback(libusb_transfer *lut);

#elif defined(WIN32)
		TransferContextList m_listReadContexts;
		TransferContextList m_listWriteContexts;
		TransferContextList::iterator m_iterNextRead;
		TransferContextList::iterator m_iterNextWrite;
		CCyUSBDevice * 		m_pCypressDevice;
		CCyBulkEndPoint* 	m_pEndPointIn;
		CCyBulkEndPoint* 	m_pEndPointOut;
#endif
	};
}

#endif /* A2300_BULK_DATAPORT_H */
