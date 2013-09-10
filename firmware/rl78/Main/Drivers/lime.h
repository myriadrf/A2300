// Name:  lime.h
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

#ifndef _Lime_H_
#define _Lime_H_

#include "System/DataTypes.h"
#include "RfProfiles.h"

#define	MAXDATABYTES	120	// support setting up to 60 registers

/**
 * Initialization
 */
void Lime_Init( );
void Lime_OnUpdate( );

/*==== This is the calibration structure stored in non-volatile memory ====*/
typedef struct
{
   byte  tune_value;		// current capacitor tuning value
   byte  tune_low;		// lowest working value
   byte  tune_high;		// highest working value
} TUNE_STRUCT;

/*---- global variables	defined in 'LimeControl.c' ----*/
extern TUNE_STRUCT	vcocap;

//********************************************
// Mid-level Functions for configuration.
//********************************************

/**
* These flags provide direct control of RF0 Lime path selection. This is really for initial testing
* The preferred approach for final implementation is to use the path variables, which apply
* a whole profile.
*/
enum RF0_PathConfig_Flags
{
	RF0_INPUT_MASK				= 0x03, 
	RF0_RXIN1_GPS				= 0x00, // RXIN1 – L1 GPS (1575.42 MHz)
	RF0_RXIN2_PCS				= 0x01, // RXIN2 – PCS (1930 MHz to 1990 MHz)
	RF0_RXIN3_WIDEBAND			= 0x02, // RXIN3 – Wideband (400 MHz to 3.8 GHz)
	
	RF0_OUTPUT_MASK				= 0x4,  
	RF0_TXOUT1_WIDEBAND			= 0x0,	// Wideband (400 MHz to 3.8 GHz)

	RF0_LOOPBACK_ENABLE			= 0x08, // Enables (1) transmit RF loopback for calibration 
										// and testing of transceiver path. This should be 
										// disabled (0) for ordinary operation
};	

/**
* These flags provide direct control of RF0 Lime path selection. This is really for initial testing
* The preferred approach for final implementation is to use the path variables, which apply
* a whole profile.
*/
enum RF1_PathConfig_Flags
{
	RF1_INPUT_MASK				= 0x03, 
	RF1_RXIN1_UHF				= 0x00, // RXIN1 – UHF (300 MHz to 650 MHz)
	RF1_RXIN2_ISM				= 0x01, // RXIN2 – ISM (2400 MHz to 2480 MHz)
	RF1_RXIN3_WIDEBAND			= 0x02, // RXIN3 – Wideband (400 MHz to 3.8 GHz)
	
	RF1_OUTPUT_MASK				= 0x4,   
	RF1_TXOUT1_ISM				= 0x0,	// ISM (2400 MHz to 2480 MHz)
	RF1_TXOUT2_WIDEBAND			= 0x1,	// Wideband (400 MHz to 3.8 GHz)

	RF1_LOOPBACK_ENABLE			= 0x08 // Enables (1) transmit RF loopback for calibration 
										// and testing of transceiver path. This should be 
										// disabled (0) for ordinary operation
};	


void	Lime_SetRxFrequency( int device, long freq );
void    Lime_SetRxNco( int device, uint32 val);
void	Lime_SetRxGain( int device, byte gain );
void	Lime_SetRxBandwidth( int device, byte bandwidth );

void	Lime_SetTxFrequency( int device, long freq );
void    Lime_SetTxNco( int device, uint32 val);
void	Lime_SetTxGain( int device, byte gain );
void	Lime_SetTxBandwidth( int device, byte bandwidth );


//********************************************
// Low-level Functions for configuration and direct control.
//********************************************

void	Lime_TuneRxCap( int device );
void	Lime_TuneTxCap( int device );
void	Lime_CalBlock( int device, byte block_addr, byte channel );
void 	Lime_RemoveRxDCBias( int device );
void 	Lime_RemoveTxDCBias( int device );
void	Lime_AnalogOutput( int device, byte onoff );
byte	Lime_Write( int device, int wlen, byte* pData );
byte	Lime_Read( int device, int addr, byte* pData );


#endif /* _Lime_H_ */

