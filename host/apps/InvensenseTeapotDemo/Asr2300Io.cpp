/** Name: Asr2300Io.h
*
* Copyright(c) 2014 Loctronix Corporation
* http://www.loctronix.com
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdexcept>
#include <vector>

#include "Asr2300Io.h"
#include "mlStructure.h"
//
//
//
#include <A2300/ConfigDevice.h>
#include <A2300/ConfigSensor.h>

using namespace A2300;

ConfigDevice* s_pConfig = NULL;
ConfigMotion* s_pMotion = NULL;
bool s_bRunning = true;

HANDLE g_hSIOThread = NULL;

DWORD SensorProcessThread(LPVOID lpvoid);

#ifdef __cplusplus
extern "C"
{
#endif

float four_bytes(byte d1,byte d2,byte d3,byte d4);



int getQuatData(tQuatPacket *qData);
extern tQuatPacket qData;

/*
 For 32-bit signed integers.
*/
float four_bytes(byte d1,byte d2,byte d3,byte d4)
{
	 float  d = d1*(1<<24) + d2*(1<<16) + d3*(1<<8) + d4;
	 if (d > 2147483648)
		 d-= 4294967296;
	return d;
}

#ifdef __cplusplus
}
#endif


/**********************************************************************/
/*                                                                    */
/* Function Name: getQuatData                                         */
/*                                                                    */
/*   Description: Parse rx data buffer to get Quaternion              */
/*                                                                    */
/*    Parameters: qData - parsed Quaternion data                      */
/*                rxBuffer - rx data buffer                           */
/*                                                                    */
/*       Returns: Return code.                                        */
/*Note: This program is not being used. Check getQuatData in sio.c    */
/*       For quick reference only                                     */
/**********************************************************************/
int getQuatData(tQuatPacket *qData)
{
	byte buff[DCI_MAX_MSGSIZE];	
	Motion_DataSet* pds = s_pMotion->ReceiveReport( buff, sizeof(buff));
   if( pds == NULL)
	   return -1;

#define SCALE_FACTOR (1.0f/(1<<30))

    ////Parse MPU Data
    qData->quat[0] = (float) (pds->Samples[0].quat[0])*SCALE_FACTOR;   
	qData->quat[1] = (float) (pds->Samples[0].quat[1])*SCALE_FACTOR;
	qData->quat[2] = (float) (pds->Samples[0].quat[2])*SCALE_FACTOR;
	qData->quat[3] = (float) (pds->Samples[0].quat[3])*SCALE_FACTOR;
    
    return 0;
}



int ASR2300SensorProcessStart()
{
	DWORD dwThreadID;
	s_pConfig = new ConfigDevice();
	// Attach to the first device we find.
	int result = s_pConfig->Attach();
	if( result < 0)
	{
		return result;
	}
	//Create Motion sensor configuration to report.
	s_pMotion = new ConfigMotion("motion", s_pConfig);
	s_pMotion->Reporting( ConfigMotion::Continuous, 50, 6, 0);

    // Create SIO thread
    g_hSIOThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SensorProcessThread,
                                0, 0, &dwThreadID);
    if( g_hSIOThread == NULL )
    {
        DWORD dwError = GetLastError();
        printf("Can't create ASR-2300 sensor processor thread: %x\r\n", dwError);
        exit(-1);
    }

    SetThreadPriority((HANDLE) g_hSIOThread, (int) THREAD_PRIORITY_HIGHEST);

    return 0;
}




DWORD SensorProcessThread(LPVOID lpvoid)
{
	s_bRunning = true;

    while( s_bRunning)
    {
		getQuatData(&qData);
		Sleep(2);
	}

	s_pConfig->Detach();
    return 0;
}

