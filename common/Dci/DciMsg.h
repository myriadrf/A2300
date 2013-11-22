/** Name: DciMsg.h
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

#ifndef DciMsg_h
#define DciMsg_h

#include "../System/DataTypes.h"

#if defined( WIN32) || defined(LINUX) || defined(APPLE)
	#pragma pack(push, 1)
#endif

#ifdef  __cplusplus
extern "C" {
#endif

/* Defines */
#define DCI_VERSION_DEFAULT  1
#define DCI_MIN_MSGSIZE 	 4
#define DCI_MAX_MSGSIZE     320

#define Dci_MsgId_Category(MsgId) ((byte)(MsgId >> 8))
#define Dci_MsgId_TypeId(MsgId) ((byte)(MsgId & (uint16)0x00ff))

#define Dci_IdleMsg_Id 		(uint16)(0x0100)
#define Dci_DebugMsg_Id 	(uint16)(0x0501)

/**
* Dci_Hdr Structure
* Dci Message header base for all DCI Messages.
*/
typedef struct Dci_Hdr
{
	byte   flags;
	byte   seq;
	byte   idCategory;
	byte   idType;
} Dci_Hdr;

/*////////////////////// ACCESSORS //////////////////////////*/

/**
* Initializes a DCI Message header at the specifed buffer location
* to a known state with specified message ID.
*/
int Dci_Hdr_Init( void* buff, byte idCategory, byte idType);

/**
 * Validates Message Header.
 */
bool Dci_Hdr_ValidateNonIdleMsg( Dci_Hdr* phdr);

uint16  Dci_Hdr_MessageId( Dci_Hdr* phdr);
byte Dci_Hdr_CategoryId( Dci_Hdr* phdr);
byte Dci_Hdr_TypeId( Dci_Hdr* phdr);
uint16 Dci_Hdr_MessageId( Dci_Hdr* phdr);

bool Dci_Hdr_IsDebugMsg( Dci_Hdr* phdr);
bool Dci_Hdr_IsIdleMsg( Dci_Hdr* phdr);

bool Dci_Hdr_MatchesId(  Dci_Hdr* phdr, byte idCategory, byte idType);
bool Dci_Hdr_MatchesId1( Dci_Hdr* phdr, uint16 id);

/* Gets the DCI message version. */
byte Dci_Hdr_GetVersion( Dci_Hdr* phdr);

/* Sets the DCI message version. */
void Dci_Hdr_SetVersion( Dci_Hdr* phdr, byte idVer);

/* Accessor returns true if Ack is required. */
byte Dci_Hdr_GetAckRequired(Dci_Hdr* phdr);

/* Accessor set the AckRequired flag. */
void Dci_Hdr_SetAckRequired( Dci_Hdr* phdr, bool bValue);

/* Returns encoding of the data. */
byte Dci_Hdr_GetEncoding(Dci_Hdr* phdr);

/* Sets the data encoding. */
void Dci_Hdr_SetEncoding(Dci_Hdr* phdr, byte encoding);

/* Accessor returns the current conversation ID. */
byte Dci_Hdr_GetConversationId(Dci_Hdr* phdr );

/* Accessor sets the current conversation ID. */
void Dci_Hdr_SetConversationId(Dci_Hdr* phdr, byte idConversation);

/* Accessor sets the sequence identifier. */
void   Dci_Hdr_SetSeq(Dci_Hdr* phdr, byte seq);
void   Dci_Hdr_SetSeq1(Dci_Hdr* phdr, byte sndSeq, byte ackSeq);

byte   Dci_Hdr_GetSeqAck(Dci_Hdr* phdr);
void   Dci_Hdr_SetSeqAck(Dci_Hdr* phdr, byte acqSeq);

byte   Dci_Hdr_GetSeqSnd(Dci_Hdr* phdr);
void   Dci_Hdr_SetSeqSnd(Dci_Hdr* phdr, byte acqSnd);

/* Toggle endianness functions */
int16 Dci_SwapInt16( int16 val);
uint16 Dci_SwapUInt16( uint16 uval);
uint32 Dci_SwapUInt32( uint32 uval);
int32 Dci_SwapInt32( int32 intVal);

#ifdef __cplusplus
}
#endif

#if defined( WIN32) || defined(LINUX) || defined(APPLE)
	#pragma pack(pop)
#endif

#endif /* DciMsg.h */
