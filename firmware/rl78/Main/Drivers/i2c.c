// Name:  i2c.c
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

//#pragma interrupt INTIICA0 ISR_I2C
//#pragma EI
#pragma NOP

/*---- Standard C include files ----*/

/*---- Custom include files ----*/

#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_serial.h"
#include "Main.h"
#include "Globals.h"
#include<Comm/Dci/WcaMsgs.h>
#include "i2c.h"
#include "Timers.h"   // Timer_Delay().

/*---- static function declarations ----*/
static int  I2C_send_address( uint8_t function, uint8_t slave_addr, uint8_t reg_addr_len, uint8_t reg_addr1, uint8_t reg_addr2 );
static void I2C_Start( void );
static void I2C_Stop( void );
static int  I2C_SendByte( BYTE data );
static byte I2C_ReadByte( int ack_flag );

#define	I2CADDR_WRITE	0x00						// LSB=0 --> write
#define	I2CADDR_READ	0x01						// LSB=1 --> read

/*---- static variable declarations ----*/

/*---- global variable declarations ----*/
// P60 <-> SCLA0  clk
// P61 <-> SDAA0  data
void SETDATA_0(void)
{
	// P61, 0==Output  PORT_OUTPUT
	PM6 = PM6 & ~0x2;  // Clear for output.
	P6 = P6 & ~0x2;    // Clear 61.
}
void SETDATA_1(void)
{
	// P61, 0==Output  PORT_OUTPUT
	PM6 = PM6 & ~0x2;  // Clear for output.
	P6 = P6 | 0x2;    // Set the output bit to 1.
}
byte GETDATA(void)
{
	PM6 = (PM6 |= 0x2);  // Set for input.
	return (P6 & 0x2) >> 1;
}

void SETCLOCK_0(void)
{
	// P60;
	P6 = P6 & ~0x1;  // P60 = 0
}

void SETCLOCK_1()
{
	P6 = P6 | 0x1;	// P60 = 1;
}

/****************************************************************

   CALL:    I2C_Init()
                             
   INTRO:   This routine initializes the I2C hardware.
            It should be called before using the I2C functions.

   INPUT:   nothing
   OUTPUT:  nothing

****************************************************************/
void  I2C_Init( void )
{
	// Set clock (P60) as output.
	PM6 = PM6 & ~0x1;  // Clear for output.
	SETCLOCK_1();
	SETDATA_1();
	I2C_Stop();
      
}

/****************************************************************

   CALL:    i2c_read()
                             
   INTRO:   This routine reads the specified number of bytes
			from the specified device, starting from the specified
			address.

			The read is actually done by performing a write to
			the register address, and then restarting and reading
			from this location.

   INPUT:   slave_addr		- slave address of device to read
            reg_addr		- register address to start read
            length			- number of bytes to read
            data			- destination buffer for read
   OUTPUT:  nothing

****************************************************************/
int i2c_read(uint8_t slave_addr, uint8_t reg_addr, int length, uint8_t *data)
{
int i;
int	ec;

	/*---- send starting address to the device (return if error) ----*/
	ec = I2C_send_address( I2CADDR_READ, slave_addr, 1, reg_addr, 0 );
	if( ec ) {
		return( ec );     /* if error, return nonzero */
	}

	/*---- read all but the last byte ----*/
	for( i=0; i<length-1; i++ ) {
		*data++ = I2C_ReadByte( 0 );
	}

	/*---- don't acknowledge on the last byte ----*/
	*data = I2C_ReadByte( 1 );

	I2C_Stop();

	/*---- done successfully ----*/
	return( 0 );
}

/****************************************************************

   CALL:    i2c_write()
                             
   INTRO:   This routine writes the specified number of bytes
			to the specified device, starting from the specified
			address.

   INPUT:   slave_addr		- slave address of device to write
            reg_addr		- register address to start write
            length			- number of bytes to write
            data			- data to write
   OUTPUT:  nothing

****************************************************************/
int i2c_write(uint8_t slave_addr, uint8_t reg_addr, int length, uint8_t *data)
{
int i;
int ec;

	/*---- send starting address to the EEPROM (return if error) ----*/
	ec = I2C_send_address( I2CADDR_WRITE, slave_addr, 1, reg_addr, 0 );
	if( ec ) {
		return( ec );     /* if error, return nonzero */
	}

	/*-------------------------------*/
	/*  loop for all data to write   */
	/*-------------------------------*/
	for( i=0 ; i<length ; i++ ) {

		/*---- write character into EEPROM ----*/
		if( I2C_SendByte( *data++ ) ) {
			I2C_Stop();
			return( 2 );      /* if error, return nonzero */
		}

	}

	I2C_Stop();
 
	/*---- successful write ----*/
	return( 0 );
}

