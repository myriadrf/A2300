// Name: WcaMsgs.c
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

#include "WcaMsgs.h"
#include "string.h"

#define min( a,b) (((a) < (b)) ? (a) : (b))
/**
* Initializes the TypedProperties(21,01) message.  Be sure to provide a buffer large enough to accomodate
* the length of the data.  Returns the total size of the message.
*/
int Dci_TypedProperties_Init( void* buff, uint16 sizeBuff, 
		byte idComponent, byte ctProperties, Dci_Property* pProps)
{
	Dci_TypedProperties* pmsg;	
	int lenData;
	int lenHdr;
	int lenMsg;

	if( pProps != NULL )
	{
		// Compute the Properities length.
		lenData = ctProperties * sizeof( Dci_Property);
		lenHdr = sizeof(Dci_TypedProperties);
		lenMsg = lenHdr + lenData;

		// Make sure it will fit in the output buffer.
		if( lenMsg <= sizeBuff )
		{
			pmsg = (Dci_TypedProperties*) buff;	
			Dci_Hdr_Init( buff, 0x21, 0x01);
			pmsg->idComponent  = idComponent;
			pmsg->ctProperties = ctProperties;
			memcpy((byte*)buff+lenHdr, pProps, lenData);
			return (lenMsg);
		}
	}
	return(0);
}

/**
* Function calculates the length of the TypedProperties message from
* internal configuration.
*/
int Dci_TypedProperties_MsgLength( Dci_TypedProperties* pmsg)
{
	int lenData = pmsg->ctProperties * sizeof(Dci_Property);
	return sizeof( Dci_TypedProperties ) + lenData;
}


/**
* Returns the Dci_Property array.
*/
Dci_Property* Dci_TypedProperties_GetProperties( Dci_TypedProperties* pmsg)
{
	// Data is appended at end of header.
	byte *pData = (byte*)pmsg + sizeof(Dci_TypedProperties);
	return (Dci_Property *)pData;
}



//*****************************************************************************
//*****************************************************************************

/**
* Initializes the TypedPropertiesQuery (21,81) message.  Be sure to provide a buffer large enough to accomodate
* the length of the data.  Returns the total size of the message.
*/
int Dci_TypedPropertiesQuery_Init( void* buff, uint16 sizeBuff, 
		byte idComponent, byte ctProperties, byte* aPropIds)
{
	Dci_TypedPropertiesQuery* pmsg;	
	int lenData;
	int lenHdr = sizeof(Dci_TypedPropertiesQuery);
	int lenMsg;

	lenData = ctProperties ;
	pmsg	= (Dci_TypedPropertiesQuery*) buff;
	lenMsg  = lenHdr + lenData;
	if( lenMsg <= sizeBuff )
	{	
		Dci_Hdr_Init( buff, 0x21, 0x81);
		pmsg->idComponent  = idComponent;
		pmsg->ctProperties = ctProperties;
		memcpy( (byte*)buff+lenHdr, aPropIds, lenData);
		return(lenMsg);
	}
	return(0);
}


/**
* Returns the property ids specified in the TypedPropertiesQuery.
*/
byte* Dci_TypedPropertiesQuery_GetPropIds( Dci_TypedPropertiesQuery* pmsg)
{
	// Data is appended at end of header.
	return( (byte*)pmsg + sizeof(Dci_TypedPropertiesQuery) );
}

//*****************************************************************************
//*****************************************************************************

/**
* Initializes the ExecuteAction message (21,02).  Be sure to provide a buffer large enough to accomodate
* the length of the data.  Returns the total size of the message.
*/
int Dci_ExecuteAction_Init( void* buff, uint16 sizeBuff, 
		byte idComponent, byte idAction, byte lenData, byte* pdata)
{
	Dci_ExecuteAction* pmsg = (Dci_ExecuteAction*) buff;
	int lenHdr = sizeof(Dci_ExecuteAction);
	int lenMsg;
	lenMsg = lenHdr + lenData;
	
	if( lenMsg <= sizeBuff )
	{
		Dci_Hdr_Init( buff, 0x21, 0x02);
		pmsg->idComponent  = idComponent;
		pmsg->idAction  = idAction;
		pmsg->lenData   = lenData;
		memcpy( ((byte*)buff+lenHdr), pdata, lenData);
		return(lenMsg);
	}
	return(0);
}

/**
* Returns the action data specified in the ExecuteAction message.
*/
byte* Dci_ExecuteAction_GetData( Dci_ExecuteAction* pmsg)
{
	// Data is appended at end of header.
	return( (byte*)pmsg + sizeof(Dci_ExecuteAction) );
}

