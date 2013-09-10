/************************************************************************
 * Name:  RFComponent.c
 * 
 * This module is the proprietary property of Loctronix Corporation
 * Copyright (C) 2007 - 2013 Loctronix Corporation
 * All Rights Reserved
 * 
 ***************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_wdt.h"
#include "r_cg_userdefine.h"
#include "main.h"
#include "RfComponent.h"

#include "dciproc.h"
#include "RfConfig.h"
#include "Drivers/lime.h"
#include "RFProfiles.h"
#include "Comm/Dci/StandardMsgs.h"
#include "Comm/Dci/WcaMsgs.h"
#include "Comm/Dci/DciMsg.h"


#include <string.h>
#include <stdio.h>

/****************************************************************************
*  Defines & Typedefs
****************************************************************************/

/****************************************************************************
*  Local Prototypes
****************************************************************************/
// DCI Message Handling 
static void OnWcaWriteProperties( Dci_Context* pctxt);
static void OnWcaReadProperties( Dci_Context* pctxt);
static void OnWcaAction( Dci_Context* pctxt);
static void OnWcaSetPath( Dci_Context* pctxt);
static void NotifyPropChanges(Dci_Property* pProps, byte ctProps, Dci_Context* pctxt);

/****************************************************************************
*  Local Declarations
****************************************************************************/
/**
* Define DCI Message map for Lime components
*/
DCI_BEGIN_MSGMAP( s_RfComponentMap)
	//Message Handlers for WCACOMP_RF0.
	DCI_WCAPROP_HANDLER( WCACOMP_RF0, 0x0, 0x9, &OnWcaWriteProperties)		//WCACOMP_RF0 Properties 0x0 through 0x9
	DCI_WCAPROP_HANDLER( WCACOMP_RF0, 0xC, 0xC, &OnWcaWriteProperties)		//WCACOMP_RF1 Property 0xC through 0xC
	DCI_WCAPROPQUERY_HANDLER( WCACOMP_RF0, 0x0, 0xE, &OnWcaReadProperties)	//WCACOMP_RF1 Property 0x0 through 0xE
	DCI_WCAMSG_HANDLER( Dci_ExecuteAction_Id, WCACOMP_RF0, &OnWcaAction)
	DCI_WCAPROP_HANDLER( WCACOMP_RF0, 0xD, 0xD, &OnWcaSetPath)		//WCACOMP_RF0 Set RxPath handler
	DCI_WCAPROP_HANDLER( WCACOMP_RF0, 0xE, 0xE, &OnWcaSetPath)		//WCACOMP_RF0 Set TxPath handler

	//Message Handlers for WCACOMP_RF1
	DCI_WCAPROP_HANDLER( WCACOMP_RF1, 0x0, 0x9, &OnWcaWriteProperties)		//WCACOMP_RF1 Property 0x0 through 0x9
	DCI_WCAPROP_HANDLER( WCACOMP_RF1, 0xC, 0xC, &OnWcaWriteProperties)		//WCACOMP_RF1 Property 0xC through 0xC
	DCI_WCAPROPQUERY_HANDLER( WCACOMP_RF1, 0x0, 0xE, &OnWcaReadProperties)	//WCACOMP_RF1 Property 0x0 through 0xE
	
	DCI_WCAMSG_HANDLER( Dci_ExecuteAction_Id, WCACOMP_RF1, &OnWcaAction)
	DCI_WCAPROP_HANDLER( WCACOMP_RF1, 0xD, 0xD, &OnWcaSetPath)		//WCACOMP_RF1 Set RxPath handler
	DCI_WCAPROP_HANDLER( WCACOMP_RF1, 0xE, 0xE, &OnWcaSetPath)		//WCACOMP_RF1 Set TxPath handler
DCI_END_MSGMAP

//Lime device indexes.
#define IDX_RF0  0
#define IDX_RF1  1

//Keep a copy of all property states so can be read back. 
#define NUM_RF_PROP (15)   // RF Properties Per RX or TX
#define RF1_OFFSET  (15)   // RF Offset to RX Table.

