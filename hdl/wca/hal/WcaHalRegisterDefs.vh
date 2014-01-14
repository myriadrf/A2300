// Name:  WcaHalRegisterDefs.vh
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

//*******************************************************************
//*******************************************************************
//WCA HAL ASR-2300 Registers (Component ID = B0h)		 
//*******************************************************************
//*******************************************************************

//Implement Mater control register.
//	Core configuration register.  
//
// bit#  |  Description
//-------|----------------------------------------------------------
//	  0		ResetRf0 - Performs a reset on RF0.
//	  1		ResetRf1 - Performs a reset on RF1.
//   2      ResetContainer - Performs a reset on HalContainer functions.
//   3      ResetUsbIF - Performs a reset on the USB Interface.  Holding high places
//				the interface in an idle state (disabled).
`define WCAHAL_MSTR_CTRL   8'hB0 

// Interrupt Control Registers.
// 
//  WCAHAL_EVENT is a pair of read and write registers that controls the event interrupt functions of the FPGA.
//  up to eight different events are supported and the user must read the event register in order to determine
//  which event was generated.  The events can be enabled and disabled by setting the appropriate bits.
//  An event generates an interrupt, which is handled accordingly then event should be cleared by writing a 0 and 1 to the event mask register.
//  
//  MASK WRITE REGISTER( WriteByteRegister)  
//    Write to this register to set the Event mask.  Register is also used to 
// 	clear an event by writing a 0 and 1 to the respective event bit.
//
// bit#  |  Description
//-------|----------------------------------------------------------
//   0     Enable / Disable Event #0
//   1     Enable / Disable Event #1
//   2     Enable / Disable Event #2
//   3     Enable / Disable Event #3
//   4     Enable / Disable Event #4
//   5     Enable / Disable Event #5
//   6     Enable / Disable Event #6
//   7     Enable / Disable Event #7
// 
//  STATUS READ REGISTER( ReadByteRegister)  
//    Read this register to get the Event status.  
// bit#  |  Description
//-------|----------------------------------------------------------
//   0     Event #0 Triggered
//   1     Event #1 Triggered
//   2     Event #2 Triggered
//   3     Event #3 Triggered
//   4     Event #4 Triggered
//   5     Event #5 Triggered
//   6     Event #6 Triggered
//   7     Event #7 Triggered
`define WCAHAL_EVENT          8'hB1


`define WCAHAL_RESERVED1                8'hB3 //Reserved.




//*******************************************************************
// LIME RF#0 interface.
//*******************************************************************

// WCAHAL_RF0_CTRL (WcaWriteByteRegister)
// bit#  |  Description
// -------|----------------------------------------------------------
//  0-1     RX Input Mode
//           0 = tx_iq loopback 
//           1 = rf_rxdata input (dc bias removed).
//	     		 2 = rf_rxdata input.
//	     		 3 = Fixed Test Pattern  i = 256, q = 256; 
//  2-3     TX Ouput Mode
//           0 = rf_rxdata loopback 
//           1 = rf_txdata input.
//	     		 2 = Fixed Test Pattern  i = 256, q = 256; 
//	     		 3 = Fixed Test Pattern  i = 0, q = 0;
//  4       "rf_rxen" output line - set to 1 to enable, 0 to disable. Enables disables the 
//          Lime chip #1 receive ADC function. 
//
//  5       "rf_txen" output pin state - set to 1 to enable, 0 to disable  Enables / disables the 
//          Lime chip #1 transmit DAC function. 
//
//  6       "rf_rxclk" enable (1) / (0).  If enabled the rf_rxclk pin will turn on and provide clock
//          signals to the receive functions of the Lime chips.  
//
//  7       "rf_txclk" enable (1) / (0).  If enabled the tx_rxclk pin will turn on and provide clock
//          signals to the transmit functions of Lime chip #1.  If disabled, clock output will be low.
`define WCAHAL_RF0_CTRL               8'hB6

// WCAHAL_RF0_RSSI (WcaReadWordRegister)
// I and Q RSSI values truncated to the 8 high order bits.  RSSI is the average absolute value 
// of the received data over the sampled interval.   24 bit counter is used so sampling interval should be
// around (~0.3 msec).  Register contains both the I and Q RSSI values as follows:
// bit#  |  Description
//-------|----------------------------------------------------------
//  0-7  |   In phase RSSI values (0 - 256) low 4 bits are not returned since
//       |   it assumed gain will always be turned up enough to hit at least 4 bits of the
//       |   the system.
// 8-15  |   Quadrature phase RSSI values 0 - 256) low 4 bits are not returned since
//       |   it assumed gain will always be turned up enough to hit at least 4 bits of the
//       |   the system.
`define WCAHAL_RF0_RSSI               8'hB7 

// WCAHAL_RF0_BIAS (WcaReadDwordRegister)
// In phase and quadrature ADC Bias Read register. Implements 24 bit integrator to compute the average bias value over 
// 4096 samples returns the high 16 bits indicating the average bias value over the period. 
// Further LPF in the microcontroller may be required to get longer term average. Returns both in phase 
// and quadrature values { bias_i, bias_q } as a single 32 bit value.
`define WCAHAL_RF0_BIAS              8'hB8 




