/************************************************************************
// Name:  motion.c
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

 #pragma interrupt INTP6 ISR_MOTION_SENSOR
 #pragma inline(inv_row_2_scale)
 
/*---- Custom include files ----*/
#include "r_cg_macrodriver.h"
#include "r_cg_serial.h"
#include "r_cg_userdefine.h"

#include <Comm/Dci/DciConversation.h>
#include <Comm/Dci/StandardMsgs.h>
#include <Comm/Dci/InfrastructureMsgs.h>
#include <Comm/Dci/WcaMsgs.h>

#include "Main.h"
#include "Globals.h"
#include "Timers.h"
#include "i2c.h"
#include "motion.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"

#include <string.h>  // memset()

//Macro enable/disable cpu_interrupt in the RL78
#define DISABLE_INT6 PMK6 = 1U; PIF6 = 0U;
#define ENABLE_INT6  PIF6 = 0U; PMK6 = 0U; 

// Sensor Information.
#define CMD_WHO_AMI     (0x75)
#define CMD_PWR_MGMT_1  (0x6B)
#define CMD_PWR_MGMT_2  (0x6C)

// Defines - Motion Sensor.
#define DEFAULT_MPU_HZ  (100)		/* Starting sampling rate. */
#define ACCEL_ON        (0x01)
#define GYRO_ON         (0x02)

#define PRINT_ACCEL     (0x01)		/* Data requested by client. */
#define PRINT_GYRO      (0x02)
#define PRINT_QUAT      (0x04)

//*******************************************************
//Internal Module Structures and Function Prototypes
//*******************************************************
// These structures are part of the Motion Sensor processing interface.
struct rx_s {
    unsigned char header[3];
    unsigned char cmd;
};
struct hal_s {
    unsigned char sensors;
    unsigned char dmp_on;
    unsigned char wait_for_tap;
    volatile unsigned char new_gyro;
    unsigned short report;
    unsigned short dmp_features;
    unsigned char motion_int_mode;
    struct rx_s rx;
};

enum packet_type_e {
    PACKET_TYPE_ACCEL,
    PACKET_TYPE_GYRO,
    PACKET_TYPE_QUAT,
    PACKET_TYPE_TAP,
    PACKET_TYPE_ANDROID_ORIENT,
    PACKET_TYPE_PEDO,
    PACKET_TYPE_MISC
};

// Local Prototypes.
static void run_self_test(void);
static void update_packet(char packet_type, void *data);
static unsigned short inv_orientation_matrix_to_scalar(const signed char *mtx);
static unsigned short inv_row_2_scale(const signed char *row);

//*******************************************************
//Static Data
//*******************************************************
/* The sensors can be mounted onto the board in any orientation. The mounting
 * matrix seen below tells the MPL how to rotate the raw data from thei
 * driver(s).
 * TODO: The following matrices refer to the configuration on an internal test
 * board at Invensense. If needed, please modify the matrices to match the
 * chip-to-body matrix for your particular set up.
 */
static signed char gyro_orientation[9] = {-1, 0, 0,
                                           0,-1, 0,
                                           0, 0, 1};
										   
// Declarations for Motion Sensor Processing Interface.
static struct hal_s hal = {0};
static unsigned short gyro_rate = 0x0;
static unsigned short gyro_fsr  = 0x0;
static unsigned char accel_fsr  = 0x0;

static bool m_isAlive = false;

// Allow interrupt to service multiple callbacks.
typedef void (*SensorCB)(void);
static SensorCB s_callback[2] = {NULL,NULL};
static int s_ctCallback = 0;

/* Every time new gyro data is available, this function is called in an
 * ISR context. In this example, it sets a flag protecting the FIFO read
 * function.
 */
static void android_orient_cb(unsigned char orientation)
{
    update_packet(PACKET_TYPE_ANDROID_ORIENT, &orientation);
}
static void gyro_data_ready_cb(void)
{
    hal.new_gyro = 1;
}
static void tap_cb(unsigned char direction, unsigned char count)
{
    char data[2];
    data[0] = (char)direction;
    data[1] = (char)count;
    update_packet(PACKET_TYPE_TAP, data);
}
/*
** Keep a callback list when the interrupt fires.
*/
void sensor_board_irq_connect(unsigned short pin, void (*cb)(void), void *arg)
{
   s_callback[s_ctCallback] = cb;
   s_ctCallback ++;   
}

