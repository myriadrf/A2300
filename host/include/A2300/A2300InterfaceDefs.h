/************************************************************************
 * Name:  A2300InterfaceDefs.h
 * 
 * This module is the proprietary property of Loctronix Corporation
 * Copyright (C) 2013 Loctronix Corporation
 * All Rights Reserved
 * 
 ***************************************************************************/
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
#define WCACOMP_FPGA		0xB0	//FPGA WCA HAL configurtion and control

#define RFCH_RX	1
#define RFCH_TX 0

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

#define RFACTION_RXTUNEVCO		0x80
#define RFACTION_TXTUNEVCO		0x81
#define RFACTION_RXCALIBRATE	0x82
#define RFACTION_TXCALIBRATE	0x83

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
// DDC / DUC component Control register. Enables and disables component
// functions
// No values currently defined.
#define DSP_DDUC_CTRL	  	0x00

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