Dci_Property  s_propState[] = {
	//RF0 Properties
	{RFPROP_RFCTRL, 	PT_BYTE, 	(byte) 0 },
	{RFPROP_SAMPLING, 	PT_BYTE, 	(byte) 0 },	
	{RFPROP_RXGAIN,		PT_BYTE, 	(byte) 0 },
	{RFPROP_RXFREQ,		PT_UINT32, 	(uint32) 0 },
	{RFPROP_RXNCO,		PT_UINT32, 	(uint32) 0 },
	{RFPROP_RXBANDWIDTH,PT_BYTE, 	(byte) 0 },
	{RFPROP_TXGAIN,		PT_BYTE, 	(byte) 0 },
	{RFPROP_TXFREQ,		PT_UINT32,	(uint32) 0 },
	{RFPROP_TXNCO,		PT_UINT32,	(uint32) 0 },
	{RFPROP_TXBANDWIDTH,PT_BYTE, 	(byte) 0 },
	{RFPROP_RXRSSI,		PT_BYTE, 	(byte) 0 },
	{RFPROP_RXIQBIAS,	PT_BYTE, 	(byte) 0 },
	{RFPROP_RFCTRL2,	PT_BYTE, 	(byte) 0 },
	{RFPROP_RXPATH,		PT_BYTE, 	(byte) 0 },
	{RFPROP_TXPATH,		PT_BYTE, 	(byte) 0 },

	//RF1 Properties
	{RFPROP_RFCTRL, 	PT_BYTE, 	(byte) 0 },
	{RFPROP_SAMPLING, 	PT_BYTE, 	(byte) 0 },	
	{RFPROP_RXGAIN,		PT_BYTE, 	(byte) 0 },
	{RFPROP_RXFREQ,		PT_UINT32, 	(uint32) 0 },
	{RFPROP_RXNCO,		PT_UINT32, 	(uint32) 0 },
	{RFPROP_RXBANDWIDTH,PT_BYTE, 	(byte) 0 },
	{RFPROP_TXGAIN,		PT_BYTE, 	(byte) 0 },
	{RFPROP_TXFREQ,		PT_UINT32,	(uint32) 0 },
	{RFPROP_TXNCO,		PT_UINT32,	(uint32) 0 },
	{RFPROP_TXBANDWIDTH,PT_BYTE, 	(byte) 0 },
	{RFPROP_RXRSSI,		PT_BYTE, 	(byte) 0 },
	{RFPROP_RXIQBIAS,	PT_BYTE, 	(byte) 0 },
	{RFPROP_RFCTRL2,	PT_BYTE, 	(byte) 0 },
	{RFPROP_RXPATH,		PT_BYTE, 	(byte) 0 },
	{RFPROP_TXPATH,		PT_BYTE, 	(byte) 0 }
};

/****************************************************************************
*  Implementation
****************************************************************************/
 
/** 
 *  initialize RfComponent
 */
void RfComp_Init()
{
    int i;

	//Bind DCI Message handlers to message map processor.
	main_AddDciMsgMap( s_RfComponentMap);


}

/**
 * Processes on going updates
 */
void RfComp_OnUpdate( )
{
//    ProcessSerialPort();
}

/****************************************************************************
*  DCI Message Handlers 
****************************************************************************/

void OnWcaWriteProperties( Dci_Context* pctxt)
{
    int i, idx, idxProp, idxTableOffset;
	Dci_Property *pProps;
	Dci_TypedProperties* ptp = (Dci_TypedProperties*) pctxt->pMsg;
    idx = ptp->idComponent - WCACOMP_RF0;

	pProps = Dci_TypedProperties_GetProperties( ptp );

	// Compute an offset to the storage table base.
	idxTableOffset = RF1_OFFSET*idx;  // idx is 0 or 1.
	
	//Process the properties.
	for( i = 0; i < ptp->ctProperties; i++)
	{
		idxProp = pProps[i].idprop + idxTableOffset;
		s_propState[idxProp] = pProps[i];
		
		switch(pProps[i].idprop)
		{
		case RFPROP_RFCTRL:
			Lime_AnalogOutput( idx, (pProps[i].value.vByte & 0x1));			
			break;
		case RFPROP_SAMPLING:
			RF_SetSamplingControl( idx, pProps[i].value.vByte);
			break;
		case RFPROP_RXGAIN:
			Lime_SetRxGain( idx, pProps[i].value.vByte);
			break;
		case RFPROP_RXFREQ:
			Lime_SetRxFrequency( idx, pProps[i].value.vUint32);
			break;
		case RFPROP_RXNCO:
			Lime_SetRxNco( idx, pProps[i].value.vUint32);
			break;
		case RFPROP_RXBANDWIDTH:
			Lime_SetRxBandwidth( idx, pProps[i].value.vByte);
			break;
		case RFPROP_TXGAIN:
			Lime_SetTxGain( idx, pProps[i].value.vByte);
			break;
		case RFPROP_TXFREQ:
			Lime_SetTxFrequency( idx, pProps[i].value.vUint32);
			break;
		case RFPROP_TXNCO:
			Lime_SetTxNco( idx, pProps[i].value.vUint32);
			break;
		case RFPROP_TXBANDWIDTH:
			Lime_SetTxBandwidth( idx, pProps[i].value.vByte);
			break;
		case RFPROP_RFCTRL2:
			RF_SetAmplifierConfig( idx, pProps[i].value.vByte); 
			break;
		default:
			break;			
		}
	}
	pctxt->bHandled = true;
}


