// Name:  globals.h
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

#ifndef globals_h
#define globals_h


enum ERROR_TYPES
{
	ERROR_UART_TXOVERFLOW,			// 0
	ERROR_UART_RXOVERFLOW,			// 1
	ERROR_UART_PARITYFRAMING,       // 2
	ERROR_NVM,						// 3
	ERROR_SPI,						// 4
	ERROR_LIMECAL,					// 5
	ERROR_PRESSURE,					// 6
	ERROR_I2C,                      // 7
	ERROR_NUMOFTYPES
};

typedef struct 
{
	BYTE	Model;
	BYTE	ChargerState;
	WORD	DCIN_Volts;			// Battery voltage in Volts*10
	BYTE	FpgaConfigStatus;
	BYTE	Error[ERROR_NUMOFTYPES];
} STATUS_STRUCT;

/*---- debug enable bits for determining what debug data is sent to test program ----*/
#define  DEBUG_MISC				0x01

#define	I2C_ADDRESS_MPU			0xd0
#define	I2C_READ				0x01
#define	I2C_WRITE				0x00

/*---- 'Options' bit defines ----*/
#define  OPTION_DISABLELOWPOWER  0x0001

#define  VERSIONID_CONFIG     1     // version of the configuration structure

/*==== This is the configuration structure stored in non-volatile memory ====*/
typedef struct 
{
	BYTE  version_id;       // indicates version of structure
	BYTE  chksum;           // checksum for structure
	/*---- programmable configuration ----*/
	BYTE  DebugEnable;      // Bits for enabling debug data to be sent to test program
	BYTE  unused;			// (padding)
	/*---- configurable enable/disables ----*/
	WORD  Options;          // Configurable options (TBD)
} CONFIG_STRUCT;


/* exports */
extern STATUS_STRUCT status;
extern STATUS_STRUCT status_save;
extern CONFIG_STRUCT config;

/* prototypes */

#endif  // globals_h

