// Name:  dciproc.c
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.


#include <string.h>
#include <stdio.h>

#include<Comm/Dci/WcaMsgs.h>
#include<Comm/Dci/InfrastructureMsgs.h>
#include<Comm/Dci/BinaryStreamFormatter.h>
#include<Comm/Dci/StandardMsgs.h>
#include<Comm/Dci/DciConversation.h>

#include "uart.h"
#include "fpga.h"
#include "dciproc.h"
#include "main.h"


#define WCA_COMPONENT_INDEX 4

//Create 4 conversations and one formatter.
static Dci_Conversation s_conv0;  
static Dci_Conversation s_conv1;
static Dci_Conversation s_conv2;  
static Dci_Conversation s_conv3;
static Dci_Conversation *s_pConv[] = {&s_conv0, &s_conv1, &s_conv2, &s_conv3};
static int s_numConvs = 4; // number of conversation

static byte			s_fmtBuffer1[ MAX_MSG_SIZE];// Formatter Processing Buffer.
static byte			s_fmtBuffer2[ MAX_MSG_SIZE];// Formatter Processing Buffer.
static Dci_Bsf		s_u1Fmt;					// Formatter for UART 1. - one formatter per stream.
static Dci_Bsf		s_u2Fmt;					// Formatter for UART 2. - one formatter per stream.

//DCI Processor is also known as the microcontroller.
#define COMPONENTID 0x80

//********************************************************************************
// Function Prototypes
//********************************************************************************
static void ReadFromUart( Uart_Config *pUart);
 
// Forward declaration of MAIN Message call back function
// ONLY dciproc gets to call this function!!!!.
void Main_OnDciMessageReceived(Dci_Context * pctxt);

 

//********************************************************************************
// Implementation
//********************************************************************************


/**
 * Initialize
 */
void Dciproc_Init( )
{
	Dci_Conversation_Init( &s_conv0, 0); //This Conversation goes to UART #1
	Dci_Conversation_Init( &s_conv1, 1); //This Conversation goes to UART #1
	Dci_Conversation_Init( &s_conv2, 2); //This Conversation goes to UART #0
	Dci_Conversation_Init( &s_conv3, 3); //This Conversation goes to UART #1

	Dci_Bsf_Init( &s_u1Fmt, s_fmtBuffer1, sizeof(s_fmtBuffer1));
	Dci_Bsf_Init( &s_u2Fmt, s_fmtBuffer2, sizeof(s_fmtBuffer2));
}


/**
 * Handles received DCI messages 
 */
void  Dciproc_OnDciMessageReceived(Dci_Context * pctxt)
{
  //TODO process Microcontroller component.  Later.
}


/**
 * Read DCI message from UART
 */
void Dciproc_OnUpdate( )
{
	ReadFromUart( &Uart_0 );  // J2 Serial Interface (DCI Conversation 2).
	ReadFromUart( &Uart_1 );  // Cypress FX-3 Interface (DCI Conversation 0,1,3)
}