/****************************************************************
   CALL:    Motion_Init()
                             
   INTRO:   Module Initializations.
****************************************************************/
void Motion_Init(void)
{
	int iErr;
	byte data;
	struct int_param_s int_param;
	
	mpu_init_structures();
	
	int_param.cb = gyro_data_ready_cb;
	int_param.pin = P6 + 0x10;  // 10000 = P64
	if( mpu_init(&int_param) != 0)
	{
		m_isAlive = false;
		return;
	}
	
	// If you're not using an MPU9150 AND you're not using DMP features, this
    // function will place all slaves on the primary bus.
	//mpu_set_bypass(0);   // Loctronix.
	
    /* Get/set hardware configuration. Start gyro. */
    /* Wake up all sensors. */
    mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    
	/* Push both gyro and accel data into the FIFO. */
    mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    mpu_set_sample_rate(DEFAULT_MPU_HZ);
	
    /* Read back configuration in case it was set improperly. */
    mpu_get_sample_rate(&gyro_rate);
    mpu_get_gyro_fsr(&gyro_fsr);
    mpu_get_accel_fsr(&accel_fsr);
    
	// Low Pass Filter ?
	// mpu_set_lpf(1);
    
	/* Initialize HAL state variables. */
    memset(&hal, 0, sizeof(hal));
    hal.sensors = ACCEL_ON | GYRO_ON;
    hal.report = PRINT_QUAT;
	
	    /* To initialize the DMP:
     * 1. Call dmp_load_motion_driver_firmware(). This pushes the DMP image in
     *    inv_mpu_dmp_motion_driver.h into the MPU memory.
     * 2. Push the gyro and accel orientation matrix to the DMP.
     * 3. Register gesture callbacks. Don't worry, these callbacks won't be
     *    executed unless the corresponding feature is enabled.
     * 4. Call dmp_enable_feature(mask) to enable different features.
     * 5. Call dmp_set_fifo_rate(freq) to select a DMP output rate.
     * 6. Call any feature-specific control functions.
     *
     * To enable the DMP, just call mpu_set_dmp_state(1). This function can
     * be called repeatedly to enable and disable the DMP at runtime.
     *
     * The following is a short summary of the features supported in the DMP
     * image provided in inv_mpu_dmp_motion_driver.c:
     * DMP_FEATURE_LP_QUAT: Generate a gyro-only quaternion on the DMP at
     * 200Hz. Integrating the gyro data at higher rates reduces numerical
     * errors (compared to integration on the MCU at a lower sampling rate).
     * DMP_FEATURE_6X_LP_QUAT: Generate a gyro/accel quaternion on the DMP at
     * 200Hz. Cannot be used in combination with DMP_FEATURE_LP_QUAT.
     * DMP_FEATURE_TAP: Detect taps along the X, Y, and Z axes.
     * DMP_FEATURE_ANDROID_ORIENT: Google's screen rotation algorithm. Triggers
     * an event at the four orientations where the screen should rotate.
     * DMP_FEATURE_GYRO_CAL: Calibrates the gyro data after eight seconds of
     * no motion.
     * DMP_FEATURE_SEND_RAW_ACCEL: Add raw accelerometer data to the FIFO.
     * DMP_FEATURE_SEND_RAW_GYRO: Add raw gyro data to the FIFO.
     * DMP_FEATURE_SEND_CAL_GYRO: Add calibrated gyro data to the FIFO. Cannot
     * be used in combination with DMP_FEATURE_SEND_RAW_GYRO.
     */
    dmp_load_motion_driver_firmware();
    dmp_set_orientation(
        inv_orientation_matrix_to_scalar(gyro_orientation));
    dmp_register_tap_cb(tap_cb);
    dmp_register_android_orient_cb(android_orient_cb);
    hal.dmp_features = DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
        DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
        DMP_FEATURE_GYRO_CAL;
    dmp_enable_feature(hal.dmp_features);
    dmp_set_fifo_rate(DEFAULT_MPU_HZ);
	
	mpu_set_dmp_state(1);
    hal.dmp_on = 1;
	
	m_isAlive = true;
	ENABLE_INT6;
	
	run_self_test();
	
	return;
}

/****************************************************************
   CALL:    Motion_Update()
                             
   INTRO:   Update sensor readings. Save in local table.
****************************************************************/
bool Motion_Update(Dci_Property *motionProp)
{
	long rawQuanternion[3];
	short rawMag[3], rawGyro[3], rawAccel[3];
	unsigned long timestamp;
	int ec;
	short sensors;
	byte more;
	
	// Check for initialized / failed sensor.
	if( !m_isAlive )
		return(false);
		
	// Get Sensor data.
	//mpu_read_fifo(gyro, accel, &timestamp, &sensors, &more);
	ec = dmp_read_fifo(rawGyro, rawAccel, rawQuanternion, &timestamp, &sensors, &more);
	if( ec )
		return(false);
			  
	if( !more)
	{
		hal.new_gyro = 0;
	}	
    if (sensors & INV_XYZ_GYRO && hal.report & PRINT_GYRO)
	{
		motionProp[0].value.vUint32 = timestamp;
		motionProp[1].value.vInt16 = rawGyro[0];
		motionProp[2].value.vInt16 = rawGyro[1];
		motionProp[3].value.vInt16 = rawGyro[2];
	}
    if (sensors & INV_XYZ_ACCEL && hal.report & PRINT_ACCEL)
	{
		motionProp[4].value.vUint32 = timestamp;
		motionProp[5].value.vInt16 = rawAccel[0];
		motionProp[6].value.vInt16 = rawAccel[1];
		motionProp[7].value.vInt16 = rawAccel[2];
	}
	
	// Get Raw Compass Data (not saved in fifo).
	ec = mpu_get_compass_reg(rawMag, &timestamp);
	if( ec == 0)
	{
		motionProp[8].value.vUint32 = timestamp;
		motionProp[9].value.vInt16  = rawMag[0];
		motionProp[10].value.vInt16 = rawMag[1];
		motionProp[11].value.vInt16 = rawMag[2];
	}
	return( ec == 0);
}

