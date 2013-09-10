// Name:  FrontPanel.c
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
#include "r_cg_timer.h"
#include "r_cg_userdefine.h"
#include "Main.h"
#include "Timers.h"
#include "FrontPanel.h"


/*---- static function declarations ----*/

/*---- static variable declarations ----*/
BYTE  LED_statecounter;
BYTE  LED_blinktimecurrent;
BYTE  LED_blinktime;
BYTE  LED_blinkcolor;
BYTE  LED_state;
BYTE  blink_cnt;

/*---- global variable declarations ----*/


/****************************************************************

   CALL:    LED_Init()
                             
   INTRO:   This routine initializes the LED processes.

   INPUT:   nothing
   OUTPUT:  nothing

****************************************************************/
void  LED_Init( void )
{

	LED_GREEN = LED_ON;
	LED_RED = LED_OFF;
	LED_blinktime = 0;

}

/****************************************************************

   CALL:    LED_ProcessTimer()
                             
   INTRO:   This routine is called 4 times per second to
            process the front panel LEDs. 
	    
   INPUT:   nothing
   OUTPUT:  nothing

****************************************************************/
void  LED_ProcessTimer( void )
{

	/*---- restart timer ----*/
	MMI_SetTimer( TIMER_LED, LED_TIME_FASTBLINK );

	CHRG_RED = 0;
	if( !LED_blinktime ) {
		CHRG_RED = 1;
		LED_blinktime = 10;
	}
	LED_blinktime--;

//	CHRG_RED ^= 1;

}



