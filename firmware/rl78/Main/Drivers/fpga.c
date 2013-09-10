// Name:  fpga.c
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

#pragma interrupt INTP0 ISR_WCAHAL_EVENTS

#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_serial.h"
#include "r_cg_timer.h"
#include "r_cg_wdt.h"
#include "r_cg_intc.h"
#include "r_cg_userdefine.h"

#include "../main.h"
#include "fpga.h"

#include "SPI.h"
#include "flash.h"

#include "Asr2300InterfaceDefs.h"
#include "FpgaWcaHalDefs.h"

#include<Comm/Dci/DciConversation.h>
#include<Comm/Dci/StandardMsgs.h>
#include<Comm/Dci/WcaMsgs.h>
#include<string.h>

//*******************************************************
//Internal Module Structures and Function Prototypes
//*******************************************************

//Macro disables the FPGA cpu_interrupt in the RL78
#define DISABLE_INTC0 PMK0 = 1U; PIF0 = 0U;

//Macro enables the FPGA cpu_interrupt in the RL78
#define ENABLE_INTC0  PIF0 = 0U; PMK0 = 0U; 

//Default HdwComponent Properties Handlers
void OnDciPropWriteDefault( Dci_Context* pctxt);
void OnDciPropReadDefault( Dci_Context* pctxt);

//FPGA Binary Image Transfer Handlers.
void OnBeginBitfileDownload( Dci_Context* pctxt);
void OnWriteBitfileFrame( Dci_Context* pctxt);

DCI_BEGIN_MSGMAP( s_fpgamap)
	DCI_WCAPROP_HANDLER( 	  WCACOMP_HALDEFAULT, WCAHAL_RFCONFIG, 	 WCAHAL_DEFAULTCOMP_MAX, &OnDciPropWriteDefault)
	DCI_WCAPROPQUERY_HANDLER( WCACOMP_HALDEFAULT, WCAHAL_IDWAVEFORM, WCAHAL_DEFAULTCOMP_MAX, &OnDciPropReadDefault)
	DCI_WCAMSG_HANDLER( Dci_BinaryImageTransfer_Id,		 WCACOMP_FPGA, &OnBeginBitfileDownload)
	DCI_WCAMSG_HANDLER( Dci_BinaryImageTransferFrame_Id, WCACOMP_FPGA, &OnWriteBitfileFrame)
DCI_END_MSGMAP


//*******************************************************
//Static Data
//*******************************************************

//Configuration parameters for directly loading FPGA from Binary Image Transfer
static Dci_BinaryImageTransfer s_bitParams;

//Current download frame ID.
static int s_idFrameDownload;

//Size of image being download.
static int s_imageSize;

//Define the array of Default properties. This structure provides the
//type definitions for all supported waveform component properties. 
//We will want to make this configurable in a future version and get
//rid of this table.
const Dci_Property  s_PropsDefault[] = {
	{ 0,					PT_BYTE,   (byte) 0 },		// RESERVED DO NOT USE.
	{ WCAHAL_IDWAVEFORM,	PT_UINT16, (uint16) 0 },	// Read Only Word Register
	{ WCAHAL_VERSION,		PT_UINT16, (uint16) 0 },	// Read Only Word Register
	{ WCAHAL_PORTCAPS,		PT_UINT16, (uint16) 0 },	// Read Only Word Register
	{ WCAHAL_RFCONFIG,		PT_BYTE, (byte)		0 },	// Write Only Byte Register
	{ WCAHAL_LED,			PT_BYTE, (byte)		0 },    // Write Only Byte Register
	{ 0x06,					PT_BYTE, (byte)		0 },	// Unassigned
	{ WCAHAL_LSDP_CONTROL,	PT_BYTE, (byte)		0 },	// Write Only Byte Register Controls LSDP FIFOS
	{ WCAHAL_LSDP_STATUS,	PT_BYTE, (byte)		0 },	// Ready Only Byte Register
	{ WCAHAL_LSDP_RATE,		PT_UINT32, (byte)   0 },	// Write Only DWord Register: rate = dataclock / [this value]
	{ 0xA			  , 	PT_BYTE, (byte) 	0 }, //TODO REMOVE ME.
	{ 0xB			  , 	PT_BYTE, (byte) 	0 }, //TODO REMOVE ME.
	{ 0xC			  , 	PT_UINT32, (byte) 	0 } //TODO REMOVE ME.
};