/**
* Internal function reads data from the specified UART.
*/
void ReadFromUart( Uart_Config *pUart)
{
	int len;
	int lenMsg;
	int idConv;
	int ctProcessed;
	
	Dci_Context dciContext;
	Dci_Hdr *pMsg;			//Pointer to DCI message.
	byte buff[RXBFRSIZE];//Stack allocated read buffer.
	byte *ptr = buff;		//Byte pointer used in reading.
	 
	//  Read all available bytes from UART
	len = Uart_Read(ptr, RXBFRSIZE, pUart);
	if( len == 0)
		return;

	// Process received DCI message
	while (len > 0)
	{
		//Use the stream formatter to detect message framing and synch.
		ctProcessed = Dci_Bsf_ProcessReceivedData( &s_u1Fmt, ptr, len);
		
		if(Dci_Bsf_MessageReady( &s_u1Fmt))
		{
			//Grab a pointer to the Message.
			lenMsg = Dci_Bsf_GetReceivedMessage( &s_u1Fmt, true, &pMsg);
			idConv = Dci_Hdr_GetConversationId(pMsg);

			// Prepare the context and send received message off for
			// processing.
			dciContext.pMsg = pMsg;
			dciContext.lenMsg = lenMsg;
			dciContext.pConv = s_pConv[idConv];
			dciContext.bHandled = false;
			dciContext.idMessage = Dci_Hdr_MessageId( pMsg);
			dciContext.idComponent = 0xFF;
		
			if (idConv >= s_numConvs)	// id out of range
			{
				//Send a message on the asynchronous conversations (e.g. g_conv1)
				//to notify that we received an invalid message. conversation.
				//may not really care if this occurs, but for demo purpose nice to 
				//show an error condition.
				main_LogMsg(LOG_ERR, WCACOMP_MICRO, "Bad Conv. ID", &dciContext);
			}
			else if( Dci_Conversation_UpdateState( s_pConv[idConv], pMsg, lenMsg) > 0)
			{
				//Send an message back on the current conversation indicating the
				//error condition.
				char buff2[16];
				len = Dci_MessageError_Init1( buff2, pMsg);
				Dciproc_SendMessage( (byte*)buff2, len, false, idConv);
				return;
			}
			else //Process Message.
			{
				//If WCA Message grab the component ID to help WCA based message processing.
				if( pMsg->idCategory == 0x21)
					dciContext.idComponent = ((byte*)pMsg)[WCA_COMPONENT_INDEX];

				//Finally let anyone process the message who wants to.
				//if it is not an idle message
				if( !Dci_Hdr_IsIdleMsg(pMsg))
					Main_OnDciMessageReceived(&dciContext);
	
				//If ack was not provided and sent previously
				//quietly send one now using an idle message.
				if( dciContext.pConv->bAckRequired)
				{
					char buff[MIN_MSG_SIZE];
					int len = Dci_IdleMsg_Init( buff);
					Dciproc_SendMessage( (byte*)buff, len, false, dciContext.pConv->idConversation);
				}
			}
		}
		//Advance to remainder of buffer if we did not process all of it.
		if( ctProcessed < len)
		{
			len -= ctProcessed;
			ptr += ctProcessed;
			if( len < 0 )
				len = 0;  // CJC
		}
		else //Otherwise reset for next read.
		{
			len = 0;
			ptr = buff;
		}
	}
	return;
}

/**
* Sends a DCI message using the specified conversation context.
*/
int Dciproc_SendMessage( byte* pmsg, int len, bool bAckRequired, int idc)
{
	//Create fake context 
	Dci_Context ctxt;
	ctxt.pMsg = NULL;
	ctxt.lenMsg = 0;
	ctxt.pConv = s_pConv[idc];
	ctxt.bHandled = true;

	// Get the conversation object, make the context.
	return Dciproc_SendMessage1(pmsg, len, bAckRequired, &ctxt);
}
/**
* Sends a DCI message using the current message conversation context.
*/
int Dciproc_SendMessage1(  byte* pmsg, int len, bool bAckRequired, Dci_Context* pctxt)
{
	byte buff[MAX_MSG_SIZE];
	Uart_Config *pUart;
	int lenMsg, lenTx;

	//Prepare the message, serialize into binary stream, select UART, then write it.
	Dci_Conversation_PrepareMessageHdr( pctxt->pConv, (Dci_Hdr*) pmsg,  bAckRequired);
	pUart = (Dci_Hdr_GetConversationId( (Dci_Hdr*) pmsg ) == 2) ? &Uart_0 : &Uart_1;
	lenMsg = Dci_Bsf_FormatMessage(buff, sizeof(buff), pmsg, len);

	// Make it a multipe of 32 and just large enough for the message
	lenTx = ((lenMsg-1) & 0xFFE0) + 0x20;
		
	memset(buff + lenMsg, 0, lenTx - lenMsg);

	return Uart_Write( buff, lenTx, pUart);
}

 
/**
* Validates the Property id within specified range. Returns true if valid.
*/
bool Dciproc_ValidatePropRange( byte idComponent, byte idProp, byte idRangeStart, byte idRangeEnd, Dci_Context* pctxt)
{
	bool bValid = (bool)( idRangeStart <= idProp && idProp <= idRangeEnd);
	if( !bValid)
	{
		char buff[24];
		sprintf(buff, "Prop ID: %d", idProp);
		main_LogMsg( LOG_ERR, idComponent, buff, pctxt);
	}
	return bValid;
}




