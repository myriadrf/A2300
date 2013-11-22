/** Name: InfrastructureMsgs.h
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

#ifndef InfrastructureMsgs_h
#define InfrastructureMsgs_h

#include "Dci/DciMsg.h"

#if defined( WIN32) || defined(LINUX) || defined(APPLE)
	#pragma pack(push, 1)
#endif

#ifdef  __cplusplus
extern "C" {
#endif

/* Infrastructure messages defined. */
#if 0
/* Defined in DciMsg.h */
#define Dci_IdleMsg_Id 		0x0100
#define Dci_DebugMsg_Id 	0x0501
#endif

#define Dci_IdentifyDeviceQuery_Id	0x0280
#define Dci_IdentifyDevice_Id		0x0200
#define Dci_MessageError_Id			0x0301

/*
* IdleMsg (0x0100)
*/
int Dci_IdleMsg_Init( void* buff );

/**
* IdentifyDeviceQuery (0x0280)
*/
int Dci_IdentifyDeviceQuery_Init( void* buff );

/**
* IdentifyDevice  (0x0201)
*/
typedef struct Dci_IdentifyDevice
{
	Dci_Hdr hdr;
	char DeviceId[32];
	char SerialNumber[32];
	char Model[32];
} Dci_IdentifyDevice;

/**
* Initializes the Identify Device Message.
*/
int Dci_IdentifyDevice_Init( void* buff, pcstr szDeviceId,
								pcstr szSerialNumber, pcstr szModel);

/**
* MessageError Class  (0x0301)
*/
typedef struct Dci_MessageError
{
	Dci_Hdr hdr;
	byte UnrecognizedCategoryId;
	byte UnrecognizedTypeId;
} Dci_MessageError;

int Dci_MessageError_Init( void* buff, byte idCategory, byte idType );
int Dci_MessageError_Init1( void* buff, Dci_Hdr *pBadMsg);

/*************************************
* Standard Debug Text Message Class (0x0501)
************************************/

enum { Dci_ERR = 0, Dci_WARN, Dci_INFO, Dci_DEBUG   };
typedef struct Dci_DebugMsg
{
	Dci_Hdr hdr;
	byte   status;
	uint16 src;
	uint16 len;
} Dci_DebugMsg;

/**
* Initializes the debug message.  Be sure to provide a buffer large enough to accomodate
* the length of the string message.  Returns the total size of the message.
*/
int Dci_DebugMsg_Init( void* buff, uint16 sizeBuff, byte status,
						 uint16 src, pcstr szMessage);

/**
* Returns the string message contained within the DCI debug message.
*/
pcstr Dci_DebugMsg_Message( Dci_DebugMsg* pmsg);

#ifdef __cplusplus
}
#endif

#if defined( WIN32) || defined(LINUX) || defined(APPLE)
	#pragma pack(pop)
#endif

#endif /* INFRASTRUCTURE_MSGS_H */