//*******************************************************************
// LIME RF#1 interface.
//*******************************************************************

// WCAHAL_RF1_CTRL (WcaWriteByteRegister)
// bit#  |  Description
// -------|----------------------------------------------------------
//  0-1     RX Input Mode
//           0 = tx_iq loopback 
//           1 = rf_rxdata input (dc bias removed).
//	          2 = rf_rxdata input.
//				 3 = Fixed Test Pattern  i = 256, q = 256; 
//  2-3     TX Ouput Mode
//           0 = rf_rxdata loopback 
//           1 = rf_txdata input.
//	        	 2 = Fixed Test Pattern  i = 256, q = 256; 
//			    3 = Fixed Test Pattern  i = 0, q = 0;
//  4       "rf_rxen" output line - set to 1 to enable, 0 to disable. Enables disables the 
//          Lime chip #1 receive ADC function. 
//
//  5       "rf_txen" output pin state - set to 1 to enable, 0 to disable  Enables / disables the 
//          Lime chip #1 transmit DAC function. 
//
//  6       "rf_rxclk" enable (1) / (0).  If enabled the rf_rxclk pin will turn on and provide clock
//          signals to the receive functions of the Lime chips.  
//
//  7       "rf_txclk" enable (1) / (0).  If enabled the tx_rxclk pin will turn on and provide clock
//          signals to the transmit functions of Lime chip #1.  If disabled, clock output will be low.
`define WCAHAL_RF1_CTRL               8'hB9

// WCAHAL_RF1_RSSI (WcaReadWordRegister)
// I and Q RSSI values truncated to the 8 high order bits.  RSSI is the average absolute value 
// of the received data over the sampled interval.   24 bit counter is used so sampling interval should be
// around (~0.3 msec).  Register contains both the I and Q RSSI values as follows:
// bit#  |  Description
//-------|----------------------------------------------------------
//  0-7  |   In phase RSSI values (0 - 256) low 4 bits are not returned since
//       |   it assumed gain will always be turned up enough to hit at least 4 bits of the
//       |   the system.
// 8-15  |   Quadrature phase RSSI values 0 - 256) low 4 bits are not returned since
//       |   it assumed gain will always be turned up enough to hit at least 4 bits of the
//       |   the system.
`define WCAHAL_RF1_RSSI               8'hBA
 
// WCAHAL_RF1_BIAS (WcaReadDwordRegister)
// In phase and quadrature ADC Bias Read register. Implements 24 bit integrator to compute the average bias value over 
// 4096 samples returns the high 16 bits indicating the average bias value over the period. 
// Further LPF in the microcontroller may be required to get longer term average. Returns both in phase 
// and quadrature values { bias_i, bias_q } as a single 32 bit value.
`define WCAHAL_RF1_BIAS              8'hBB

//*******************************************************************
//*******************************************************************
//DEFAULT WAVEFORM ASR-2300 Registers (Component ID = 0)		 
//*******************************************************************
//*******************************************************************

//*******************************************************************
// RF I/O Selection and Configuration Register
//*******************************************************************

// WCAHAL_IDWAVEFORM ( WcaReadWordRegister)
`define WCAHAL_IDWAVEFORM	8'h1

// WCAHAL_VERSION ( WcaReadWordRegister)
`define WCAHAL_VERSION		8'h2

// WCAHAL_PORTCAPS ( WcaReadWordRegister)
// Read only property specifies WCA port capabilities. This property indicates which host output (FPGA read) 
// and host input (FPGA write) ports are active. The lower eight bits specify the eight possible host output (FPGA read) ports, 
// the upper eight bits specify the eight possible host input (FPGA write) ports. Ports are numbered by the 
// bit position. For example: bit0 = port0 (read), bit9 = port9 (write).
`define WCAHAL_PORTCAPS		8'h3