/**
* Event Status register indicates what events are set by the WCAAL_EVENT
* interrupt service routine.  These flags are or'ed by the ISR and must be cleared
* after they have been handled.
*/
byte s_FpgaEventStatus;

/**
* Event mask indicates what events are enabled and disabled.
* This is a local copy of what is written to the FPGA.
* Since we cannot read it.
*/
byte s_FpgaEventMask;

byte s_FpgaMstrCtrlReg;
//*******************************************************
// Common Module Interface Implementation
//*******************************************************

/**
 * Initialization
 */
void Fpga_Init( )
{
	s_FpgaEventStatus = 0;
	s_FpgaMstrCtrlReg = 0;
	//Is the FPGA loaded at this point?

	main_AddDciMsgMap( s_fpgamap);

	//Enable CPU interrupt
	ENABLE_INTC0
}

/**
 * Processes on going updates
 */
void  Fpga_OnUpdate( )
{
	//NOTHING TODO.
}


//*******************************************************
// DCI WCA DEFAULT Waveform Component Message Handlers.
//*******************************************************

/**
* Handler calld to process property changes on the default component 0x00.
*/
void OnDciPropWriteDefault( Dci_Context* pctxt)
{
	int i;
	int idx;
	Dci_TypedProperties* ptd	= (Dci_TypedProperties*) pctxt->pMsg;
	Dci_Property*		 pProps = (Dci_Property*) Dci_TypedProperties_GetProperties( ptd);  
	
	//Iterate properties saving and dumping the properties.
	for( i = 0; i < ptd->ctProperties; i++)
	{
		switch( pProps[i].idtype ) {
			case PT_BYTE:
				Fpga_WriteByte( pProps[i].idprop, pProps[i].value.vByte );
				break;
			case PT_UINT16:
			case PT_INT16:
				Fpga_WriteWord( pProps[i].idprop, pProps[i].value.vUint16);
				break;
			case PT_UINT32:
			case PT_INT32:
				Fpga_WriteDword( pProps[i].idprop, pProps[i].value.vUint32);
				break;
		}
	}
	pctxt->bHandled = true;
}

/**
* Handler called to read property on the default component 0x00.
*/
void OnDciPropReadDefault( Dci_Context* pctxt)
{
	byte buff[MAX_MSG_SIZE];
	int i;

	Dci_TypedPropertiesQuery* ptdq	= (Dci_TypedPropertiesQuery*) pctxt->pMsg;
	byte*					  pids = (byte*) Dci_TypedPropertiesQuery_GetPropIds( ptdq);  

	int				len		= Dci_TypedProperties_Init( buff, sizeof(buff), pctxt->idComponent, ptdq->ctProperties, NULL );
	Dci_Property*	pProps	= (Dci_Property*) Dci_TypedProperties_GetProperties( (Dci_TypedProperties*)buff);  
	
	//Iterate properties copying the current default properties.
	for( i = 0; i < ptdq->ctProperties; i++) 
	{
		Dci_Property* prp = (pProps + i);
		//Copy the property.
		*prp = *(s_PropsDefault + pids[i]);

		//Read the property value.
		switch( prp->idtype)
		{
		case PT_BYTE:
			prp->value.vByte = Fpga_ReadByte( prp->idprop);
			break;
		case PT_UINT16:
			prp->value.vUint16 = (uint16) Fpga_ReadWord( prp->idprop);
			break;
		case PT_INT16:
			prp->value.vInt16 = (int16) Fpga_ReadWord( prp->idprop);
			break;
		case PT_UINT32:
			prp->value.vUint32 = (uint32) Fpga_ReadDword( prp->idprop);
			break;
		case PT_INT32:
			prp->value.vInt32 = (int32) Fpga_ReadDword( prp->idprop);
			break;
		}
	}
	pctxt->bHandled = true;

	//Send the requested properties back.
	Dciproc_SendMessage1(  buff, len, false, pctxt);
}

