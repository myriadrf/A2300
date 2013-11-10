// Name: BulkDataPort_cypressUsb.cpp
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

#include <System/DataTypes.h>
#include <A2300/BulkDataPort.h>
#include <stdexcept>

//*************************************************************************
//*************************************************************************
//* BulkDataPort Implementation.
//*************************************************************************
//*************************************************************************
A2300::BulkDataPort::BulkDataPort( byte epidIn, byte epidOut)
: PortBase( epidIn, epidOut)
    , m_timeout(10)
	, m_pCypressDevice( NULL )
	, m_pEndPointIn( NULL )
	, m_pEndPointOut( NULL )
{
	TransferContextList::iterator m_iterNextRead = m_listReadContexts.begin();
	TransferContextList::iterator m_iterNextWrite = m_listWriteContexts.begin();
}

A2300::BulkDataPort::~BulkDataPort()
{
}

int A2300::BulkDataPort::OnInit()
{
	return(0);
}

/**
 *
 */
void A2300::BulkDataPort::Close( )
{
	//Tear down endpoint input
	m_pEndPointIn = NULL; //Is delete needed?

	//Tear down endpoint output
	m_pEndPointOut = NULL; //Is delete needed?

	//Clear out the read and write transfer contexts.
	//Note the user must make sure they delete everything.
	m_listReadContexts.clear();
	m_listWriteContexts.clear();
}

/**
 *
 */
void A2300::BulkDataPort::Open( )
{
	if( !IsInitialized() )
		throw std::runtime_error("Port must be initialized before Open().");

	m_pCypressDevice = CyUSBDevice();

	// Enumerate the EP to find the match, skip EP0 (control EP)
	int epCnt = m_pCypressDevice->EndPointCount();

	for (int e = 1; e < epCnt; e++)
	{
		if( m_pCypressDevice->EndPoints[e]->Attributes != 2)
			continue; // not bulk end point

		if ( (m_pCypressDevice->EndPoints[e]->Address) == epidIn())
		{
			m_pEndPointIn = (CCyBulkEndPoint *)m_pCypressDevice->EndPoints[e];
		}

		if ( (m_pCypressDevice->EndPoints[e]->Address) == epidOut())
		{
			m_pEndPointOut = (CCyBulkEndPoint *)m_pCypressDevice->EndPoints[e];
		}
	}

	if ((epidIn() > 0 && m_pEndPointIn == NULL) || 
		(epidOut() > 0 && m_pEndPointOut == NULL))
	{
		// Exception error.
		throw std::runtime_error("Unable to open port.");
	}

	//Initialize the context lists.
	m_iterNextRead  = m_listReadContexts.end();
	m_iterNextWrite = m_listWriteContexts.end();
}

A2300::BulkDataPort::TransferContext*	A2300::BulkDataPort::CreateReadTransferContext(byte* bufFrame, size_t sizeFrame)
{
	TransferContext* pctxt = new TransferContext(this, 
		this->m_pEndPointIn, bufFrame, sizeFrame);
	m_listReadContexts.push_back(pctxt);
	return pctxt;

}
A2300::BulkDataPort::TransferContext*	A2300::BulkDataPort::CreateWriteTransferContext(byte* bufFrame, size_t sizeFrame)
{
	TransferContext* pctxt = new TransferContext(this, 
		this->m_pEndPointOut, bufFrame, sizeFrame);
	m_listWriteContexts.push_back(pctxt);
	return pctxt;
}



/**
 * Read the specified number of bytes from the port.
 */
int A2300::BulkDataPort::Read( byte * pdata, int ctBytes, int msecTimeout )
{
	LONG ctBytesRead = ctBytes;
	//Do not do anything if we cannot read 
	if( m_pCypressDevice == NULL || m_pEndPointIn == NULL)
		return -1;

 	m_pEndPointIn->TimeOut = msecTimeout;

	if( !m_pEndPointIn->XferData(pdata, ctBytesRead) ) // bytesRead is the actual bytes read after return
	{
		return(0);
	}
	return (int)ctBytesRead;
}

/**
 * Writes the specified number bytes to the port.
 */
int A2300::BulkDataPort::Write(byte * pdata, int ctBytes, int msecTimeout )
{
	LONG ctBytesWrote = (LONG)ctBytes;
	//Do not do anything if we cannot read 
	if( m_pCypressDevice == NULL || m_pEndPointOut == NULL)
		return -1;

	m_pEndPointOut->TimeOut = msecTimeout;
	CCyIsoPktInfo info;
	if (!m_pEndPointOut->XferData(pdata, ctBytesWrote, &info)) // bytesRead is the actual bytes read after return
	{

		return(0);
	}
	//printf(" bytesWritten= %d\n", ctBytesWrote);
	return (int)ctBytesWrote;
}


/**
* Win32 interface, waits for a read transfer event and initiates callbacks 
*/
A2300::BulkDataPort::TransferContext&  A2300::BulkDataPort::WaitForReadTransferEvent(int timeout)
{
	//Make sure we are initialized properly
	//and have the next transfer context.
	if( m_iterNextRead == m_listReadContexts.end() )
	{
		m_iterNextRead = m_listReadContexts.begin();
		if( m_listReadContexts.empty())
			throw std::runtime_error("BulkDataPort aynschronous read transfer not properly initialized. No transfer contexts defined.");
	}

	//Wait for the next transfer context to become available.
	//Finish the transfer accordingly.
	TransferContext& ctxt = *(*m_iterNextRead);

	ctxt.bCompleted = ctxt.WaitForTransfer( timeout);
	if(ctxt.bCompleted)
	{
		ctxt.FinishTransfer();
	}
	else
	{
		ctxt.Cancel();
	}

	//Fire callback 
	m_evtRead( &ctxt);

	//Set up for next read.
	m_iterNextRead++;

	return ctxt;
}

/**
* Win32 interface, waits for a write transfer event and initiates callbacks 
*/
A2300::BulkDataPort::TransferContext&  A2300::BulkDataPort::WaitForWriteTransferEvent(int timeout)
{
	//Make sure we are initialized properly
	//and have the next transfer context.
	if( m_iterNextWrite == m_listWriteContexts.end() )
	{
		m_iterNextWrite = m_listWriteContexts.begin();
		if( m_listWriteContexts.empty())
			throw std::runtime_error("BulkDataPort aynschronous write transfer not properly initialized. No transfer contexts defined.");
	}

	//Wait for the next transfer context to become available.
	//Finish the transfer accordingly.
	TransferContext& ctxt = *(*m_iterNextWrite);

	ctxt.bCompleted = ctxt.WaitForTransfer( timeout);
	if(ctxt.bCompleted)
	{
		ctxt.FinishTransfer();
	}
	else
	{
		ctxt.status = -1; // timeout.
		ctxt.Cancel();
	}

	//Fire callback 
	m_evtWrite( &ctxt);

	//Set up for next write.
	m_iterNextWrite++;

	return ctxt;
}
