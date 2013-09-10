// Name:  Main.h
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


#ifndef MAIN_H
#define MAIN_H

#include <Comm/Dci/DciUtils.h>
#include <Comm/Dci/DciConversation.h>
#include "Asr2300InterfaceDefs.h" 
#include "dciproc.h"

/******************************************************************************
Macro Defines
******************************************************************************/


/* data type defintion */
typedef  unsigned long  ULONG;
typedef  unsigned long  DWORD;
typedef  unsigned int   UINT;
typedef  unsigned short USHORT;
typedef  unsigned char  UCHAR;
typedef  unsigned char  BOOL;
typedef  unsigned char  BYTE;
typedef  unsigned int   WORD;

#define  TRUE        (1)
#define  FALSE       (0)

/*---- pin definitions ----*/
#define	SPI_FLASH_CS		P0.4
#define	SPI_RF2_SEL			P0.5
#define	SPI_RF1_SEL			P0.6
#define	FPGM_SPI_PROGRAM	P1.0
#define	FPGM_SPI_DONE		P1.1
#define	FPGM_SPI_INITB		P1.2
#define	TP1					P1.6
#define	LOW_RX_ENABLE		P1.7
#define	WB1_TX_ENABLE		P2.0
#define	WB1_RX_ENABLE		P2.1
#define	PCS_LNA_BYPASS		P2.2
#define	GPS_LNA_ENABLE		P2.3
#define	WB2_TX_ENABLE		P2.4
#define	WB2_RX_ENABLE		P2.5
#define	ISM_TX_ENABLE		P2.6
#define	ISM_LNA_BYPASS		P2.7
#define	RF_ONOFF			P3.0
#define	CHRG_RED			P3.1
#define	CHRG_EN				P4.1
#define	CLKSEL_INT			P4.2
#define	CLKSEL_EXT			P4.3
#define	FX3_CTL15			P5.2
#define	FX3_RESET			P5.3
#define	CPU_RSTROBE			P5.4
#define	CPU_CLOCK			P5.5
#define	LED_GREEN			P6.2
#define	LED_RED				P6.3
#define	CPU_DBUS			P7
#define	CPU_DBUS_OUTPUT		PM7 = 0x00
#define	CPU_DBUS_INPUT		PM7 = 0xff
#define	CPU_DBUS0			P7.0
#define	CPU_DBUS1			P7.1
#define	CPU_DBUS2			P7.2
#define	CPU_DBUS3			P7.3
#define	CPU_DBUS4			P7.4
#define	CPU_DBUS5			P7.5
#define	CPU_DBUS6			P7.6
#define	CPU_DBUS7			P7.7
#define	ADC_VERSION			P12.0
#define	USB_OV				P12.3
#define	CHRGDET				P12.4
#define	FPGA_RESET			P13.0
#define	CPU_INT				P13.7
#define	CPU_ASTROBE			P14.1
#define	CPU_WSTROBE			P14.6
#define	ADC_DC_IN			P14.7

#define	SPI_DOUT			P1.3
#define	SPI_DIN				P1.4
#define	SPI_SCK				P1.5

#define	SPI_DOUT_FLASH		P0.1
#define	SPI_SCK_FLASH		P0.0

#define	LED_ON				1	// active high
#define	LED_OFF				0

#define  WATCHDOG_RESET          {WDTE = 0xACU;}



/**
* Log status flags for using in main_LogMsg.
*/
enum   LogStatusEnum { LOG_ERR = 0, LOG_WARN, LOG_INFO, LOG_DEBUG};
#define IDC_DEFAULT -1 //Default conversation ID.
/**
* Message logging facility sends log data to host if enabled.
*/
void   main_LogMsg( byte logStatus, uint16 srcid, pcstr szMessage, Dci_Context* pctxt);
void   main_LogMsgIdc( byte logStatus, uint16 srcid, pcstr szMessage, int idConversation );


/**
* Adds the specified message map to the the master message map manager.
*/
void   main_AddDciMsgMap( Dci_MapEntry* map);

uint32  main_GetTimecode( void );

#endif

