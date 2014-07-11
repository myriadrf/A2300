* Name:  2ChRx_Readme.txt
* 
* Copyright(c) 2013 - 2014 Loctronix Corporation
* http://www.loctronix.com
* 
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License
*  as published by the Free Software Foundation; either version 2
*  of the License, or (at your option) any later version.
* 
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*  GNU General Public License for more details.
*
* Description:
*  Standard Two Channel Receiver (transmitter functions disabled).
* 
*  Logic file contains WCA HAL Compliant component implementing a basic 2 channel 
*  receiver function with configurable down sampling of baseband data streams.
*
*  HAL COMPONENT IDENTIFIER:  	h0001
*  VERSION:			h0111
*  PORTCAPS:			h0300  -- 2 Receive, 0 Transmit Ports.
*  Version Date:		7/11/2014
*
********************************************************************************************************************
Documentation:


In addition to the standard registers for the ASR-2300 WCA HAL.  

See WcaHal definition headers for details.


********************************************************************************************************************
VERSION NOTES:

** Ver 1, rev. 17 (h0111 - 7/11/14)
1)  Added rising edge detection on lime IF aclr (Master Control Bit 4, ClearRf0Rf1) to tighten 
    up timing between data and dsp clock domains. This may improve stability.

** Ver 1, rev. 16 (h0110 - 7/8/14)
1)  Fixed bug introduced in last revision, which had removed the synch enable capability for some
    reason.  It was restored to implementation in rev 8.  All other changes from rev. 9 remain.


** Ver 1, rev. 9 (h0108 - 7/6/14)
1) Added synchronous RF clear option in the Master Control Register to clear the internal state
   of both Lime RF interfaces. The master control register has been updated as follows:

	 bit#  |  Description
	-------|----------------------------------------------------------
	  0	 ResetRf0 - Performs a reset on RF0.
	  1	 ResetRf1 - Performs a reset on RF1.
	  2      ResetContainer - Performs a reset on HalContainer functions.
	  3      ResetUsbIF - Performs a reset on the USB Interface.  Holding high places
		 the interface in an idle state (disabled).
	  4      ClearRf0Rf1 - Clears state of RF0 and RF1 without changing configuration
		 This synchronizes internal counters in order to eliminate interchannel biases.

** Ver 1, rev. 8 (h0108 - 7/6/14)
1) Added synchronous clear mode capability to synchronize RX0 and RX1 data streams.  Added 7th bit
   in DDUC control register to enable synchronous operation. See 2ChRxRegisterDefs.vh for detailed specification
   of DDUC control register.   Note that this capability has not been added to TX functionality yet. DDUC control   
   register has following spec.  

	 bit#  |  Description
	 -------|----------------------------------------------------------
	  0	  (1)Enable / (0)Disable Component USB port streaming.
	  1	  Reset FIFOS. 
	  2	  Bypass Cordic rotation (DYNAMIC_CONFIG only)
	  3	  Bypass CIC Decimation  (DYNAMIC_CONFIG only)
	  4	  Bypass CIC Decimation strobe (DYNAMIC_CONFIG only)
		  When enabled, the decimation strobe is bypassed, and the original 
		  sampling strobe is used to drive down stream.  
	  5       Bypass Half band filter (DYNAMIC_CONFIG only).  
	  6       Synch mode enable,  if true, synchronizes reset of fifos with other
	 	  DDUC components that have synch enabled.   This eliminates interchannel
  		  biases, or at least makes then constant.  



** Ver 1, rev. 7 (h0107 - 6/25/14)
1) Synchronized Open source receiver version with Loctronix standard default build.


** Ver 1, rev. 6 (h0106 - 11/25/13)
1) Implemented DC Bias removal functions inside the lime IF.  These are on by default, and can be bypassed by setting 
   the RF(N) control register. WCA HAL component (0x00),properties WCAHAL_RF0_CTRL (06h) / WCAHAL_RF0_CTRL (09h) register 
   spec :

    bit#  |  Description
   -------|----------------------------------------------------------
    0-1     RX Input Mode
             0 = tx_iq loopback 
             1 = rf_rxdata input (dc bias removed).
             2 = rf_rxdata input.
             3 = Fixed Test Pattern  i = 256, q = 256; 
    2-3     TX Ouput Mode
             0 = rf_rxdata loopback;
             1 = rf_txdata input;
             2 = Fixed Test Pattern  i = 256, q = 256; 
             3 = Fixed Test Pattern  i = 0, q = 0;
     4      "rf_rxen" output line - set to 1 to enable, 0 to disable. Enables disables the 
            Lime chip #1 receive ADC function. 
  
     5      "rf_txen" output pin state - set to 1 to enable, 0 to disable  Enables / disables the 
            Lime chip #1 transmit DAC function. 
  
     6      "rf_rxclk" enable (1) / (0).  If enabled the rf_rxclk pin will turn on and provide clock
            signals to the receive functions of the Lime chips.  
  
     7      "rf_txclk" enable (1) / (0).  If enabled the tx_rxclk pin will turn on and provide clock
            signals to the transmit functions of Lime chip #1.  If disabled, clock output will be low.


** Ver 1, rev. 5 (h0105 - 11/9/13)
1) Added dynamic bypass configuration of DDC cordic, cic, or hbf. Enables user to bypass the internal functions
   by setting the DDC(0,1) configuration registers according to the following table.
  
   DDC / DUC component Control register.  Bit flags 2 through 5 are
   only availble if the waveform was compiled with dynamic configuration
   enabling bypass of internal functions.  To achieve maximum 32 MHz sampling
   rate bypass CIC decimation, CIC strobe, and half band filters (0x39, 57 dec.).  To achieve 
   16 MHz sampling rate,  bypass CIC functions  (0x19, 25 dec.).  To directly sample front-end
   with decimation sampling rate, bypass decimation, but use sampling strobe (0x2D, 45 dec. )
    
   bit#  |  Description
   -------|----------------------------------------------------------
    0	   (1)Enable / (0)Disable Component USB port streaming.
    1	   Reset FIFOS. 
    2	   Bypass Cordic rotation (DYNAMIC_CONFIG only)
    3	   Bypass CIC Decimation  (DYNAMIC_CONFIG only)
    4	   Bypass CIC Decimation strobe (DYNAMIC_CONFIG only)
  	   When enabled, the decimation strobe is bypassed, and the original 
  	   sampling strobe is used to drive down stream.  
    5      Bypass Half band filter (DYNAMIC_CONFIG only).  

2) Turned on both DDC ports (ep1 = 0x88, ep3 = 0x98) for streaming RX0 and RX1 data.
3) Validated speed up to the limit of USB 2.0.

** Ver 1, rev. 4 (h0104 - 11/4/13)
1) Fixed address mode on Port 1 (RX0) only. 
2) Added in DDC component control registers to manage reset and enable/disable of component functions.
   When disabled, information will not stream.  Reseting will clear buffers.