void update_packet(char packet_type, void *data)
{
	/* CJC: TODO
	
	if (packet_type == PACKET_TYPE_ACCEL || packet_type == PACKET_TYPE_GYRO) {
        short *sdata = (short*)data;
		motionProp[1].value.vInt16 = sdata[0];
		motionProp[2].value.vInt16 = sdata[1];
		motionProp[3].value.vInt16 = sdata[2];
    } else if (packet_type == PACKET_TYPE_QUAT) {
        long *ldata = (long*)data;
        buf[2] = (char)(ldata[0] >> 24);
        buf[3] = (char)(ldata[0] >> 16);
        buf[4] = (char)(ldata[0] >> 8);
        buf[5] = (char)ldata[0];
        buf[6] = (char)(ldata[1] >> 24);
        buf[7] = (char)(ldata[1] >> 16);
        buf[8] = (char)(ldata[1] >> 8);
        buf[9] = (char)ldata[1];
        buf[10] = (char)(ldata[2] >> 24);
        buf[11] = (char)(ldata[2] >> 16);
        buf[12] = (char)(ldata[2] >> 8);
        buf[13] = (char)ldata[2];
        buf[14] = (char)(ldata[3] >> 24);
        buf[15] = (char)(ldata[3] >> 16);
        buf[16] = (char)(ldata[3] >> 8);
        buf[17] = (char)ldata[3];
        length = 18;
    } else if (packet_type == PACKET_TYPE_TAP) {
        buf[2] = ((char*)data)[0];
        buf[3] = ((char*)data)[1];
        length = 4;
    } else if (packet_type == PACKET_TYPE_ANDROID_ORIENT) {
        buf[2] = ((char*)data)[0];
        length = 3;
    } else if (packet_type == PACKET_TYPE_PEDO) {
        long *ldata = (long*)data;
        buf[2] = (char)(ldata[0] >> 24);
        buf[3] = (char)(ldata[0] >> 16);
        buf[4] = (char)(ldata[0] >> 8);
        buf[5] = (char)ldata[0];
        buf[6] = (char)(ldata[1] >> 24);
        buf[7] = (char)(ldata[1] >> 16);
        buf[8] = (char)(ldata[1] >> 8);
        buf[9] = (char)ldata[1];
        length = 10;
    } else if (packet_type == PACKET_TYPE_MISC) {
        buf[2] = ((char*)data)[0];
        buf[3] = ((char*)data)[1];
        buf[4] = ((char*)data)[2];
        buf[5] = ((char*)data)[3];
        length = 6;
    }
	*/
}

/* These next two functions converts the orientation matrix (see
 * gyro_orientation) to a scalar representation for use by the DMP.
 * NOTE: These functions are borrowed from Invensense's MPL.
 */
static unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}

static unsigned short inv_orientation_matrix_to_scalar(
    const signed char *mtx)
{
    unsigned short scalar;

    /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */

    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;


    return scalar;
}


static void run_self_test(void)
{
    int result;
    char test_packet[4] = {0};
    long gyro[3], accel[3];

    result = mpu_run_self_test(gyro, accel);
    if (result == 0x7) {
        /* Test passed. We can trust the gyro data here, so let's push it down
         * to the DMP.
         */
        float sens;
        unsigned short accel_sens;
        mpu_get_gyro_sens(&sens);
        gyro[0] = (long)(gyro[0] * sens);
        gyro[1] = (long)(gyro[1] * sens);
        gyro[2] = (long)(gyro[2] * sens);
        dmp_set_gyro_bias(gyro);
        mpu_get_accel_sens(&accel_sens);
        accel[0] *= accel_sens;
        accel[1] *= accel_sens;
        accel[2] *= accel_sens;
        dmp_set_accel_bias(accel);
    }

    /* Report results. */
    test_packet[0] = 't';
    test_packet[1] = (char)result;
    //CJC send_packet(PACKET_TYPE_MISC, test_packet);
}


__interrupt static void ISR_MOTION_SENSOR(void)
{
	int i;

	EI();  // allow higher priority interrupts to occur
	

	for(i=0; i<s_ctCallback; i++)
	{
		if( s_callback[i] != NULL)
			s_callback[i];
	}
}