//*******************************************************
// DCI BIT FILE DOWNLOAD HANDLERS
//*******************************************************

/**
* Handler processes DCI BIT File Download Message and sets up
* to write to the FPGA Directly.
*/
void OnBeginBitfileDownload( Dci_Context* pctxt)
{
	byte buff[MAX_MSG_SIZE];
	int lenMsg; 
	
	Dci_BinaryImageTransfer *pBit = (Dci_BinaryImageTransfer*) pctxt->pMsg;
	s_bitParams = *pBit;	// transfer parameters 
	s_idFrameDownload = 0;			// used to check frame-id order

	//Send a respose that we are initiating.
	lenMsg = Dci_BinaryImageTransferStatus_Init( buff, WCACOMP_FPGA, 
		s_bitParams.idTransfer, s_idFrameDownload, 0, BSE_InitiatingTransfer);
	Dciproc_SendMessage1(buff, lenMsg, true, pctxt);

	pctxt->bHandled = true;

	FPGM_SPI_PROGRAM = 0;	// restart FPGA configuration interface
}

/**
* Handler processes the DCI Bit Frame Transfer Message and writes
* a single frame of data to the FPGA.
*/
void OnWriteBitfileFrame( Dci_Context* pctxt)
{
	byte buff[MAX_MSG_SIZE];
	int lenMsg;
	int ec;
	byte idStatus = 0x0;

	Dci_BinaryImageTransferFrame* pBitf = (Dci_BinaryImageTransferFrame*) pctxt->pMsg;

	// check transfer id
	if (pBitf->idTransfer == s_bitParams.idTransfer) 
	{
		// Check frame id and set up status.
		idStatus = (pBitf->idFrame != s_idFrameDownload)
			? BSE_FrameError 
			: ( s_idFrameDownload < s_bitParams.ctFrames-1) 
				? BSE_ReadyNextFrame : BSE_TransferComplete;	

		/*---- write frame to the FPGA using SPI ----*/
		FPGM_SPI_PROGRAM = 1;		// allow configuration to start in the FPGA SPI slave interface
		
		//Check to make sure the Device is ready to receive.		
		if( FPGM_SPI_DONE && FPGM_SPI_INITB== 0)		
		{		
			main_LogMsg( LOG_ERR, WCACOMP_FLASH, "Programming failure", pctxt);
			idStatus = BSE_WriteError;
		}
		else //write the data.
		{
			ec = SPI_WriteReadData( SPI_DEVICE_FPGA, pBitf->ctBytes, Dci_BinaryImageTransferFrame_GetData(pBitf), 0, 0, FALSE );
			if( ec)
				idStatus = BSE_WriteError;
		}

		//Send the current status so process can continue.
		lenMsg = Dci_BinaryImageTransferStatus_Init( buff, WCACOMP_FLASH, 
			s_bitParams.idTransfer, s_idFrameDownload, pBitf->ctBytes, idStatus);
		Dciproc_SendMessage1(buff, lenMsg, false, pctxt);
		
		//Update frame transfer status.
		if( ec || idStatus == BSE_TransferComplete) 
		{
			s_idFrameDownload =-1; //All done.
			
			//Reset all components.
			Fpga_ResetComponent(FPGA_MSTR_ALL_RESET);

		}
		else 
			++s_idFrameDownload;
	}
	pctxt->bHandled = true;
}

//*******************************************************
// FLASH LOADER SUPPORT
//*******************************************************


