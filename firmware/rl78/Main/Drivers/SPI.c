// Name:  SPI.c
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

#pragma interrupt INTCSI20 ISR_SPI

/*---- Standard C include files ----*/

/*---- Custom include files ----*/
#include "r_cg_macrodriver.h"
#include "r_cg_serial.h"
#include "r_cg_userdefine.h"
#include "Main.h"
#include "Globals.h"
#include "SPI.h"

#define SPI_USE_GPIO	1		// uncomment to use GPIO instead of SPI peripheral

/*---- static function declarations ----*/
static void	 SPI_UseGPIO( void );
static void  SPI_WriteByte( BYTE Data );
static BYTE  SPI_ReadByte( void );
static void	 SPI_UseGPIOFlash( void );
static void  SPI_WriteByteFlash( BYTE Data );
static BYTE  SPI_ReadByteFlash( void );

/*---- static variable declarations ----*/
int		spi_txLen;
BYTE*	spi_txData;
int		spi_rxLen;
BYTE*	spi_rxData;
BYTE	spi_done;

/*---- global variable declarations ----*/


/****************************************************************

   CALL:    SPI_Init()
                             
   INTRO:   This routine initializes the SPI hardware.
            It should be called before using the SPI functions.

   INPUT:   nothing
   OUTPUT:  nothing

****************************************************************/
void  SPI_Init( void )
{

	/*---- start the SPI peripheral ----*/
#ifndef SPI_USE_GPIO
	R_CSI20_Start();  
#endif
      
}

/****************************************************************

   CALL:    SPI_WriteReadData()
                             
   INTRO:   This routine writes data to the specified device
			using the SPI bus, and then optionally reads data
			from the device.

   INPUT:   Device		- device to read (memory, FPGA, Lime1/2)
			txLen		- lengh of data to write
			txData		- pointer to write data
			rxLen		- lengh of data to read
			rxData		- pointer to result of data
			continue_flag	- TRUE will leave device select low upon exit
   OUTPUT:  0 if no error

****************************************************************/
int  SPI_WriteReadData( BYTE Device, int txLen, BYTE* txData, int rxLen, BYTE* rxData, BOOL continue_flag )
{
int	i;

	/*---- lower appropriate chip select ----*/
	switch( Device ) {
		case SPI_DEVICE_FLASH:
			SPI_FLASH_CS = 0;
			break;
		case SPI_DEVICE_LIME1:
			SPI_RF1_SEL = 0;
			break;
		case SPI_DEVICE_LIME2:
			SPI_RF2_SEL = 0;
			break;
		case SPI_DEVICE_FPGA:
//			SPI_FPGA_XXX = 0;
			break;
	}

	/*---- setup static vars for interrupt routine ----*/
	spi_txLen  = txLen;
	spi_txData = txData;
	spi_rxLen  = rxLen;
	spi_rxData = rxData;

	if( Device == SPI_DEVICE_FLASH ) {

		SPI_UseGPIOFlash();
		spi_txLen = spi_rxLen = 0;

	} else {

#ifdef SPI_USE_GPIO
		SPI_UseGPIO();
		spi_txLen = spi_rxLen = 0;
#else

		if( rxLen ) {
			spi_txLen++;	// when reading data, add 1 dummy write to read first byte
		}
		spi_done = 0;

		/*---- write first byte to device and kickoff interrupt ----*/
	    CSIMK20 = 1U;			// disable INTCSI20 interrupt
	    SIO20 = *txData++;		// started by writing data to SDR[7:0]
	    CSIMK20 = 0U;			// enable INTCSI20 interrupt
		spi_txLen--;

		/*---- wait for interrupt to complete ----*/
		while( !spi_done )
			;
#endif

	}

	/*---- raise chip select ----*/
	if( !continue_flag ) {
		SPI_FLASH_CS = 1;
		SPI_RF1_SEL = 1;
		SPI_RF2_SEL = 1;
//		SPI_FPGA_XXX = 1;
	}

	return( spi_txLen + spi_rxLen );	// return error if any
}

/****************************************************************

   CALL:    ISR_SPI()
                             
   INTRO:   This is the interrupt service routine for the SPI
			serial device - INTCSI10.

   INPUT:   nothing
   OUTPUT:  nothing

****************************************************************/
__interrupt static void ISR_SPI(void)
{
static uint8_t err_type;

	EI();       // allow higher priority interrupts to occur

    err_type = (uint8_t)(SSR10 & _0001_SAU_OVERRUN_ERROR);
    SIR10 = (uint16_t)err_type;		// clear error bit

    if( err_type ) {
		/*---- SPI error - indicate in status ----*/
		if( status.Error[ERROR_SPI] < 255 ) {
			status.Error[ERROR_SPI]++;
		}
		spi_done = 1;				// indicate we're done
    } else {
    
    	if( spi_txLen ) {
    		/*---- send next byte ----*/ 
		    err_type = SIO20;		// do read first to start SPI again
		    SIO20 = *spi_txData++;	// now write next byte
			spi_txLen--;
		} else if( spi_rxLen ) {
			/*---- read next byte ----*/
		    *spi_rxData++ = SIO20;
			spi_rxLen--;
			/*---- if more bytes to read, then send dummy write ----*/
			if( spi_rxLen ) {
			    SIO20 = 0x55;		// dummy write to read next byte
			} else {
				spi_done = 1;		// indicate we're done reading
			}
		} else {
		    err_type = SIO20;		// do read to start SPI again
			spi_done = 1;			// indicate we're done
		}
    }

}



