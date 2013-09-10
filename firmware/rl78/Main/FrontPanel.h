// Name:  FrontPanel.h
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


#ifndef _frontpanel_h
#define _frontpanel_h

#define  LED_TIME_SLOWBLINK         50       // slow blink rate = 2 times/second
#define  LED_TIME_FASTBLINK         24       // fast Blink rate = 4 times/second
#define  LED_TIME_VERYFASTBLINK     14       // very fast Blink rate = 7 times/second
#define  LED_TIME_INITIALSTATUS     30*100   // show initial LED status for first 30 seconds

#define	LED_
/*---- define LED states ----*/
enum LED_States
{
   LEDSTATE_STARTINGUP = 0,                  // starting up after power up
   LEDSTATE_SHOWINIT,                        // show condition first time unit stabilized after power up
   LEDSTATE_DONE                             // done with LED status, just show green or red (if shut down)
};



/*---- functions used external to 'Timers.c' ----*/
void  LED_Init( void );
void  LED_ProcessTimer( void );

/*---- global variables defined in 'FrontPanel.c' ----*/

#endif