/****************************************************************

   CALL:    Fpga_LoadConfigFromFlash()
                             
   INTRO:   This routine loads the FPGA configuration bit file
			into the FPGA on power up.  The file should be
			contained in the serial EPROM.

			The file is loaded via SPI and can be started as
			soon as the FPGM_SPI_PROGRAM signal is raised.  The
			FPGM_SPI_DONE and FPGM_SPI_INITB signals inform the
			CPU if the load was done correctly.

   INPUT:   flen		- length of data
			fdata		- pointer to data
   OUTPUT:  nothing
   RETURN:  True = Success; False=Falure.
****************************************************************/
bool Fpga_LoadConfigFromFlash( void )
{
	Flash_Hdr flashHdr;
	int  	ec = 0;
	int  	blink = 10;
	byte 	buff[256];
	uint32 	ctBytesRemain, addr;
	uint16  ctBytesTransfer;

	//NO Interrupts
	DISABLE_INTC0
	
	
	FPGM_SPI_PROGRAM = 1;		// allow configuration to start in the FPGA SPI slave interface

	// Get flash header from flash.
	ec = Flash_Hdr_Read(&flashHdr);	

	//If we got a good header, process the data.  
	if( !ec && Flash_Hdr_Validate(&flashHdr) )
	{
		//Initialize counters
		addr 		  = FLASH_IMAGE_OFFSET;
		ctBytesRemain = flashHdr.bitParams.sizeImg;
				
		/*---- load all frames until end of memory or FPGA is done ----*/
		while( (ctBytesRemain > 0) && !ec )
		{
			//Current Transfer size.
			ctBytesTransfer = (int)( (ctBytesRemain >= 256) ? (256):(ctBytesRemain) );
			
			// Read from flash (do a full page read)
			ec = Flash_Read( addr, 256, buff );
			
			//If there was a read problem, or FPGA is signalling its done or
			//there was a problem loading --> break out.
			if( ec || FPGM_SPI_DONE || FPGM_SPI_INITB==0)
			{
				main_LogMsgIdc( LOG_ERR, WCACOMP_FLASH, 
					ec ? "Flash Read Failure" : "Programming failure",  IDC_DEFAULT);
				break;
			}

			addr += ctBytesTransfer;  	// Update to next flash read address
			ctBytesRemain -= ctBytesTransfer;
	
			// Write buffer to FPGA
			ec = SPI_WriteReadData( SPI_DEVICE_FPGA, ctBytesTransfer, buff, 0, 0, FALSE );
			
			//Blink the CHRG_RED LED to let it know we are alive.
			if( !blink ) {
				CHRG_RED ^= 1;			
				blink = 10;
			}
			blink--;

			WATCHDOG_RESET 
		}
		CHRG_RED ^= 1;
	}
	
	//Turn interrupts back on.
	ENABLE_INTC0;
	
	//Reset all components.
	Fpga_ResetComponent(FPGA_MSTR_ALL_RESET);
	
	
	// Note: At this point (addr == (flashHdr.sizeImg+256))
	return( ec == 0x0 );
}

//**********************************
// WCA HAL Property Accessors
//**********************************
byte Fpga_GetMstrCtrlRegister( )
{
	return s_FpgaMstrCtrlReg;
}

void  Fpga_SetMstrCtrlRegister( byte mask, byte flags)
{
	s_FpgaMstrCtrlReg =  (~mask & s_FpgaMstrCtrlReg) | flags;
	Fpga_WriteByte( WCAHAL_MSTR_CTRL, s_FpgaMstrCtrlReg);
}
void  Fpga_ResetComponent(byte flags)
{
	DISABLE_INTC0
	
	s_FpgaMstrCtrlReg &= ~FPGA_MSTR_ALL_RESET;
	
	CPU_DBUS_OUTPUT;
	CPU_DBUS = WCAHAL_MSTR_CTRL;	
	CPU_ASTROBE = 1;	
	CPU_ASTROBE = 0;
	CPU_DBUS 	= s_FpgaMstrCtrlReg | flags;
	CPU_WSTROBE = 1;
	CPU_WSTROBE = 0;
	
	NOP();
	NOP();
	NOP();
	
	CPU_DBUS 	= s_FpgaMstrCtrlReg;	
	CPU_WSTROBE = 1;
	CPU_WSTROBE = 0;

	ENABLE_INTC0
}


//*******************************************************
// FPGA Register Access Routines
//*******************************************************

