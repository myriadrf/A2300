// Name:  Timers.c
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

#pragma interrupt INTTM00 ISR_SystemTimer
#pragma EI

/*---- determine how to compile ----*/
#include "Compile.h"

/*---- Standard C include files ----*/

/*---- Custom include files ----*/
#include "r_cg_macrodriver.h"
#include "r_cg_timer.h"
#include "r_cg_adc.h"
#include "r_cg_userdefine.h"
#include "Main.h"
#include "Timers.h"

/*---- static function declarations ----*/

/*---- static variable declarations ----*/
volatile MMITIMER_STRUCT   mmi_timer[NUMBER_OF_MMITIMERS];
volatile BYTE  counter_1ms;

/*---- global variable declarations ----*/
volatile DWORD timer_tick = 0;


/***************************************************************

   CALL:    Timer_Init()

   INTRO:   This routine initializes the timers used on the
            the repeater.  These timers are maintained by the
            Atmel CPU Periodic Interval Timer (PIT), which is
            designed to drive the system timer.

   INPUT:   nothing
   OUTPUT:  nothing

****************************************************************/
void  Timer_Init( void )
{

   /*---- initialize interval timer hardware ----*/
   R_TAU0_Channel0_Start();
   
   /*---- clear out the 1ms counter to get to 10ms ----*/
   counter_1ms = 0;  

}

/***************************************************************

   CALL:    Timer_Delay()

   INTRO:   This routine delays the specified number of usec.

   INPUT:   usdelay           - number of usec to delay
   OUTPUT:  nothing

****************************************************************/
void  Timer_Delay( int usdelay )
{
int   i;

   for( i=0 ; i<usdelay ; i++ ) {
      /*---- delay 1us ----*/
      NOP();
      NOP();
      NOP();
      NOP();
      NOP();
      NOP();
   }

}


/*********************************************************

   CALL:    MMI_SetTimer()

   INTRO:   This function sets the specified MMI timer
            to expire in the specified time.  Each timer
            tick is 10ms, thus a setting of 100 will
            expire in 1 second.
            
            Calling this function with the time parameter
            set to 0 will disable the timer.
            
            When the timer expires, it is process by the
            main event loop by calling the MMI_CheckTimers()
            function below.

   INPUT:   timer       - timer number
            time        - expire time in ms*10
   OUTPUT:  nothing

*********************************************************/
void  MMI_SetTimer( BYTE timer, int time )
{
   if( time ) {
      /*---- enable timer to expire in 'time' ms*10 ----*/
      mmi_timer[timer].enable = 1;
      mmi_timer[timer].expire_time = time;
   } else {
      /*---- disable timer ----*/
      mmi_timer[timer].enable = 0;
   }
}

/*********************************************************

   CALL:    MMI_GetTimer()

   INTRO:   This function gets the specified MMI timer
            current value.

   INPUT:   timer       - timer number
   OUTPUT:  time        - expire time in ms*10

*********************************************************/
int   MMI_GetTimer( BYTE timer )
{
   if( mmi_timer[timer].enable ) {
      return( mmi_timer[timer].expire_time );
   } else {
      return( 0 );
   }
}

/*********************************************************

   CALL:    MMI_CheckTimers()

   INTRO:   This function checks all MMI timers to see
            if one expired.  If it did, then this function
            returns the timer number.

   INPUT:   nothing
   OUTPUT:  timer number if timer expired, else 0

*********************************************************/
int   MMI_CheckTimers( void )
{
int   i;

   for( i=0 ; i<NUMBER_OF_MMITIMERS ; i++ ) {
      if( mmi_timer[i].enable ) {
         /*---- if timer counted down to zero, then it expired ----*/
         if( mmi_timer[i].expire_time==0 ) {
            mmi_timer[i].enable = 0;
            return( i );
         }
      }
   }
   
   /*---- return indicating no timer expired ----*/
   return( 0 );
   
}

/*********************************************************

   CALL:    MMI_TimerEnabled()

   INTRO:   This function checks one MMI timer to see
            if it is enabled.

   INPUT:   timer       - timer number
   OUTPUT:  enabled     - 1
            not enabled - 0

*********************************************************/
BYTE  MMI_TimerEnabled( int timer )
{
   return (BYTE)mmi_timer[timer].enable;
}

/****************************************************************

   CALL:    ISR_SystemTimer()
                             
   INTRO:   This routine is the interrupt service routine for
            the 16-bit timer.

            The timer buffer is checked and each active timer
            is processed.

   INPUT:   nothing
   OUTPUT:  nothing

****************************************************************/
__interrupt static void ISR_SystemTimer(void)
{
static BYTE  i;

   EI();       // allow higher priority interrupts to occur

// TP1 = 1;
   timer_tick++;


   /************************************/
   /****       10MS INTERVAL         ***/
   /************************************/

   for( i=0 ; i<NUMBER_OF_MMITIMERS ; i++ ) {
      /*---- if timer is non-zero, then decrement it ----*/
      if( mmi_timer[i].expire_time ) {
         mmi_timer[i].expire_time--;
      }
   }

// TP1 = 0;

}

/****************************************************************
   CALL:    Timer_GetTime()                             
   INTRO:   Return time (msec) based on 10msec interrupt counter.
****************************************************************/
void Timer_GetTime(unsigned long *timeMsec)
{
	// Interrupts at 10msec intervals.
	*timeMsec = 10LU * timer_tick;
}
