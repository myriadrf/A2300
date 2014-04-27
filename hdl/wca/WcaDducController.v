`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    16:20:02 02/06/2014 
// Design Name: 
// Module Name:    WcaDucStrobeGen 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module WcaDducController(
	input              clock,
	input              reset,
	input              enable,
	input	  wire		 strobe_if,			//DUC IF strobe.

	output  wire [7:0]   cfg,				//Configuration lines. 
	output  wire [12:0] 	rate_interp,	//Interpolation rate_interp output required by DUC block.
	output  wire 			rate_interp_we,
	output  wire [3:0]	log2_rate,		// log2 of the decim/interp rate
	
	output  wire       	strobe_cic,   	//CIC interpolation strobe 
	output  wire 			strobe_bb,    	//Baseband data strobe
	output  wire [31:0]  phase_cordic, 	//Generated Cordic phase.
	

   //CPU Interface.
	input   wire  [11:0] rbusCtrl,   // Address and control lines(12 total) { addr[7:0], readEnable, writeEnable, dataStrobe, clkbus}
	inout   wire  [7:0]  rbusData		// Tri-state I/O data.
 );
   parameter IF_FREQ_ADDR 		= 0;
   parameter INTERP_RATE_ADDR = 1;
   parameter CONFIG_ADDR		= 2;
   parameter MODE  =  4'hF; 		//Dynamic configuration enabled by default.

	
	//DDC Configuration Mode flags:
   `define CORDIC_ENABLED 3'h1
	`define CIC_ENABLED 3'h2
	`define HBF_ENABLED 3'h4
	`define DYNAMIC_CONFIG 4'h8

	wire [2:0] dumpReg;
	
	// DDC / DUC component Control register.  Bit flags 2 through 5 are
	// only availble if the waveform was compiled with dynamic configuration
	// enabling bypass of internal functions.  To achieve maximum 32 MHz sampling
	// rate bypass CIC decimation, CIC strobe, and half band filters (0x39, 57 dec.).  To achieve 
	//	16 MHz sampling rate,  bypass CIC functions  (0x19, 25 dec.).  To directly sample front-end
	// with decimation sampling rate, bypass decimation, but use sampling strobe (0x29, 45 dec. )
	//  
	// bit#  |  Description
	// -------|----------------------------------------------------------
	//  0	      (1)Enable / (0)Disable Component USB port streaming.
	//  1	      Clear FIFOS and resets counters. 
	//  2	   	Bypass Cordic rotation (DYNAMIC_CONFIG only)
	//  3	 	   Bypass CIC Decimation  (DYNAMIC_CONFIG only)
	//  4			Bypass CIC Decimation strobe (DYNAMIC_CONFIG only)
	//				When enabled, the decimation strobe is bypassed, and the original 
	//			   sampling strobe is used to drive down stream.  
	//  5       Bypass Half band filter (DYNAMIC_CONFIG only). 
	WcaWriteByteReg #(CONFIG_ADDR) wr_config
							(.reset(reset), 
							.out( cfg), 
							.rbusCtrl(rbusCtrl), .rbusData(rbusData) );	
	 
	//Generate Phase for frequency translation.
	WcaPhaseGen #(IF_FREQ_ADDR, 32) phase_generator
	(
	.clock(clock), .reset(reset), .aclr(cfg[1]), .enable(enable), .strobe(strobe_if),
	.rbusCtrl(rbusCtrl), .rbusData(rbusData), 
	.phase(phase_cordic)
	);	 
	 
	//10 bit Data rate_interp Configuration Register.
	WcaWriteWordReg #(INTERP_RATE_ADDR) reg_interp_rate
	(.reset(reset), .out({dumpReg,rate_interp}), .nd(rate_interp_we),
	 .rbusCtrl(rbusCtrl), .rbusData(rbusData) );	
	
	reg [11:0] strobeGenerator;

	wire clear = strobeGenerator[11:2] == rate_interp[9:0];
	always @( posedge clock)
		begin
			if( reset | ~enable | clear | cfg[1])
				begin 
					strobeGenerator <= 12'h1;
				end
			else 
			  begin
				strobeGenerator <= strobeGenerator + 12'h1;
			end
		end

	//Bypass cic strobe if bit 4 of config is set or Cordic disabled
	assign strobe_cic =  enable & ((MODE[3] & cfg[4]) ?  strobe_if  : strobeGenerator == 12'h3);
	assign strobe_bb  =  enable & ((MODE[3] & cfg[5]) ?  strobe_cic : strobe_cic); //TODO create divide by 2 for strobe BB in the future.
	

	// Calculate the logarithm (base-2) of the interpolation rate.  This value is used to 
	// determine the bit growth of the interpolator/decimator.
	assign log2_rate = rate_interp[7] ? 4'h7 :  
							 rate_interp[6] ? 4'h6 : 
							 rate_interp[5] ? 4'h5 : 
							 rate_interp[4] ? 4'h4 : 
							 rate_interp[3] ? 4'h3 : 
							 rate_interp[2] ? 4'h2 : 
							 rate_interp[1] ? 4'h1 : 4'h0;

endmodule
