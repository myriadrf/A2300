// Name:  Timers.h
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


#ifndef _timers_h
#define _timers_h


#define  ONESECOND_TIME       100   /* 1 second time expiry */
#define  AUTOSTATUS_TIME      50    /* send auto status every 1/2 second */


/*---- define MMI timers ----*/
enum MMITimers
{
   TIMER_LED = 1,             // used to control LED blinking
   NUMBER_OF_MMITIMERS
};

typedef struct {
   BYTE  enable;              /* 1=enable, 0=disable */
   int   expire_time;         /* time that timer will expire */
} MMITIMER_STRUCT;

/*---- functions used external to 'Timers.c' ----*/
void  Timer_Init( void );
void  Timer_Delay( int usdelay );
void  Timer_GetTime( unsigned long *timeMsec );

void  MMI_SetTimer( BYTE timer, int time );
int   MMI_GetTimer( BYTE timer );
int   MMI_CheckTimers( void );
BYTE  MMI_TimerEnabled( int timer );


/*---- global variables defined in 'Timers.c' ----*/
extern volatile DWORD   timer_tick;


#endif
