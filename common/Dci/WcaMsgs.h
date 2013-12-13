/** Name: WcaMsgs.h
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

#ifndef WcaMsgs_h
#define WcaMsgs_h

#include "DciMsg.h"

#if defined( WIN32) || defined(LINUX) || defined(APPLE)
	#pragma pack(push, 1)
#endif

#ifdef  __cplusplus
extern "C" {
#endif

/* WCA messages supported. */
#define Dci_WcaCategoryId 				0x21
#define Dci_TypedProperties_Id			0x01
#define Dci_TypedPropertiesQuery_Id		0x81
#define Dci_ExecuteAction_Id			0x02
#define Dci_BinaryImageTransfer_Id		0x03
#define Dci_BinaryImageTransferFrame_Id	0x04
#define Dci_BinaryImageTransferQuery_Id	0x83
#define Dci_HalEventNotification_Id		0x05
#define Dci_BinaryImageTransferStatus_Id 0x06

#define WCA_COMPONENT_INDEX ((byte)4)
/*****************************************************************************/
/*****************************************************************************/
/**
* Structure defines a typed property used in the TypedProperties message (21,01).
*/
typedef struct Dci_Property
{
	byte idprop;
	byte idtype;
	union {
		byte	vByte;
		uint16	vUint16;
		int16	vInt16;
		uint32	vUint32;
		int32	vInt32;
		float	vFloat;
		double	vDouble;
#if 0
		uint64	vUint64;
		int64	vInt64;
#endif
		byte    data[8];
	} value;
} Dci_Property;

/**
* Defines the acceptable property types
*/
enum Dci_PropertyTypeEnum
{
	PT_BYTE = 0,
	PT_UINT16 = 1,
	PT_INT16 = 2,
	PT_UINT32 = 3,
	PT_INT32 = 4,
	PT_FLOAT = 5,
	PT_DOUBLE = 6,
	PT_UINT64 = 7,
	PT_INT64 = 8
};

/**
* Defines property query (21,81)flags
*/
enum Dci_PropertyQueryFlags
{
	PQF_TYPEIDS_VALID =  0x1
};

/**
* Structure for typed properties (21,01) messages.
*/
typedef struct Dci_TypedProperties
{
	Dci_Hdr	hdr;
	byte		idComponent;
	byte		ctProperties;
} Dci_TypedProperties;

/**
* Initializes the TypedProperties(21,01) message.  Be sure to provide a buffer large enough to accommodate
* the length of the data.  Returns the total size of the message.
*/
int Dci_TypedProperties_Init( void* buff, uint16 sizeBuff,
		byte idComponent, byte ctProperties, Dci_Property* pProps );

/**
* Function calculates the length of the TypedProperties message from
* internal configuration.
*/
int Dci_TypedProperties_MsgLength( Dci_TypedProperties* pmsg);

/**
* Returns the Dci_Property array.
*/
Dci_Property* Dci_TypedProperties_GetProperties( Dci_TypedProperties* pmsg);

/*****************************************************************************/
/*****************************************************************************/

/**
* Queries for the specified TypedProperties (21,81).
*/
typedef struct Dci_TypedPropertiesQuery
{
	Dci_Hdr	hdr;
	byte		idComponent;
	byte		flags;
	byte		ctProperties;
} Dci_TypedPropertiesQuery;

/**
* Initializes the TypedPropertiesQuery (21,81) message.  Be sure to provide a buffer large enough to accomodate
* the length of the data.  Ok to pass aPropIds or aTypeIds as NULL, they are not initialized in that case.
* Length of message is increased by the number of prop ids and type Ids if they are defined.
* otherwise, message is specified as the size of the TypedPropertiesFixed properties
* by default. The caller must specify the length of data and type ids.
* Returns the total size of the message.
*/
int Dci_TypedPropertiesQuery_Init( void* buff, uint16 sizeBuff,
		byte idComponent, byte ctProperties, const byte* aPropIds, const byte* aTypeIds);

/**
* Returns the property ids specified in the TypedPropertiesQuery.
*/
byte* Dci_TypedPropertiesQuery_GetPropIds( Dci_TypedPropertiesQuery* pmsg);

/**
* Returns the propery type ids if specified in the flags.  Otherwise this
* returns NULL.
*/
byte* Dci_TypedPropertiesQuery_GetTypeIds( Dci_TypedPropertiesQuery* pmsg);

/*****************************************************************************/
/*****************************************************************************/

/**
* Queries for the specified ExecuteAction (21,02).
*/
typedef struct Dci_ExecuteAction
{
	Dci_Hdr	hdr;
	byte		idComponent;
	byte		idAction;
	uint16		lenData;
} Dci_ExecuteAction;

