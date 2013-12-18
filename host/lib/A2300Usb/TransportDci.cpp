/** Name: TransportDci.cpp
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

#include <A2300/TransportDci.h>
#include <stdio.h>
#include <string.h>

#define COUNT_MAX_MESSAGES 	(5)
#define SIZE_MSGBUFF (COUNT_MAX_MESSAGES*DCI_MAX_MSGSIZE)

#define TO_MSEC(A) ((int)((A)*((double)1000.0)))

const double A2300::TransportDci::DEFAULT_TIMEOUT_VALUE = 0.1;
const int A2300::TransportDci::USE_DEFAULT_TIMEOUT = -1;

A2300::TransportDci::TransportDci()
	: m_pPort(NULL), m_timeout( 0.1), m_pConv(NULL) //, m_pbuff(NULL)
{

}

A2300::TransportDci::~TransportDci()
{
//	if( m_pbuff != NULL) delete m_pbuff;
}

int A2300::TransportDci::Init(A2300::BulkDataPort* pPort, double timeout, Dci_Conversation* pconv)
{
	m_pPort = pPort;
	m_timeout = timeout;
	m_pConv = pconv;

	/* TODO Implement Asynchronous Messaging

		m_pbuff = new byte[SIZE_MSGBUFF];
		//Initialize the asynchronous processing.
		m_pPort->ReadTransfer() = BulkDataPort::TransferEvent( static_cast<void*>(this),
				&Delegate<void, TransportDci, BulkDataPort::TransferContext*, &TransportDci::OnMessageReceived>);

		//Initialize the buffers and queue for processing.
		for( size_t i = 0; i < COUNT_MAX_MESSAGES; i++)
		{
			BulkDataPort::TransferContext* pctxt = m_pPort->CreateReadTransferContext(
					m_pbuff + i*DCI_MAX_MSGSIZE, DCI_MAX_MSGSIZE);
			pctxt->Submit();
		}
	*/
	return 0;
}
void A2300::TransportDci::Term()
{
	//Nothing todo currently.
}

/**
* Translates debug message to a string.
*/
std::string A2300::TransportDci::DebugMsgToString( Dci_DebugMsg* pmsg)
{
	static const char* pszStatus[]={"ERR", "WRN", "INF", "DBG"};
	char buff[320];
	pcstr	szMsg = Dci_DebugMsg_Message( pmsg);
	int		ct	  = sprintf( buff, "%s (id=%02Xh): %s", 
						pszStatus[pmsg->status], pmsg->src, szMsg);
	return std::string(buff,(size_t)ct);
}

/**
 * Send a DCI Message
 */
int A2300::TransportDci::SendMsg( byte* pmsg, size_t lenMsg, bool bRequestAck, double timeout )
{
	Dci_Conversation_PrepareMessageHdr( m_pConv, (Dci_Hdr*) pmsg, bRequestAck);

	if(timeout == USE_DEFAULT_TIMEOUT)
		timeout = m_timeout;

	//Send the DCI command.
	return m_pPort->Write(pmsg, (int) lenMsg, TO_MSEC(timeout) );
}

/**
 * Receive DCI Message.
 */
int A2300::TransportDci::ReceiveMsg( byte* pmsg, size_t lenMax, double timeout)

{
	if(timeout == USE_DEFAULT_TIMEOUT)
		timeout = m_timeout;

	//Process DCI Response.
	int ctRead = m_pPort->Read(pmsg, (int) lenMax, TO_MSEC(timeout));

	if( ctRead > 0)
		Dci_Conversation_UpdateState( m_pConv, pmsg, (uint16) ctRead);
	return ctRead;
}

int A2300::TransportDci::GetProperty( byte idComponent, Dci_Property& prop, double timeout)
{
	byte buff[DCI_MAX_MSGSIZE];

	//Query Identify Device.
	int len = Dci_TypedPropertiesQuery_Init(buff, DCI_MAX_MSGSIZE, idComponent, 1, &(prop.idprop), &(prop.idtype) );

	Dci_Conversation_PrepareMessageHdr( m_pConv, (Dci_Hdr*) buff, false);

	//Send the DCI command.
	m_pPort->Write(buff, len, TO_MSEC(timeout));

	//Process DCI Response.
	int ctRead = m_pPort->Read(buff, DCI_MAX_MSGSIZE, TO_MSEC(timeout));

	if( ctRead > 0)
	{
		Dci_Conversation_UpdateState( m_pConv, buff, (uint16) ctRead);

		Dci_TypedProperties* ptp = (Dci_TypedProperties*) buff;

		//If we got back what we expected.
		if( Dci_Hdr_MatchesId( (Dci_Hdr *)buff, Dci_WcaCategoryId, Dci_TypedProperties_Id)
			&& ptp->idComponent == idComponent)
		{
			Dci_Property *pProp = Dci_TypedProperties_GetProperties(ptp);
			memcpy( prop.value.data, pProp->value.data, 8);
			return 0;
		}
	}
	return -1;
}

int A2300::TransportDci::SetProperty( byte idComponent, Dci_Property& prop, double timeout)
{
	byte buff[DCI_MAX_MSGSIZE];
	int len = Dci_TypedProperties_Init(buff, DCI_MAX_MSGSIZE, idComponent, 1, &prop );

	Dci_Conversation_PrepareMessageHdr( m_pConv, (Dci_Hdr*) buff, true);

	//Send the DCI command.
	int wresult = m_pPort->Write(buff, len, TO_MSEC(timeout));

	//Wait for Ack
	//Process DCI Response.
	int ctRead = m_pPort->Read(buff, DCI_MAX_MSGSIZE, TO_MSEC(timeout));

	return (ctRead > 0 ) ? wresult : -1;

}

