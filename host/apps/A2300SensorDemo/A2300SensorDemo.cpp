/** Name: A2300SensorDemo.cpp
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

#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN 
	#include<windows.h>
	#define SLEEP_SEC(a)  Sleep((a)*1000)
#else
	#include <unistd.h>
	#define SLEEP_SEC(a) sleep((a))
#endif

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdexcept>
#include <vector>

#include <A2300/ConfigDevice.h>
#include <A2300/ConfigSensor.h>

using namespace A2300;

// This program demonstrates how to use the ConfigeDevice class to easily attach
// to an ASR-2300 device.  
int main(int, char**)
{

	// Attach to the first device we find.
	ConfigDevice config;
	int result = config.Attach();
	if( result < 0)
	{
		printf("Could not attach ASR-2300 device\n");
		return result;
	}
	else
		printf( "ASR-2300 Device Opened\n");


	printf("%s\n\n", config.IdentifyDevice().c_str());

	//Create Motion sensor configuration to report.
	ConfigMotion cm("motion", &config);
	cm.Reporting( ConfigMotion::Continuous, 50, 6, 0);


	//Use helper functions on the ConfigMotion template 
	//To access Motion data. Note this is a synchronous blocking
	//call.  Need to implement asynchronous overlapped transfer
	//model for reduced latency, increased speed and continuity.
	byte buff[DCI_MAX_MSGSIZE];
	for( int i = 1; i < 100; i++)
	{
		Motion_DataSet* pds = cm.ReceiveReport( buff, sizeof(buff));
		if( pds != NULL)
		{
			printf("%xyz: d, %d, %d\r\n", pds->Samples[0].accel[0],pds->Samples[0].accel[1],pds->Samples[0].accel[2]);
		}
	}

	cm.Reporting( ConfigMotion::Disabled, 50,6,0);

	//Detach from ASR-2300 
	config.Detach();
	return(0);
}

