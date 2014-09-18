`timescale 1ns / 1ps

// Name:  WcaDownConverter_stimulus.v
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
`include "../wca/hal/WcaHalRegisterDefs.vh" //grab register addresses.
`include "../containers/4ChTxRxRegisterDefs.vh"
module WcaDownConverter_stimulus;

parameter SYSCLK_PERIOD = 	 10.0; // 100 MHz
parameter DSPCLK_PERIOD =   1000.0/128.0; //128 MHz
parameter LIMECLK_PERIOD =  1000.0/64.0; // 64 MHz
parameter CPUCLK_PERIOD =   1000.0/32.0; // 32 MHz

parameter RESET_START	=   5*CPUCLK_PERIOD;
parameter EVENT_MASK		=   8'h02; // LSDP0, LSDP1;
parameter LSDP_ENABLE	=   8'h20;	//Enables LSDP0 and LSDP1.
parameter LSDP_RESET		=   8'h02;  //Resets LSDP0 and LSDP1.
parameter RF0_CTRL_FLAGS = 8'hF5; 	//Set the RX IQ to rx and Txd Data.,
parameter RX_SAMP_DELAY  = 1; // Six nanoseconds

`define DDC0_CTRL  (`WCACOMP_DSP_DDC0 + `DSP_DDUC_CTRL)

/////////////////////////////////////////
// Wires and registers
/////////////////////////////////////////
	reg reset, cpuclk, dspclk, limeclk, comp_reset;
	reg sysclk;
	reg [23:0] iq_in;
	wire [31:0] iq_out;
	wire dstrobe_out;

	// Bidirs
	reg [11:0] rbusCtrl;
	wire [7:0] rbusData;
	
	//CPU I/O and Processing Signals
	reg [7:0] adata;		//Address set register
	reg [7:0] rdata;		//Read bus result register.
	reg [7:0] wdata;		//write data register.
	wire [7:0] dbus;		//CPU data bus

	reg address, write, read; //address,write, read control lines.
	
//////////////////////////////////////////////////////////////////////
// WCA HAL Functions
//////////////////////////////////////////////////////////////////////
	reg [11:0]  rf0_rxd;
	reg  			rf0_rxiqsel;
	wire 			rf0_rxclk, rf0_rxen, rf0_txclk, rf0_txiqsel, rf0_txen;
	wire [11:0] rf0_txd;
	wire [23:0] rx0_iq;
	wire [23:0] tx0_iq;
	wire 			rx0_strobe, tx0_strobe, rx1_strobe, tx1_strobe;



//////////////////////////////////////////////////////////////////////
// Downconverter Test.
//////////////////////////////////////////////////////////////////////
	WcaRegBus cpuif (
		.clock(sysclk), 
		.reset(reset), 
		.address(address), 
		.write(write), 
		.read(read), 
		.dbus(dbus), 
		.rbusCtrl(rbusCtrl), 
		.rbusData(rbusData)
	);	


	wire [7:0] ddc0_ctrl;
	WcaWriteByteReg #(`DDC0_CTRL) wr_ddc0_ctrl
							(.reset(reset), 
							.out( ddc0_ctrl), 
							.rbusCtrl(rbusCtrl), .rbusData(rbusData) );	

	// Instantiate the Unit Under Test (UUT)
	WcaDownConverter uut (
		.clock(dspclk), 
		.reset(reset), 
		.enable(ddc0_ctrl[0]), 
		.aclr( ddc0_ctrl[1]),
		.dstrobe_in(rx0_strobe), 
		.cfgflags(ddc0_ctrl[5:2]), 
		.iq_in(rx0_iq), 
		.iq_out(iq_out), 
		.dstrobe_out(dstrobe_out), 
		.rbusCtrl(rbusCtrl), 
		.rbusData(rbusData)
	);




	// Lime RF#1 Interface implementation.
   WcaLimeIF  #( `WCAHAL_RF0_CTRL, `WCAHAL_RF0_RSSI, `WCAHAL_RF0_BIAS)
				  lime1_if ( .clock_dsp(dspclk), .clock_rx(limeclk), .clock_tx(limeclk), .reset(reset), 
                      .rbusCtrl(rbusCtrl),.rbusData(rbusData),
                      .rf_rxclk(rf0_rxclk), .rf_rxiqsel(rf0_rxiqsel), .rf_rxen(rf0_rxen), .rf_rxdata(rf0_rxd[11:0]), 
                      .rf_txclk(rf0_txclk), .rf_txiqsel(rf0_txiqsel), .rf_txen(rf0_txen), .rf_txdata(rf0_txd[11:0]), 
                      .rx_strobe( rx0_strobe),  .rx_iq(rx0_iq), .tx_strobe( tx0_strobe), .tx_iq( tx0_iq) ); 
	
