// Name:  pressure.h
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

#ifndef  _PRESSURE_
#define  _PRESSURE_


#define PARAM_MG      3038        /*calibration parameter */
#define PARAM_MH     -7357        /*calibration parameter */
#define PARAM_MI      3791        /*calibration parameter */

/* this structure holds all device specific calibration parameters */
typedef struct 
{
   short ac1;
   short ac2;
   short ac3;
   unsigned short ac4;
   unsigned short ac5;
   unsigned short ac6;
   short b1;
   short b2;
   short mb;
   short mc;
   short md;
} bmp180_calibration_param_t;

/* Allow data access by enumeration
 * These enumerations line up with the WSA Property ID's
 */
typedef enum enPressureId
{
	Pressure_Time = 0x0,
	Pressure_Pressure,
	Pressure_Temperature,
	Pressure_TSize
} PressureId;

/*---- functions used external to 'Pressure.c'  ----*/
void  Pressure_Init( void );
void  Pressure_Update();
bool  Pressure_GetValue(PressureId id, int32 *pValue);
/*---- global variables defined in 'Pressure.c' ----*/

#endif /* pressure.h */

