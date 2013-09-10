// Name:  DataPorts.c
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

#include "main.h"
#include "Drivers/fpga.h"
#include "Drivers/FpgaWcaHalDefs.h"

#include<Comm/Dci/DciConversation.h>
#include<Comm/Dci/StandardMsgs.h>
#include<Comm/Dci/WcaMsgs.h>
#include<string.h>
#include"DataPorts.h"


//*******************************************************
//Internal Module Structures and Function Prototypes
//*******************************************************

//LSDP Configuration Management.
void LsdpConfigure( DataPorts_Def*ps, byte mode, int idc);
void LsdpReset( DataPorts_Def* ps);

//Property and Data Handlers for LSDPs
int  OnLsdpReadData( byte idxPort, uint32 timecode);
void OnHandleLsdpTxEvent( byte idxPort);
void OnLsdpWriteProperties( Dci_Context* pctxt);
void OnLsdpReadProperties( Dci_Context* pctxt);
void OnLsdpWriteData( Dci_Context* pctxt);

DCI_BEGIN_MSGMAP( s_lsdpmap)
	DCI_WCAPROP_HANDLER( WCACOMP_LSDP0, 0, 0, &OnLsdpWriteProperties)
	DCI_WCAPROP_HANDLER( WCACOMP_LSDP1, 0, 0, &OnLsdpWriteProperties)
	DCI_WCAPROP_HANDLER( WCACOMP_LSDP2, 0, 0, &OnLsdpWriteProperties)
	DCI_WCAPROP_HANDLER( WCACOMP_LSDP3, 0, 0, &OnLsdpWriteProperties)
	DCI_WCAPROPQUERY_HANDLER( WCACOMP_LSDP0, 1, 1, &OnLsdpReadProperties)
	DCI_WCAPROPQUERY_HANDLER( WCACOMP_LSDP1, 1, 1, &OnLsdpReadProperties)
	DCI_WCAPROPQUERY_HANDLER( WCACOMP_LSDP2, 1, 1, &OnLsdpReadProperties)
	DCI_WCAPROPQUERY_HANDLER( WCACOMP_LSDP3, 1, 1, &OnLsdpReadProperties)
	DCI_TYPEDDATA_HANDLER( WCACOMP_LSDP0, OnLsdpWriteData )  //Receives LSPD Typed Data Messages 
	DCI_TYPEDDATA_HANDLER( WCACOMP_LSDP2, OnLsdpWriteData )  //Receives LSPD Typed Data Messages 
DCI_END_MSGMAP


//Event status will be this.
enum LsdpFifoEvents
{
	EVENT_FIFO_LSDP0 = 0x1,
	EVENT_FIFO_LSDP1 = 0x2,
	EVENT_FIFO_LSDP2 = 0x4,
	EVENT_FIFO_LSDP3 = 0x8,
	EVENT_FIFO_ERROR = 0x10 
};

//*******************************************************
//Static Data
//*******************************************************

/**
* Array structure keeps track of LSDP port configuration and 
* status.
*/
DataPorts_Def s_portdefs[] = { 
	{WCACOMP_LSDP0, LSDP_INPUT,  0, false, 0}, 
	{WCACOMP_LSDP1, LSDP_OUTPUT, 0, false, 0}, 
	{WCACOMP_LSDP2, LSDP_INPUT,  0, false, 0}, 
	{WCACOMP_LSDP3, LSDP_OUTPUT, 0, false, 0}
};
#define COUNT_PORTS 4		
enum { LSDP0_IDX = 0, LSDP1_IDX, LSDP2_IDX, LSDP3_IDX};

// Specification allows a maximum of 64 16-bit (I,Q) pairs, total 32 bits.
#define MAX_IQ_PAIRS (64)
#define MAX_IQ_PAIRS_BYTES (MAX_IQ_PAIRS * 4)  // 4 bytes per pair.

/**
* Copy of the WCAHAL_LSDP_CONTROL Register
* Used to manipulate state of the various LSDPs.
*/
byte s_FpgaLsdpControl;

//*******************************************************
// Common Module Interface Implementation
//*******************************************************

/**
 * Initialization
 */
void DataPorts_Init( )
{
	int i;
	s_FpgaLsdpControl = 0;

	main_AddDciMsgMap( s_lsdpmap);
	
	//Initialize the Ports to a known state.
	for( i = 0; i < COUNT_PORTS; i++)
		LsdpReset( s_portdefs + i);
}

/**
 * Processes on going updates
 */
