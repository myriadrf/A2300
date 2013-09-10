// Name:  sensor.c
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
#include "r_cg_serial.h"
#include "r_cg_userdefine.h"
#include "Main.h"
#include "Globals.h"

#include <string.h>
#include <Comm/Dci/DciConversation.h>
#include <Comm/Dci/StandardMsgs.h>
#include <Comm/Dci/InfrastructureMsgs.h>
#include <Comm/Dci/WcaMsgs.h>

#include "sensor.h"
#include "pressure.h"
#include "battery.h"
#include "motion.h"

// Defines
#define MOTION_COMPONENT_ID   0x83
#define PRESSURE_COMPONENT_ID 0x87

/*DCI Functions */
extern void OnMotionPropQuery( Dci_Context* pctxt);
extern void OnPressurePropQuery( Dci_Context* pctxt);

//DCI Handler Declaration
DCI_BEGIN_MSGMAP( s_sensorMap)
	DCI_WCAPROPQUERY_HANDLER( MOTION_COMPONENT_ID, 0x0, 0xB, &OnMotionPropQuery)
	DCI_WCAPROPQUERY_HANDLER( PRESSURE_COMPONENT_ID, 0x0, 0x1, &OnPressurePropQuery)
DCI_END_MSGMAP

// Keep Sensor parameters as follows because of the different data types.
// Note 3rd column contains test values (ie 0x00A).
#define MAX_MOTION_PROP  (12)
Dci_Property  s_motionProp[] = {
	{ 0, PT_UINT32,(uint32)0x00A}, //Accelerometer (txyz)
	{ 1, PT_INT16, (int16) 0x0A1}, //161
	{ 2, PT_INT16, (int16) 0x0A2}, //
	{ 3, PT_INT16, (int16) 0x0A3}, //
	{ 4, PT_UINT32,(uint32)0x00F}, //Gyro
	{ 5, PT_INT16, (int16) 0x0F1}, //
	{ 6, PT_INT16, (int16) 0x0F2}, //
	{ 7, PT_INT16, (int16) 0x0F3}, //
	{ 8, PT_UINT32,(uint32)0x00C}, //Compass
	{ 9, PT_INT16, (int16) 0x0C1}, //
	{10, PT_INT16, (int16) 0x0C2}, //
	{11, PT_INT16, (int16) 0x0C3}  //
};

/**
 * Initialization
 */
void Sensor_Init( )
{
	main_AddDciMsgMap( s_sensorMap);
	
	// Note: These devices are under development until 
	// the i2c interface is working.
	Motion_Init();
	Pressure_Init();
	Battery_Init();
}

/**
 * Handles received DCI messages 
 */
void Sensor_OnDciMessageReceived(Dci_Context * pctxt)
{
}

/**
 * Processes on going updates for all sensors.
 */
void Sensor_OnUpdate( )
{
	// Note: Awaiting I2C driver.
    Pressure_Update();
	Battery_Update();
	Motion_Update(s_motionProp);
}

/****************************************************************
   PRESSURE SENSOR ACCESS
****************************************************************/

/**
* Handler called to read Pressure Sensor properties
*/
void OnPressurePropQuery( Dci_Context* pctxt)
{
	byte buff[MAX_MSG_SIZE];
	Dci_Property *pPropData;
	int i;

	// Get information about the data request.
	Dci_TypedPropertiesQuery* ptdq	= (Dci_TypedPropertiesQuery*) pctxt->pMsg;
	byte*					  pids = (byte*) Dci_TypedPropertiesQuery_GetPropIds( ptdq);  

	// Initialize the return packet.
	int				len		= Dci_TypedProperties_Init( buff, sizeof(buff), pctxt->idComponent, ptdq->ctProperties, NULL );
	Dci_Property*	pProps	= (Dci_Property*) Dci_TypedProperties_GetProperties( (Dci_TypedProperties *)buff );  
	
	//Iterate properties copying the current default properties.
	for( i = 0; i < ptdq->ctProperties; i++, pProps++)
	{
		pProps->idprop = pids[i];
		pProps->idtype = PT_INT32;
		Pressure_GetValue((PressureId)pids[i], &(pProps->value.vInt32));
	}
	pctxt->bHandled = true;

	//Send the requested properties back.
	Dciproc_SendMessage1(  buff, len, false, pctxt);
}

/****************************************************************
   MOTION SENSOR ACCESS
****************************************************************/

/**
* Handler called to read Motion Sensor properties
*/
void OnMotionPropQuery( Dci_Context* pctxt)
{
	byte buff[MAX_MSG_SIZE];
	unsigned idx;
	int i;
	int iErr = 0x0;

	// Decode Message In.
	Dci_TypedPropertiesQuery* ptdq	= (Dci_TypedPropertiesQuery*) pctxt->pMsg;
	byte*					  pids = (byte*) Dci_TypedPropertiesQuery_GetPropIds( ptdq);  

	// Create Message Out.
	int				len	   = Dci_TypedProperties_Init( buff, sizeof(buff), pctxt->idComponent, ptdq->ctProperties, NULL );
	Dci_Property*	pProps = (Dci_Property*) Dci_TypedProperties_GetProperties( (Dci_TypedProperties*)buff);  
	
	//Iterate properties copying the current default properties.
	for( i = 0; i < ptdq->ctProperties; i++, pProps++)
	{
		idx = (unsigned) pids[i];
		if( idx < MAX_MOTION_PROP )
			memcpy(pProps, &s_motionProp[idx], sizeof(Dci_Property));
		else
		{
			iErr = i;
			break;
		}
	}
	pctxt->bHandled = true;

	// Respond.
	if( iErr == 0x0 )
	{
		//Send the requested properties back.
		Dciproc_SendMessage1(  buff, len, false, pctxt);
	}
	else // Error.
	{
		//Send an message back on the current conversation indicating the
		//error condition.
		char buff2[16];
		len = Dci_MessageError_Init1( buff2, pctxt->pMsg);
		Dciproc_SendMessage( (byte*)buff2, len, false, pctxt->pConv->idConversation);
	}
}