/**
* Initializes the ExecuteAction message (21,02).  Be sure to provide a buffer large enough to accomodate
* the length of the data.  Returns the total size of the message.
*/
int Dci_ExecuteAction_Init( void* buff, uint16 sizeBuff,
		byte idComponent, byte idAction, byte lenData, byte* pdata);

/**
* Returns the action data specified in the ExecuteAction message.
*/
byte* Dci_ExecuteAction_GetData( Dci_ExecuteAction* pmsg);

/*****************************************************************************/
/*****************************************************************************/

/**
* The message specifies (21,03) binary image transfer information, which is sent prior to uploading or
* downloading binary image frames. It specifies information about the image, the transfer size and
* number of frames that will be transferred.
*/
typedef struct Dci_BinaryImageTransfer
{
	Dci_Hdr	hdr;
	byte		idComponent;
	byte		flags;
	char		szName[32];
	char		szDescription[32];
	uint32		sizeImg;
	uint16		sizeFrame;
	uint32		ctFrames;
	byte		idTransfer;
} Dci_BinaryImageTransfer;

/**
* Initializes the BinaryImageTransfer (21,03) message.
*/
int Dci_BinaryImageTransfer_Init( void* buff,
			 byte idComponent,    byte flags,   pcstr  szName,
			 pcstr szDescription, uint32  sizeImg, uint16 sizeFrame,
			 uint32 ctFrames, byte idTransfer );

/**
* Base class message for Binary Image Transfer Frames (21,04)
* This message is a frame of a binary image transfer. A series
* of these frames are sent to transfer a  binary image of data
* from the source to the target. A transfer id is provided to
* uniquely identify the transfer operation.
*/
typedef struct Dci_BinaryImageTransferFrame
{
	Dci_Hdr	hdr;
	byte		idComponent;
	byte		idTransfer;
	uint16		flags;
	uint32		idFrame;
	uint16		ctBytes;
} Dci_BinaryImageTransferFrame;

/**
* Initializes the BinaryImageTransfer (21,04) message.
*/
int Dci_BinaryImageTransferFrame_Init( void* buff, byte idComponent,
			 byte idTransfer,    byte flags,   uint32 idFrame,
			 uint16 ctBytes, byte* pdata);

/**
* Returns the action data specified in the ExecuteAction message.
*/
byte* Dci_BinaryImageTransferFrame_GetData( Dci_BinaryImageTransferFrame* pmsg);

/*****************************************************************************/
/*****************************************************************************/

/**
* BIT status identifiers.
*/
enum Dci_BitStatusEnum
{
    BSE_InitiatingTransfer = 0,
    BSE_TransferComplete = 1,
    BSE_ReadyNextFrame = 2,
    BSE_FrameError = 3,
    BSE_WriteError = 4,
    BSE_ReadError = 5,
    BSE_OperationNotAvailable =6,
	BSE_OperationCancelled = 7
};

/**
* Target sends the BitStatus (21,06) message in response to Info, and Frame messages.
*/
typedef struct Dci_BinaryImageTransferStatus
{
	Dci_Hdr	hdr;
	byte 		idComponent;
	byte 	  	idTransfer;
	uint16		idFrame;
	uint32		ctTransferred;
	byte 	  	idStatus;
} Dci_BinaryImageTransferStatus;

/**
* Initializes the Dci_BinaryImageTransferStatus structure
*/
int Dci_BinaryImageTransferStatus_Init( void* buff,
		byte idComponent, byte idTransfer, uint16 idFrame,
		uint32 ctTransferred, byte idStatus);

/*****************************************************************************/
/*****************************************************************************/

/**
* Requested device responds with a Binary Image Transfer Information (21,03) and Binary Image
* Transfer Frames (21,83) if specified.
*/
typedef struct Dci_BinaryImageTransferQuery
{
	Dci_Hdr	hdr;
	byte		idComponent;
	byte		idTransfer;
	byte		flags;
} Dci_BinaryImageTransferQuery;

/**
* Initializes the Dci_BinaryImageTransferQuery (21,83)
*/
int Dci_BinaryImageTransferQuery_Init( void* buff,
		byte idComponent, byte idTransfer, byte flags);

/*****************************************************************************/
/*****************************************************************************/

/**
* Asynchronous event notification of a WCA HAL triggered event (21,05).
*/
typedef struct Dci_HalEventNotification
{
	Dci_Hdr	hdr;
	byte idComponent;
	byte idEvent;
	uint16 flags;
	uint32 info;
} Dci_HalEventNotification;

/**
* Initializes the Dci_HalEventNotification message (21,05)
*/
int Dci_HalEventNotification_Init( void* buff,
		byte idComponent, byte idEvent, uint16 flags, uint32 info);

#ifdef __cplusplus
}
#endif

#if defined( WIN32) || defined(LINUX) || defined(APPLE)
	#pragma pack(pop)
#endif

#endif /* WcaMsgs_h */
