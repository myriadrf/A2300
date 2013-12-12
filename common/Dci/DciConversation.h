/** Name: DciConversation.h
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

#ifndef DciConversation_h
#define DciConversation_h

#include "DciMsg.h"

#ifdef  __cplusplus
extern "C" {
#endif

/**
* Structure manages conversation sequencing, basic implementation does not perform
* execution of the conversation, just keeps track of what is what.
*/
typedef struct Dci_Conversation
{
	byte bAckRequired;
	byte seqAck;
	byte seqTx;
	byte idConversation;
} Dci_Conversation;

/**
* Initializes a conversation.
*/
void Dci_Conversation_Init( Dci_Conversation* pconv, byte idConversation );

/**
* Gets the conversation ID.  Can also look at it directly in the structure.
*/
byte Dci_Conversation_Id( Dci_Conversation* pconv);

/**
*  If true, indicates that receiver is waiting on an acknowledgement
*  of receipt of last message.  This will get cleared by sending a message
*  or Idle message to the receiver.
*/
byte Dci_Conversation_ReceiptAckNeeded( Dci_Conversation* pconv);

/**
* Method processes buffer, This updates
* basic information about the conversation from the received message
* to make sure everything is in good shape.
* <param name="pDataMsg"></param>
* <returns>Retval = 0, ok; 1, invalid conversation id, 2, Ack receive message overflow.</returns>
*/
int Dci_Conversation_UpdateState( Dci_Conversation* pconv, void* buff,uint16 len);

/**
* Method called to prepare next message for transmission.
* <param name="msg"></param>
*/
void Dci_Conversation_PrepareMessageHdr( Dci_Conversation* pconv, Dci_Hdr* phdr, bool bAckRequired);

/**
* Resets the Conversate ACQ and SEQ state information. Do this if
* an error occurs in the transmission or receipt of message data.
*/
void Dci_Conversation_Reset( Dci_Conversation* pconv);

#ifdef __cplusplus
}
#endif

#endif /* DciConversation_h */
