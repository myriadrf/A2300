// Name:  battery.c
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

#include "r_cg_macrodriver.h"
#include "r_cg_serial.h"
#include "r_cg_userdefine.h"

#include "Main.h"
#include "Globals.h"
#include "Timers.h"
#include "i2c.h"
#include "battery.h"

// Battery Register Commands.
#define CMD_STATUS1_REG  (0x05)

//*******************************************************
//Static Data
//*******************************************************
static bool m_isAlive = false;

/**
 * Initialization
 */
void Battery_Init(void)
{
	int iErr;
	byte b;
	// Note: NOP() is used for testing only.
	iErr = i2c_read(SLAVE_ADDR_BATTERY, CMD_STATUS1_REG, 1, &b);
	NOP();
	// Should be 0x7F (reads 0xE1 but probably ok)
	iErr = i2c_read(SLAVE_ADDR_BATTERY, 0x0, 1, &b);
	NOP();
	// Should be 0x63
	iErr = i2c_read(SLAVE_ADDR_BATTERY, 0x1, 1, &b);
	NOP();
	// Should be 0x10
	iErr = i2c_read(SLAVE_ADDR_BATTERY, 0x2, 1, &b);
	NOP();
	// Should be 0x07 (ok for 10kOhm NTC)
	iErr = i2c_read(SLAVE_ADDR_BATTERY, 0x3, 1, &b);
	// Should be 0x8A
	iErr = i2c_read(SLAVE_ADDR_BATTERY, 0x4, 1, &b);
	
	m_isAlive = (bool)(iErr == 0x0);
}

/**
 * Update values
 */
bool Battery_Update()
{
	if( m_isAlive)
	{
		// Should get the current battery voltage level and report
		// Hi, Medium, low, danger.
	}
	return(m_isAlive);
}