/****************************************************************

   CALL:    Fpga_ReadByte()
                             
   INTRO:   This routine reads a byte of data from the specified
			FPGA register address.

   INPUT:   address		- address to write
   OUTPUT:  data

****************************************************************/
byte Fpga_ReadByte( byte address )
{
	byte val;
	
	DISABLE_INTC0

	CPU_DBUS_OUTPUT;
	CPU_DBUS = address;	// Register Address;
	CPU_ASTROBE = 1;	// Latch the address
	CPU_ASTROBE = 0;
	CPU_DBUS_INPUT;

	CPU_RSTROBE = 1;	// Read Enable
	NOP();				// Nop gives read line time to settle given double-debounce latency.
	NOP();
	val = CPU_DBUS;	// Store the register value.
	CPU_RSTROBE = 0;	// Read Disable	
	
	ENABLE_INTC0	
	
	return val;
}

/****************************************************************

   CALL:    Fpga_ReadWord()
                             
   INTRO:   This routine reads a byte of data from the specified
			FPGA register address.

   INPUT:   address		- address to write
   OUTPUT:  data

****************************************************************/
uint16 Fpga_ReadWord( byte address )
{
	uint16 rval;
	byte* pval = (byte*) &rval;
	
	DISABLE_INTC0

	CPU_DBUS_OUTPUT;
	CPU_DBUS = address;	// Register Address;
	CPU_ASTROBE = 1;	// Latch the address
	CPU_ASTROBE = 0;
	CPU_DBUS_INPUT;

	CPU_RSTROBE = 1;	// Read Enable
	*pval++ = CPU_DBUS;	// Store the register value.
	CPU_RSTROBE = 0;	// Read Disable

	CPU_RSTROBE = 1;	// Read Enable
	*pval++ = CPU_DBUS;	// Store the register value.
	CPU_RSTROBE = 0;	// Read Disable

	ENABLE_INTC0	
	
	return( rval );
}


/****************************************************************

   CALL:    Fpga_ReadDword()
                             
   INTRO:   This routine reads a byte of data from the specified
			FPGA register address.

   INPUT:   address		- address to write
   OUTPUT:  data

****************************************************************/
uint32 Fpga_ReadDword( byte address )
{
	uint32 rval;
	byte* pval = (byte*) &rval;

	DISABLE_INTC0
	
	CPU_DBUS_OUTPUT;
	CPU_DBUS = address;	// Register Address;
	CPU_ASTROBE = 1;	// Latch the address
	CPU_ASTROBE = 0;
	CPU_DBUS_INPUT;

	CPU_RSTROBE = 1;	// Read Enable
	*pval++ = CPU_DBUS;	// Store the register value.
	CPU_RSTROBE = 0;	// Read Disable

	CPU_RSTROBE = 1;	// Read Enable
	*pval++ = CPU_DBUS;	// Store the register value.
	CPU_RSTROBE = 0;	// Read Disable

	CPU_RSTROBE = 1;	// Read Enable
	*pval++ = CPU_DBUS;	// Store the register value.
	CPU_RSTROBE = 0;	// Read Disable

	CPU_RSTROBE = 1;	// Read Enable
	*pval   = CPU_DBUS;	// Store the register value.
	CPU_RSTROBE = 0;	// Read Disable

	ENABLE_INTC0
	
	return( rval );
}



/****************************************************************

   CALL:    Fpga_WriteByte()
                             
   INTRO:   This routine writes a byte of data to the specified
			FPGA register address.

   INPUT:   address		- address to write
			rval		- data to write
   OUTPUT:  nothing

****************************************************************/
void Fpga_WriteByte( byte address, byte rval )
{
	DISABLE_INTC0

	CPU_DBUS_OUTPUT;
	CPU_DBUS = address;	// Register Address;
	CPU_ASTROBE = 1;	// Latch the address
	CPU_ASTROBE = 0;
	CPU_DBUS = rval;	// Register value;
	CPU_WSTROBE = 1;
	CPU_WSTROBE = 0;

	ENABLE_INTC0
}

