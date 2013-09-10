// Name:  Version.h
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

#ifndef _version_h
#define _version_h

/***************************************************/
/*       STANDARD VERSION INFORMATION              */
/***************************************************/
// Define Firmware Version.
#define VER_MAJOR  (0)
#define VER_MINOR  (0)
#define VER_MAINT  (1)
#define VER_REVISION (3)
	
#define  VERSION_MONTH        8
#define  VERSION_DAY          13
#define  VERSION_YEAR         13


/* 
-------------------------------------------------------------------------------
   version V00.01.00  2/15/13

      - Initial version created from 2.5.2 of the YX545.
         
-------------------------------------------------------------------------------
   version V00.01.01  3/14/13

      - When going into factory test mode, turn on all paths' power in case
        they were disabled during normal mode.
         
-------------------------------------------------------------------------------
*/


/*---- functions used external to 'Version.c' ----*/

/*---- global variables defined in 'Version.c' ----*/

#endif