// WCAHAL_RFSELECT (WcaWriteByteRegister)
// bit#  |  Description
// -------|----------------------------------------------------------
//  0	        GPS 3.3V DC Bias selection set to 1 to enable, 0 to disable active antenna feature.
//  1  		GPS external input selection: set to 1 for external input, 0 for internal antenna input.
//  2	      	ISM band external input selection: set to 1 for external input, 0 for internal antenna input.
//  3		ISM band transmit selection: set to 1 to transmit, set to 0 receive.  ISM band path only 
//		supports half duplex operation.
//  4		Wideband RF #1 RX receive select: set to 1 to receive, set to 0 to transmit.  Provides
//		half-duplex operation on the Wideband RF#1 external RF I/O connector.
//  5		Wideband RF #2 RX receive select: set to 1 to receive, set to 0 to transmit.  Provides
//		half-duplex operation on the Wideband RF#2 external RF I/O connector.
`define WCAHAL_RFCONFIG    8'h4


//*******************************************************************
// LED Control Register
//*******************************************************************

// WCAHAL_LED (WcaWriteByteRegister)
// bit#  |  Description
// -------|----------------------------------------------------------
//  0	      	Turns on (1) and off (0) Led_0 
//  1  		Turns on (1) and off (0) Led_1 
//  2	      	Turns on (1) and off (0) Led_2 
//  3	 	Turns on (1) and off (0) Led_3
`define WCAHAL_LED		8'h5


//*******************************************************************
//*******************************************************************
//DEFAULT WAVEFORM HARDWARE COMPONENT REGISTERS		 
//*******************************************************************
//*******************************************************************



//*******************************************************************
//Standard Register definitions.
//*******************************************************************

//  WCAHAL_EVENT MAPPINGS The following events are used 
//  by this component. WCAHAL_EVENT masks and status 
//  registers are configured in accordance with this specification.
//  
// bit#  |  Description
//-------|----------------------------------------------------------
//   0     LSDP0  TX FIFO almost empty
//   1     LSDP1  RX FIFO almost full
//   2     LSDP2  TX FIFO almost empty
//   3     LSDP3  RX FIFO almost full
//   4     FIFO Overflow / Underflow Error, Check the WCAHAL_LSDP_STATUS register for specific fifo.
//   5     - not nassigned -
//   6     - not nassigned -
//   7     - not nassigned -
// 


// WCAHAL_LSDP_CONTROL (WcaWriteByteRegister)
// Sets the configuration and state of low speed data port FIFOS.
// bit#  |  Description
// -------|----------------------------------------------------------
//		0	 		LSDP0 reset FIFO 	(TX) toggle 1 then 0 to reset.
//		1	 		LSDP1 reset FIFO	(RX) toggle 1 then 0 to reset.
//		2	 		LSDP2 reset FIFO	(TX) toggle 1 then 0 to reset.
//		3	 		LSDP3 reset FIFO	(RX) toggle 1 then 0 to reset.
//		4  		LSDP0 Enable		(TX) 1(enable) / 0 (disable)
//		5	 		LSDP1 Enable		(RX) 1(enable) / 0 (disable)
//		6	 		LSDP2 Enable		(TX) 1(enable) / 0 (disable)
//		7	 		LSDP3 Enable		(RX) 1(enable) / 0 (disable)
`define WCAHAL_LSDP_CONTROL	8'h7

// WCAHAL_LSDP_STATUS (WcaReadByteRegister)
// Reads the current status of low speed data port FIFOS.
// bit#  |  Description
// -------|----------------------------------------------------------
//		0	 		LSDP0 Underflow 	(TX)
//		1	 		LSDP0 Overflow		(TX)
//		2	 		LSDP1 Underflow	(RX)
//		3	 		LSDP1 Overflow		(RX)
//		4  		LSDP2 Underflow	(TX)
//		5	 		LSDP2 Overflow		(TX)
//		6	 		LSDP3 Underflow	(RX)
//		7	 		LSDP3 Overflow		(RX)
`define WCAHAL_LSDP_STATUS	8'h8

// WCAHAL_LSDP_RATE (WcaWriteDwordRegister)
// Low Speed Data Rate Divisor(max 24 bits unsigned).
// The data rate = dataclock / <WCAHAL_LSDP_RATE> .
`define WCAHAL_LSDP_RATE	8'h9		

`define WCAHAL_LSDP0_FIFO   8'hA   	// Low Speed Data Port #0 (TX)
`define WCAHAL_LSDP1_FIFO   8'hB		// Low Speed Data Port #1 (RX)
`define WCAHAL_LSDP2_FIFO   8'hC		// Low Speed Data Port #2 (TX)
`define WCAHAL_LSDP3_FIFO   8'hD		// Low Speed Data Port #3 (RX)



