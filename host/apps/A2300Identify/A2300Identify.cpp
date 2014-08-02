/** Name: A2300Identify.cpp
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
#include <Windows.h>
#endif

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdexcept>
#include <vector>

#include <A2300/ConfigDevice.h>
using namespace A2300;

int main(int, char**)
{
	printf("*********************************************************************\n"
		   "* ASR-2300 Identification and Version Information Tool \n"
		   "*********************************************************************\n"
		   "*\n"
		   "* This software example provided by Loctronix Corporation (2013) \n"
		   "* www.loctronix.com\n" 
		   "*********************************************************************\n");


	// Find the list of addresses at the specified VID/PID.
	printf ("\n");
	printf ("Enumerating ASR-2300 devices...\n");	 
	printf ("--------------------------------------\n");

	// Get a list of all attached devices.
	std::vector<int> addrs;
	int ctDevices = A2300::UsbDevice::FindAttached(addrs, A2300_VENDOR_ID, A2300_PRODUCT_ID);
	if (ctDevices == 0)
	{
		printf("ERROR: No devices found.\n\n");
		return -1;
	}

	//Identify each attached device.
	try
	{
		// Display devices and properties.
		for( int nn = 0; nn<ctDevices; ++nn)
		{
		  int addr = addrs.at((size_t)nn);
			if( addr >= 0 )
			{
				ConfigDevice config;
				config.Attach(addr);

				//Clear out any previous messages.
				config.Dci0Transport().ClearReceiveQueue();

				//Print out Device information
				std::string sId 		= config.IdentifyDevice();
				std::string sVer 		= config.FirmwareVersion();
				uint16	    idFpga 		= config.FpgaId();
				uint16 		verFpga 	= config.FpgaVersion();
				int  		iVer = (verFpga>>8);
				int	 		iRev = (verFpga& 0x00ff);

				printf("[USB Address: %d] \n", addr);
				printf("  Identity:     %s\n", sId.c_str());
				printf("  CPU FW Ver:   %s\n", sVer.c_str());
				printf("  FX3 FW Ver:   %s\n", "-Not implemented-");
				printf("  FPGA ID-Ver:  %04X - %02X.%02X\n\n", idFpga, iVer, iRev);

				config.Detach();
			}
		}
	}
	catch( std::runtime_error& re)
	{
		printf("ERROR: %s\n", re.what() );
		return -1;
	}
	return(0);
}
