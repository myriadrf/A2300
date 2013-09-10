// Name:  Asr2300InterfaceDefs.h
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

#ifndef Asr2300InterfaceDefs_H_
	#define Asr2300InterfaceDefs_H_
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



#endif