template <> int A2300::TransportDci::GetProperty( byte idComponent, byte idProp, byte& value)
{
	Dci_Property prop;
 	prop.idtype = PT_BYTE;
	prop.idprop = idProp;
	int retval = GetProperty( idComponent, prop, m_timeout);
	value = prop.value.vByte;
	return retval;
}

template <> int A2300::TransportDci::GetProperty( byte idComponent, byte idProp, uint16& value)
{
	Dci_Property prop;
 	prop.idtype = PT_UINT16;
	prop.idprop = idProp;
	int retval = GetProperty( idComponent, prop, m_timeout);
	value = prop.value.vUint16;
	return retval;
}

template <> int A2300::TransportDci::GetProperty( byte idComponent, byte idProp, int16& value)
{
	Dci_Property prop;
 	prop.idtype = PT_INT16;
	prop.idprop = idProp;
	int retval = GetProperty( idComponent, prop, m_timeout);
	value = prop.value.vInt16;
	return retval;
}

template <> int A2300::TransportDci::GetProperty( byte idComponent, byte idProp, uint32& value)
{
	Dci_Property prop;
 	prop.idtype = PT_UINT32;
	prop.idprop = idProp;
	int retval = GetProperty( idComponent, prop, m_timeout);
	value = prop.value.vUint32;
	return retval;
}

template <> int A2300::TransportDci::GetProperty( byte idComponent, byte idProp, int32& value)
{
	Dci_Property prop;
 	prop.idtype = PT_INT32;
	prop.idprop = idProp;
	int retval = GetProperty( idComponent, prop, m_timeout);
	value = prop.value.vInt32;
	return retval;
}

template <> int A2300::TransportDci::GetProperty( byte idComponent, byte idProp, float& value)
{
	Dci_Property prop;
 	prop.idtype = PT_FLOAT;
	prop.idprop = idProp;
	int retval = GetProperty( idComponent, prop, m_timeout);
	value = prop.value.vFloat;
	return retval;
}

template <> int A2300::TransportDci::GetProperty( byte idComponent, byte idProp, double& value)
{
	Dci_Property prop;
 	prop.idtype = PT_DOUBLE;
	prop.idprop = idProp;
	int retval = GetProperty( idComponent, prop, m_timeout);
	value = prop.value.vDouble;
	return retval;
}

#if 0

template <> int A2300::TransportDci::GetProperty( byte idComponent, byte idProp, uint64& value)
{
	Dci_Property prop;
 	prop.idtype = PT_UINT64;
	prop.idprop = idProp;
	int retval = GetProperty( idComponent, prop, m_timeout);
	value = prop.value.vUint64;
	return retval;
}

template <> int A2300::TransportDci::GetProperty( byte idComponent, byte idProp, int64& value)
{
	Dci_Property prop;
 	prop.idtype = PT_INT64;
	prop.idprop = idProp;
	int retval = GetProperty( idComponent, prop, m_timeout);
	value = prop.value.vInt64;
	return retval;
}

#endif

template <> int A2300::TransportDci::SetProperty( byte idComponent, byte idProp, byte value)
{
	Dci_Property prop;
	prop.idtype = PT_BYTE;
	prop.idprop = idProp;
	prop.value.vByte = value;
	return SetProperty( idComponent, prop, m_timeout );
}

template <> int A2300::TransportDci::SetProperty( byte idComponent, byte idProp, uint16 value)
{
	Dci_Property prop;
	prop.idtype = PT_UINT16;
	prop.idprop = idProp;
	prop.value.vUint16 = value;
	return SetProperty( idComponent, prop, m_timeout );
}

template <> int A2300::TransportDci::SetProperty( byte idComponent, byte idProp, int16 value)
{
	Dci_Property prop;
	prop.idtype = PT_INT16;
	prop.idprop = idProp;
	prop.value.vInt16 = value;
	return SetProperty( idComponent, prop, m_timeout );
}

template <> int A2300::TransportDci::SetProperty( byte idComponent, byte idProp, uint32 value)
{
	Dci_Property prop;
	prop.idtype = PT_UINT32;
	prop.idprop = idProp;
	prop.value.vUint32 = value;
	return SetProperty( idComponent, prop, m_timeout );
}

template <> int A2300::TransportDci::SetProperty( byte idComponent, byte idProp, int32 value)
{
	Dci_Property prop;
	prop.idtype = PT_INT32;
	prop.idprop = idProp;
	prop.value.vInt32 = value;
	return SetProperty( idComponent, prop, m_timeout );
}

template <> int A2300::TransportDci::SetProperty( byte idComponent, byte idProp, float value)
{
	Dci_Property prop;
	prop.idtype = PT_FLOAT;
	prop.idprop = idProp;
	prop.value.vFloat = value;
	return SetProperty( idComponent, prop, m_timeout );
}

template <> int A2300::TransportDci::SetProperty( byte idComponent, byte idProp, double value)
{
	Dci_Property prop;
	prop.idtype = PT_DOUBLE;
	prop.idprop = idProp;
	prop.value.vDouble= value;
	return SetProperty( idComponent, prop, m_timeout );
}

#if 0

template <> int A2300::TransportDci::SetProperty( byte idComponent, byte idProp, uint64 value)
{
	Dci_Property prop;
	prop.idtype = PT_UINT64;
	prop.idprop = idProp;
	prop.value.vUint64 = value;
	return SetProperty( idComponent, prop, m_timeout );
}

template <> int A2300::TransportDci::SetProperty( byte idComponent, byte idProp, int64 value)
{
	Dci_Property prop;
	prop.idtype = PT_INT64;
	prop.idprop = idProp;
	prop.value.vInt64 = value;
	return SetProperty( idComponent, prop, m_timeout );
}

#endif