void OnWcaSetPath( Dci_Context* pctxt)
{
	Dci_TypedProperties* ptp;
	Dci_Property *pProps;
	int i;
	ptp		= (Dci_TypedProperties*) pctxt->pMsg;
	pProps  = Dci_TypedProperties_GetProperties( ptp);

	for( i = 0; i < ptp->ctProperties; i++)
	{
		//Set the specified path.
		int ec = RfComp_SetPath( pProps[i].value.vByte, pctxt );
		if( ec > 0)
		{
			main_LogMsg( LOG_ERR, pctxt->idComponent, "RfComp_SetPath Failed", pctxt);
		}
	}
	pctxt->bHandled = true;
}

/**
* Reads the specified properties from the Lime WCA Component.
*/
void OnWcaReadProperties( Dci_Context* pctxt)
{
	byte buff[MAX_MSG_SIZE], ctProps=0;
	int i, idxDevice, idxProp, idxTableOffset, len;
	Dci_Property* pProps;	
	Dci_Property *p;
	
	//Get the Query
	Dci_TypedPropertiesQuery* ptdq = (Dci_TypedPropertiesQuery*) pctxt->pMsg;
	byte*					  pids = (byte*) Dci_TypedPropertiesQuery_GetPropIds( ptdq);  
    
	//Which device [0,1]
	idxDevice		= ptdq->idComponent - WCACOMP_RF0;
	//Some values are stored locally in our table.
	idxTableOffset  = RF1_OFFSET*idxDevice;

	//Initialize Response Message.	
	len	   = Dci_TypedProperties_Init( buff, sizeof(buff), pctxt->idComponent, 0, NULL );
	pProps = Dci_TypedProperties_GetProperties( (Dci_TypedProperties*)buff);  
	
	//Iterate properties copying the specified property values.
	//Process the properties.
	for( i = 0; i < ptdq->ctProperties; i++, pProps++)
	{
		switch(pids[i])
		{
        case RFPROP_RXRSSI:    
			pProps->idtype = PT_UINT16;
			pProps->idprop = RFPROP_RXRSSI;
			pProps->value.vUint16 = RF_GetReceivedSignalStrength(idxDevice);
			ctProps++;
            break;
        case RFPROP_RXIQBIAS:  
			pProps->idtype = PT_UINT32;
			pProps->idprop = RFPROP_RXIQBIAS;
			pProps->value.vUint32 = RF_GetReceivedAdcBiasOffset(idxDevice);
			ctProps++;
			break;
		default:
			if( pids[i] < NUM_RF_PROP )
			{
				idxProp = idxTableOffset + pids[i];
				*pProps = s_propState[idxProp];
				ctProps++;
			}
			break;			
		}
	}
	//Prepare response message (update count in case it changed).
	((Dci_TypedProperties*)buff)->ctProperties = ctProps; //Sets the actual number processed.
	len = Dci_TypedProperties_MsgLength( (Dci_TypedProperties*) buff);

	//Return the specified properties. Property count less than 
	//what was requested means the property was not defined or write only.
	Dciproc_SendMessage1( buff, len, false, pctxt);
	pctxt->bHandled = true;
}

/**
* Executes the specified action
*/
void OnWcaAction( Dci_Context* pctxt)
{
	//Convert to typed properties message.
	Dci_ExecuteAction* pea = (Dci_ExecuteAction*) pctxt->pMsg;
    int				   idx = pea->idComponent - WCACOMP_RF0;   // Device ID.

	switch( pea->idAction)
	{
	case RFACTION_RXTUNEVCO:
		Lime_TuneRxCap( idx);
		break;
	case RFACTION_TXTUNEVCO:
		Lime_TuneTxCap( idx);
		break;
	case RFACTION_RXCALIBRATE:
		Lime_RemoveRxDCBias( idx);
		break;
	case RFACTION_TXCALIBRATE:
		Lime_RemoveTxDCBias( idx);
		break;
	default:
		break;
	}
	pctxt->bHandled = true;
}

/****************************************************************************
*  RF Set Path Functions.
****************************************************************************/

