// Name:  NVM.h
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


#ifndef _nvm_h
#define _nvm_h

#include "fdl.h"
#include "fdl_descriptor.h"
#include "fdl_types.h"

#define BLOCK_A		((unsigned char *)0x03000)	/* BLOCK A first address */
#define BLOCK_B		((unsigned char *)0x03400)	/* BLOCK B first address */
#define BLOCK_C		((unsigned char *)0x03800)	/* BLOCK C first address */
#define BLOCK_D		((unsigned char *)0x03C00)	/* BLOCK D first address */

#define RECORD_PER_BLOCK	16					/* Record number per 1 block */
#define RECORD_SIZE			64					/* Record size */

#define BLOCK_NUMBER		4					/* BLOCK Number (BLOCK A, BLOCK B, BLOCK C and BLOCK D) */

#define BLOCK_A_SELECT		0					/* Use BLOCK A */
#define BLOCK_B_SELECT		1					/* Use BLOCK B */
#define BLOCK_C_SELECT		2					/* Use BLOCK C */
#define BLOCK_D_SELECT		3					/* Use BLOCK D */

#define	NORMAL				0x00				/* Normal after full status check */
#define	CMD_SEQ_ERROR		0x01				/* Command sequence error after full status check */
#define	ERS_BLK_CHK_ERROR	0x02				/* Erase error/bkank check error after full status check */
#define	PROGRAM_ERROR		0x03				/* Program error after full status check */


/************************************************************************
		SPECIFIC DEFINES TO M2300 CALIBRATION DATA IN NVM
************************************************************************/
#define	FLASH_STARTADDRESS				(BYTE*)0xf1000
#define	BLOCK_SIZE						1024

/*---- locations for config data ----*/
#define  NVM_CAL_OFFSET        			0x000
#define  NVM_CAL_STARTADDRESS       	(BYTE*)(FLASH_STARTADDRESS + NVM_CAL_OFFSET)
#define  NVM_CAL_BLOCK					0
#define  NVM_CAL_POOLADDRESS        	(NVM_CAL_OFFSET/4)

/************************************************************************
************************************************************************/

/*---- functions used external to 'NVM.c' ----*/
void NVM_Init( void );
fal_status_t   NVM_EraseBlock( WORD block );
fal_status_t   NVM_Write( WORD address, int length, DWORD* dwordPtr );


/*---- global variables defined in 'NVM.c' ----*/


#endif