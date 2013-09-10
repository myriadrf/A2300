// Name:  pressure.c
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


/*---- Standard C include files ----*/

/*---- Custom include files ----*/
#include "r_cg_macrodriver.h"
#include "r_cg_serial.h"
#include "r_cg_userdefine.h"

#include "Main.h"
#include "Globals.h"
#include "Timers.h"
#include "i2c.h"
#include "Pressure.h"

/*---- define the BMP-183 registers ----*/
#define	BMPREG_OUT_XLSB			0xF8
#define	BMPREG_OUT_LSB			0xF7
#define	BMPREG_OUT_MSB			0xF6
#define	BMPREG_CTRL_MEAS		0xF4
#define	BMPREG_SOFT_RESET		0xE0
#define	BMPREG_ID				0xD0
#define	BMPREG_CALIB_START		0xAA

// Define the Oversampling Rate on the BMP183.
enum OversampleRate
{
	Oversample_Off = 0,
	Oversample_1x = 1,
	Oversample_2x = 2,
	Oversample_4x = 3,
	Oversample_8x = 4
};

/*---- static function declarations ----*/
static int   read_calibration( void );
static short calc_temperature( unsigned long ut );
static long  calc_pressure(unsigned long up);

static int GetPressure( long *pPressure );
static int GetTemperature( short *pTempC );

/*---- static variable declarations ----*/
static int s_dataTable[Pressure_TSize] = { 0x0, 0x0, 0x0 };

static long	param_b5;
static int		number_of_samples;
static short	s_oversampling_setting = Oversample_Off;
static bmp180_calibration_param_t cal_param;

/****************************************************************

   CALL:    Pressure_Init()
                             
   INTRO:   This routine initializes the Bosch BMP183 pressure
            sensor.

   INPUT:   nothing
   OUTPUT:  nothing

****************************************************************/
void  Pressure_Init( void )
{
	int iErr;	
	short tempC;
	BYTE b;

	// If set to 0xB6, will perform the same sequence as power on reset.
	//b = 0xB6;
	//I2C_WriteData( SLAVE_ADDR_BMP180, BMPREG_SOFT_RESET, 1, &b);
	
	/*---- verify that pressure sensor can be read ----*/
	iErr = i2c_read( SLAVE_ADDR_BMP180, BMPREG_ID, 1, &b );
	if( iErr || (b != 0x55) )
	{
		/*---- indicate an error in the status ----*/
		if( status.Error[ERROR_PRESSURE] < 255 ) {
			status.Error[ERROR_PRESSURE]++;
		}
	}
#ifdef CJC_HACK
	/*---- configure oversampling rate ----*/
	s_oversampling_setting = Oversample_Off;  // 0x00;

	/*---- read all calibration parameters ----*/
	iErr = read_calibration();
	if( iErr == 0x0)
	{
		/*---- make sure to read temp first to initialize temp variable used in pressure calibration ----*/
		GetTemperature(&tempC);
		tempC = (tempC + 5)/ 10;  // Round
	}
#endif
}

/****************************************************************
   CALL:    Pressure_Update()
                             
   INTRO:   Update (Refresh) Sensor Readings
****************************************************************/
void Pressure_Update()
{
	long  pressure = 0L;
	short tempC = 0;
	
	s_dataTable[Pressure_Time]++;   // Simple counter for now.
	if( GetPressure(&pressure) == 0x0 )
		s_dataTable[Pressure_Pressure] = (int)pressure;	
	if( GetTemperature(&tempC) == 0x0 )
		s_dataTable[Pressure_Temperature] = (int)tempC;
}

/****************************************************************

   CALL:    GetPressure()
                             
   INTRO:   This routine returns the calibrated pressure reading.

   INPUT:   nothing
   OUTPUT:  Pressure
   Return:  0x0 if no error (<0 = Error).

****************************************************************/
int GetPressure( long *pPressure )
{
int		i;
int     iErr;
BYTE	b;
BYTE	data[3];
long	up;

	/*---- start sampling ----*/
	b = (BYTE)((s_oversampling_setting << 6) + 0x34);
	iErr = i2c_write( SLAVE_ADDR_BMP180, BMPREG_CTRL_MEAS, 1, &b );
	if( iErr == 0x0 )
	{
		/*---- wait until complete ----*/
		for( i=0 ; i<10000 && (iErr = 0x0); i++ ) {
			iErr = i2c_read( SLAVE_ADDR_BMP180, BMPREG_CTRL_MEAS, 1, &b );
			if( !(b & 0x20) ) {
				break;
			}
		}
	}
	
	/*---- if no error, read value ----*/
	if( (iErr == 0x0) && (i<10000) )
	{
		iErr = i2c_read( SLAVE_ADDR_BMP180, BMPREG_OUT_MSB, 3, data );
		up = (((unsigned long) data[0] << 16) 
		    | ((unsigned long) data[1] << 8) 
			|  (unsigned long) data[2]) >> (8-s_oversampling_setting);
	}
	if( iErr )
	{
		/*---- indicate an error in the status ----*/
		if( status.Error[ERROR_PRESSURE] < 255 ) {
			status.Error[ERROR_PRESSURE]++;
		}
		return( -1 );
	}

	/*---- calibrate reading ----*/
	*pPressure = calc_pressure(up);
	return(0);
}

