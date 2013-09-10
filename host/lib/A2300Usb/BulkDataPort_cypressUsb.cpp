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
#if defined(WIN32)
	//Tear down endpoint input
	m_pEndPointIn = NULL; //Is delete needed?

	//Tear down endpoint output
	m_pEndPointOut = NULL; //Is delete needed?
#endif
}

/**
 *
 */
void A2300::BulkDataPort::Open( )
{
	if( !IsInitialized() )
		throw std::runtime_error("Port must be initialized before Open().");

#if defined(WIN32)
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
#endif
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
	if (!m_pEndPointOut->XferData(pdata, ctBytesWrote)) // bytesRead is the actual bytes read after return
	{
		return(0);
	}
	//printf(" bytesWritten= %d\n", ctBytesWrote);
	return (int)ctBytesWrote;
}
