// Name:  Uart.c
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

// Uart-0
#pragma interrupt INTST0 ISR_UART0_TxD
#pragma interrupt INTSR0 ISR_UART0_RxD
#pragma interrupt INTSRE0 ISR_UART0_Error
// Uart-1
#pragma interrupt INTST1 ISR_UART1_TxD
#pragma interrupt INTSR1 ISR_UART1_RxD
#pragma interrupt INTSRE1 ISR_UART1_Error
#pragma EI

/*---- determine how to compile ----*/
#include "Compile.h"

/*---- Standard C include files ----*/

/*---- Custom include files ----*/
#include "r_cg_macrodriver.h"
#include "r_cg_serial.h"
#include "r_cg_userdefine.h"
#include "Main.h"
#include "Globals.h"
#include "Uart.h"

// Macros to Enable/Disable Uart Interrupts
#define INTR_DISABLE(A) ( ((A)==(&Uart_0))?(STMK0=1U):(STMK1=1U) )
#define INTR_ENABLE(A)  ( ((A)==(&Uart_0))?(STMK0=0U):(STMK1=0U) )
// Macro to set Transmit TX register
#define SET_TXD(A,B)    ( ((A)==(&Uart_0))?(TXD0=(B)):(TXD1=(B)) )

/*---- static variable declarations ----*/
Uart_Config Uart_0;
Uart_Config Uart_1;

/*---- local prototypes ----*/
static void UART_CheckTxStart( Uart_Config* pUart);

/****************************************************************
   CALL:    Uart_Config_Init()
                             
   INTRO:   Initialize the Uart configuration structure.
****************************************************************/
static void Uart_Config_Init( Uart_Config* pUart )
{
	/*---- reset head/tail pointers ----*/
	pUart->RxHead = pUart->RxTail = 0;
	pUart->TxHead = pUart->TxTail = 0;
}

/****************************************************************

   CALL:    Uart_Init()
                             
   INTRO:   This routine initializes the UART hardware.
            It should be called before using the UART functions.

   INPUT:   nothing
   OUTPUT:  nothing

****************************************************************/
void  Uart_Init( void )
{
	//Set up the UART buffers
	Uart_Config_Init( &Uart_0 );
	Uart_Config_Init( &Uart_1 );
	
   /*---- start UART ----*/
	R_UART0_Start();
	R_UART1_Start();
}

/****************************************************************

   CALL:    Uart_OnUpdate()
                             
   INTRO:   This routine is called regularly from the main
			event loop.  It just checks to see if the transmitter
			needs to be kick started.

   INPUT:   pUart - pointer to UART0 or UART1
   OUTPUT:  nothing

****************************************************************/
void Uart_OnUpdate( Uart_Config *pUart )
{
   /*---- see if we need to kick start the Tx data ----*/
   UART_CheckTxStart(pUart);
}

/****************************************************************

   CALL:    Uart_Read()
                             
   INTRO:   This routine 

   INPUT:   ptr			- pointer to destination for data
			readSize	- max size to read
			pUart		- pointer to UART0 or UART1
   OUTPUT:  ctRead		- number of bytes read from port

****************************************************************/
int Uart_Read(byte* ptr, int readSize, Uart_Config *pUart)
{
	int ctRead = 0;   // Number of bytes Read.
	WORD RxTail;
		
	/*---- first check if there's a character in the buffer ----*/
	RxTail = pUart->RxTail;
	while( RxTail != pUart->RxHead ) 
	{
		/*---- get next byte from circular buffer ----*/
		*ptr++ = pUart->RxBuffer[RxTail];
		RxTail = (RxTail+1) & RXBFRMASK;

		/*---- bail if we reach max bytes allowed ----*/
		if( ++ctRead >= readSize ) {
			break;
		}
	}
	pUart->RxTail = RxTail;

	return( ctRead );		// return how many bytes added to buffer
}

/****************************************************************

   CALL:    Uart_Write()
                             
   INTRO:   This routine writes a DCI message to the UART.

   INPUT:   msg			- pointer to data to write
            len			- length of msg in bytes
			pUart		- pointer to UART0 or UART1
   OUTPUT:  size		- number of bytes written to port

****************************************************************/
int Uart_Write( byte* msg, int len, Uart_Config *pUart)
{
	int i;
    int ctWrite = 0;   // Number of Bytes Written.
	WORD TxHeadNext;
		
	/*---- send packet data (if any) ----*/
    INTR_DISABLE(pUart);	/* disable INTST interrupt */
	for(i=0; i<len; i++)
	{
		/*---- if buffer is full, indicate error ----*/
		TxHeadNext = (pUart->TxHead+1) & TXBFRMASK;
		if( TxHeadNext == pUart->TxTail ) {
			if( status.Error[ERROR_UART_TXOVERFLOW] < 255 ) {
				status.Error[ERROR_UART_TXOVERFLOW]++;
			}
			break;
		} else {
			/*---- save character in buffer and increment head pointer ----*/
			pUart->TxBuffer[pUart->TxHead] = msg[i];
			pUart->TxHead = TxHeadNext;
			++ctWrite;
		}
	}
	INTR_ENABLE(pUart);    /* enable INTST interrupt */

	/*---- see if we need to kick start the transmitter ----*/
	UART_CheckTxStart(pUart);

	return( ctWrite );		// return the number of bytes written to port

}


