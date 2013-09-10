// Name:  RFConfig.c
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


/*---- Custom include files ----*/
#include "r_cg_macrodriver.h"
#include "r_cg_wdt.h"
#include "r_cg_userdefine.h"

#include "Main.h"
#include "Timers.h"
#include "Fpga.h"
#include <Drivers/FpgaWcaHalDefs.h>
#include "RFConfig.h"

/*---- Standard C include files ----*/

/*---- static function declarations ----*/

//Macro makes it easy to implement switches.
#define ApplyRfConfigFlag( bEnable, flag) RF_SetConfiguration((s_RfConfigFlags& (~flag)) | ((bEnable) ? flag : 0) )

/*---- static variable declarations ----*/
byte s_rf0AmpFlags;
byte s_rf1AmpFlags;
byte		 s_RfConfigFlags;
byte		 s_Rf0SamplingFlags;
byte		 s_Rf1SamplingFlags;

//***********************************************************
// WCACOMP_RF0 or WCACOMP_RF1 Amplifier settings Implementaiton
//***********************************************************

/**
* Routine configures WCACOMP_RF0 and WCACOMP_RF1 related board amplifiers.
*/
void RF_SetAmplifierConfig( int device, byte flags)
{
	if( device == 0x00)
	{
		s_rf0AmpFlags = flags;
		GPS_LNA_ENABLE = (flags & RF0_IN1_GPSLNA_ENABLE)	? 1 : 0;
		PCS_LNA_BYPASS  = (flags & RF0_IN2_PCSBYPASS_ENABLE) ? 1 : 0;
		WB1_RX_ENABLE  = (flags & RF0_IN3_WBAMP_ENABLE)		? 1 : 0;

		WB1_TX_ENABLE  = (flags & RF0_OUT1_WBAMP_ENABLE)	? 1 : 0;
	}
	else
	{
		s_rf1AmpFlags = flags;
		LOW_RX_ENABLE  = (flags & RF1_IN1_UHFAMP_ENABLE)	? 1 : 0;
		ISM_LNA_BYPASS = (flags & RF1_IN2_ISMBYPASS_ENABLE) ? 1 : 0;
		WB2_RX_ENABLE  = (flags & RF1_IN3_WBAMP_ENABLE)		? 1 : 0;
		
		ISM_TX_ENABLE  = (flags & RF1_OUT1_ISMAMP_ENABLE)	? 1 : 0;
		WB2_TX_ENABLE  = (flags & RF1_OUT2_WBAMP_ENABLE)	? 1 : 0;
	}
}


/** 
* Routine gets the WCACOMP_RF0 and WCACOMP_RF1 amplifier configuration.
*/
byte RF_GetAmplifierConfig(int device)
{
	return device ? s_rf0AmpFlags : s_rf1AmpFlags  ;
}

//***********************************************************
// RF Sampling Control.
//***********************************************************

/** 
* Function sets the sampling control register controlling sampling functions 
* for WCACOMP_RF0 (device =0) or WCACOMP_RF1 (device = 1).
*/
void RF_SetSamplingControl( int device, byte flags)
{
	if( device == 0)
	{
		s_Rf0SamplingFlags = flags;
		Fpga_WriteByte( WCAHAL_RF0_CTRL, flags);
	}
	else
	{
		s_Rf1SamplingFlags = flags;
		Fpga_WriteByte( WCAHAL_RF1_CTRL, flags);
	}
}

/** 
* Function gets the sampling control register controlling sampling functions 
* for WCACOMP_RF0 (device =0) or WCACOMP_RF1 (device = 1).
*/
byte RF_GetSamplingControl(int device  )
{
	return (device == 0) ? s_Rf0SamplingFlags : s_Rf1SamplingFlags;
}


//***********************************************************
// RF External / Internal Configuration Implementation.
//***********************************************************

/** 
* Sets the RF external / internal configuration.
*/
void RF_SetConfiguration( byte flags )
{
	s_RfConfigFlags = flags;
	Fpga_WriteByte( WCAHAL_RFCONFIG, flags);
}

/**
* Gets the RF external / internal configuration flags.
*/
byte RF_GetConfiguration() 
{ 
	return s_RfConfigFlags;
}

// Following routines set the flags indiviually.
void RF_SelectGpsBias( bool bEnable)			{ ApplyRfConfigFlag( bEnable, RF_GpsBiasSelect); }
void RF_SelectGpsExternalInput( bool bEnable)	{ ApplyRfConfigFlag( bEnable, RF_GpsExternalInput); }
void RF_SelectIsmExternalInput( bool bEnable)	{ ApplyRfConfigFlag( bEnable, RF_IsmExternalInput); }
void RF_SelectIsmTransmit( bool bEnable)		{ ApplyRfConfigFlag( bEnable, RF_IsmTransmitSelect); }
void RF0_SelectWidebandReceive( bool bEnable)	{ ApplyRfConfigFlag( bEnable, RF0_WidebandReceiveSelect); }
void RF1_SelectWidebandReceive( bool bEnable)	{ ApplyRfConfigFlag( bEnable, RF1_WidebandReceiveSelect); }


void RF_SetPower( bEnable)
{ 
	if( RF_ONOFF==0 && bEnable) 
	{
		RF_ONOFF = 1;
		Timer_Delay( 250 );		// wait for RF to power up
	} 
	else if( !bEnable)
	{
		RF_ONOFF = 0;
	}
}


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
uint16 RF_GetReceivedSignalStrength( int device)
{
	if( device == 0)
	{
		return Fpga_ReadWord( WCAHAL_RF0_RSSI);
	}
	else
	{
		return Fpga_ReadWord( WCAHAL_RF1_RSSI);
	}
}

/**
* Returns the ADC bias offsets tracked by the FPGA (registers WCAHAL_RF0_BIAS and WCAHAL_RF1_BIAS).
*
* In phase and quadrature ADC Bias Read register. Implements 24 bit integrator to compute the average bias value over 
* 4096 samples returns the high 16 bits indicating the average bias value over the period. 
* Further LPF in the microcontroller may be required to get longer term average. Returns both in phase 
* and quadrature values { bias_i, bias_q } as a single 32 bit value.
*/
uint32 RF_GetReceivedAdcBiasOffset( int device)
{
	if( device == 0)
	{
		return Fpga_ReadDword( WCAHAL_RF0_BIAS);
	}
	else
	{
		return Fpga_ReadDword( WCAHAL_RF0_BIAS);
	}
}

