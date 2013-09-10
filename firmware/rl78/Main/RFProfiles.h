// Name:  RFProfiles.h
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

#ifndef	_RFDEFS_
#define	_RFDEFS_

#include "System/DataTypes.h"

#if 0 //Deprecated
#define  NUMOFRFPATHS 	7
extern const LMINIT_STRUCT	LMInit[];
#endif 

/**
* Structure holds a single register address, mask, and data.  
* the process is to first read the lime register, apply the mask and updated values
* and then write it back.  
*
* writeval = (regval & ~mask) | val;
*/
typedef struct RF_LimeRegData_tag
{
	 byte addr; // Register address
	 byte val;	// Value to apply
	 byte mask; // Mask to apply   
} RF_LimeRegData;
 
/**
* Structure defines single byte register mask and data values.
* This structure is used to specify various configuration and control 
* registers relating to RF configuration.
*
*  writeval = (regval & ~mask) | val;
*/
typedef struct RF_RegData_tag
{
	 byte val;
	 byte mask;
} RF_RegData;
 
/**
* Structure defines a TX or RX Path Definition, which comprises Lime register settings
* sampling control, amplifier config, and RF switches configurations.  In addition default
* values for gain, bandwidth, frequency, and description are provided to enhance utility
*/
typedef struct RF_PathDef_tag
{
	// Path Definition identifer
	//  bits	| description
	// ---------------------------------
	//  0-3		| Definition id -- up to 16 per TX/RX channel
	//  4		| (0) Standard Definition,  (1) User Defined Definition
	//  5		| Reserved
	//  6-7		| TX / RX Channel ID.
	//			| 0 = RF0 TX
	//			| 1 = RF0 RX 
	//			| 2 = RF1 TX
	//			| 3 = RF2 RX 
	byte id;    

	// 17 character description of the path definition.
	char  Descr[17];

	// Index into s_dataLimeReg for start of Lime Register Data.
	uint16  idxRegData;

	// Count of register data entries for this definition in LimeReg
	uint16  ctEntries;

	// Value and Mask settings to configure the sampling control within the FPGA  
	RF_RegData  SamplingControl;

	// Value and mask settings to configure the RF amplifier configurations 
	RF_RegData  AmplifierConfig;

	// Value and mask settings to configure RF control register in the FPGA
	RF_RegData  RfSwitchesConfig;

	// Initial Gain Settings (Lime format)
	byte    Gain;

	//Initial Bandwidth  (Lime format)
	byte    Bandwidth;

	//Initial Frequency  (KHz)
	uint32  Frequency; 
} RF_PathDef;

//*********************************
//RF Path Profile functions
//*********************************

/**
* Function returns the specified path Definition given the identifier.
*/
const RF_PathDef* RF_GetPathProfile( byte id);

/**
* Function Copies the array of available PIds available. Returns
* the number of PIDs copied.
*/
int  RF_EnumerateProfiles( byte *pIds, int maxlen);

/**
* Returns a pointer to the specified lime register data given
* the RF_PathDef id and the 
*/
const RF_LimeRegData* RF_GetLimeRegData( const RF_PathDef* pdef);

//TODO Implement Non-Volatile Memory Write functions for User defined 
//Profiles.

#endif