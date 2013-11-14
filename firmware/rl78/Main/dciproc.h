/************************************************************************
 * Name:  dciproc.h
 * 
 * This module is the proprietary property of Loctronix Corporation
 * Copyright (C) 2007 - 2013 Loctronix Corporation
 * All Rights Reserved
 * 
 ***************************************************************************/
#ifndef _DCIPROC_H_
#define _DCIPROC_H_

#include<Comm/Dci/DciUtils.h>

#ifndef MAX_MSG_SIZE
	#define MAX_MSG_SIZE 320
#endif
#define MIN_MSG_SIZE 12

struct Dci_Property;


/**
 * Initialization
 */
void Dciproc_Init( );

/**
 * Handles received DCI messages processed directly by the DCI Processor.
 */
void  Dciproc_OnDciMessageReceived(Dci_Context * pctxt);

/**
 * Processes on going updates
 */
void  Dciproc_OnUpdate( );

/**
* Sends a DCI message using the specified conversation context.
*/
int Dciproc_SendMessage( byte* pmsg, int len, bool bAckRequired, int idc);

/**
* Sends a DCI message using the current message conversation context.
*/
int Dciproc_SendMessage1(  byte* pmsg, int len, bool bAckRequired, Dci_Context* pctxt);

/**
* Validates the Property id within specified range. Returns true if valid.
*/
bool Dciproc_ValidatePropRange( byte idComponent, byte idProp, byte idRangeStart, byte idRangeEnd, Dci_Context* pCtxt);

/**
* Function regsters a Bit client the DCI Processor.
*/
void Dciproc_BitRegisterClient( Dci_BitClient* pclient);


#endif