/****************************************************************

   CALL:    Fpga_WriteWord()
                             
   INTRO:   This routine writes a word (uint16) of data to the specified
			FPGA register address.

   INPUT:   address		- address to write
			rval		- data to write
   OUTPUT:  nothing

****************************************************************/
void Fpga_WriteWord( byte address, uint16 rval )
{
	DISABLE_INTC0
	
	//Write Little Endian.
	CPU_DBUS_OUTPUT;
	CPU_DBUS = address;	// Register Address;
	CPU_ASTROBE = 1;	// Latch the address
	CPU_ASTROBE = 0;
	CPU_DBUS = (byte)(rval&0xff); // Register value;
	CPU_WSTROBE = 1;
	CPU_WSTROBE = 0;
	CPU_DBUS = (byte)(rval>>8);	  // Register value;
	CPU_WSTROBE = 1;
	CPU_WSTROBE = 0;

	ENABLE_INTC0
}

/****************************************************************

   CALL:    Fpga_WriteDword()
                             
   INTRO:   This routine writes a word (uint16) of data to the specified
			FPGA register address.

   INPUT:   address		- address to write
			rval		- data to write
   OUTPUT:  nothing

****************************************************************/
void Fpga_WriteDword( byte address, uint32 rval )
{
	byte* pval = (byte*) &rval;

	DISABLE_INTC0

	//Write Little Endian.
	CPU_DBUS_OUTPUT;
	CPU_DBUS = address;	// Register Address;
	CPU_ASTROBE = 1;	// Latch the address
	CPU_ASTROBE = 0;
	CPU_DBUS = (byte)*pval++; // Register value;
	CPU_WSTROBE = 1;
	CPU_WSTROBE = 0;
	CPU_DBUS = (byte)*pval++; // Register value;
	CPU_WSTROBE = 1;
	CPU_WSTROBE = 0;
	CPU_DBUS = (byte)*pval++; // Register value;
	CPU_WSTROBE = 1;
	CPU_WSTROBE = 0;
	CPU_DBUS = (byte)*pval;   // Register value;
	CPU_WSTROBE = 1;
	CPU_WSTROBE = 0;

	ENABLE_INTC0	
}

/****************************************************************

   CALL:    Fpga_WriteFifo()
                             
   INTRO:   Writes to a 4 byte wide FIFO on the FPGA.

   INPUT:   address		- address to write
			pBuff		- data to write
			len			- length of buffer (multiples of 4 bytes)
   OUTPUT:  nothing

****************************************************************/
void Fpga_WriteFifo( byte address, byte* pBuff, int len)
{
	byte *p, *pend;
	p		= pBuff;
	pend	= p + len;

	DISABLE_INTC0
	
	//Write Address.
	CPU_DBUS_OUTPUT;
	CPU_DBUS = address;	// Register Address;
	CPU_ASTROBE = 1;	// Latch the address
	CPU_ASTROBE = 0;

	//Write data.
	while( p < pend)
	{
		CPU_DBUS = *p++;   
		CPU_WSTROBE = 1;
		CPU_WSTROBE = 0;

		CPU_DBUS = *p++;   
		CPU_WSTROBE = 1;
		CPU_WSTROBE = 0;

		CPU_DBUS = *p++;   
		CPU_WSTROBE = 1;
		CPU_WSTROBE = 0;

		CPU_DBUS = *p++; 
		CPU_WSTROBE = 1;
		CPU_WSTROBE = 0;
	}

	ENABLE_INTC0
}

/****************************************************************

   CALL:    Fpga_ReadFifo()
                             
   INTRO:   Reads from a 4 byte wide FIFO on the FPGA

   INPUT:   address		- address to read
			pBuff		- buffer to store read data.
			len			- number of bytes to read (multiples of 4 bytes)
   OUTPUT:  nothing

****************************************************************/
void Fpga_ReadFifo( byte address, byte* pBuff, int len)
{
	byte *p, *pend;
	p		= pBuff;
	pend	= p + len;
	
	DISABLE_INTC0
	
	//Write Address.
	CPU_DBUS_OUTPUT;
	CPU_DBUS = address;	// Register Address;
	CPU_ASTROBE = 1;	// Latch the address
	CPU_ASTROBE = 0;
	CPU_DBUS_INPUT;

	//Read the data.
	while(p < pend)
	{
		CPU_RSTROBE = 1;	// Read Enable
		*p++ = CPU_DBUS;	// Store the register value.
		CPU_RSTROBE = 0;	// Read Disable

		CPU_RSTROBE = 1;	// Read Enable
		*p++ = CPU_DBUS;	// Store the register value.
		CPU_RSTROBE = 0;	// Read Disable

		CPU_RSTROBE = 1;	// Read Enable
		*p++ = CPU_DBUS;	// Store the register value.
		CPU_RSTROBE = 0;	// Read Disable

		CPU_RSTROBE = 1;	// Read Enable
		*p++ = CPU_DBUS;	// Store the register value.
		CPU_RSTROBE = 0;	// Read Disable
	}
	
	ENABLE_INTC0
	
}

