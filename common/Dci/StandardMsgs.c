// Name: StandardMsgs.c
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

#include "StandardMsgs.h"
#include "string.h"

#ifndef MIN
#define MIN(A,B) (((A) <= (B)) ? (A) : (B))
#endif

/**
* Reset a Device (20,01)
*/	
int Dci_Reset_Init( void* buff) { return Dci_Hdr_Init( buff, 0x20, 0x01);}

/**
* Initializes VersionInfo Message (20,02).
*/
int Dci_VersionInfo_Init( void* buff, byte vMajor, byte vMinor, byte vMaint, uint16 rev)
{
	Dci_VersionInfo* pmsg = (Dci_VersionInfo*) buff;
	Dci_Hdr_Init( buff, 0x20, 0x02);

	pmsg->VerMajor = vMajor;
	pmsg->VerMinor = vMinor;
	pmsg->VerMaintenance = vMaint;
	pmsg->Revision  = rev;
	return sizeof( Dci_VersionInfo);
}

/**
* Queries for Version Information (20,82)
*/	
int Dci_VersionInfoQuery_Init( void* buff) 
{ 
	return Dci_Hdr_Init( buff, 0x20, 0x82);
}

/**
* Initializes the StringProperties message.  Be sure to provide a buffer large enough to accomodate
* the length of the string message.  Returns the total size of the message.
*/
int Dci_StringProperties_Init( void* buff, uint16 sizeBuff, uint16 flags, pcstr pszMessage)
{
	const int lenHdr = sizeof(Dci_StringProperties);
	Dci_StringProperties* pmsg = (Dci_StringProperties*) buff;	
	int lenMsg = lenHdr + (int) strlen(pszMessage);
	// Check for way too small even for header.
	if( lenMsg <= sizeBuff)
	{
		Dci_Hdr_Init( buff, 0x20, 0x05);	
		pmsg->flags = flags; 
		pmsg->len   = (uint16)strlen(pszMessage);  // len not including terminator.
		strcpy( (char *)buff+lenHdr, pszMessage);
		return( lenMsg );
	}
	return(0);
}

/**
* Returns the string message contained within the DCI debug message.
*/
pcstr Dci_StringProperties_GetProperties( Dci_StringProperties* pmsg)
{
	// Add string terminator and return string.
	char *szMsg = (char*)pmsg + sizeof(Dci_StringProperties);
	szMsg[ pmsg->len] = '\0';
	return (pcstr)szMsg;
}

/**
* QueryStringProperties (20,85)
*/
int Dci_StringPropertiesQuery_Init( void* buff )
{ 
	return Dci_Hdr_Init( buff, 0x20, 0x85);
}

/**
* Initializes the TypedDataRecord message (20,13).  Be sure to provide a buffer large enough to accomodate
* the length of the data.  Returns the total size of the message.
*/
int Dci_TypedDataRecord_Init( void* buff, uint16 sizeBuff, 
		uint16 week, double sec,
		uint32 timecode,  uint16 idtype,
		uint16 flags, uint16 lenData, void* pdata)
{
	const int lenHdr = sizeof(Dci_TypedDataRecord);
	Dci_TypedDataRecord* pmsg;
	int lenMsg;

	int32 secWhole;
	int16 secFrac;
	lenMsg = lenHdr + lenData;
	if( (pdata != NULL) && (lenMsg <= sizeBuff))
	{
		secWhole = (int32) sec;
		secFrac  = (int16)(0.5 + (sec - secWhole)*1e4);
		
		pmsg = (Dci_TypedDataRecord*) buff;
		
		Dci_Hdr_Init( buff, 0x20, 0x13);
		pmsg->week = week;
		pmsg->secWhole = secWhole;
		pmsg->secFrac  = secFrac;
		pmsg->timecode = timecode;
		pmsg->idtype   = idtype;
		pmsg->flags    = flags; 
		pmsg->lenData  = MIN( sizeBuff, lenData);
		memcpy((byte*)buff+lenHdr, pdata, lenData);
		return(lenMsg);
	}
	return 0;
}

/**
* Returns the string message contained within the DCI debug message.
*/
byte* Dci_TypedDataRecord_GetData( Dci_TypedDataRecord* pmsg)
{
	return( (byte *)pmsg + sizeof(Dci_TypedDataRecord) );
}

/**
* Queries for the specified TypedDataRecord(20,93)
*/	
int Dci_TypedDataRecordQuery_Init( void* buff, uint16 idtype, uint16 flags) 
{ 
	Dci_Hdr_Init( buff, 0x20, 0x93);
	((Dci_TypedDataRecordQuery*)buff)->idtype = idtype;
	((Dci_TypedDataRecordQuery*)buff)->flags = flags;
	return sizeof( Dci_TypedDataRecordQuery);
}

