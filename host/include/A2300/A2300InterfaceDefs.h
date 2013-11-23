// Name: A2300InterfaceDefs.h
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#ifndef A2300InterfaceDefs_H_
#define A2300InterfaceDefs_H_
//*******************************************************
// WCA COMPONENT Idetifiers
//*******************************************************

#define WCACOMP_HALDEFAULT 	0x00	//Default WCA FPGA HAL Component
#define WCACOMP_MICRO	 	0x80	//Microcontroller General Configuration
#define WCACOMP_RF0			0x81	//Lime #1 transceiver interface
#define WCACOMP_RF1			0x82	//Lime #2 transceiver interface
#define WCACOMP_MOTION		0x83	//Aggregate Motion Sensor interface
#define WCACOMP_ACCEL		0x84	//Accelerometer Sensor
#define WCACOMP_GYRO		0x85	//Gyroscope Sensor
#define WCACOMP_COMPASS		0x86	//Compass Sensor
#define WCACOMP_PRESSURE	0x87	//Pressure Sensor
#define WCACOMP_FLASH		0x88	//Flash Memory for FPGA and other Binary Data
#define WCACOMP_BATTERY		0x89	//Battery Charger Controller
#define WCACOMP_FX3A		0x8A	//FX-3 Component A (reserved)
#define WCACOMP_FX3B		0x8B	//FX-3 Component B (reserved)
#define WCACOMP_LSDP0		0x90	//Low speed data port 0 (input, Transmit)
#define WCACOMP_LSDP1		0x91	//Low speed data port 1 (output, Receive)
#define WCACOMP_LSDP2		0x92	//Low speed data port 2 (input, Transmit)
#define WCACOMP_LSDP3		0x93	//Low speed data port 3 (output, Receive)
#define WCACOMP_RFPROFILES  0x94    //RF Profiles component.
#define WCACOMP_FPGA		0xB0	//FPGA WCA HAL configurtion and control


//*******************************************************
// WCA WCACOMP_MICRO Component(0x80) Interface Spec.
//*******************************************************
#define MICRO_UpdateFirmware	0x00 //Action updates firmware from flash memory
#define MICRO_ResetRF0If		0x10 //Action issues reset on RF0 FPGA interface.
#define MICRO_ResetRF1If		0x11 //Action issues Reset on RF1 FPGA interfcae.
#define MICRO_ResetContainer	0x12 //Action issues reset on FPGA WCA container.
#define MICRO_ResetUsbIf		0x13 //Action issues reset on FPGA USB Interface.
#define MICRO_ResetMask			0x03 //Mask of actions to generate reset register bits.

//*******************************************************
// WCA FLASH Component(0x88) Interface Spec.
//*******************************************************

//FLASH Supports WCA Binary Image Transfer messages
//to program flash.

#define FLASH_ActionErase 		0x00 //Action erases flash memory
#define FLASH_ActionValidate 	0x01 //Action does a validation (checksum) of data in flash.

//Flash Properties -- none currently defined.

//*******************************************************
// RF Component (WCACOMP_RF0 and WCACOMP_RF1)
//*******************************************************
// These properties are kept in sequential order for Lime Table access.
#define RFPROP_RFCTRL			0x00
#define RFPROP_SAMPLING			0x01
#define RFPROP_RXGAIN			0x02
#define RFPROP_RXFREQ			0x03
#define RFPROP_RXNCO			0x04
#define RFPROP_RXBANDWIDTH		0x05
#define RFPROP_TXGAIN			0x06
#define RFPROP_TXFREQ			0x07
#define RFPROP_TXNCO			0x08
#define RFPROP_TXBANDWIDTH		0x09
#define RFPROP_RXRSSI			0x0A
#define RFPROP_RXIQBIAS			0x0B
#define RFPROP_RFCTRL2			0x0C
#define RFPROP_RXPATH			0x0D
#define RFPROP_TXPATH			0x0E

#define RFACTION_RXTUNEVCO		0x80  //Tune the RX PLL VCO
#define RFACTION_TXTUNEVCO		0x81  //Tune the TX PLL VCO
#define RFACTION_RXCALIBRATE	0x82  //Calibrate the receiver using the current configuration
#define RFACTION_TXCALIBRATE	0x83  //Calibrate the transmitter configuration using the current configuration.
#define RFACTION_TOPCALIBRATE   0x84  //Calibrate Top Level.
#define RFACTION_SAVERXPROFILE	0x85  //Updates the stored RX profile with calibration data
									  //Must be in cached mode to actually save the data.
#define RFACTION_SAVETXPROFILE	0x86  //Updates the stored TX profile with calibration data
									  //Must be in cached mode to actually save the data.
#define RFACTION_RESETTOPCALIB  0x87  //Resets the top level calibration of the RF Component
#define RFACTION_RESETRXCALIB  	0x88  //Resets the RX calibration of the RF component.
#define RFACTION_RESETTXCALIB  	0x89  //Resets the TX calibration of the RF component.


//Enum defines the default RX pathes for the RF0 component.  Depending on which RF profiles are loaded into the
//device, these definitions may not be valied.
enum Rx0DefaultProfilesEnum
{
	RX0DPE_Disabled = 0x40,
	RX0DPE_GpsL1Int=0x41,
	RX0DPE_GpsL1Ext =0x42,
	RX0DPE_PcsExt =0x43,
	RX0DPE_Wideband=0x44
};

