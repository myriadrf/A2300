/** Name: BinaryStreamFormatter.h
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

#ifndef BINARY_STREAM_FORMATTER_H
#define BINARY_STREAM_FORMATTER_H

#include "Dci/DciMsg.h"

#ifdef  __cplusplus
extern "C" {
#endif

enum Dci_FormatterModeEnum
{
	FME_FrameSynch,
	FME_ReadingLength,
	FME_ReadingHeader,
	FME_ReadingPayload,
	FME_ReadingChecksum,
	FME_EndSeq,
	FME_MsgReady
};

/**
 * This structure is an implementation of the <code>MessageFormatter</code>
 * interface.  It is used to pick out messages from a byte stream that are
 * surrounded or "framed" by a set number of leading bytes, the header, and a
 * set number of trailing bytes, the footer.  A complete message includes the
 * header, the footer, and all the bytes inbetween.
 * <br><br>
 * Subsclasses will implement methods to verify the checksum of messages,
 * extract the message body from the full message (i.e. header, footer,
 * checksum, etc.), format a message body for transport to the receiver.
  */
typedef struct Dci_Bsf
{
	/** Current byte processing mode */
	enum Dci_FormatterModeEnum mode;

	/** The current unformatted message being built */
	byte* pBuff;

	/** The maximum size of the buffer */
	int sizeBuff;

	/** Pointer into the buffer */
	byte* pNext;

	/** The length of the payload*/
	int iPayload;

	/** Message byte count.  Meaning varies by state.*/
	int ct;
} Dci_Bsf;

/**
 * Initializes the binary stream formatter object.
 */
void Dci_Bsf_Init( Dci_Bsf *pfmttr, byte* pBuff, int sizeBuff);

/**
 * <summary>
 * Encodes payload and calculates the checksum. Returns the checksum value.
 * </summary>
 */
short Dci_Bsf_EncodePayloadType1(
	byte* pOut, short* pLenOutput, const byte* pDciMsg,
#if 0
	byte lenMsg
#else
	short lenMsg
#endif
);

/**
 * <summary>
 * Decodes payload and calculates the checksum. Returns the checksum value.
 * </summary>
 */
short Dci_Bsf_DecodePayloadType1( byte* pOut, int* pLenOutput, byte* pHdr, byte* pbuf, short nLen );

/**
 *<summary>
 * Formats a DCI Message in accordance to Binary Serial Stream Transport requirements.
 *</summary>
 */
int Dci_Bsf_FormatMessage(
	byte* pOutBuff, int nMaxBuffLen, const byte* pDciMsg,
#if 0
	byte lenMsg
#else
	short lenMsg
#endif
);

/**
 *<summary>
 * Calculates the checksum for a payload and header.
 *</summary>
 */
short Dci_Bsf_CalcChecksum( const byte* pDciMsg, short nLen );

/**
 * <summary>
 * Call this to get Received Message when state is MessageReady.
 * This only works once, so it is a good idea to copy the message
 * if needed for long-term. as Buffers will be overriden in next
 * processing.
 *
 * Returns the length including the header.
 * </summary>
 */
int Dci_Bsf_GetReceivedMessage(
	Dci_Bsf *pfmttr, bool bUnencode, Dci_Hdr** ppMsg);

/**
 * This is called by the <code>MessageTransport</code> instance in order to
 * determine if a message is ready - i.e. the formatter has determined that
 * all elements of the message are present and the header, checksum, and
 * footer are valid.  When this method returns true, it indicates that the
 * <code>getMessage()</code> method can be safely called without returning
 * <code>null</code>.
 * @return <code>true</code> if a message is ready; <code>false</code>
 * otherwise
 */
bool Dci_Bsf_MessageReady( Dci_Bsf *pfmttr );

/**
* Returns true if have synchronization
*/
bool Dci_Bsf_HaveSynch(Dci_Bsf *pfmttr);

/**
 * <summary>
 * Method called to process received data from a provider.  Returns the total number of
 * bytes processed, if less than ctBytes, means a message is ready or an error occurred.
 * </summary>
 */
int Dci_Bsf_ProcessReceivedData( Dci_Bsf *pfmttr, byte* pbuff, int ctBytes );

/**
 * Helper method used to reset the formatter for the next message.
 */
void Dci_Bsf_Reset(Dci_Bsf *pfmttr );

#ifdef __cplusplus
}
#endif

#endif /* BINARY_STREAM_FORMATTER_H */
