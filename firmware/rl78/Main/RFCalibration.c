// Name:  RFCalibration.c
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


/*---- Custom include files ----*/
#include "r_cg_macrodriver.h"
#include "r_cg_wdt.h"
#include "r_cg_userdefine.h"

#include "Main.h"
#include "Timers.h"

#include "RFProfiles.h"
#include "RFCalibration.h"
#include "NVM.h"
#include "System/DataTypes.h"

/*---- Standard C include files ----*/
#include <string.h>

/*---- static function declarations ----*/

/*---- static variable declarations ----*/
#define MAX_CPATHS 12
#define MAX_CPATH_PARAMETERS  6
#define MAX_CALIBRATION_DATA (MAX_CPATH_PARAMETERS * MAX_CPATHS )

/**
* Calibration Data structure stored in non-volatile flash ram.
*/
typedef struct RF_CalibrationData_tag
{
	uint16 			chksum;
	byte   			sizePathData [MAX_CPATHS];
	RF_LimeRegData  data[MAX_CALIBRATION_DATA];
} RF_CalibrationData;

uint16 CalcChecksum( RF_CalibrationData *pcalib);

static RF_CalibrationData s_calib;

//***********************************************************
//API Implementation.
//***********************************************************

/**
* Initializes RF Calibration data by loading from flash and validating checksum.
*/
int RF_CalibrationInit()
{
	int retval = RF_LoadCalibrationDataFromFlash( (byte*) &s_calib, sizeof( s_calib)) == 0;
		
	if( retval == 0)
	{
		//Validate the checksum.
		int chksum = CalcChecksum( &s_calib);
		if( chksum != s_calib.chksum)
			return -1; 
	}
}

/**
* Loads the calibration Data into RAM. 
*/
int RF_LoadCalibrationDataFromFlash(byte* pMem, int maxbytes)
{
	memcpy( pMem, (BYTE*)NVM_CAL_STARTADDRESS, maxbytes );
	return 0;
}


/**
* Save calibration Data to NVM
*/
int RF_SaveCalibrationDataToFlash(byte* pMem, int maxbytes)
{
int ec;

	ec = NVM_EraseBlock( NVM_CAL_BLOCK );
	if( !ec ) {
		/*---- write calibration data to flash ----*/
		ec = NVM_Write( NVM_CAL_POOLADDRESS, maxbytes, (DWORD*)pMem );
	}
	return ec;	
}

/**
* Gets the specified path calibration values.
*/
int RF_LookUpCalibrationValues( int device, enum RF_CalibrationPaths_Enum cpath, int ctMaxRegisters, RF_LimeRegData* pRegDat)
{
	//Get the specified calibration valus and coppy them.
	int idx = MAX_CPATH_PARAMETERS* cpath;
	int size = s_calib.sizePathData[ cpath];
	size = (size > ctMaxRegisters) ? ctMaxRegisters : size;
	memcpy( pRegDat, s_calib.data + idx, size); 
	return size;
}


/** 
* Sets the specified path calibration values.
*/
int RF_UpdateCalibrationValues( int device, enum RF_CalibrationPaths_Enum cpath, int ctRegisters, RF_LimeRegData* pRegDat)
{
	int idx = MAX_CPATH_PARAMETERS* cpath;
	int size = (ctRegisters < MAX_CPATH_PARAMETERS) ? ctRegisters : MAX_CPATH_PARAMETERS;
	s_calib.sizePathData[ cpath] = (byte) size;
	memcpy( s_calib.data + idx, pRegDat, size); 
	return size;
}


/**
* Method applies the specified calibration
*/
int RF_ApplyCalibration( int device, enum RF_CalibrationPaths_Enum cpath )
{
	RF_LimeRegData rd[MAX_CPATH_PARAMETERS];
	int size = RF_LookUpCalibrationValues( device, cpath, MAX_CPATH_PARAMETERS, rd);

	//Write the calibration values to the Lime chip.
	//TODO
	return 0;
}


/** 
* Performs a RF calibration on the specified path and saves to flash if specified, otherwise calibration held in memory only.
*/
int RF_CalibratePath( int device, enum RF_CalibrationPaths_Enum cpath, bool bSaveToFlash)
{
	RF_LimeRegData rd[MAX_CPATH_PARAMETERS];
	//Apply the calibration values.
	//int size = RF_LookUpCalibrationValues( device, cpath, MAX_CPATH_PARAMETERS, rd);
	int size = 0;
	int retval = 0;
	//Update local calibration values.
	RF_UpdateCalibrationValues( device, cpath, size, rd);
	
	
	if( bSaveToFlash)
	{
		s_calib.chksum = CalcChecksum( &s_calib);
		retval = RF_SaveCalibrationDataToFlash((byte*) &s_calib, sizeof( RF_CalibrationData) );
	}
	return retval;
}

/** 
* Performs a RF calibration on all paths and saves to flash if specified, otherwise calibration held in memory only.
*/
int RF_CalibrateAllPaths(bool bSaveToFlash)
{
	int idxDevice, idx;
	int retval = 0;
	for( idxDevice = 0; idxDevice < 2 && retval == 0; idxDevice++)
	{
		for( idx = 0; idx < 6 && retval == 0; idx++)
		{
			bool bSave = (bool) (bSaveToFlash && idxDevice == 1 && idx == 5);
			retval = RF_CalibratePath( idxDevice, (enum RF_CalibrationPaths_Enum) idx, bSave);
		}
	}
	return retval;
}


//***********************************************************
//API Implementation.
//***********************************************************

/**
* Helper function calcuates a checksum on
*/
uint16 CalcChecksum( RF_CalibrationData* pcalib)
{
	return 0;
}
