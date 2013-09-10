// Name:  flash.h
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

#ifndef _FLASH_H_
#define _FLASH_H_

#include "WcaMsgs.h"  // Dci_BinaryImageTransfer saved in Flash Header.

// Flash Commands.
#define	WRITE_ENABLE		0x06
#define	WRITE_DISABLE		0x04
#define	READ_ID				0x9F
#define	READ_STATUS			0x05
#define	WRITE_STATUS		0x01
#define	READ_DATA			0x03
#define	FAST_READ			0x0B
#define	PAGE_PROGRAM		0x02
#define	SECTOR_ERASE		0xD8
#define	BULK_ERASE			0xC7
#define	DEEP_POWERDOWN		0xB9

#define	FLASHSTATUS_WIP		0x01

// Structure to define a flash header block
// Keep at 256 bytes for now to avoid flash issues.
typedef struct tagFlash_Hdr
{
	Dci_BinaryImageTransfer bitParams;
	uint16 crc;         // 16-bit crc.
} Flash_Hdr;
#define FLASH_IMAGE_OFFSET 	(256lu)				//Address of where data starts in the flash.
/**
 *  Flash Header at the start of block
 */
void Flash_Hdr_Init(Flash_Hdr *pHdr, Dci_BinaryImageTransfer *pBinImageParams, uint16 crc);
bool Flash_Hdr_Validate( Flash_Hdr *pHdr);
int  Flash_Hdr_Read(Flash_Hdr *pHdr);
int  Flash_Hdr_Write(Flash_Hdr *pHdr);




/**
 * Initialization
 */
void Flash_Init( );

/**
 * Processes on going updates
 */
void Flash_OnUpdate( );

int  Flash_Read( uint32 addr, int len, BYTE* dptr );
int  Flash_Write( uint32 addr, int len, BYTE* sptr );
int  Flash_Erase( void );


int  Flash_ReadDummy( long addr, int len, BYTE* dptr);
int  Flash_ComputeFrames( long sizeImg);

#endif
