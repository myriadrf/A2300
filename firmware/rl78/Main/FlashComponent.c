/************************************************************************
 * Name:  flash.c
 * 
 * This module is the proprietary property of Loctronix Corporation
 * Copyright (C) 2007 - 2013 Loctronix Corporation
 * All Rights Reserved
 * 
 ***************************************************************************/
#include "main.h"
#include "FlashComponent.h"
#include <string.h>
#include <stdio.h>

#include <Comm/Dci/WcaMsgs.h>

//*******************************************************
//Internal Module Structures and Function Prototypes
//*******************************************************

//Define BIT client for Flash data transfer.
Dci_BitClient s_bitFlash;

//Define BIT handlers.
static byte OnBitInitiateSourceTransfer(Dci_BitOperation* pbop);
static byte OnBitInitiateTargetTransfer(Dci_BitOperation* pbop);
static int  OnBitGetFrameData(Dci_BitOperation* pbop, byte* buff, uint16 ctBytes);
static int  OnBitSetFrameData(Dci_BitOperation* pbop, byte* buff, uint16 ctBytes);
static void OnBitTransferComplete(Dci_BitOperation* pbop, byte idStatus);

//Wca Message Handlers.
static void OnWcaAction( Dci_Context* pctxt);

DCI_BEGIN_MSGMAP( s_flashmap)
	DCI_WCAMSG_HANDLER( Dci_ExecuteAction_Id, 			 WCACOMP_FLASH, &OnWcaAction)
DCI_END_MSGMAP

//*******************************************************
// Common Module Interface Implementation
//*******************************************************

/**
 * Initialization
 */
void FlashComponent_Init( )
{
	main_AddDciMsgMap( s_flashmap);
	
	//Initialize a BIT client and register.
	Dci_BitClient_Init( &s_bitFlash, WCACOMP_FLASH);
	s_bitFlash.fncInitiateSourceTransfer 	= &OnBitInitiateSourceTransfer;
	s_bitFlash.fncInitiateTargetTransfer 	= &OnBitInitiateTargetTransfer;
	s_bitFlash.fncGetFrameData 				= &OnBitGetFrameData;
	s_bitFlash.fncSetFrameData 				= &OnBitSetFrameData;
	s_bitFlash.fncTransferComplete 			= &OnBitTransferComplete;
	
	Dciproc_BitRegisterClient( &s_bitFlash);
}

/**
 * Processes on going updates
 */
void Flash_OnUpdate( )
{
	//CHeck to support BIT FILE UPLOAD.
}

//*******************************************************
// DCI MESSAGE HANDLERS Implementation
//*******************************************************

/**
* Action Message Handler
*/
void OnWcaAction( Dci_Context* pctxt)
{
	int	 ec;
	//TODO, need to define Action ID for Clearing Flash Memory.  
	//This needs to be updated in the ASR-2300 Communications Interface Specification.
	Dci_ExecuteAction *pact = (Dci_ExecuteAction*) pctxt->pMsg;
	switch( pact->idAction)
	{
		case FLASH_ActionErase:
			main_LogMsg( LOG_INFO, WCACOMP_FLASH,"Erasing", pctxt);
			ec = Flash_Erase();
			main_LogMsg( LOG_INFO, WCACOMP_FLASH,ec? "Erase Err":"Erase Success", pctxt);
		break;
		case FLASH_ActionValidate:
			//TODO
		break;
	}
	pctxt->bHandled = true;
}

//*******************************************************
// BIT Operation handlers
//*******************************************************

byte OnBitInitiateSourceTransfer(Dci_BitOperation* pbop)
{
	int ec;
	Flash_Hdr hdr;

	// Get flash header from flash.
	ec = Flash_Hdr_Read(&hdr);

	//If we were successful in reading header initiate transfer.
	if( ec == 0x0 && Flash_Hdr_Validate( &hdr) )
	{
		//Initialize the pbop with hdr information.
		Dci_BinaryImageTransfer* pbiti = &(pbop->bitinfo);
		Dci_BinaryImageTransfer* pbitr = &(hdr.bitParams);
		
		memcpy( pbiti->szName, pbitr->szName, 32);
		memcpy( pbiti->szName, pbitr->szName, 32);
		pbiti->sizeImg   = pbitr->sizeImg;
		pbiti->sizeFrame = pbitr->sizeFrame;
		pbiti->ctFrames  = pbitr->ctFrames;
		
		return BSE_InitiatingTransfer;
	}

	return BSE_ReadError;
}
byte OnBitInitiateTargetTransfer(Dci_BitOperation* pbop)
{
	//Nothing todo we are ready to go.
	return BSE_InitiatingTransfer;

}
int  OnBitGetFrameData(Dci_BitOperation* pbop, byte* buff, uint16 ctBytes  )
{
	int ec;	
	long addr;
	int sizeFrame = pbop->bitinfo.sizeFrame;	

	// Compute the Flash read address and read the data.
	addr  = FLASH_IMAGE_OFFSET + (pbop->idFrame * sizeFrame);	
	ec 	  = Flash_Read(addr, (int) ctBytes, buff);
	
	// Verify.
	#if defined(CJC_VERIFIED_THIS_GOOD)
	for(i=0; i<256; i++)
	{
		if( *(buff+i) != (byte)i )
			++nErr;
	}
	#endif	
	
	return (ec) ? -1 : ctBytes;
	
}
int  OnBitSetFrameData(Dci_BitOperation* pbop, byte* buff, uint16 ctBytes )
{
	int ec;	
	long addr;
	int  ctPad = 0;
	int  sizeFrame = pbop->bitinfo.sizeFrame;
	
	/*---- write frame to EEPROM ----*/
	addr = FLASH_IMAGE_OFFSET + (pbop->idFrame * sizeFrame);

	//Pad the frame with zero so we have writing on even boundaries.
	if( ctBytes < sizeFrame)
	{
		//Calculate the number of bytes to pad out to boundary.
		ctPad = FLASH_FRAME_SIZE - (ctBytes & (FLASH_FRAME_SIZE-1));
		memset( buff + ctBytes, 0, ctPad); //zero out remaining elements.
	}
	
	//Write the data and/or any padded bytes to the flash.
	ec = Flash_Write( addr, ctBytes+ctPad, buff);
	return (ec) ? 0 : ctBytes; // a write error occurred.
}
void OnBitTransferComplete(Dci_BitOperation* pbop, byte idStatus)
{
	// If transfer complete finish writing whole block and header.
	if( idStatus == BSE_TransferComplete 
		&& pbop->state  == DCI_BOS_TARGET_TRANSFER)
	{
		Flash_Hdr hdr;
		
		//TODO Calculate Checksum		
		Flash_Hdr_Init( &hdr, &(pbop->bitinfo), 0u);		
				
		//Write the hdr. 
		Flash_Hdr_Write(&hdr);
	}
}

