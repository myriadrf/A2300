// Name:  RFConfig.h
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

#ifndef	_RFCONFIG_
#define	_RFCONFIG_

#include <System/DataTypes.h>

//******************************
// RF Amplifier Configuration
//******************************

/**
* Flags are used to giver amplifier settings for WCACOMP_RF0. Or them together to form
* the desired configuration.
*/
enum RF0_AmpFlags
{
	RF0_IN1_GPSLNA_ENABLE		= 0x1,	// Enable / Disable GPS LNA
	RF0_IN2_PCSBYPASS_ENABLE	= 0x2,	// Enable / Disable PCS BYPASS
	RF0_IN3_WBAMP_ENABLE		= 0x4,	// Enable / Disable Wideband RX Amplifier
	RF0_OUT1_WBAMP_ENABLE		= 0x8	// Enable / Disable Wideband TX Amplifier.
};

/**
* Flags are used to giver amplifier settings for WCACOMP_RF1. Or them together to form
* the desired configuration.
*/
enum RF1_AmpFlags
{
	RF1_IN1_UHFAMP_ENABLE			= 0x01,	// Enable / Disable UHF Amplifier.
	RF1_IN2_ISMBYPASS_ENABLE		= 0x02, // Enable / Disable Bypass ISM RX LNA for Strong signals.
	RF1_IN3_WBAMP_ENABLE			= 0x04,	// Enable / Disable Wideband RX Amplifier.
	RF1_OUT1_ISMAMP_ENABLE			= 0x08, // Enable / Disable ISM TX Amplifier
	RF1_OUT2_WBAMP_ENABLE			= 0x10	// Enable / Diable  Wideband TX Amplifier. 
};

/**
* Routine configures WCACOMP_RF0 and WCACOMP_RF1 related board amplifiers.
*/
void RF_SetAmplifierConfig( int device, byte flags);

/** 
* Routine gets the WCACOMP_RF0 and WCACOMP_RF1 amplifier configuration.
*/
byte RF_GetAmplifierConfig(int device);


//******************************
// RF Sampling / Function Control
//******************************

/**
* Flags specifiy available sampling configuration and Lime function
* control.
*/
enum RF_SamplingCtrlFlags
{
	RX_INPUTMODE_MASK				= 0x03, 
	RX_INPUTMODE_LOOPBACK			= 0x00, // Put the TX data into the RX Path 
	RX_INPUTMODE_RXDATA				= 0x01, // Capture Lime Micro receive data
	RX_INPUTMODE_IQ0				= 0x02, // Set to fixed test pattern: i = 0, q = 0
	RX_INPUTMODE_IQ256				= 0x03, // Set to fixed test pattern: i = 256, q = 256

	TX_OUTPUTMODE_MASK				= 0x0C, 
	TX_OUTPUTMODE_LOOPBACK			= 0x00, // Put the RX data into the TX Path
	TX_OUTPUTMODE_TXDATA			= 0x04, // Send transmit data to Lime Micro
	TX_OUTPUTMODE_IQ0				= 0x08, // Set to fixed test pattern: i = 0, q = 0
	TX_OUTPUTMODE_IQ256				= 0x0C, // Set to fixed test pattern: i = 256, q = 256

	RX_ENABLE						= 0x10, // output line - set to 1 to enable, 0 to disable. 
											// Enables disables the Lime chip receive function.

	TX_ENABLE						= 0x20, // output pin state - set to 1 to enable, 0 to disable  
											// Enables / disables the Lime chip transmit function.

	RX_CLK_ENABLE					= 0x40, // Enable (1) / (0).  If enabled the rf_rxclk pin will 
											// turn on and provide clock signals to the receive 
											// functions of the Lime chips.  If disabled, clock output will be low.
	
	TX_CLK_ENABLE					= 0x80  // Enable (1) / (0).  If enabled the tx_rxclk pin will 
											// turn on and provide clock signals to the transmit functions 
											// of Lime chip #1.  If disabled, clock output will be low.
};	

/** 
* Function sets the sampling control register controlling sampling functions 
* for WCACOMP_RF0 (device =0) or WCACOMP_RF1 (device = 1).
*/
void RF_SetSamplingControl( int device, byte flags);

/** 
* Function gets the sampling control register controlling sampling functions 
* for WCACOMP_RF0 (device =0) or WCACOMP_RF1 (device = 1).
*/
byte RF_GetSamplingControl(int device  );



//******************************
// RF External Input Configuration
//******************************

/**
* When available, allows direct configuration of extnernal RF Interfaces
* via FPGA.  Note the availability of these functions is dependent upon
* the logic load in the FPGA.  May not be supported in all cases.
*/
enum RF_ConfigurationFlags
{
	RF_GpsBiasSelect				= 0x01,	// Turn on GPS 3.3V DC bias selection (External only)		
	RF_GpsExternalInput				= 0x02, // Select External (1) / Internal (0) GPS Antenna.
	RF_IsmExternalInput				= 0x04, // Select External (1) / Internal (0) ISM Antenna.
	RF_IsmTransmitSelect			= 0x08, // Select Transmit (1) / Receive  (0) ISM Band Path.
	RF0_WidebandReceiveSelect		= 0x10, // Select Receive (1) / Transmit (0) WCACOMP_RF1 Wideband 
	RF1_WidebandReceiveSelect		= 0x20	// Select Receive (1) / Transmit (0) RF2 Wideband 
};

/** 
* Sets the RF external / internal configuration.
*/
void RF_SetConfiguration( byte flags );

/**
* Gets the RF external / internal configuration flags.
*/
byte RF_GetConfiguration();

// Following routines set the flags indiviually.
void RF_SelectGpsBias( bool bEnable);
void RF_SelectGpsExternalInput( bool bEnable);
void RF_SelectIsmExternalInput( bool bEnable);
void RF_SelectIsmTransmit( bool bEnable);
void RF0_SelectWidebandReceive( bool bEnable);
void RF1_SelectWidebandReceive( bool bEnable);

void RF_SetPower( bEnable);

//******************************
// Baseband Sampled Data Signal Registers
//******************************

/**
* Returns the received signal strength indication
* tracked by the FPGA lime interface (registers WCAHAL_RF0_RSSI and WCAHAL_RF1_RSSI).
*
* I and Q RSSI values truncated to the 8 high order bits.  RSSI is the average absolute value 
* of the received data over the sampled interval.   24 bit counter is used so sampling interval should be
* around (~0.3 msec).  Register contains both the I and Q RSSI values as follows:
* bit#  |  Description
*-------|----------------------------------------------------------
*  0-7  |   In phase RSSI values (0 - 256) low 4 bits are not returned since
*       |   it assumed gain will always be turned up enough to hit at least 4 bits of the
*       |   the system.
* 8-15  |   Quadrature phase RSSI values 0 - 256) low 4 bits are not returned since
*       |   it assumed gain will always be turned up enough to hit at least 4 bits of the
*       |   the system.
*/
uint16 RF_GetReceivedSignalStrength( int device);

/**
* Returns the ADC bias offsets tracked by the FPGA (registers WCAHAL_RF0_BIAS and WCAHAL_RF1_BIAS).
*
* In phase and quadrature ADC Bias Read register. Implements 24 bit integrator to compute the average bias value over 
* 4096 samples returns the high 16 bits indicating the average bias value over the period. 
* Further LPF in the microcontroller may be required to get longer term average. Returns both in phase 
* and quadrature values { bias_i, bias_q } as a single 32 bit value.
*/
uint32 RF_GetReceivedAdcBiasOffset( int device);
#endif

