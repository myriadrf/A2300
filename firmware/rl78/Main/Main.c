// Name:  Main.c
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


/*---- determine how to compile ----*/
#include "Compile.h"

/*---- Custom include files ----*/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_serial.h"
#include "r_cg_timer.h"
#include "r_cg_wdt.h"
#include "r_cg_adc.h"
#include "r_cg_userdefine.h"
#include "main.h"
#include "Globals.h"

//Include Low-Level Driver Modules
#include "Drivers/Timers.h"
#include "Drivers/SPI.h"
#include "Drivers/i2c.h"
#include "Drivers/fpga.h"
#include "Drivers/lime.h" 
#include "Drivers/uart.h"
#include "Drivers/power.h" 
#include "Sensors/sensor.h"
#include "Drivers/flash.h"
#include "Version.h"

//Include High-Level Modules.
#include "FrontPanel.h"
#include "RFComponent.h"
#include "DataPorts.h"

//Includes for communications.
#include<Comm/Dci/InfrastructureMsgs.h>
#include<Comm/Dci/StandardMsgs.h>
#include<System/Util.h>

/*---- Standard C include files ----*/
#include <string.h>


//*******************************************************
//Internal Module Structures and Function Prototypes
//*******************************************************
uint32 s_timecode;
Dci_MapMgr s_mapmgr;
static int 	g_ctReceivedMsgs = 0;
int count = 0;
static int s_idcDefault = 2;

// Use #define to do Windows Timing analysis.
#undef ENABLE_TIMING_ANALYSIS
#undef RXMSG_TICK 100  //undefine to eliminate tick.

#define LOGGING_ENABLED

//*******************************************************
// Implementation
//*******************************************************
void   main_AddDciMsgMap( Dci_MapEntry* map);

#if defined(ENABLE_TIMING_ANALYSIS)
	//Uncomment to do windows timing analysis.
	LARGE_INTEGER liFreq, liTickCur, liTickLast, tickpsec, liDiff ;
	double dt;
#endif


/****************************************************************

	CALL:	main()
	         
	INTRO:	This routine starts the M2300 board.  The main
			event loop is contained in this file.

   INPUT:	nothing
   OUTPUT:	nothing

****************************************************************/
void main (void) 
{
int   timerId = -1;
BYTE  i;
BYTE  powerup_ready = 0;

#if 0
	/*---- initialize all GPIO output values ----*/
	P0  = 0x00;
	P1  = 0x00;
	P2  = 0x00;
	P3  = 0x00;
	P4  = 0x00;
	P5  = 0x00;
	P6  = 0x00;
	P7  = 0x00;
	P12 = 0x00;
	P13 = 0x00;
	P14 = 0x00;
	/*---- port directions: 0=output, 1=input ----*/
	PM0  = 0x0f;
	PM1  = 0x10;
	PM2  = 0x00;
	PM3  = 0x00;
	PM4  = 0x00;
	PM5  = 0x01;
	PM6  = 0x03;
	PM7  = 0x00;
	PM12 = 0xff;
	PM14 = 0xbd;
#endif

	//Initialize main
	Dci_MapMgr_Init( &s_mapmgr);

    EI();			// start allowing interrupts

	//*************************
	// Driver Module Initialization
	//*************************
//	NVM_Init();
	
	SPI_Init( );
	I2C_Init();
	Power_Init( );
	Dciproc_Init( );
	Flash_Init( );
	Fpga_Init( );
	Lime_Init( );
	Sensor_Init( );
	Uart_Init( );
	Timer_Init( );
	
	/*---- allow FX3 to run by releasing the reset line ----*/
	FX3_RESET = 1;

	/*---- initialize ADC hardware ----*/
	R_ADC_Set_OperationOn();
//	ADS = xxx;
	ADIF = 0U;  		// clear INTAD interrupt flag
//	ADMK = 0U;  		// enable INTAD interrupt
	ADCS = 1U;			// start first conversion

   /*---- load non-volatile settings from flash ----*/
//	NVM_Init();

	/*---- try to load the FPGA configuration from the flash EPROM ----*/
	Fpga_LoadConfigFromFlash();

	/*---- start timer for processing LEDs ----*/
	MMI_SetTimer( TIMER_LED, LED_TIME_FASTBLINK );

	//Reset Timecode.
	s_timecode = 0;

	//*************************
	// Application Module Initialization
	//*************************

	//Initialize High-Level Modules last.
	RfComp_Init( );
	DataPorts_Init();	
	
	
	WATCHDOG_RESET 

	/***************************************/
	/*          MAIN EVENT LOOP            */
	/***************************************/

	main_LogMsgIdc(LOG_DEBUG, -1, "Testing log", 0);
	while (1)
	{

		/*---- process all expired timers ----*/
		timerId = MMI_CheckTimers();
		if( timerId == TIMER_LED )
		{
			/*---- process LEDs ----*/
			LED_ProcessTimer();
		}

		//Call Update Handlers on each of the modules supporting it.
		Dciproc_OnUpdate( );
		//Flash_OnUpdate( ); //Not currently needed
		//Fpga_OnUpdate( );  //Not currently needed
		//Lime_OnUpdate( );  //Not currently needed
		//Sensor_OnUpdate( );  //Future - Uncomment.

		RfComp_OnUpdate();
		DataPorts_OnUpdate();
		
		s_timecode++;

#if defined(ENABLE_TIMING_ANALYSIS)
		//timing analysis in Windows.
		QueryPerformanceCounter( &liTickCur);
		liDiff.QuadPart = liTickCur.QuadPart - liTickLast.QuadPart;
		dt = (double) liDiff.QuadPart / (double) tickpsec.QuadPart;
		if( s_timecode % 1000 == 0)
			printf( "%lf\n", dt);
		liTickLast = liTickCur;
#endif

		status.DCIN_Volts = ADCR>>6;
		/*---- start next conversion ----*/
		ADCS = 1U;

		/*---- determine FPGA status ----*/
		if( !FPGM_SPI_DONE ) {
			status.FpgaConfigStatus = FPGACONFIG_LOADING;
		} else {
			if( FPGM_SPI_INITB ) {
				status.FpgaConfigStatus = FPGACONFIG_NOCRCERROR;
			} else {
				status.FpgaConfigStatus = FPGACONFIG_CRCERROR;
			}
		}

		/*---- hit watchdog ----*/
		WATCHDOG_RESET 
	}  
}