//*****************************************************************************
//*****************************************************************************

/**
* Initializes the BinaryImageTransfer (21,03) message.
*/
int Dci_BinaryImageTransfer_Init( void* buff, 
			 byte idComponent,    byte flags,   pcstr  szName,
			 pcstr szDescription, uint32  sizeImg, uint16 sizeFrame,
			 uint32 ctFrames, byte idTransfer )
{
	Dci_BinaryImageTransfer* pmsg;	
	pmsg	 = (Dci_BinaryImageTransfer*) buff;
	
	Dci_Hdr_Init( buff, 0x21, 0x03);

	pmsg->idComponent			= idComponent;
	pmsg->flags					= flags;
	strncpy( pmsg->szName, szName, 32);
	strncpy( pmsg->szDescription, szDescription, 32);
	pmsg->sizeImg				= sizeImg;
	pmsg->sizeFrame				= sizeFrame;
	pmsg->ctFrames				= ctFrames;
	pmsg->idTransfer			= idTransfer;
	return sizeof( Dci_BinaryImageTransfer);
}


/**
* Initializes the BinaryImageTransfer (21,04) message. Returns the size
* of the message in bytes.
*/
int Dci_BinaryImageTransferFrame_Init( void* buff, byte idComponent,
			 byte idTransfer,    byte flags,   uint32 idFrame,
			 uint16 ctBytes, byte* pdata)
{
	Dci_BinaryImageTransferFrame* pmsg = (Dci_BinaryImageTransferFrame*) buff;
	int lenHdr = sizeof(Dci_BinaryImageTransferFrame);
	
	if( pdata != NULL )
	{
		Dci_Hdr_Init( buff, 0x21, 0x04);
		pmsg->idTransfer  = idTransfer;
		pmsg->flags		  = flags;
		pmsg->idFrame	  = idFrame;
		pmsg->ctBytes	  = ctBytes;
		pmsg->idComponent = idComponent;  // CJC This was missing.
		memcpy( (byte*)buff+lenHdr, pdata, ctBytes);
	}
	return (lenHdr + ctBytes);
}


/**
* Returns the action data specified in the ExecuteAction message.
*/
byte* Dci_BinaryImageTransferFrame_GetData( Dci_BinaryImageTransferFrame* pmsg)
{
	// Data is appended at end of header.
	return( (byte*)pmsg + sizeof(Dci_BinaryImageTransferFrame) );
}

//*****************************************************************************
//*****************************************************************************

/** 
* Initializes the Dci_BinaryImageTransferQuery (21,06)
*/
int Dci_BinaryImageTransferStatus_Init( void* buff, 
		byte idComponent, byte idTransfer, uint16 idFrame,
		uint32 ctTransferred, byte idStatus)
{
	Dci_BinaryImageTransferStatus* pmsg;	
	Dci_Hdr_Init( buff, 0x21, 0x06);

	pmsg	 = (Dci_BinaryImageTransferStatus*) buff;
	
	pmsg->idComponent			= idComponent;
	pmsg->idTransfer			= idTransfer;
	pmsg->idFrame				= idFrame;
	pmsg->ctTransferred			= ctTransferred;
	pmsg->idStatus				= idStatus;
	return sizeof( Dci_BinaryImageTransferStatus);

}


//*****************************************************************************
//*****************************************************************************

/** 
* Initializes the Dci_BinaryImageTransferQuery (21,83)
*/
int Dci_BinaryImageTransferQuery_Init( void* buff, 
		byte idComponent, byte idTransfer, byte flags)
{
	Dci_BinaryImageTransferQuery* pmsg;	
	pmsg	 = (Dci_BinaryImageTransferQuery*) buff;
	
	Dci_Hdr_Init( buff, 0x21, 0x83);

	pmsg->idComponent			= idComponent;
	pmsg->idTransfer			= idTransfer;
	pmsg->flags					= flags;
	return sizeof( Dci_BinaryImageTransferQuery);
}

//*****************************************************************************
//*****************************************************************************

/** 
* Initializes the Dci_HalEventNotification message (21,05)
*/
int Dci_HalEventNotification_Init( void* buff, 
		byte idComponent, byte idEvent, uint16 flags, uint32 info)
{
	Dci_HalEventNotification* pmsg;	
	pmsg	 = (Dci_HalEventNotification*) buff;
	
	Dci_Hdr_Init( buff, 0x21, 0x05);

	pmsg->idComponent			= idComponent;
	pmsg->idEvent				= idEvent;
	pmsg->flags					= flags;
	pmsg->info					= info;
	return sizeof( Dci_BinaryImageTransfer);
}



