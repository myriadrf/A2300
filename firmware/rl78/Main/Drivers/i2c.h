// Name:  i2c.h
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

#ifndef I2C_H
#define I2C_H

// Error Codes.
enum I2C_ERROR_TYPES
{
	I2C_ERROR_NOACK = 1,
	I2C_ERROR_WRITEDATAACK
};

// I2C Device Address
#define	SLAVE_ADDR_BMP180	0xEE
#define SLAVE_ADDR_MOTION   0xD0
#define SLAVE_ADDR_BATTERY  0x8C

// Function Prototypes
void I2C_Init( void );
int  i2c_read(uint8_t slave_addr, uint8_t reg_addr, int length, uint8_t *data);
int  i2c_write(uint8_t slave_addr, uint8_t reg_addr, int length, uint8_t *data);
#endif /* I2C_H */
