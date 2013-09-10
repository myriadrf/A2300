// Name:  flash.c
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

#include "r_cg_macrodriver.h"
#include "main.h"
#include "flash.h"
#include "Timers.h"
#include "SPI.h"

#include <string.h>
#include <stdio.h>
#include<Comm/Dci/WcaMsgs.h>


//*******************************************************
//Internal Module Structures and Function Prototypes
//*******************************************************
#define FLASH_MEMORY_SIZE 1024*1024
#define FLASH_FRAME_SIZE 0x100  //256 bytes per read or write.

static Flash_Hdr 	s_flashHdr;
static uint16  		s_idFrame;
static uint32  		s_ctBytesRemaining;

// DCI Message Handlers.
void OnFlashImgBeginDownload( Dci_Context* pctxt);
void OnFlashImgBeginUpload( Dci_Context* pctxt);
void OnFlashImgDowloadFrame( Dci_Context* pctxt);
void OnFlashImgUploadFrame( Dci_Context* pctxt);
void OnWcaAction( Dci_Context* pctxt);

/**
* Send binary image 
*/

//*******************************************************
//Static Data
//*******************************************************
DCI_BEGIN_MSGMAP( s_flashmap)
	DCI_WCAMSG_HANDLER( Dci_BinaryImageTransfer_Id,    	 WCACOMP_FLASH, &OnFlashImgBeginDownload)
	DCI_WCAMSG_HANDLER( Dci_BinaryImageTransferQuery_Id, WCACOMP_FLASH, &OnFlashImgBeginUpload)
	DCI_WCAMSG_HANDLER( Dci_BinaryImageTransferFrame_Id, WCACOMP_FLASH, &OnFlashImgDowloadFrame)
	DCI_WCAMSG_HANDLER( Dci_BinaryImageTransferStatus_Id,WCACOMP_FLASH, &OnFlashImgUploadFrame)	
	DCI_WCAMSG_HANDLER( Dci_ExecuteAction_Id, 			 WCACOMP_FLASH, &OnWcaAction)
DCI_END_MSGMAP

//*******************************************************
// Common Module Interface Implementation
//*******************************************************

/**
 * Initialization
 */
