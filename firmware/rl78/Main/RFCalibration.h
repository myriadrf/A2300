// Name:  RFCalibration.h
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

#ifndef	_RFCALIBRATION_
#define	_RFCALIBRATION

#include "RFProfiles.h"

//******************************
// RF Amplifier Configuration
//******************************

/**
* Enum defines the 6 possible calibration pathes for each Lime chip.
*/
enum RF_CalibrationPaths_Enum
{
	RF_CPATH_RXA = 0x0,
	RF_CPATH_RXB = 0x1,
	RF_CPATH_RXC = 0x2,
	RF_CPATH_TXA = 0x3,
	RF_CPATH_TXB = 0x4,
	RF_CPATH_TXC = 0x5
};


/**
* Initializes RF Calibration data by loading from flash and validating checksum.
*/
int RF_CalibrationInit();


/**
* Loads the calibration Data into RAM. 
*/
int RF_LoadCalibrationDataFromFlash(byte* pMem, int maxbytes);

/**
* Save calibration Data to NVM
*/
int RF_SaveCalibrationDataToFlash(byte* pMem, int maxbytes);

/**
* Gets the specified path calibration values.
*/
int RF_LookUpCalibrationValues( int device, enum RF_CalibrationPaths_Enum cpath, int ctMaxRegisters, RF_LimeRegData *pRegDat);

/** 
* Sets the specified path calibration values.
*/
int RF_UpdateCalibrationValues( int device, enum RF_CalibrationPaths_Enum cpath, int ctRegisters, RF_LimeRegData *pRegDat);

/**
* Method applies the specified calibration
*/
int RF_ApplyCalibration( int device, enum RF_CalibrationPaths_Enum cpath );

/** 
* Performs a RF calibration on the specified path and saves to flash if specified, otherwise calibration held in memory only.
*/
int RF_CalibratePath( int device, enum RF_CalibrationPaths_Enum cpath, bool bSaveToFlash);

/** 
* Performs a RF calibration on all paths and saves to flash if specified, otherwise calibration held in memory only.
*/
int RF_CalibrateAllPaths(bool bSaveToFlash);



#endif