/****************************************************************

   CALL:    UART_CheckTxStart()
                             
   INTRO:   This routine checks to see if the UART transmitter
            is inactive and there's data in the Tx buffer.  If
            so, it will start up the transmitter.

            This routine should be called in the main event
            loop or whenever a set of data it put into the 
            transmit buffer.

   INPUT:   pUart: Pointer to Uart[0,1]
   OUTPUT:  nothing

****************************************************************/
static void UART_CheckTxStart( Uart_Config *pUart)
{
   	// Quickly check for idle.
	int busy = (pUart == &Uart_0) ? (SSR00 & _0040_SAU_UNDER_EXECUTE)
							      : (SSR02 & _0040_SAU_UNDER_EXECUTE);
							 
	/*---- check if transmitter is idle ----*/
	if( busy==0 ) {
		/*---- if there's data in the buffer, then kick start it! ----*/
   	    if( pUart->TxTail != pUart->TxHead )
		{
			/*---- get next byte from circular buffer and write to UART Tx ----*/
			INTR_DISABLE(pUart);             // disable INTST0 interrupt first
			SET_TXD( pUart, pUart->TxBuffer[pUart->TxTail]); 
			pUart->TxTail = (pUart->TxTail+1) & TXBFRMASK;
	        INTR_ENABLE(pUart);             // enable INTST0 interrupt
	   }
   }

}

/****************************************************************

   CALL:    ISR_UART0_RxD()
                             
   INTRO:   This routine is the interrupt service routine for
            the UART receiver.

            The received character is saved in the circular
            buffer 'RxBuffer[]'.  The head pointer is also
            incremented.

   INPUT:   nothing
   OUTPUT:  nothing

****************************************************************/
__interrupt static void ISR_UART0_RxD(void)
{
	static BYTE rx_data;
	WORD RxHeadNext;
	EI();       // allow higher priority interrupts to occur

	rx_data = RXD0;

	/*---- if buffer is full, indicate error ----*/
	RxHeadNext = (Uart_0.RxHead+1) & RXBFRMASK;
	if( RxHeadNext == Uart_0.RxTail ) {
		if( status.Error[ERROR_UART_RXOVERFLOW] < 255 ) {
			status.Error[ERROR_UART_RXOVERFLOW]++;
		}
	} else {
		/*---- save character in buffer and increment head pointer ----*/
		Uart_0.RxBuffer[Uart_0.RxHead] = rx_data;
		Uart_0.RxHead = RxHeadNext;
	}
}

/****************************************************************
   CALL:    ISR_UART1_RxD()          
   INTRO:   Interrupt service routine for the UART receiver.
****************************************************************/
__interrupt static void ISR_UART1_RxD(void)
{
	static BYTE rx_data;
	WORD RxHeadNext;

	EI();       // allow higher priority interrupts to occur
	rx_data = RXD1;
	
	/*---- if buffer is full, indicate error ----*/
	RxHeadNext = (Uart_1.RxHead+1) & RXBFRMASK;
	if( RxHeadNext == Uart_1.RxTail ) {
		if( status.Error[ERROR_UART_RXOVERFLOW] < 255 ) {
			status.Error[ERROR_UART_RXOVERFLOW]++;
		}
	} else {
		/*---- save character in buffer and increment head pointer ----*/
		Uart_1.RxBuffer[Uart_1.RxHead] = rx_data;
		Uart_1.RxHead = RxHeadNext;
	}
}

/****************************************************************

   CALL:    ISR_UART0_Error()
                             
   INTRO:   This routine is the interrupt service routine for
            a UART0 error.

   INPUT:   nothing
   OUTPUT:  nothing

****************************************************************/
__interrupt static void ISR_UART0_Error(void)
{
static BYTE rx_data;
static WORD rx_error;

	EI();       // allow higher priority interrupts to occur

	rx_data = RXD0;
	rx_error = SSR01 & 0x7;
	SIR01L = 0x7;

	if( status.Error[ERROR_UART_PARITYFRAMING] < 255 ) {
		status.Error[ERROR_UART_PARITYFRAMING]++;
	}
}

/****************************************************************
   CALL:    ISR_UART1_Error()          
   INTRO:   Interrupt service routine for the UART errors.
****************************************************************/
__interrupt static void ISR_UART1_Error(void)
{
static BYTE rx_data;
static WORD rx_error;

	EI();       // allow higher priority interrupts to occur
	rx_data = RXD1;
	rx_error = SSR03 & 0x7;
	SIR03L = 0x7;

	if( status.Error[ERROR_UART_PARITYFRAMING] < 255 ) {
		status.Error[ERROR_UART_PARITYFRAMING]++;
	}
}

/****************************************************************

   CALL:    ISR_UART0_TxD()
                             
   INTRO:   This routine is the interrupt service routine for
            the UART transmitter.

   INPUT:   nothing
   OUTPUT:  nothing

****************************************************************/
__interrupt static void ISR_UART0_TxD(void)
{
	EI();       // allow higher priority interrupts to occur

	if( Uart_0.TxTail != Uart_0.TxHead ) {
		/*---- get next byte from circular buffer ----*/
		TXD0 = Uart_0.TxBuffer[Uart_0.TxTail];
		Uart_0.TxTail = (Uart_0.TxTail+1) & TXBFRMASK;
	}
}

/****************************************************************
   CALL:    ISR_UART1_TxD()          
   INTRO:   Interrupt service routine for the UART transmitter.
****************************************************************/
__interrupt static void ISR_UART1_TxD(void)
{
	EI();       // allow higher priority interrupts to occur

	if( Uart_1.TxTail != Uart_1.TxHead ) {
		/*---- get next byte from circular buffer ----*/
		TXD1 = Uart_1.TxBuffer[Uart_1.TxTail];
		Uart_1.TxTail = (Uart_1.TxTail+1) & TXBFRMASK;
	}
}