//////////////////////////////////////////////////////////////////////
// Lime signals simulations
//////////////////////////////////////////////////////////////////////
	
	//NOTE limeclk is used to drive these simulation timings, 
	//since the others are not set up during the reset.
	reg [11:0] rx0_count;
	
	always @(posedge rf0_rxclk)
	begin
		if(reset) rx0_count <= 12'h0;
		else if( rf0_rxiqsel) rx0_count <= rx0_count + 12'h1;
	end
	
	//Simulate the RF0 iqselection.
	always @(posedge rf0_rxclk)
	begin
		if( reset) #(RX_SAMP_DELAY)
		  begin
			rf0_rxiqsel <= 1'h0;
			rf0_rxd  <= 12'h0;
		  end
		else if( rf0_rxen) #(RX_SAMP_DELAY)
		  begin
			rf0_rxiqsel <= ~rf0_rxiqsel;
			rf0_rxd  <= (rf0_rxiqsel) ? 12'h17F : 12'hF81;
		  end
	end
	

//////////////////////////////////////////////////////////////////////
// Simulation
//////////////////////////////////////////////////////////////////////
initial begin
	//Set initial state of clocks lime interface related regisers.
	#0
	sysclk = 1;
	dspclk=1;
	limeclk = 1;
	rf0_rxiqsel <= 1'h0;
	rf0_rxd  <= 12'h0;
	rx0_count <=12'h0;
	 	
end

initial begin

	//Set initial state of CPU related registers.
    #0
	 cpuclk = 1;
	 address = 0;
	 write = 0;
	 read = 0;
	 wdata = 8'hEF;
	 reset = 1'b0;
	 comp_reset = 1'b0;

	//Reset the part.
	#(RESET_START) 			reset = 1'b1;
	#(CPUCLK_PERIOD*3)	 	reset = 1'b0;	 
	
	//Reset the HdwComponent after the main reset.
	#(CPUCLK_PERIOD*10)		comp_reset = 1'b1;
	#(CPUCLK_PERIOD*3)		comp_reset = 1'b0;	 

	//Set the Sampling Control on RF1 to receive RF and transmit DSP.
	#(CPUCLK_PERIOD * 4)  adata = `WCAHAL_RF0_CTRL;
	#(CPUCLK_PERIOD)  	address = 1'b1;
	#(CPUCLK_PERIOD)  	address= 1'b0;
	#(CPUCLK_PERIOD)     wdata = RF0_CTRL_FLAGS; 
	#(CPUCLK_PERIOD)  	write = 1'b1;
	#(CPUCLK_PERIOD)		write = 1'b0;
	
end

//////////////////////////////////////////////////////////////////////
// CPU Addressing Simluator
//////////////////////////////////////////////////////////////////////

	//Set up Registers for controlling and databus.
	assign dbus = (address) ? adata : (write) ? wdata : 8'bz;
	
	always @(posedge cpuclk)
	begin
		if( reset)
			rdata <= 8'h0;
		else if( read)
			rdata <= dbus;
	end


	
//////////////////////////////////////////////////////////////////////
// Clocks
//////////////////////////////////////////////////////////////////////		
always @(sysclk	)
    #(SYSCLK_PERIOD / 2.0) sysclk <= !sysclk;



always @(cpuclk	)
    #(CPUCLK_PERIOD / 2.0) cpuclk <= !cpuclk;
		
always @(dspclk)
    #(DSPCLK_PERIOD / 2.0) dspclk<= !dspclk;

always @( posedge dspclk)
    limeclk <= !limeclk;


      
endmodule