/****************************************************************

   CALL:    I2C_send_address()

   INTRO:   This routine is used to send the correct
            address bytes to the serial device using the
            I2C bus.  When the function specified is READ,
            then after sending the address bytes (2), a
            stop and start is issued followed by the first
            address byte with read indicated.

            After calling this routine, the data can be
            read or written sequentially from the EEPROM.

   INPUT:   function    	- I2CADDR_WRITE or I2CADDR_READ
            slave_addr  	- device slave address
			reg_addr_len	- number of register addresses to write (1 or 2)
			reg_addr1		- 1st register address
			reg_addr2		- 2nd register address
   OUTPUT:  Error       - error code

****************************************************************/
static   int  I2C_send_address( uint8_t function, uint8_t slave_addr, uint8_t reg_addr_len, uint8_t reg_addr1, uint8_t reg_addr2 )
{

	/*---- send I2C start command ----*/
	I2C_Start();

	/*---- send slave address ----*/
	if( I2C_SendByte(I2CADDR_WRITE|slave_addr) ) {
		I2C_Stop();
		return( 1 );      /* if error, return nonzero */
	}

	/*---- send register address ----*/
	if( I2C_SendByte(reg_addr1) ) {
		I2C_Stop();
		return( 2 );      /* if error, return nonzero */
	}

	if( reg_addr_len==2 ) {
		/*---- send register address ----*/
		if( I2C_SendByte(reg_addr2) ) {
			I2C_Stop();
			return( 3 );  /* if error, return nonzero */
		}
	}

	/*---- if reading, then stop,start,and send address again ----*/
	if( function==I2CADDR_READ ) {
		I2C_Stop();
		I2C_Start();                                 /* Resend start command */
		if( I2C_SendByte(I2CADDR_READ|slave_addr) ) {   /* Resend slave address */
			I2C_Stop();
			return( 4 );   /* if error, return nonzero */
		}
	}

	/*---- done successfully ----*/
	return( 0 );
}



/************************************************************

   CALL:    I2C_Start

   INTRO:   This routine generates the start condition
            on the I2C bus.  This is performed by
            forcing the DATA low while the CLOCK is high.
   
   INPUT:   nothing
   OUTPUT:  nothing

************************************************************/
void  I2C_Start( void )
{

	/*---- force DATA low while CLOCK is high ----*/
	Timer_Delay(3);
	SETDATA_0();
	Timer_Delay(3);

}


/************************************************************

   CALL:    I2C_SendByte

   INTRO:   This routine serially clocks a byte of data
            on the I2C bus.  The bits are sent starting
            with the MSB.

            After sending all 8 bits, an acknowledge
            clock pulse is sent and the data line is
            checked for an acknowledge from the slave
            receiver.  If the acknowledge is received,
            then this routine returns 0, else 1.
   
   INPUT:   data        - data byte to send
   OUTPUT:  0 if no error, non-zero if error

************************************************************/
static   int   I2C_SendByte( BYTE data )
{
BYTE  bit_mask;
int   i;

	bit_mask = 0x80;

	/*---- send all 8 bits, starting with MSB ----*/
	while( bit_mask ) {

		/*---- force CLOCK low ----*/
		SETCLOCK_0();

		if( data & bit_mask ) {
			SETDATA_1();
		} else {
			SETDATA_0();
		}
		bit_mask >>= 1;

		/*---- delay to satisfy 4.7us ----*/
		Timer_Delay(5);

		/*---- force CLOCK high ----*/
		SETCLOCK_1();

		/*---- delay to satisfy 4.7us ----*/
		Timer_Delay(5);
	}

	/*==== get acknowledge from slave ====*/

	/*---- force CLOCK low ----*/
	SETCLOCK_0();
	/*---- let DATA go high ----*/
	SETDATA_1();
	/*---- wait to complete 4.7us ----*/
	Timer_Delay(5);
	/*---- let CLOCK go high ----*/
	SETCLOCK_1();

	/*---- verify that acknowledge is on data ----*/
	for( i=0 ; i<50 ; i++ ) {
		if( GETDATA()==0 ) {
			/*---- indicate no error, and exit loop ----*/
			i = 0;
			break;
		}
	}
	/*---- finish with CLOCK low ----*/
	SETCLOCK_0();

	/*---- return indicating result (0 if no error) ----*/
	return(i);

}


/************************************************************

   CALL:    I2C_ReadByte

   INTRO:   This routine serially reads a byte of data
            on the I2C bus.  The bits are read starting
            with the MSB.

            If this is not the last byte read (determined
            by input parameter) then a positive acknowledge
            is sent to the slave; else a negative 
            acknowledge is sent.
   
   INPUT:   last_byte   - non-zero if last byte to read
   OUTPUT:  data byte

************************************************************/
static byte I2C_ReadByte( int last_byte )
{
byte  result;
int   i;

	/*---- let data line float high ----*/
	SETDATA_1();

	/*---- loop for all 8 bits, shifting data into 'result' ----*/
	for( i=0 ; i<8 ; i++ ) {

		/*---- force CLOCK low ----*/
		SETCLOCK_0();

		/*---- delay to satisfy 4.7us ----*/
		Timer_Delay(5);

		/*---- let CLOCK go high ----*/
		SETCLOCK_1();

		/*---- delay to satisfy 4.7us ----*/
		Timer_Delay(5);

		/*---- process bit ----*/
		result <<= 1;
		if( GETDATA() ) {
			result |= 0x01;
		}

	}


	/*---- force CLOCK low ----*/
	SETCLOCK_0();

	/*---- send acknowledge if not last byte ----*/
	if( !last_byte ) {
		/*---- acknowledge by setting data low ----*/
		SETDATA_0();
	}

	/*---- let CLOCK go high ----*/
	SETCLOCK_1();
	/*---- wait to complete 4.0us ----*/
	Timer_Delay(4);

	/*---- finish with CLOCK low ----*/
	SETCLOCK_0();

	return( result );

}

/************************************************************

   CALL:    I2C_Stop

   INTRO:   This routine sends a stop sequence to the
            slave on the I2C bus.  A stop sequence has
            the data line go high while the clock is
            high.
   
   INPUT:   nothing
   OUTPUT:  nothing

************************************************************/
void  I2C_Stop( void )
{
	/*---- force DATA low while CLOCK is low ----*/
	SETDATA_0();

	/*---- set CLOCK high ----*/
	SETCLOCK_1();

	/*---- wait to complete 4.7us ----*/
	Timer_Delay(5);

	/*---- let DATA go high while CLOCK is high ----*/
	SETDATA_1();
	Timer_Delay(3);
}


