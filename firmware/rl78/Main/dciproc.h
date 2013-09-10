// Name:  dciproc.h
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

#ifndef _DCIPROC_H_
#define _DCIPROC_H_

#include<Comm/Dci/DciUtils.h>


#define MAX_MSG_SIZE 320
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


#endif