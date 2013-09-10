// Name:  DataPorts.h
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

#ifndef _DataPorts_H_
#define _DataPorts_H_

#include <System/DataTypes.h>


//**********************************
// Common module interface.
//**********************************

/**
 * Initialization
 */
void DataPorts_Init( );

/**
 * Processes on going updates
 */
void  DataPorts_OnUpdate( );



//**********************************
// Low Speed data ports (LSDP #0, #1, #2, #3)
//**********************************
typedef struct DataPorts_Def
{
	byte IdComponent;
	byte PortStatus;
	uint32		  Framecount;		//Current data frame.
	bool		  bDoSingleShot;	//Is single shot mode.
	int			  idc;				//DCI Conversation to report on. 
} DataPorts_Def;

enum DataPorts_LsdpIdEnum { LSDP0 = 0x90, LSDP1 = 0x91, LSDP2 = 0x92, LSDP3 = 0x93};

DataPorts_Def* DataPorts_GetPort( byte idPort);



#endif