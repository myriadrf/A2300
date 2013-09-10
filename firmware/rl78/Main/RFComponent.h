// Name:  RFComponent.h
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

#ifndef _RfComponent_H_
#define _RfComponent_H_

/**
 * Initialization
 */
void RfComp_Init( );

/**
 * Processes on going updates
 */
void RfComp_OnUpdate( );

//******************************
// Path Selection 
//******************************

/**
* Enum specifies the standard paths (hardcoded)
* built into the ASR2300. Additional Paths may 
* be specified using user defined non-volatile 
* memory.
*/
enum RF_StandardPath_Enum
{
	TX0_DISABLED			= 0x0, // TX DISABLED 
	TX0_WIDEBANDEXTERNAL	= 0x1, // Wideband (400 MHz to 3.8 GHz) Output

	RX0_DISABLED			= 0x40, // RX DISABLED
	RX0_L1GPSINTERNAL		= 0x41, // L1 GPS (1575.42 MHz, 3 MHz BW) Internal Antenna
	RX0_L1GPSEXTERNAL		= 0x42, // L1 GPS (1575.42 MHz) External Input
	RX0_PCSEXTERNAL			= 0x43, // PCS (1930 MHz to 1990 MHz) Input
	RX0_WIDEBANDEXTERNAL	= 0x44, // Wideband (400 MHz to 3.8 GHz) Input

	TX1_DISABLED			= 0x80, // TX DISABLED
	TX1_ISMINTERNAL			= 0x81, // ISM (2.4 GHz to 2.48 GHz) Antenna Output
	TX1_ISMEXTERNAL			= 0x82, // ISM (2.4 GHz to 2.48 GHz) External Output
	TX1_WIDEBANDEXTERNAL	= 0x83, // Wideband (400 MHz to 3.8 GHz) Output

	RX1_DISABLED			= 0xC0, // RX DISABLED
	RX1_UHFEXTERNAL			= 0xC1, // UHF (300 MHz to 650 MHz) Input
	RX1_ISMINTERNAL			= 0xC2, // ISM (2.4 GHz to 2.48 GHz) Antenna Input
	RX1_ISMEXTERNAL			= 0xC3, // ISM (2.4 GHz to 2.48 GHz) External Input
	RX1_WIDEBANDEXTERNAL	= 0xC4  // Wideband (400 MHz to 3.8 GHz) Input

};

/**
* Function applies the specified path settings.
*/
int RfComp_SetPath( byte pathflags, Dci_Context* pctxt);



#endif /* _RfComponent_H_ */

