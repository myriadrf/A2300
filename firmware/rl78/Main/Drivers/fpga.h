// Name:  fpga.h
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


#ifndef _Fpga_H_
#define _Fpga_H_

#include <System/DataTypes.h>

#define FPGACONFIG_LOADING		1
#define FPGACONFIG_CRCERROR		2
#define FPGACONFIG_NOCRCERROR	3


//Master Control Register Flags.
enum Fpga_MasterCtrlFlags
{
	FPGA_MSTR_RF0_RESET 		=	0x0,
	FPGA_MSTR_RF1_RESET 		=	0x2,
	FPGA_MSTR_HALCONTAINR_RESET =	0x4,
	FPGA_MSTR_ALL_RESET			=  	0x7
};

//**********************************
// Common module interface.
//**********************************
/**
 * Initialization
 */
void  Fpga_Init( );
void  Fpga_OnUpdate( );

//**********************************
// WCA HAL Property Accessors
//**********************************
byte  Fpga_GetMstrCtrlRegister( );
void  Fpga_SetMstrCtrlRegister( byte mask, byte flags);
void  Fpga_ResetComponent(byte flags);


//**********************************
// WCA Property Accessors
//**********************************
//These functions get/set the WCA properties specified 
//ASR-2300 Communications Interface Spec.  They provide the means
//for the microcontroller code to access them as well.
uint16 Fpga_WaveformVersion();
uint16 Fpga_PortCaps();
void   Fpga_SetRfConfig( byte config );
void   Fpga_SetLedStatus( byte status);

//**********************************
//FPGA Programming Routines.
//**********************************
bool	Fpga_LoadConfigFromFlash( void );

//**********************************
// FPGA Register / FIFO Access Routines.
//**********************************
byte	Fpga_ReadByte( byte address );
uint16	Fpga_ReadWord( byte address );
uint32	Fpga_ReadDword( byte address );

void	Fpga_WriteByte( byte address, byte rval );
void	Fpga_WriteWord( byte address, uint16 rval );
void	Fpga_WriteDword( byte address, uint32 rval );

void	Fpga_WriteFifo( byte address, byte* pBuff, int len);
void	Fpga_ReadFifo( byte address, byte* pBuff, int len);


//**********************************
// FPGA Events 
//**********************************

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
void Fpga_SetEventMask( byte mask, byte events);

/**
* Returns the current event mask.
*/
byte Fpga_GetEventMask( );



/**
* Returns true of the event has occurred.
*/
bool  Fpga_IsEventStatus( byte eventflags);
byte  Fpga_GetEventStatus();

/**
* Clears the specified event status.
*/
void  Fpga_ClearEventStatus( byte eventflags, bool bEnable);





#endif