/**
* Function applies the specified path settings.
*/
int RfComp_SetPath( byte idPath, Dci_Context* pctxt)
{
	const RF_LimeRegData* pregdata;
	byte regCur, regWrite, ctProps;
	bool bTx;
	int  device, i, addr, ec, idxPropOffset;
	byte wdata[2];

	//Look up path definition
	const RF_PathDef* pdef = RF_GetPathProfile( idPath);
	if( pdef == NULL) return false;

	//Determine if we are transmitter side and which 
	//lime device
	device  = (byte)((pdef->id >> 7) & 0x1);  // [0,1]
	bTx		= (bool)((pdef->id & 6) == 0);

	//Make sure RF Power is on.
	RF_SetPower( true );

	//Pull in the Register data and initialize the lime registers.
	pregdata = RF_GetLimeRegData( pdef);

	//Configure the RF Switches
	regCur	 = RF_GetConfiguration();
	regWrite = (regCur & ~(pdef->RfSwitchesConfig.mask)) | pdef->RfSwitchesConfig.val;
	RF_SetConfiguration( regWrite);

	//Configure the RF Amplifiers.
	regCur = RF_GetAmplifierConfig( device);
	regWrite = (regCur & ~(pdef->AmplifierConfig.mask)) | pdef->AmplifierConfig.val;
	RF_SetAmplifierConfig( device, regWrite);

	//Configure the Sampling Interface
	regCur = RF_GetSamplingControl( device);
	regWrite = (regCur & ~(pdef->SamplingControl.mask)) | pdef->SamplingControl.val;
	RF_SetSamplingControl( device, regWrite);

	//Set up the lime Registers
	for( i = 0; i < pdef->ctEntries; i++)
	{
		//Read the register value.
		addr = pregdata[i].addr;
		ec = Lime_Read( device, addr, &regCur);
		if( ec != 0) return ec;

		//Apply mask and updated settings.
		regWrite = (regCur & ~(pregdata[i].mask)) | pregdata[i].val;

		//Write the register value.
		wdata[0] = (byte)addr;
		wdata[1] = regWrite;
		ec = Lime_Write( device, 2, wdata);
	}

	//Apply Initial Frequency, bandwidth, gain.
	idxPropOffset = device* RF1_OFFSET;
	if( bTx) //Set the Transmitter side.
	{
		//Save updated state first.
		s_propState[ RFPROP_TXGAIN + idxPropOffset].value.vByte = pdef->Gain;
		s_propState[ RFPROP_TXBANDWIDTH + idxPropOffset].value.vByte = pdef->Bandwidth;
		s_propState[ RFPROP_TXFREQ + idxPropOffset].value.vUint32 = pdef->Frequency;
		
		Lime_SetTxGain( device, pdef->Gain); 
		Lime_SetTxBandwidth( device, pdef->Bandwidth);
		Lime_SetTxFrequency( device, pdef->Frequency );
		
		//Notify Prop Changes
		//Only send the properties updated above (plus txnco).
		ctProps = (RFPROP_TXBANDWIDTH - RFPROP_TXGAIN) + 1;
		NotifyPropChanges( s_propState + idxPropOffset + RFPROP_TXGAIN, ctProps, pctxt );
	}
	else //Set Receiver side.
	{
		//Save updated state first.
		s_propState[ RFPROP_RXGAIN + idxPropOffset].value.vByte = pdef->Gain;
		s_propState[ RFPROP_RXBANDWIDTH + idxPropOffset].value.vByte = pdef->Bandwidth;
		s_propState[ RFPROP_RXFREQ + idxPropOffset].value.vUint32 = pdef->Frequency;
		
		Lime_SetRxGain( device, pdef->Gain);
		Lime_SetRxBandwidth( device, pdef->Bandwidth);
		Lime_SetRxFrequency( device, pdef->Frequency );

		//Notify Prop Changes
		ctProps = (RFPROP_RXBANDWIDTH - RFPROP_RXGAIN) + 1;
		NotifyPropChanges( s_propState + idxPropOffset + RFPROP_RXGAIN, ctProps, pctxt );
	}
}

/**
* Helper function notifies property changes given the contiguous array.
*/
void NotifyPropChanges(Dci_Property* pProps, byte ctProps, Dci_Context* pctxt)
{
	byte buff[MAX_MSG_SIZE];
	int len;
	//Send a notification back to the server, that we changed this.
	//Get the properties and update the message state.	
	len = Dci_TypedProperties_Init( buff, sizeof(buff), pctxt->idComponent, ctProps, pProps);
	Dciproc_SendMessage1( buff, len, false, pctxt);
}