//*******************************************************
// FPGA EVENT Routines
//*******************************************************

/**
* Sets the event mask on the FPGA, user provides 
* both a mask to address only the events of interest
* and the actual events to enable or disable.
*
* params:
*  mask   = specifies the valid events bits to manipulate.
*  events = specifies the bits to enable and disable.
*
*
* NOTE: When an FPGA occurrs, it is disabled in the FPGA
* 		hardware until status is cleared and renabled
*		by the particular handler. Typically at the end 
*		of event processing, it is renabled by calling
*  	
*		Fpga_ClearEventStatus( evtFlags, true); //Renables the events.
*	 
*/
void Fpga_SetEventMask( byte mask, byte events)
{
	//Calculate which events are enabled and disabled.	
	s_FpgaEventMask = (~mask & s_FpgaEventMask) | (mask & events) ;
	Fpga_WriteByte( WCAHAL_EVENT, s_FpgaEventMask);
}

/**
* Returns the current event mask.
*/
byte Fpga_GetEventMask()
{
	return s_FpgaEventMask;
}

/**
* Returns true of the event has occurred.
*/
bool  Fpga_IsEventStatus( byte eventflags)
{
	return (s_FpgaEventStatus & eventflags ) > 0;
}

byte  Fpga_GetEventStatus()
{
	return s_FpgaEventStatus;
}

/**
* Clears the specified event status, and renables the event 
* if specified.
*/
void  Fpga_ClearEventStatus( byte eventflags, bool bEnable)
{
		s_FpgaEventStatus &= ~eventflags;

		if( bEnable)
		{
			s_FpgaEventMask |= (eventflags) ;
			Fpga_WriteByte( WCAHAL_EVENT, s_FpgaEventMask);
		}

}



/****************************************************************

   CALL:    ISR_WCAHAL_EVENTS()
                             
   INTRO:   This routine is the interrupt service routine for
            the FPGA WCAHAL_EVENT register.   It processes received interrupts and sets 
			the appropriate signals for routine processing.

   INPUT:   nothing
   OUTPUT:  nothing

****************************************************************/
__interrupt static void ISR_WCAHAL_EVENTS(void)
{
	byte status;

	EI();  // allow higher priority interrupts to occur

	//Set the Address the WCAHAL_EVENT
	CPU_DBUS_OUTPUT;
	CPU_DBUS = WCAHAL_EVENT;// Register Address;
	CPU_ASTROBE = 1;	// Latch the address
	CPU_ASTROBE = 0;

	//Read the FPGA Event Status Register to determine what happened.
	CPU_DBUS_INPUT;
	CPU_RSTROBE = 1;	// Read Enable
	NOP();				// Nop gives read line time to settle given double-debounce latency.
	NOP();
	status = CPU_DBUS;	// Store the register value.
	CPU_RSTROBE = 0;	// Read Disable	
	
	//Save the event that happened.
	s_FpgaEventStatus |= status;

	//Turn off interrupts while status is set.
	s_FpgaEventMask &= ~status;
	
	//Write out the the WCAHAL_EVENT Register to 
	//Reset the interrupt.  Don't need to specify 
	//address since we did it in ReadByteRegister
	CPU_DBUS_OUTPUT;
	CPU_DBUS 	= s_FpgaEventMask; // Clear the specific event
	CPU_WSTROBE = 1;
	CPU_WSTROBE = 0;

	//Do not re-enable interrupts here, since we did not handle it yet.
}