void Flash_Init( )
{
BYTE cmd;
BYTE id[32];
byte image[256];  // For testing only.
int	 ec;
int  i;

	main_AddDciMsgMap( s_flashmap);

	/*---- wait for power up before accessing flash ----*/
	Timer_Delay( 10000 );

	/*---- read memory ID (doesn't work on M25P80) ----*/
	cmd = READ_ID;
	ec = SPI_WriteReadData( SPI_DEVICE_FLASH, 1, &cmd, 3, id, FALSE );
	// Debug read of first block.
	ec = Flash_Read( 0lu, 256, image );
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
* Returns true if we are in a valid transfer state.
*/
bool IsTransferring( byte idTransfer)
{
	return Flash_Hdr_Validate(&s_flashHdr) 
		&& idTransfer == s_flashHdr.bitParams.idTransfer && s_idFrame >= 0;
}

/**
* Clears the transfer operation.
*/ 
void ResetTransfer()
{
	memset( &s_flashHdr, 0, sizeof( Flash_Hdr));
	s_idFrame = -1;
	s_ctBytesRemaining = 0;
}

/*
** Host Write information
*/
void OnFlashImgBeginDownload( Dci_Context* pctxt)
{
	byte buff[MAX_MSG_SIZE];
	int lenMsg;
	
	Dci_BinaryImageTransfer *pBit = (Dci_BinaryImageTransfer*) pctxt->pMsg;
	s_idFrame = 0;		// used to check frame-id order

	//Initialize Message Header for file transfer.
	Flash_Hdr_Init( &s_flashHdr, pBit, 0u);
	
	//Send a response that we are initiating.
	lenMsg = Dci_BinaryImageTransferStatus_Init( buff, WCACOMP_FLASH, 
		pBit->idTransfer, s_idFrame, 0, BSE_InitiatingTransfer);
	Dciproc_SendMessage1(buff, lenMsg, false, pctxt);
			
	pctxt->bHandled = true;
}

/*
** Receive data from the Host and save to flash
*/
void OnFlashImgDowloadFrame( Dci_Context* pctxt)
{
	byte buff[MAX_MSG_SIZE];
	byte *pdata;
	int lenMsg, i, nErr, ec = 0;
	byte idStatus;
	long addr;
	ushort ctPad = 0;
	
	Dci_BinaryImageTransferFrame* pBitf = (Dci_BinaryImageTransferFrame*) pctxt->pMsg;

	// check transfer id
	if (!IsTransferring( pBitf->idTransfer ) )
		return;
		
	// Check frame id and set up status.
	idStatus = (pBitf->idFrame != s_idFrame)
		? BSE_FrameError 
		: ( s_idFrame < (s_flashHdr.bitParams.ctFrames-1)) 
			? BSE_ReadyNextFrame : BSE_TransferComplete;
	
	/*---- write frame to EEPROM ----*/
	addr = FLASH_IMAGE_OFFSET + (pBitf->idFrame * s_flashHdr.bitParams.sizeFrame);
	pdata = Dci_BinaryImageTransferFrame_GetData(pBitf);

	//Pad the frame with zero so we have writing on even boundaries.
	if( pBitf->ctBytes < s_flashHdr.bitParams.sizeFrame)
	{
		//Calculate the number of bytes to pad out to boundary.
		ctPad = FLASH_FRAME_SIZE - (pBitf->ctBytes & (FLASH_FRAME_SIZE-1));
		memset( pdata + pBitf->ctBytes, 0, ctPad); //zero out remaining elements.
	}
	
	//Write the data and/or any padded bytes to the flash.
	ec = Flash_Write( addr, pBitf->ctBytes + ctPad, pdata);
	if( ec ) {
		idStatus = BSE_WriteError;
		ResetTransfer();
	}
	
	//Send the current status so process can continue.
	lenMsg = Dci_BinaryImageTransferStatus_Init( buff, WCACOMP_FLASH, 
		pBitf->idTransfer, s_idFrame, pBitf->ctBytes, idStatus);
	Dciproc_SendMessage1(buff, lenMsg, false, pctxt);
	
	if( !ec )
		++s_idFrame;
		
	// If transfer complete finish writing whole block and header.
	if( idStatus == BSE_TransferComplete )
	{
		//Write Flash Header with calculated check sum value.
		s_flashHdr.crc = 0x0;				//TODO Calculate Checksum
		Flash_Hdr_Write(&s_flashHdr);
	}
	
	pctxt->bHandled = true;
}


/*
** Host Query (read) for information
*/
void OnFlashImgBeginUpload( Dci_Context* pctxt)
{
	int ec;

	Dci_BinaryImageTransferQuery* pbitq = (Dci_BinaryImageTransferQuery*)pctxt->pMsg;	
	
	// Get flash header from flash.
	ec = Flash_Hdr_Read(&s_flashHdr);
	
	//If we were successful in reading header initiate transfer.
	if( ec == 0x0 && Flash_Hdr_Validate( &s_flashHdr) )
	{
		//Send back the bit params we stored previously.
		Dciproc_SendMessage1((byte*) &(s_flashHdr.bitParams), 
			sizeof( Dci_BinaryImageTransfer), false, pctxt);
		
		//Initialize the frame id so we can begin sending.
		s_idFrame = 0;
	}
	else // error reply
	{
		byte buff[MAX_MSG_SIZE];
		int lenMsg = Dci_BinaryImageTransferStatus_Init( buff, WCACOMP_FLASH, 
					pbitq->idTransfer, s_idFrame, 0, BSE_ReadError);
		Dciproc_SendMessage1(buff, lenMsg, false, pctxt);
		ResetTransfer();
	}

	pctxt->bHandled = true;
}



/*
** Handle Bit Status Message to report Flash Data to the Host.
*/
void OnFlashImgUploadFrame(Dci_Context* pctxt)
{
	byte buff[MAX_MSG_SIZE];
	uint32 addr;
	int lenMsg;
	int ec = 0x0;
	int i;
	int nErr = 0;
	
	//Switch on the Status Message ID.
	Dci_BinaryImageTransferStatus *pMsg = (Dci_BinaryImageTransferStatus *) pctxt->pMsg;
	
	//Do not proceed if we are not actually doing a binary transfer upload.
	if( !IsTransferring( pMsg->idTransfer))
		return; 
	
	switch(pMsg->idStatus)
	{
		case(BSE_InitiatingTransfer):		// Ready to send.
			// Host indicates we can start sending now (Send Enable).
			s_ctBytesRemaining = s_flashHdr.bitParams.sizeImg;

			// Continue to respond with a frame.
		case(BSE_ReadyNextFrame):			// Send next frame.
			// Send a frame of data.
			{
				byte *pdata;
				uint16 ctBytes;
				// Compute the Flash read address.		
				addr    = FLASH_IMAGE_OFFSET +
						 ((uint32)s_idFrame * (uint32)s_flashHdr.bitParams.sizeFrame);

				ctBytes = (s_ctBytesRemaining > s_flashHdr.bitParams.sizeFrame) 
						? s_flashHdr.bitParams.sizeFrame 
						: (uint16) s_ctBytesRemaining;
				
				// Create Frame message and get point to read data into it.
				lenMsg = Dci_BinaryImageTransferFrame_Init( buff, WCACOMP_FLASH, 
							pMsg->idTransfer, 0, s_idFrame, ctBytes, NULL);
				pdata  = Dci_BinaryImageTransferFrame_GetData( (Dci_BinaryImageTransferFrame *)buff );
				
				ec 	  = Flash_Read(addr, (int)ctBytes, pdata);
				
				// Verify.
				#if defined(CJC_VERIFIED_THIS_GOOD)
				for(i=0; i<256; i++)
				{
					if( *(pdata+i) != (byte)i )
						++nErr;
				}
				#endif
				
				//If everything ok, send it.
				if( ec == 0x0 )
				{
					Dciproc_SendMessage1(buff, lenMsg, false, pctxt);
					++s_idFrame;
					s_ctBytesRemaining -= ctBytes;
					
					if( s_idFrame < s_flashHdr.bitParams.ctFrames)
					{
						break; //Done get out.
					}
				}
				else // Send the error message and then quit.
				{
					// If we get here, a read error of some sort ocurred, drop out 
					// and kill the transfer operation.
					lenMsg = Dci_BinaryImageTransferStatus_Init( buff, WCACOMP_FLASH, 
								pMsg->idTransfer, s_idFrame, 0, BSE_ReadError);
					Dciproc_SendMessage1(buff, lenMsg, false, pctxt);
				}

				//if we get here, we failed
				ResetTransfer();
			}
			break;
			
		// Default conditions hault transfer
		case(BSE_FrameError):
		case(BSE_OperationNotAvailable):
		case(BSE_WriteError):
		default:
			ResetTransfer();
			break;
	}

	pctxt->bHandled = true;
}

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
// Internal Implementation
//*******************************************************


/****************************************************************

   CALL:    Flash_Read()
                             
   INTRO:   This routine reads data from the serial flash eprom.

   INPUT:   addr		- 24-bit address
			len			- length of data to read
			dptr		- pointer to dest for read
   OUTPUT:  0 if no error

****************************************************************/
int  Flash_Read( uint32 addr, int len, BYTE* dptr )
{
BYTE cmd[4];
int	 ec;
	
	cmd[0] = READ_DATA;
	cmd[1] = (BYTE)((addr>>16) & 0xFF);
	cmd[2] = (BYTE)((addr>>8) & 0xFF);
	cmd[3] = (BYTE)(addr & 0xFF);
	ec = SPI_WriteReadData( SPI_DEVICE_FLASH, 4, cmd, len, dptr, FALSE );

	return( ec );
}

/****************************************************************

   CALL:    Flash_Write()
                             
   INTRO:   This routine writes data to the serial flash eprom.

			This routine does not support writing beyond a 256
			byte page address.

   INPUT:   addr		- 24-bit address
			len			- length of data to write
			sptr		- pointer to data to write
   OUTPUT:  0 if no error

****************************************************************/
int  Flash_Write( uint32 addr, int len, BYTE* sptr )
{
BYTE cmd[4];
BYTE status = 0x0;
int	 ec = 0x0;

	/*---- if reading the first address, check to see if the we're still erasing the part ----*/
	cmd[0] = READ_STATUS;
	ec = SPI_WriteReadData( SPI_DEVICE_FLASH, 1, cmd, 1, &status, FALSE );
	if( (status & FLASHSTATUS_WIP) || ec ) {
		return( 0xff );		// we're still busy writing or there's an error...
	}

	/*---- first must set the write enable bit ----*/
	cmd[0] = WRITE_ENABLE;
	ec = SPI_WriteReadData( SPI_DEVICE_FLASH, 1, cmd, 0, 0, FALSE );
	/*---- now send the "page program" to write up to 256 bytes of page ----*/
	cmd[0] = PAGE_PROGRAM;
	cmd[1] = (BYTE)((addr>>16) & 0xFF);
	cmd[2] = (BYTE)((addr>>8) & 0xFF);
	cmd[3] = (BYTE)(addr & 0xFF);
	ec = SPI_WriteReadData( SPI_DEVICE_FLASH, 4, cmd, 0, 0, TRUE );		// write command and leave CS low
	ec = SPI_WriteReadData( SPI_DEVICE_FLASH, len, sptr, 0, 0, FALSE );	// write flash data

	/*---- now wait until write is complete by polling status register ----*/
	cmd[0] = READ_STATUS;
	do {
		ec = SPI_WriteReadData( SPI_DEVICE_FLASH, 1, cmd, 1, &status, FALSE );
		WATCHDOG_RESET 
	} while( (status & FLASHSTATUS_WIP) && !ec );

	return( ec );
}

/****************************************************************

   CALL:    Flash_Erase()
                             
   INTRO:   This routine erases the entire flash

   INPUT:   nothing
   OUTPUT:  0 if no error

****************************************************************/
int  Flash_Erase( void )
{
BYTE cmd[4];
BYTE status;
int	 ec;

	/*---- first must set the write enable bit ----*/
	cmd[0] = WRITE_ENABLE;
	ec = SPI_WriteReadData( SPI_DEVICE_FLASH, 1, cmd, 0, 0, FALSE );
	/*---- now send the "bulk erase" command to erase entire flash ----*/
	cmd[0] = BULK_ERASE;
	ec = SPI_WriteReadData( SPI_DEVICE_FLASH, 1, cmd, 0, 0, FALSE );

#if 0
!!!! IT TAKES 8 SECONDS TO DO A COMPLETE CHIP ERASE !!!!
	/*---- now wait until write is complete by polling status register ----*/
	cmd[0] = READ_STATUS;
	do {
		ec = SPI_WriteReadData( SPI_DEVICE_FLASH, 1, cmd, 1, &status, FALSE );
		WATCHDOG_RESET 
	} while( (status & FLASHSTATUS_WIP) && !ec );
#endif

	return( ec );
}

/*
** Flash Header Support
** Write a header block to flash
*/
void Flash_Hdr_Init(Flash_Hdr *pHdr, Dci_BinaryImageTransfer *pBinaryImageTransfer, uint16 crc)
{
	if( pBinaryImageTransfer != NULL)
		memcpy( &(pHdr->bitParams), pBinaryImageTransfer, sizeof(Dci_BinaryImageTransfer) );
	else //Clear the structure.
		memset(&(pHdr->bitParams), 0, sizeof( Dci_BinaryImageTransfer));
		
	pHdr->crc = crc;
}

bool Flash_Hdr_Validate( Flash_Hdr *pHdr)
{
	return pHdr->bitParams.sizeImg != 0x00000000UL && pHdr->bitParams.sizeImg != 0xffffffffUL;
}

/**
 * Read Header from Flash Page-0.
**/
int Flash_Hdr_Read(Flash_Hdr *pHdr)
{
	return Flash_Read(0x0, sizeof( Flash_Hdr), (byte*)pHdr);
}

/**
 * Write Header to Flash Page-0.
**/
int Flash_Hdr_Write(Flash_Hdr *pHdr )
{
	return Flash_Write(0x0, sizeof( Flash_Hdr), (byte*)pHdr);
}

 
