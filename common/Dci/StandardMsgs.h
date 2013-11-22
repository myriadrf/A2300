/** Name: StandardMsgs.h
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

#ifndef StandardMsgs_h
#define StandardMsgs_h

#include "Dci/DciMsg.h"

#if defined( WIN32) || defined(LINUX) || defined(APPLE)
	#pragma pack(push, 1)
#endif

#ifdef  __cplusplus
extern "C" {
#endif

/* Standard DCI Messages defined */
#define Dci_DeviceInfo_Id				0x0200
#define Dci_DeviceInfoQuery_Id			0x0280

#define Dci_Reset_Id					0x2001
#define Dci_VersionInfo_Id				0x2002
#define Dci_VersionInfoQuery_Id			0x2082
#define Dci_StringProperties_Id			0x2005
#define Dci_StringPropertiesQuery_Id	0x2085
#define Dci_TypedDataRecord_Id			0x2013
#define Dci_TypedDataRecordQuery_Id		0x2093

/* Time sync messages not currently implemented. */

/**
* Reset a Device (20,01)
*/
int Dci_Reset_Init( void* buff);

/**
* Report Version Information (20,02)
*/
typedef struct Dci_VersionInfo
{
	Dci_Hdr hdr;
	byte	VerMajor;
	byte	VerMinor;
	byte	VerMaintenance;
	uint16  Revision;
} Dci_VersionInfo;

/**
* Initializes VersionInfo Message (20,02).
*/
int Dci_VersionInfo_Init( void* buff, byte vMajor, byte vMinor, byte vMaint, uint16 rev);

/**
* Queries for Version Information (20,82)
*/
int Dci_VersionInfoQuery_Init( void* buff);

/** <summary>
* DCI Message for Setting and getting string converted properties (20,05)
* </summary>
*/
typedef struct Dci_StringProperties
{
	Dci_Hdr  hdr;
	uint16		flags;
	uint16		len;
} Dci_StringProperties;

/**
* Initializes the StringProperties message.  Be sure to provide a buffer large enough to accomodate
* the length of the string message.  Returns the total size of the message.
*/
int Dci_StringProperties_Init( void* buff, uint16 sizeBuff, uint16 flags, pcstr pszMessage);

/**
* Returns the string message contained within the DCI debug message.
*/
pcstr Dci_StringProperties_GetProperties( Dci_StringProperties* pmsg);

/**
* QueryStringProperties (20,85)
*/
int Dci_StringPropertiesQuery_Init( void* buff );

/** <summary>
* Message reports typed binary data (20,13).  TypeId provides means to determine the binary encoding scheme
* used.   This is a generic message to provide easy transport of various types of data.
* </summary>
*/
typedef struct Dci_TypedDataRecord
{
	Dci_Hdr hdr;
	uint16 week;
	int32 secWhole;
	int16 secFrac;
	uint32 timecode;
	uint16 idtype;
	uint16 flags;
	uint16 lenData;
} Dci_TypedDataRecord;

/**
* Initializes the TypedDataRecord (20,13) message.  Be sure to provide a buffer large enough to accommodate
* the length of the data.  Returns the total size of the message.
*/
int Dci_TypedDataRecord_Init( void* buff, uint16 sizeBuff,
		uint16 week, double sec,
		uint32 timecode,  uint16 idtype,
		uint16 flags, uint16 lenData, void* pdata);

/**
* Returns the TypedDataRecord (20,13) data contained.
*/
byte* Dci_TypedDataRecord_GetData( Dci_TypedDataRecord* pmsg);

/**
* Queries for the specified TypedDataRecord(20,93)
*/
typedef struct Dci_TypedDataRecordQuery
{
	Dci_Hdr hdr;
	uint16 idtype;
	uint16 flags;
} Dci_TypedDataRecordQuery;

/**
* Queries for the specified TypedDataRecord(20,93)
*/
int Dci_TypedDataRecordQuery_Init( void* buff, uint16 idtype, uint16 flags);

#ifdef __cplusplus
}
#endif

#if defined( WIN32) || defined(LINUX) || defined(APPLE)
	#pragma pack(pop)
#endif

#endif
