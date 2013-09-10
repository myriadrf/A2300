// Name:  SPI.h
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

#ifndef  _SPI_
#define  _SPI_


enum SPI_DEVICES
{
	SPI_DEVICE_FLASH = 1,
	SPI_DEVICE_LIME1,
	SPI_DEVICE_LIME2,
	SPI_DEVICE_FPGA
};

/*---- functions used external to 'SPI.c'  ----*/
void  SPI_Init( void );
int  SPI_WriteReadData( BYTE Device, int txLen, BYTE* txData, int rxLen, BYTE* rxData, BOOL continue_flag );

/*---- global variables defined in 'SPI.c' ----*/

#endif