/****************************************************************

   CALL:    GetTemperature()
                             
   INTRO:   This routine returns the calibrated temp reading.

   INPUT:   nothing
   OUTPUT:  nothing

****************************************************************/
int GetTemperature( short *pTempC )
{
int		i;
int     iErr;
BYTE	b;
BYTE	data[2];
WORD	ut;

	/*---- start sampling ----*/
	b = (BYTE)((s_oversampling_setting << 6) + 0x2e);
	iErr = i2c_write( SLAVE_ADDR_BMP180, BMPREG_CTRL_MEAS, 1, &b );

	/*---- wait until complete ----*/
	for( i=0 ; (i<10000) && (iErr == 0x0); i++ ) {
		iErr = i2c_read( SLAVE_ADDR_BMP180, BMPREG_CTRL_MEAS, 1, &b );
		if( !(b & 0x20) ) {
			break;
		}
	}

	/*---- if no error, read value ----*/
	if( (iErr == 0x0) && (i<10000) ) {
		iErr = i2c_read( SLAVE_ADDR_BMP180, BMPREG_OUT_MSB, 2, data );
		ut = (data[0] << 8) | data[1];
	}
	
	if( iErr )
	{
		/*---- indicate an error in the status ----*/
		if( status.Error[ERROR_PRESSURE] < 255 ) {
			status.Error[ERROR_PRESSURE]++;
		}
		return( -1 );
	}

	/*---- calibrate reading ----*/
	*pTempC = calc_temperature((DWORD)ut);
	return(0);
}


/****************************************************************

   CALL:    read_calibration()
                             
   INTRO:   This routine reads the calibration parameters used
   			to calculate the temperature and pressure.  This 
			shoudl be called on power up.

			This routine is written by Bosch.

   INPUT:   nothing
   OUTPUT:  nothing

****************************************************************/
static int read_calibration( void )
{
	int iErr;
	unsigned char data[22];

	/*---- read out the cal data ----*/
	iErr = i2c_read( SLAVE_ADDR_BMP180, BMPREG_CALIB_START, 22, data );
	if( iErr == 0x0 )
	{
		/*parameters AC1-AC6*/
		cal_param.ac1 =  (data[0] << 8) | data[1];
		cal_param.ac2 =  (data[2] << 8) | data[3];
		cal_param.ac3 =  (data[4] << 8) | data[5];
		cal_param.ac4 =  (data[6] << 8) | data[7];
		cal_param.ac5 =  (data[8] << 8) | data[9];
		cal_param.ac6 = (data[10] << 8) | data[11];

		/*parameters B1,B2*/
		cal_param.b1 =  (data[12] << 8) | data[13];
		cal_param.b2 =  (data[14] << 8) | data[15];

		/*parameters MB,MC,MD*/
		cal_param.mb =  (data[16] << 8) | data[17];
		cal_param.mc =  (data[18] << 8) | data[19];
		cal_param.md =  (data[20] << 8) | data[21];
	}
	return(iErr);
}

/****************************************************************

   CALL:    calc_temperature()
                             
   INTRO:   This routine returns the calibrated temperature.
			This routine is written by Bosch.

   INPUT:   ut		- temp reading from BMP183
   OUTPUT:  calibrated temperature (in degC * 10)

****************************************************************/
static short calc_temperature( unsigned long ut )
{
short temperature;
long x1, x2;

	x1 = (((long)ut - (long)cal_param.ac6) * (long)cal_param.ac5) >> 15;
	x2 = ((long)cal_param.mc << 11) / (x1 + cal_param.md);
	param_b5 = x1 + x2;
	temperature = (short)((param_b5 + 8) >> 4);
	return temperature;
}

/****************************************************************

   CALL:    calc_pressure()
                             
   INTRO:   This routine returns the calibrated pressure.
			This routine is written by Bosch.

   INPUT:   up		- pressure reading from BMP183
   OUTPUT:  calibrated pressure (Pa)

****************************************************************/
static long calc_pressure(unsigned long up)
{
   long pressure, x1, x2, x3, b3, b6;
   unsigned long b4, b7;

   b6 = param_b5 - 4000;
   /*****calculate B3************/
   x1 = (b6*b6) >> 12;
   x1 *= cal_param.b2;
   x1 >>= 11;

   x2 = (cal_param.ac2*b6);
   x2 >>= 11;

   x3 = x1 + x2;

   b3 = (((((long)cal_param.ac1)*4 + x3) << s_oversampling_setting)+2) >> 2;

   /*****calculate B4************/
   x1 = (cal_param.ac3 * b6) >> 13;
   x2 = (cal_param.b1 * ((b6*b6) >> 12)) >> 16;
   x3 = ((x1 + x2) + 2) >> 2;
   b4 = (cal_param.ac4 * (unsigned long) (x3 + 32768)) >> 15;

   b7 = ((unsigned long)(up - b3) * (50000>>s_oversampling_setting));
	if (b7 < 0x80000000)
		pressure = (b7 << 1) / b4;
	else
		pressure = (b7 / b4) << 1;

   x1 = pressure >> 8;
   x1 *= x1;
   x1 = (x1 * PARAM_MG) >> 16;
   x2 = (pressure * PARAM_MH) >> 16;
   pressure += (x1 + x2 + PARAM_MI) >> 4;/* pressure in Pa*/
   return pressure;
}

/****************************************************************
   CALL:    Pressure_GetValue()
                             
   INTRO:   Get the "last" updated pressure sensor reading.
****************************************************************/
bool Pressure_GetValue(PressureId id, int32 *pValue)
{
	bool bRet = false;
	if( ((uint)id < Pressure_TSize) &&		// Valid Range
		(s_dataTable[id] != 0) )	// 0x0 = Value Error.
	{
		*pValue = s_dataTable[id];
		bRet = true;
	}
	return(bRet);
}