/****************************************************************

   CALL:    SPI_UseGPIO()
                             
   INTRO:   This routine writes the SPI using GPIO.
				spi_txLen  = number of bytes to write
				spi_txData = data to write
				spi_rxLen  = number of bytes to read
				spi_rxData = result of data read

   INPUT:   nothing
   OUTPUT:  nothing

****************************************************************/
static void	SPI_UseGPIO( void )
{
int	i;

	/*---- write data ----*/
	for( i=0 ; i<spi_txLen ; i++ ) {
		SPI_WriteByte( *spi_txData++ );
	}
	/*---- read data ----*/
	for( i=0 ; i<spi_rxLen ; i++ ) {
		*spi_rxData++ = SPI_ReadByte();
	}
}

/****************************************************************

   CALL:    SPI_WriteByte()
                             
   INTRO:   This routine writes data to SPI using GPIO.

   INPUT:   Data       - byte data to write out
   OUTPUT:  nothing

****************************************************************/
static void  SPI_WriteByte( BYTE Data )
{
int	i;

	/*---- shift and write all 8 bits ----*/
	SPI_SCK = 1;				// make sure clock is high first
	for( i=0 ; i<8 ; i++ ) {
		if( Data & 0x80 ) {		// set output based on MSB	
			SPI_DOUT = 1;
		} else {
			SPI_DOUT = 0;
		}
		SPI_SCK = 0;			// now we can lower the clock
		Data <<= 1;				// shift data for next bit
		NOP();
#if 0
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
#endif
		SPI_SCK = 1;			// raise clock to shift in data
	}

}

/****************************************************************

   CALL:    SPI_ReadByte()
                             
   INTRO:   This routine reads data from SPI using GPIO.

   INPUT:   nothing
   OUTPUT:  Data result

****************************************************************/
static BYTE  SPI_ReadByte( void )
{
int	 i;
BYTE result = 0;

	/*---- shift and read all 8 bits ----*/
	SPI_SCK = 1;				// make sure clock is high first
	for( i=0 ; i<8 ; i++ ) {
		SPI_SCK = 0;			// lower clock to allow device to put data on DIN
		result <<= 1;			// shift result to get ready for next bit
		if( SPI_DIN ) {
			result |= 0x01;		// set result bit if DIN is high
		}
		SPI_SCK = 1;			// raise clock to finish bit read
	}

	/*---- return 8-bit result ----*/
	return( result );

}


/****************************************************************

   CALL:    SPI_UseFlashGPIO()
                             
   INTRO:   Use dedicated CLK and DOUT.

****************************************************************/
static void	SPI_UseGPIOFlash( void )
{
int	i;

	/*---- write data ----*/
	for( i=0 ; i<spi_txLen ; i++ ) {
		SPI_WriteByteFlash( *spi_txData++ );
	}
	/*---- read data ----*/
	for( i=0 ; i<spi_rxLen ; i++ ) {
		*spi_rxData++ = SPI_ReadByteFlash();
	}
}

/****************************************************************

   CALL:    SPI_WriteByteFlash()
                             
   INTRO:   Use dedicated CLK and DOUT.

****************************************************************/
static void  SPI_WriteByteFlash( BYTE Data )
{
int	i;

	/*---- shift and write all 8 bits ----*/
	SPI_SCK_FLASH = 1;			// make sure clock is high first
	for( i=0 ; i<8 ; i++ ) {
		if( Data & 0x80 ) {		// set output based on MSB	
			SPI_DOUT_FLASH = 1;
		} else {
			SPI_DOUT_FLASH = 0;
		}
		SPI_SCK_FLASH = 0;		// now we can lower the clock
		Data <<= 1;				// shift data for next bit
		NOP();
#if 0
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
#endif
		SPI_SCK_FLASH = 1;		// raise clock to shift in data
	}

}

/****************************************************************

   CALL:    SPI_ReadByteFlash()
                             
   INTRO:   Use dedicated CLK and DOUT.

****************************************************************/
static BYTE  SPI_ReadByteFlash( void )
{
int	 i;
BYTE result = 0;

	/*---- shift and read all 8 bits ----*/
	SPI_SCK_FLASH = 1;			// make sure clock is high first
	for( i=0 ; i<8 ; i++ ) {
		SPI_SCK_FLASH = 0;		// lower clock to allow device to put data on DIN
		result <<= 1;			// shift result to get ready for next bit
		if( SPI_DIN ) {
			result |= 0x01;		// set result bit if DIN is high
		}
		SPI_SCK_FLASH = 1;		// raise clock to finish bit read
	}

	/*---- return 8-bit result ----*/
	return( result );

}

