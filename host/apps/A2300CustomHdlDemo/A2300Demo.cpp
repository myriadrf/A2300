/** Name: A2300Demo.cpp
*
* Copyright(c) 2013 Loctronix Corporation
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
using namespace A2300;

// This program demonstrates how to use the ConfigeDevice class to easily attach
// to an ASR-2300 device.  Then using the TransportDci to set a custom property on the default waveform 
// component.  The property (A0) sets an LED blink rate in the custom HDL container demonstrated at the 
// Loctronix A2300 Open Source Webinar presentation dated 2/18/14.  Be sure to build and load the corresponding
// verilog container in the hdl/containers directory to use with this demo application. See
// http://www.loctronix.com/news/webinars/LoctronixWebinar-2-140218.pdf for discussion and setup.
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

	//Get the FPGA ID and Version
	uint16	    idFpga 		= config.FpgaId();
	uint16 		verFpga 	= config.FpgaVersion();
	int  		iVer = (verFpga>>8);
	int	 		iRev = (verFpga& 0x00ff);
	printf("  FPGA ID-Ver:  %04X - %02X.%02X\n\n", idFpga, iVer, iRev);


	//Set the custom register( id = A0) setting the frequency
	//on the LED blinker from slowest rate to 16 times slowest rate
	TransportDci& dt = config.Dci0Transport();
	for( uint32 i = 0; i < 4; i++)
	{
		uint32 countval = 1 + i*16;

		printf("Setting Blink Rate Count Value = %d\n", countval);  
		
		dt.SetProperty<uint32>( 0x00, 0xA0, 1 + i*16);
		SLEEP_SEC(5);		
	}

	//Detach from ASR-2300 
	config.Detach();
	return(0);
}