/**
 * Callback function from dciproc Handles received DCI messages 
 */
void Main_OnDciMessageReceived(Dci_Context * pctxt)
{
	int lenMsg;  
	int idc = Dci_Hdr_GetConversationId(pctxt->pMsg);
	g_ctReceivedMsgs++;

#ifdef RXMSG_TICK
	//Make a tick on the console every 100 messages to 
	//indicate we are alive.
	if( RXMSG_TICK%100 == 0) putchar( '.');
#endif
	//Use the Message Map Manager to call
	//appropriate message handlers.
	Dci_MapMgr_Process( &s_mapmgr, pctxt);

	// Handle device identification and version query
	if( !pctxt->bHandled)
	{
		byte buff[MAX_MSG_SIZE];
		switch( pctxt->idMessage)
		{
		case Dci_IdentifyDeviceQuery_Id: // device identification
			lenMsg = Dci_IdentifyDevice_Init( buff, "ASR-2300", "-N/A-", "ASR-2300");
			Dciproc_SendMessage( buff, lenMsg, false, idc);
			pctxt->bHandled = true;
			break;
			
		case Dci_VersionInfoQuery_Id:	// version info
			lenMsg = Dci_VersionInfo_Init( buff, VER_MAJOR, VER_MINOR, VER_MAINT, VER_REVISION);
			Dciproc_SendMessage( buff, lenMsg, false, idc);
			pctxt->bHandled = true;
			break;
			
		case Dci_DeviceInfo_Id:        // Device Information.
			{
				Dci_IdentifyDevice *pDeviceInfo = (Dci_IdentifyDevice *)pctxt->pMsg;
				pctxt->bHandled = true;
			}
			break;
			
		case Dci_DebugMsg_Id: // Debug Message.
			{
				Dci_DebugMsg* pdbg = (Dci_DebugMsg*) pctxt->pMsg;
	//			printf("Received Debug Message from status=%u, source=%u, msg=%s\n", pdbg->status, pdbg->src, Dci_DebugMsg_Message( pdbg));
				pctxt->bHandled = true;
			}
			break;
		default:			// Respond with unhandled message.
			lenMsg = Dci_MessageError_Init1( buff, pctxt->pMsg);
			Dciproc_SendMessage( buff, lenMsg, false, idc);
			pctxt->bHandled = true;
			break;
		}
	}
}


/**
* Function returns the current time code value
*/
uint32  main_GetTimecode( void ) { return s_timecode;}

/**
* Message logging facility sends log data to host if enabled.
*/
void   main_LogMsg( byte logStatus, uint16 srcid, pcstr szMessage, Dci_Context* pctxt)
{
#ifdef LOGGING_ENABLED
	byte buff[MAX_MSG_SIZE];
	int len = Dci_DebugMsg_Init( buff, sizeof(buff), logStatus, srcid, szMessage);
	Dciproc_SendMessage1(  buff, len, false, pctxt);
#endif
}

/**
* Message logging facility sends log data to host if enabled.
*/
void   main_LogMsgIdc( byte logStatus, uint16 srcid, pcstr szMessage, int idConversation )
{
#ifdef LOGGING_ENABLED
	byte buff[MAX_MSG_SIZE];
	int len = Dci_DebugMsg_Init( buff, sizeof(buff), logStatus, srcid, szMessage);

	idConversation = (idConversation == IDC_DEFAULT) ? s_idcDefault : idConversation;
	Dciproc_SendMessage(  buff, len, false, idConversation);
#endif
}


/**
* Adds the specified message map to the the master message map manager.
*/
void   main_AddDciMsgMap( Dci_MapEntry* map)
{
	Dci_MapMgr_Add( &s_mapmgr, map);
}