void  DataPorts_OnUpdate( )
{
	uint32 timecode = main_GetTimecode();

	//Update output ports if any active
	if( s_FpgaLsdpControl > 0)
	{
		byte statusEvt = Fpga_GetEventStatus();

		//If there is a FIFO ERROR. Terminate the port and notify
		//Host.
 		if( statusEvt & EVENT_FIFO_ERROR)
		{
			int i;
			
			//Get the FIFO Status Register information about the error.
			byte status = Fpga_ReadByte( WCAHAL_LSDP_STATUS);

			//Process the status bits.
			for( i = 0; i < COUNT_PORTS; i++, status >> 2)
			{ 
				if( (status&0x3) > 0)
				{
					//Disable the port and log that we had an error
					LsdpConfigure( s_portdefs + i, (byte) LSDP_DISABLED, s_portdefs[i].idc);
					main_LogMsgIdc( LOG_ERR, s_portdefs[i].IdComponent, "Overflow / Underflow Error", s_portdefs[i].idc);					
				}
			}
			
			//Clear the Error Status
			Fpga_ClearEventStatus(EVENT_FIFO_ERROR, false);
		}
		
		//Handle Events for TX LSDPs
		if( statusEvt & EVENT_FIFO_LSDP0) OnHandleLsdpTxEvent( LSDP0_IDX);
		if( statusEvt & EVENT_FIFO_LSDP2) OnHandleLsdpTxEvent( LSDP2_IDX);	
		if( statusEvt & EVENT_FIFO_LSDP1) OnLsdpReadData( LSDP1_IDX, timecode);
		if( statusEvt & EVENT_FIFO_LSDP3) OnLsdpReadData( LSDP3_IDX, timecode);
	}
	else //Just clear the events since we aren't processing.
		Fpga_ClearEventStatus( 0x1F, false);
}


//*******************************************************
// Low Speed Data Ports (LSDP)
//*******************************************************

/**
* Handler called to write properties on Lows Speed Data Port Components 0x90,
* 0x91, 0x92, 0x93.
*/
void OnLsdpWriteProperties( Dci_Context* pctxt)
{
	int i;
	Dci_TypedProperties* ptd	= (Dci_TypedProperties*) pctxt->pMsg;
	Dci_Property*		 pProps = (Dci_Property*) Dci_TypedProperties_GetProperties( ptd);  

	// Grab port def.
	int idxPort = pctxt->idComponent - 0x90;
	DataPorts_Def* pdef = s_portdefs + idxPort;

	//Iterate properties saving and dumping the properties.
	for( i = 0; i < ptd->ctProperties; i++)
	{
		if( Dciproc_ValidatePropRange( pctxt->idComponent, pProps[i].idprop, 0, 0, pctxt )
			&& pProps[i].idprop == 0)
		{
			//Update the status to reflect the change in control
			LsdpConfigure( pdef, (pProps[i].value.vByte) & LSDP_MODE_MASK, pctxt->pConv->idConversation);
		}
	}
	pctxt->bHandled = true;
}
/**
* Handler called to Read properties on Lows Speed Data Port Components 0x90,
* 0x91, 0x92, 0x93.
*/
void OnLsdpReadProperties( Dci_Context* pctxt)
{
	byte buff[MAX_MSG_SIZE];
	byte ctProps =0;
	int idxPort, len,i;
	DataPorts_Def* pdef;
	Dci_Property* pProps;

	//Get the Query
	Dci_TypedPropertiesQuery* ptdq = (Dci_TypedPropertiesQuery*) pctxt->pMsg;
	byte*					  pids = (byte*) Dci_TypedPropertiesQuery_GetPropIds( ptdq);  

	// Grab port def.
	idxPort = pctxt->idComponent - WCACOMP_LSDP0;
	pdef = s_portdefs + idxPort;

	//Initialize Response Message.
	len		= Dci_TypedProperties_Init( buff, sizeof(buff), pctxt->idComponent, ptdq->ctProperties, NULL );
	pProps	= (Dci_Property*) Dci_TypedProperties_GetProperties( (Dci_TypedProperties*)buff);  
	ctProps = 0;
	
	//Iterate properties copying the specified property values.
	for( i = 0; i < ptdq->ctProperties; i++)
	{
		Dci_Property *p = pProps + ctProps;
		switch( pids[i])
		{
		case FPGA_LSDP_STATUS://Port Status
			p->idtype = PT_BYTE;
			p->idprop = FPGA_LSDP_STATUS;
			p->value.vByte = pdef->PortStatus;
			ctProps++;
			break;
		}
	}
	
	//Prepare response message 	
	((Dci_TypedProperties*)buff)->ctProperties = ctProps; //Sets the actual number processed.
	len = Dci_TypedProperties_MsgLength( (Dci_TypedProperties*) buff);

	//Return the specified properties. Property count less than 
	//what was requested means the property was not defined or write only.
	Dciproc_SendMessage1( buff, len, false, pctxt);
	pctxt->bHandled = true;
}

