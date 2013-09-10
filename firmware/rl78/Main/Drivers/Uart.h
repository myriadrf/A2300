// Name:  Uart.h
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

#ifndef  _UART_
#define  _UART_

/*---- buffer defines ----*/
#define  RXBFRSIZE            128
#define  RXBFRMASK            (RXBFRSIZE-1)
#define  TXBFRSIZE            512
#define  TXBFRMASK            (TXBFRSIZE-1)

// Circular Ring Buffer to Contain Read/Write data.
typedef struct Uart_Config
{
	unsigned RxHead;
	unsigned RxTail;
	unsigned TxHead;
	unsigned TxTail;
	byte RxBuffer[RXBFRSIZE];
	byte TxBuffer[TXBFRSIZE];
}
Uart_Config;

// Expose Uart.c declarations for configurations.
extern Uart_Config Uart_0;  
extern Uart_Config Uart_1;

// Prototypes
void Uart_Init();
int  Uart_Read(byte* ptr, int readSize, Uart_Config *pUart);
int  Uart_Write( byte* msg, int len, Uart_Config *pUart);

#endif