//Enum defines the default RX pathes for the RF1 component.  Depending on which RF Profiles are loaded
//into the device, these definititons may not be valid.
enum Rx1DefaultProfilesEnum
{
	RX1DPE_Disabled = 0xC0,
	RX1DPE_UhfExt=0xC1,
	RX1DPE_IsmInt =0xC2,
	RX1DPE_IsmExt =0x43,
	RX1DPE_Wideband=0x44
};


//*******************************************************
// RF Profiles Component (0x94) Interface Spec.
//*******************************************************
#define RFP_ACTION_CACHEDATA    	0x00	//Enter Cache mode of RF Profiles (use for calibration editing)
#define RFP_ACTION_DISCARDCHANGES 	0x01	//Exits Cache mode and discards changes
#define RFP_ACTION_SAVECHANGES		0x02	//Exits Cache mode and saves Profile changes to NVM.
											//Rolls the revision number.
//*******************************************************
// FPGA Component(0xB0) Interface Spec.
//*******************************************************
#define FPGA_LSDP_CONTROL	0x00	//Port Control WCA Property (Byte Write only)
#define FPGA_LSDP_STATUS	0x01	//Port Status  WCA Property  (Byte Read only)

//Enum specifies LSDP control and status flags.
#define LSDP_DISABLED 		0x0	 //port is disabled.
#define LSDP_SINGLESHOT 	0x01 //port is in single shot mode.
#define LSDP_CONTINUOUS 	0x02 //port is in continuous mode.
#define LSDP_MODE_MASK		0x03

#define LSDP_INPUT   		0x0 // RF Transmit Port
#define LSDP_OUTPUT  		0x4 // RF Receive Port
#define LSDP_DIRMASK 		0x4

#define LSDP_WCAEVENT_ALMOSTEMPTY 		0x0  //WCA Event notification fired when TX buff is almost empty.
#define LSDP_WCAEVENT_ALMOSTFULL		0x1

//*******************************************************************
// 4CH TX/RX Register definitions.
//*******************************************************************

//The Logic defines four DSP channels identified by a WCA Component ID
//as follows:
//
#define WCACOMP_DSP_DUC0 0x10 // Digital Up Converter
#define WCACOMP_DSP_DDC0 0x20 // Digital Down Converter

#define WCACOMP_DSP_DUC1 0x30 // Digital Up Converter
#define WCACOMP_DSP_DDC1 0x40 // Digital Down Converter.

//Each of these DSP channels define the following registers.  The specific
//register address is the sum of the DSP component ID and the register index below.

// DSP_DDUC_CTRL (WcaWriteByteRegister)
// DDC / DUC component Control register.  Bit flags 2 through 5 are
// only availble if the waveform was compiled with dynamic configuration
// enabling bypass of internal functions.  To achieve maximum 32 MHz sampling
// rate bypass CIC decimation, CIC strobe, and half band filters (0x39, 57 dec.).  To achieve
// 16 MHz sampling rate,  bypass CIC functions  (0x19, 25 dec.).  To directly sample front-end
// with decimation sampling rate, bypass decimation, but use sampling strobe (0x29, 45 dec. )
//
// bit#  |  Description
// -------|----------------------------------------------------------
//  0	   (1)Enable / (0)Disable Component USB port streaming.
//  1	   Reset FIFOS.
//  2	   Bypass Cordic rotation (DYNAMIC_CONFIG only)
//  3	   Bypass CIC Decimation  (DYNAMIC_CONFIG only)
//  4	   Bypass CIC Decimation strobe (DYNAMIC_CONFIG only)
// 	       When enabled, the decimation strobe is bypassed, and the original
// 	       sampling strobe is used to drive down stream.
//  5      Bypass Half band filter (DYNAMIC_CONFIG only).
//
#define DSP_DDUC_CTRL	  				0x00
#define DSP_DDUC_CTRL_DISABLED			0x00
#define DSP_DDUC_CTRL_ENABLED			0x01
#define DSP_DDUC_CTRL_RESET 			0x02
#define DSP_DDUC_CTRL_BYPASSCORDIC		0x04
#define DSP_DDUC_CTRL_BYPASSCIC			0x08
#define DSP_DDUC_CTRL_BYPASSCICSTROBE	0x10
#define DSP_DDUC_CTRL_BYPASSHALFBAND	0x20


// DSP_DDUC_PHASERATE(WcaWriteDwordRegister)
// Frequency Translation rate.  This is the phase rotation to apply
// at the 32 MHz sample rate to change the center frequency of the RF
// baseband input/output. These values are a 32 bit counter value, where
// the top 12 bits are used to drive cordic rotations.
#define DSP_DDUC_PHASERATE 0x01

// DSP_DDUC_SAMPRATE(WcaWriteWordRegister)
// Decimation / Integration sampling rate conversion .  This is the
// rate for down/up sampling sampling the 32 MHz sampling rate.  Valid
// range is 2 to 4096 equivalent to 8 MHz to  3906.25 KHz.
#define DSP_DDUC_SAMPRATE 0x02

#endif /* A2300InterfaceDefs_H_ */