/**
* Helper function sets up port state when changing modes.
*/
void LsdpConfigure( DataPorts_Def *pdef, byte mode, int idconv)
{
	byte buff[MAX_MSG_SIZE];
	int len;
	Dci_Property *pP;
	
	//Update the port status information.
	pdef->PortStatus    = (pdef->PortStatus & ~LSDP_MODE_MASK) | mode & LSDP_MODE_MASK;
	pdef->idc		    = idconv;
	pdef->bDoSingleShot = (bool)(mode == LSDP_SINGLESHOT);
	pdef->Framecount 	= 0;
	
	//Reset the port to match the current configuration.
	LsdpReset( pdef);

	//Send a notification back to the server, that we changed this.
	//Get the properties and update the message state.
	len = Dci_TypedProperties_Init( buff, sizeof(buff),pdef->IdComponent, 1, NULL);
	pP  = Dci_TypedProperties_GetProperties( (Dci_TypedProperties*) buff);
	
	pP->idprop		= FPGA_LSDP_STATUS;
	pP->idtype		= PT_BYTE;
	pP->value.vByte = pdef->PortStatus;

	Dciproc_SendMessage( buff, len, false, pdef->idc);
}


/**
* Helper function resets LSDP State locally and on FPGA.
*/
void LsdpReset( DataPorts_Def* pdef)
{
	byte offset = pdef->IdComponent - WCACOMP_LSDP0; //range 0-3
	byte  mode   = (pdef->PortStatus & LSDP_MODE_MASK);

	//Get Bits to create masks first.
	byte reset   = (byte)(0x01 << offset);
	byte enable	 = (byte)(0x10 << offset);
	byte events  = (byte)(0x01 << offset); 
	byte maskCtl = ~(reset | enable);
	byte maskEvt = events;
	bool bIsOutputPort = (bool)((pdef->PortStatus & LSDP_OUTPUT) == LSDP_OUTPUT);

	
	//Set enable and events value to reflect if it is actually being enabled or disabled.
	bool bEnable = (byte)((mode == LSDP_CONTINUOUS) || (LSDP_SINGLESHOT == mode && pdef->bDoSingleShot)); 
	reset  = (bEnable) ? reset  : 0;

	//Only Enable LSDP Port if it is an output (RX) port and it
	//needs to be enabled, otherwise disabled.
	enable = (bEnable && bIsOutputPort) ?  enable : 0;

	//Only turn on events for this port if it is an output (RX) port and
	//it needs to be enabled, otherwise disable always.
	events = (bEnable && bIsOutputPort ) ? events : 0;
	
	//Save as the current state
	s_FpgaLsdpControl = (s_FpgaLsdpControl & maskCtl) | enable;

	//Apply changes to LSDP Control Register in the FPGA and do a reset if needed.
	if( reset) Fpga_WriteByte( WCAHAL_LSDP_CONTROL, s_FpgaLsdpControl | reset);
	Fpga_WriteByte( WCAHAL_LSDP_CONTROL, s_FpgaLsdpControl);

	//ClearEventStatus and enable the LSDP Event.
	//Do not mess with other events mask bits.
	Fpga_ClearEventStatus( events, false);
	Fpga_SetEventMask( maskEvt, events); //Turns on/off the events.

	//Update the reporting state.
	pdef->Framecount = 0;
}

