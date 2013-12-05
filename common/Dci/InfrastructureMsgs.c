/** Name: InfrastructureMsgs.c
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

#include "Dci/InfrastructureMsgs.h"
#include <string.h>

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

/*
* IdleMsg (0x0100)
*/
int Dci_IdleMsg_Init( void* buff )
{
	return Dci_Hdr_Init( buff, 0x01, 0x00);
}

/**
* IdentifyDeviceQuery (0x0280)
*/
int Dci_IdentifyDeviceQuery_Init( void* buff )
{
	return Dci_Hdr_Init( buff, 0x02, 0x80);
}
/**
* Initializes the Identify Device Message (02,01).
*/
int Dci_IdentifyDevice_Init( void* buff, pcstr szDeviceId, pcstr szSerialNumber, pcstr szModel)
{
	Dci_IdentifyDevice* pmsg = ( Dci_IdentifyDevice*) buff;
	memset( pmsg, 0, sizeof( Dci_IdentifyDevice));
	Dci_Hdr_Init( buff, 0x02, 0x00);
	strncpy( pmsg->DeviceId, szDeviceId, 32);
	strncpy( pmsg->SerialNumber, szSerialNumber, 32);
	strncpy( pmsg->Model, szModel, 32);
	return (int) sizeof( Dci_IdentifyDevice);
}

/**
* Initializes the MessageError Message (03,01).
*/
int Dci_MessageError_Init( void* buff, byte idCategory, byte idType )
{
	Dci_MessageError* pmsg = (Dci_MessageError*) buff;
	Dci_Hdr_Init( buff, 0x03, 0x01);
	pmsg->UnrecognizedCategoryId = idCategory;
	pmsg->UnrecognizedTypeId = idType;
	return (int) sizeof( Dci_MessageError);
}

int Dci_MessageError_Init1( void* buff, Dci_Hdr *pBadMsg)
{
	Dci_MessageError* pmsg = (Dci_MessageError*) buff;
	Dci_Hdr_Init( buff, 0x03, 0x01);
	pmsg->UnrecognizedCategoryId = pBadMsg->idCategory;
	pmsg->UnrecognizedTypeId     = pBadMsg->idType;
	return (int) sizeof( Dci_MessageError);
}

/**
* Initializes the debug message (05,01).  Be sure to provide a buffer large enough to accomodate
* the length of the string message.  Returns the total size of the message.
*/
int Dci_DebugMsg_Init( void* buff, uint16 sizeBuff, byte status,
						 uint16 src, pcstr szMessage)
{
	Dci_DebugMsg* pmsg = (Dci_DebugMsg*) buff;
	Dci_Hdr_Init( buff, 0x05, 0x01);
	pmsg->status = status;
	pmsg->src    = src;
	pmsg->len    = MIN( sizeBuff, (uint16)strlen( szMessage));
	strncpy(  (((char*)&(pmsg->len))+sizeof(uint16)), szMessage, pmsg->len);
	return (int) (sizeof( Dci_DebugMsg) + pmsg->len);
}

/**
* Returns the string message contained within the DCI debug message.
*/
pcstr Dci_DebugMsg_Message( Dci_DebugMsg* pmsg)
{
	pstr szMsg = (((char*) &(pmsg->len)) + sizeof(uint16));
	szMsg[ pmsg->len] = '\0';

	return szMsg;
}
