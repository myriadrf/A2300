/* Name: DualRxToFile.cpp
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

#include <errno.h>
#include <limits.h>
#include <math.h>

#if defined(LINUX) || defined(APPLE)
#include <pthread.h>
#include <signal.h>
#include <termios.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#include <stdexcept>
#include <string>
#include <vector>


#include <System/ArgParser.hpp>
#include "DataStreamLogger.h"

using namespace A2300;

/******************************************************************
* Static Data
*****************************************************************/

ARGPARSER_BEGIN_MAP( s_argmap ) 
	ARGPARSER_VAR( "address",   ArgParser::Entry::LONG, "-1", "Device address, default is (-1) meaning it selects first ASR-2300 it finds.")
	ARGPARSER_VAR( "duration",  ArgParser::Entry::LONG, "0",  "Duration of Logging in seconds. Default(0) is infinite.")
ARGPARSER_END_MAP


static bool			s_bRunning = false;
static bool			s_bKeyHit = false;

static ArgParser		s_args;
static ConfigDevice		s_cfgDevice;	
static DataStreamLogger s_s1(4, 1024*64, 16);
static DataStreamLogger s_s2(4, 1024*64, 16);

/******************************************************************
* Forward Declarations.
*****************************************************************/

static int  ReceiveData (size_t msecDur);
static void WriteHeader();
static void PrintUsage();
static void DumpDeviceInformation();

#if defined(LINUX) || defined(APPLE)
static void* KeyThreadFunc (void* arg);
static bool setCbreak (struct termios& saveTermios);
static bool unsetCbreak (struct termios& savedTermios);
#endif


/******************************************************************
* Functions.
*****************************************************************/

/**
* <summary>
* Main Program Entry Point.
* </summary>
*/
int main(int argc, const char** argv) 
{
	int retval = 0;
	WriteHeader();

	//Add all the arguments.
	s_args.AddMapEntries( s_argmap);
	s_s1.RegisterArgs( "s1", s_args);
	s_s2.RegisterArgs( "s2", s_args);

	//Parse the arguments.
	retval = s_args.Parse( argc, argv);
	if( retval > 0)
	{
		int addr = (int) s_args.GetLong( "address");
		//try {
		//	addr = s_cfgDevice.Attach(addr);
		//	printf("Attached to ASR-2300 at address = %d\n", addr);

		//} catch (std::runtime_error& re) {
		//	printf("Error:  %s\n", re.what());
		//	return -1;
		//}

		//// Dump the device information
		//DumpDeviceInformation();

		//Initialize the stream loggers.
		retval = s_s1.Init( s_args, &s_cfgDevice);
		if( retval == 0)
			retval = s_s2.Init( s_args, &s_cfgDevice);
	
		//Receive the data and store to disk.
		if( retval == 0)
			retval = ReceiveData((size_t) (s_args.GetDouble("duration")*1000.0));

		s_s1.Terminate();
		s_s2.Terminate();
		s_cfgDevice.Detach();
	}
	else //Arguments were not right.
	{
		PrintUsage();
	}
	return retval;
}

/**
* <summary>
* Function configures the ASR-2300 to receive a specified frequency
* and then streams the data to the host via BulkDataPort interface.
* </summary>
*/
static int ReceiveData (size_t msecDur)
{
	printf( msecDur == 0 ? "Duration: inifinite\n" : "Duration: %lu sec.\n",
		msecDur);

	printf("\n--> Starting Run\n");
	printf("Hit any key to stop data collection ...\n");

	// start a separate thread waiting on any keyboard entry
#if defined(LINUX) || defined(APPLE)
	pthread_t keyThread;
	if (pthread_create (&keyThread, NULL, KeyThreadFunc, NULL) != 0) {
		printf ("Error creating key entry thread; use ^C to exit data connection if necessary.\n");
	}
#endif

	// Periodically poll and check status of processing.
	int retval = 0;
	while ((retval == 0) && (!s_bKeyHit))
	{
#if defined(HAVE_LIBUSB)
		retval= device.PollAsynchronousEvents();
#elif defined(WIN32) || defined(WIN64)
		Sleep(10);
#endif
		//Check status of processing.
		if( !retval)
			retval = s_s1.CheckStatus();
		if( !retval)
			retval = s_s2.CheckStatus();
	}

	printf("\n--> Completed Run\n");

	// join the keyboard entry thread
#if defined(LINUX) || defined(APPLE)
	if (!s_bKeyHit) {
		pthread_kill (keyThread, SIGINT);
	}
	pthread_join (keyThread, NULL);
#endif

	return 0;
}





/**
* <summary>
* Writes the application header information to the standard output.
* </summary>
*/

static void  WriteHeader() {
	printf(
		"*********************************************************************\n"
		"* Dual RxToFile\n"
		"*********************************************************************\n"
		"* This software example provided by Loctronix Corporation (2014) \n"
		"* www.loctronix.com\n"
		"*********************************************************************\n");
}

/**
* <summary>
* Prints usage of this program, on command line parsing error.
* </summary>
*/
static void PrintUsage() {
	printf( "\nUsage for DualRxToFile:\n\n"
		"  DualRxToFile <s1.filename> <s2.filename> [[<var>=<value>] ...]\n\n");

	s_args.WriteDescriptions();
}

/**
* <summary>
* Print out Device information.
* </summary>
*/

static void DumpDeviceInformation ()
{
	std::string sId = s_cfgDevice.IdentifyDevice();
	std::string sVer = s_cfgDevice.FirmwareVersion(0);
	uint16 idFpga = s_cfgDevice.FpgaId();
	uint16 verFpga = s_cfgDevice.FpgaVersion();
	int iVer = (verFpga >> 8);
	int iRev = (verFpga & 0x00ff);

	printf("\n");
	printf("Identity:    %s\n", sId.c_str());
	printf("FW Ver:      %s\n", sVer.c_str());
	printf("FPGA ID-Ver: %04X-%02X.%02X\n\n", idFpga, iVer, iRev);
}

#if defined(LINUX) || defined(APPLE)
/**
* <summary>
* Thread to get key hit, unbuffered but blocking, to stop data collection
* </summary>
*/
static void* KeyThreadFunc (void* /* arg */)
{
	struct termios saveTermios;
	setCbreak (saveTermios);
	s_bKeyHit = false;
	getchar();
	unsetCbreak (saveTermios);
	s_bKeyHit = true;
	return NULL;
}
/**
* <summary>
* Set terminal character mode to unbuffered, read at will; returns
* true on success, false on failure (for any reason).
* </summary>
*/
static bool setCbreak (struct termios& saveTermios)
{

	// Remove line buffering from stdin
	if (setvbuf (stdin, NULL, _IONBF, 0) != 0) {
		return false;
	}

	// Store previous terminal settings
	if (tcgetattr (0, &saveTermios)) {
		return false;
	}

	// Modify terminal settings, using a copy of the previous settings
	struct termios buf = saveTermios;
	buf.c_lflag &= ~(ECHO|ICANON);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	// Set new terminal settings
	if (tcsetattr (0, TCSAFLUSH, &buf) < 0) {
		return false;
	}
	return true;
}

static bool unsetCbreak (struct termios& savedTermios)
{
	// Return line buffering to stdin
	if (setvbuf (stdin, NULL, _IOLBF, 0) != 0) {
		return false;
	}

	// Return original terminal settings
	if (tcsetattr (0, TCSAFLUSH, &savedTermios) < 0) {
		return false;
	}
	return true;
}
#endif
