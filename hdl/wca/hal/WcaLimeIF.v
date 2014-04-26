// Name:  WcaLimeIF.v
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
module WcaLimeIF
(
	input					 clock_dsp, //DSP Sampling Clock, which can be substantially faster than the other clocks.
	input              clock_rx,  //Rx Clock Interface 2x Sample rate
	input              clock_tx,  //Tx Clock Interface 2x Sample rate
	input              reset,

	//Internal Bus I/O
	input  wire [23:0] tx_iq,			//Transmit I/Q data stream input (12 bits each).
	output reg  [23:0] rx_iq,			//Receive  I/Q data stream output (12 bits each).
	output wire 		 rx_strobe,    //Baseband processing receive sample strobe output.
	output wire			 tx_strobe,		//Baseband processing transmit sample strobe output.

	// LIME Chip Baseband interface.
	output wire         rf_rxclk,           //Lime receive ADC clock.
	input  wire         rf_rxiqsel,
	output wire         rf_rxen,
	input wire [11:0]   rf_rxdata,

	output wire         rf_txclk,           //Lime transmit ADC clock.
	output wire         rf_txiqsel,
	output wire         rf_txen,
	output wire [11:0]   rf_txdata,

	//Control Interface.
	input   wire  [11:0] rbusCtrl,  // Address and control lines(12 total) { addr[7:0], readEnable, writeEnable, dataStrobe, clkbus}
	inout   wire  [7:0]  rbusData	// Tri-state I/O data.
);
parameter CTRL_ADDR = 0;
parameter RSSI_ADDR = 0;
parameter RXBIAS_ADDR = 0;

	//RF Interface. (WRITE)
	// bit#  |  Description
	//-------|----------------------------------------------------------
	//  0-1     RX Input Mode
	//           0 = tx_iq loopback 
	//           1 = rf_rxdata input (dc bias removed).
	//	         2 = rf_rxdata input (raw).
	//			 3 = Fixed Test Pattern  i = 256, q = -256; 
	//  2-3     TX Ouput Mode
	//           0 = rf_rxdata loopback 
	//           1 = rf_txdata input.
	//	         2 = Fixed Test Pattern  i = 256, q = -256; 
	//			 3 = Fixed Test Pattern  i = 0, q = 0;
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

	wire [7:0] rf_ctrl;
	
	//Control Register
	WcaWriteByteReg #(CTRL_ADDR) wr_rf_ctrl
	(.reset(reset), .out( rf_ctrl), .rbusCtrl(rbusCtrl), .rbusData(rbusData) );

	//------------------------------------------------------------
	// Lime chip control.
	//------------------------------------------------------------   
	assign rf_rxen  = rf_ctrl[4];   
	assign rf_txen  = rf_ctrl[5];

	//Buffer the clock output so the internal clock
	//fanout is not driving the output pins.
	ODDR2 oddr2_rxclk	( .D0(1'b1), .D1(1'b0), .CE(rf_ctrl[6]),	.C0(clock_rx),	.C1(~clock_rx), .Q(rf_rxclk) );
	ODDR2 oddr2_txclk ( .D0(1'b1), .D1(1'b0),	.CE(rf_ctrl[7]),	.C0(clock_tx),	.C1(~clock_tx), .Q(rf_txclk) );


	//------------------------------------------------------------
	// Receiver Interface Selector.
	//------------------------------------------------------------   
	
	wire  [11:0] rx_DciBiasRemoved; //These are provided by the DC Offset calculation 
	reg   [1:0] reg_rxstrobe;

	//Shape RX strobe as a 1 clock pulse strobe.
	always @(posedge clock_dsp)
	begin
		if( reset )
			begin
				reg_rxstrobe[0] <= 1'b0;
				reg_rxstrobe[1] <= 1'b0;
			end
		else	
			begin
				reg_rxstrobe[0] <= ~rf_rxiqsel & ~reg_rxstrobe[1] &~reg_rxstrobe[0];
				reg_rxstrobe[1] <= reg_rxstrobe[0];
				//reg_rxstrobe[2] <= reg_rxstrobe[1];
				//reg_rxstrobe[3] <= reg_rxstrobe[2];

			end
	end
	assign rx_strobe = reg_rxstrobe[1] & rf_ctrl[6];
	
	//Break out receive data into separate I/Q paths
	//so we can process in dsp.
	always @(posedge clock_dsp)
	begin
	  case( { rf_ctrl[1:0]} )
		2'b00 : // 0 is loop back.tx back on to rx.
		 begin
			rx_iq <= #1 tx_iq;
		 end
		2'b01 : //Rx data with DC Offset Removed.
		 begin
			if( rf_rxiqsel )
				 rx_iq[11:0] <= #1 rx_DciBiasRemoved;
			else
				 rx_iq[23:12]  <= #1 rx_DciBiasRemoved;		 
		 end
		2'b10 :  //2 raw rx data,.
		 begin
			if( rf_rxiqsel )
				 rx_iq[11:0] <= #1 rf_rxdata;
			else
				 rx_iq[23:12]  <= #1 rf_rxdata;
		 end
		default: // 2 is fixed test pattern
		 begin
			 rx_iq <= #1 24'hF00100; //256,-256 I & Q.
		 end
	  endcase
	end

	//------------------------------------------------------------
	// Transmitter Interface Selector.
	//------------------------------------------------------------ 
	reg   [1:0] reg_txstrobe; 
	reg [23:0] txdata;
	reg   txiqsel; 
	// Generate TX IQ select clock.
	always @(posedge clock_tx)
	begin
		if( reset ) 
			txiqsel <= 1'h0;
		else
			txiqsel <= ~txiqsel;
	end
	
	//disable IQ select when no clocking.
	assign rf_txiqsel = txiqsel & rf_ctrl[7];
	
	// Generate TX IQ select clock.	
	always @(posedge clock_dsp)
	begin
		if( reset )
			begin
				reg_txstrobe[0] <= 1'b0;
				reg_txstrobe[1] <= 1'b0;
				txdata <= 24'h0;
			end
		else	
			begin
				reg_txstrobe[0] <= ~rf_txiqsel & ~reg_txstrobe[1] &~reg_txstrobe[0];
				reg_txstrobe[1] <= reg_txstrobe[0];		
				txdata <= (reg_txstrobe[0])  ? tx_iq : txdata;
			end
	end
	

	assign tx_strobe  = reg_txstrobe[1] & rf_ctrl[7];
	
	assign rf_txdata = ( rf_ctrl[3:2] == 2'b00) // Loop back tx to rx
						   ? rf_rxdata 
							: ((rf_ctrl[3:2] == 2'b01) // 1 tx.
								? ((txiqsel)? txdata[11:00]  :  txdata[23:12] ) //When iqsel is 1
								: ((rf_ctrl[3:2] == 2'b10) // test pattern
									? ((txiqsel) ? 12'h100 : 12'hF00)
									: 12'h0)); 


	//------------------------------------------------------------
	// RX RSSI Implementation
	//------------------------------------------------------------   
	wire [7:0] rssi_q;
	wire [7:0] rssi_i;

	//Construct Inphase RSSI function
	WcaRssi RssiInphase(
	  .clock(clock_dsp), .reset(reset), .strobe(rx_strobe), .adc(rx_iq[11:00]), .rssi(rssi_i) );

	//RSSI Quadrature Function
	WcaRssi RssiQuadrature(
	  .clock(clock_dsp), .reset(reset), .strobe(rx_strobe), .adc(rx_iq[23:12]), .rssi(rssi_q) );

	//Place the RSSI values into a register for retrieval.
	WcaReadWordReg #(RSSI_ADDR) RxRssiReadReg
	( .reset(reset), .clock(clock_dsp), .enableIn(rx_strobe), .in( {rssi_q, rssi_i} ), .rbusCtrl(rbusCtrl), .rbusData(rbusData) );

	//------------------------------------------------------------
	// RX DC Bias Detection.
	//------------------------------------------------------------   
	wire  [15:0] dcOffset;
		
	WcaDcOffset DcOffsetRemove(
	  .clock(clock_rx),   .reset(reset), .strobe(1'h1), .iqSel(rf_rxiqsel), .sig_in(rf_rxdata), .dcoffset( dcOffset), .sigout(rx_DciBiasRemoved));

	//Place the DCOffset values into registers for retrieval.
	//WcaReadDwordReg #(RXBIAS_ADDR) RxBiasReadReg
	//( .reset(reset), .clock(clock_dsp), .enableIn(rx_strobe), .in( {dcOffset_i, dcOffset_q} ), .rbusCtrl(rbusCtrl), .rbusData(rbusData) );
	  
endmodule // WcaLimeIF