//Handler called to process TX LSDP FIFO Events.
void OnHandleLsdpTxEvent( byte idxPort)
{
	//Get the status of the LSDP FIFO and process accordingly.
	DataPorts_Def* pdef = s_portdefs + idxPort;
	int	mode = (pdef->PortStatus & LSDP_MODE_MASK); 

	byte *pdat;
	byte  addr	  = WCAHAL_LSDP0_FIFO + idxPort; 
	byte  maskEvt = (byte)(0x1 << idxPort);

	//Fire event at host letting them know we are almost empty.
	//SEND MORE DATA PLEASE!!!!
	byte buff[MAX_MSG_SIZE];
	int  lenMsg = Dci_HalEventNotification_Init( buff, 
		pdef->IdComponent, LSDP_WCAEVENT_ALMOSTEMPTY, 0, pdef->Framecount);
				
	//Send data to the conversation that initiated it.
	Dciproc_SendMessage( buff, lenMsg, false, pdef->idc);
	
	//Clear the status register but do not re-enable
	//the event since we don't have any data to clear the event;
	//wait for the host to send more data before doing that.
	Fpga_ClearEventStatus(maskEvt, false);
}
/*
	int lastFrameCount = -1;
	int ctGoodFrame = 0;
	int ctMissFrame = 0;
	int accumDelta = 0;
*/
/**
* Low Speed Data Port Input Message.
*/
void OnLsdpWriteData( Dci_Context* pctxt)
{
	int  i, idx, mode;
	byte addr;
	Dci_TypedDataRecord *pdat;
	DataPorts_Def* pdef;		

	//Get important data.
	pdat	= (Dci_TypedDataRecord*) pctxt->pMsg;
	addr	= ((byte) pdat->idtype) - 0x80;
	idx		= addr - 0x10;
	pdef 	= s_portdefs + idx;
	mode	= (pdef->PortStatus & LSDP_MODE_MASK); 

	if( mode == LSDP_CONTINUOUS || pdef->bDoSingleShot )
	{
		/*
		if( pdef->Framecount == 0)
		{
			byte b = 0;
			Fpga_WriteFifo( addr,&b, 1);
		}
		*/
		
		// Send the data to the FPGA.
		Fpga_WriteFifo( addr, Dci_TypedDataRecord_GetData( pdat), pdat->lenData);
		
		// CJC: Frame Counting Test.
		/*
		if( (lastFrameCount != -1) && ((lastFrameCount+1) != pdat->timecode) )
		{
			int delta;
			delta = pdat->timecode - lastFrameCount;
			lastFrameCount = -1;
			++ctMissFrame;
			accumDelta += delta;
		}
		else
		{
			lastFrameCount = pdat->timecode;
			++ctGoodFrame;
		}
		*/
		pdef->Framecount++;
		
		if (pdef->Framecount > 4) //Reset the event so we are notified of potential underflow.
		{
			byte event = (byte)(0x01 << idx);
			Fpga_SetEventMask( event, event);
		}			
		else if( pdef->Framecount == 4)
		{
			//If we have filled with two messages, turn on almost 
			//empty event to be made aware of potential underflow.
			byte event = (byte)(0x01 << idx);
			Fpga_SetEventMask( event, event);

			//Enable reading of the FIFO so data goes
			//out.
			s_FpgaLsdpControl |= (byte)(0x10 << idx);
			Fpga_WriteByte( WCAHAL_LSDP_CONTROL, s_FpgaLsdpControl);	
		} 
		else if( pdef->bDoSingleShot)
		{
			//If in single shot mode, stop now.
			//Terminate if we are in single shot mode.				
			pdef->bDoSingleShot = false;

			//Enable reading of the FIFO so data goes
			//out.
			s_FpgaLsdpControl |= (byte)(0x10 << idx);
			Fpga_WriteByte( WCAHAL_LSDP_CONTROL, s_FpgaLsdpControl);
		}	
	}
	else if( (pdat->timecode % 500) == 0 )
	{
		main_LogMsg( LOG_WARN, pdef->IdComponent, "Input (TX) Port Disabled", pctxt);					
	}

	pctxt->bHandled = true;
}


/** 
 * Generates test data for output ports.
 */
int OnLsdpReadData( byte idxPort, uint32 timecode)
{
	//Get the status of the LSDP FIFO and process accordingly.
	DataPorts_Def* pdef = s_portdefs + idxPort;
	int	mode = (pdef->PortStatus & LSDP_MODE_MASK); 
	
	//If the fifo is nearly full, and we are running. Build a message and send it.
	if( mode == LSDP_CONTINUOUS || pdef->bDoSingleShot )
	{
		byte buff[MAX_MSG_SIZE];
		int i, lenMsg;
		byte *pdat;
		byte	addr	= pdef->IdComponent - 0x80;
		byte	maskEvt = (byte)(0x1 << idxPort);
			
		//If there is FIFO Data to send, do so.
		if( (Fpga_IsEventStatus(maskEvt))) 
		{
			//If first frame, clear off the extraneous byte.
			//Per FIFO design, the first byte is always
			//0 after a reset.
			if( pdef->Framecount == 0)
			{
				Fpga_ReadFifo( addr, buff, 1);
			}
			
			//Create the typed data record so we can copy properties into it.
			lenMsg = Dci_TypedDataRecord_Init( buff, sizeof( buff),
				0, 0, pdef->Framecount, pdef->IdComponent, 0, MAX_IQ_PAIRS_BYTES, NULL);

			//Get pointer to the data and read FIFO.
			pdat = Dci_TypedDataRecord_GetData( (Dci_TypedDataRecord*) buff);
			Fpga_ReadFifo( addr, pdat, MAX_IQ_PAIRS_BYTES);

			//Send data to the conversation that initiated it.
			Dciproc_SendMessage( buff, lenMsg, false, pdef->idc);

			//Update for next output.
			pdef->Framecount++;

			//Clear the status register to await next event and
			//re-enable events on the chip.
			if( mode == LSDP_CONTINUOUS)
			{
				Fpga_ClearEventStatus(maskEvt, true);
			}
			else if( pdef->bDoSingleShot)
			{
				//Terminate if we are in single shot mode.				
				pdef->bDoSingleShot = false;
				LsdpReset(pdef);
			}
			
		}
	}

	return 0;
}

