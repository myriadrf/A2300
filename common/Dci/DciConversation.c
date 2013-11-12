/** Name: DciConversation.c
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

#include "Dci/DciConversation.h"
#include <string.h>

/**
* Initializes a conversation.
*/
void Dci_Conversation_Init( Dci_Conversation* pconv, byte idConversation )
{
	pconv->idConversation = idConversation;
	Dci_Conversation_Reset( pconv);
}

/**
* Gets the conversation ID.  Can also look at it directly in the structure.
*/
byte Dci_Conversation_Id( Dci_Conversation* pconv)
{
	return pconv->idConversation;
}

/**
*  If true, indicates that receiver is waiting on an acknowledgement
*  of receipt of last message.  This will get cleared by sending a message
*  or Idle message to the receiver.
*/
byte Dci_Conversation_ReceiptAckNeeded( Dci_Conversation* pconv)
{
	return pconv->bAckRequired && pconv->seqAck < 0x10;
}


/**
* Method processes buffer, This updates
* basic information about the conversation from the received message
* to make sure everything is in good shape.
* <param name="pDataMsg"></param>
* <returns>Retval = 0, ok; 1, invalid conversation id, 2, Ack receive message overflow.</returns>
*/
int Dci_Conversation_UpdateState( Dci_Conversation* pconv, void* buff,uint16 len)
{
	Dci_Hdr* phdr = (Dci_Hdr*) buff;


	/* Check and see if we have a proper message for this conversation. */
	if( len >= 4 && Dci_Hdr_GetConversationId( phdr) != pconv->idConversation)
		return 1;

	/* Check and see if we have overflowed the message Ack sequence. */
#if 0
	if( Dci_Hdr_GetAckRequired(phdr) && pconv->seqAck == 0xf0 x10)
#if 1
		return 2;
#else
		throw new InvalidOperationException("Overflow of received messages, need to Acknowledge receipt of previous more often");
#endif
#endif

	/* Update the conversation state. */
	pconv->bAckRequired = Dci_Hdr_GetAckRequired( phdr);
	pconv->seqAck		= Dci_Hdr_GetSeqSnd( phdr);

	/* Get ack of last message we sent */
#if 0
	byte ackLast = (byte) (dat.get_SEQ() & 0xF);
#endif
	return 0;
}

/*
* <summary>
* Method called to prepare next message for transmission.
* </summary>
* <param name="msg"></param>
*/
void Dci_Conversation_PrepareMessageHdr( Dci_Conversation* pconv,
										   Dci_Hdr* phdr, bool bAckRequired)
{
	byte sndSeq;

	/* Fulfill Acq requirements. */
	Dci_Hdr_SetAckRequired( phdr, bAckRequired);
	pconv->bAckRequired = false;

	/* Set the conversation ID. */
	Dci_Hdr_SetConversationId( phdr, pconv->idConversation);

	/* Update sndSeq. */
	sndSeq = pconv->seqTx;
	pconv->seqTx = (byte) ((sndSeq + 1) % 0x10);

	/* Update the SEQ data on the message. */
	Dci_Hdr_SetSeq1( phdr, sndSeq,  pconv->seqAck);
}


/**
* Resets the Conversate ACQ and SEQ state information. Do this if
* an error occurs in the transmission or receipt of message data.
*/
void Dci_Conversation_Reset( Dci_Conversation* pconv)
{
	/* Reset the Ack and SEQ states only. */
	pconv->seqTx		= 0;
	pconv->bAckRequired	= false;
	pconv->seqAck		= 0x10;
}
