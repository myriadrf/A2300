/** Name: DciMsg.c
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

#include "Dci/DciMsg.h"
#include <string.h>

/**
* Initializes a DCI Message header at the specifed buffer location
* to a known state with specified message ID.
*/
int Dci_Hdr_Init( void* buff, byte idCategory, byte idType)
{
	Dci_Hdr* phdr = (Dci_Hdr*) buff;
	phdr->flags = 0x20; /* Version 1, no ack, no encoding, conversation 0 */
	phdr->seq = 0;
	phdr->idCategory  =  idCategory;
	phdr->idType     =  idType;
	return (int) sizeof( Dci_Hdr);
}

/**
 * Validates Message Header.
 */
bool Dci_Hdr_ValidateNonIdleMsg( Dci_Hdr* phdr)
{
	return ((phdr->idCategory != 0x01) && (phdr->idType != 0));
}

byte Dci_Hdr_CategoryId( Dci_Hdr* phdr) { return phdr->idCategory;}
byte Dci_Hdr_TypeId( Dci_Hdr* phdr) 	  { return phdr->idType;}
uint16 Dci_Hdr_MessageId( Dci_Hdr* phdr){ return ((uint16)(((uint16)phdr->idCategory) << 8)) | phdr->idType;}

bool Dci_Hdr_IsDebugMsg( Dci_Hdr* phdr) {return Dci_Hdr_CategoryId( phdr) == Dci_MsgId_Category(Dci_DebugMsg_Id);}
bool Dci_Hdr_IsIdleMsg( Dci_Hdr* phdr)  {return phdr->idCategory == 0x01 && phdr->idType == 0;}

bool Dci_Hdr_MatchesId(  Dci_Hdr* phdr, byte idCategory, byte idType)
{
	return ((phdr->idCategory == idCategory) && (phdr->idType == idType));
}

bool Dci_Hdr_MatchesId1( Dci_Hdr* phdr, uint16 id)
{ 	uint16 idMsg = ((uint16)(((uint16)phdr->idCategory) << 8)) | phdr->idType;
	return idMsg == id;
}

/* Gets the DCI message version. */
byte Dci_Hdr_GetVersion( Dci_Hdr* phdr) { return (phdr->flags >> 5) & 0x7;}

/* Sets the DCI message version. */
void Dci_Hdr_SetVersion( Dci_Hdr* phdr, byte idVer)
{
	phdr->flags = ((idVer <<5) & 0xE0) | (phdr->flags & 0x1F);
}

/* Accessor returns true if Ack is required. */
byte Dci_Hdr_GetAckRequired(Dci_Hdr* phdr) { return (phdr->flags&0x10) == 0x10;}

/* Accessor set the AckRequired flag. */
void Dci_Hdr_SetAckRequired( Dci_Hdr* phdr, bool bValue)
{
	phdr->flags = (bValue ? 0x10 : 0x00) | (phdr->flags & 0xEF);
}

/* Returns encoding of the data. */
byte Dci_Hdr_GetEncoding(Dci_Hdr* phdr)
{
	return (phdr->flags>>2) & 0x3;
}

/* Sets the data encoding. */
void Dci_Hdr_SetEncoding(Dci_Hdr* phdr, byte encoding)
{
	phdr->flags = ((encoding<<2) & 0xC) | (phdr->flags & 0xF3);
}

/* Accessor returns the current conversation ID. */
byte Dci_Hdr_GetConversationId(Dci_Hdr* phdr )  {return phdr->flags & 0x3;}

/* Accessor sets the current conversation ID. */
void Dci_Hdr_SetConversationId(Dci_Hdr* phdr, byte idConversation)
{
	phdr->flags = (phdr->flags & 0xFC) | (idConversation & 0x3);
}

/* Accessor sets the sequence identifier. */
void   Dci_Hdr_SetSeq(Dci_Hdr* phdr, byte seq) 		{ phdr->seq = seq; }
void   Dci_Hdr_SetSeq1(Dci_Hdr* phdr, byte sndSeq, byte ackSeq) { phdr->seq = ((byte)((sndSeq & (byte) 0xF) << 4)) | (ackSeq & (byte)0xF); }

byte   Dci_Hdr_GetSeqAck(Dci_Hdr* phdr) 				{ return phdr->seq & 0xF;}
void   Dci_Hdr_SetSeqAck(Dci_Hdr* phdr, byte acqSeq)  { phdr->seq = (phdr->seq & (byte) 0xF0) | (acqSeq & 0x0F);}

byte   Dci_Hdr_GetSeqSnd(Dci_Hdr* phdr) 				{ return ((phdr->seq & 0xF0) >> 4);}
void   Dci_Hdr_SetSeqSnd(Dci_Hdr* phdr, byte acqSnd) { phdr->seq = (phdr->seq & (byte) 0x0F) | ((byte)(acqSnd << 4));}

#if 0

// Toggle endianness functions
//int16 Dci_SwapInt16( int16 val)
//{
//	return Dci_SwapUInt16( (uint16) val);
//}
//
//uint16 Dci_SwapUInt16( uint16 uval)
//{
//	return (uval>>8) | (uval<<8);
//}
//
//int32 Dci_SwapInt32( int32 intVal)
//{
//	return Dci_SwapUInt32((uint32)intVal);
//}
//
//uint32 Dci_SwapUInt32( uint32 uval)
//{
//	byte* val = (byte*)(&uval);
//	byte newVal[4];
//
//	newVal[0] = val[3];
//	newVal[1] = val[2];
//	newVal[2] = val[1];
//	newVal[3] = val[0];
//
//	return *((uint32*)newVal);
//}

#endif
