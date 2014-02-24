// Name:  WcaUpConverter.v
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

module WcaUpConverter
(
	//General functions.
	input              	clock,
	input              	reset,
	input              	enable,

	//Lines from DDUC Controller.
	input  	wire [7:0]  	cfg,				//Configuration lines. 
	input  	wire [12:0] 	rate_interp,	//Interpolation rate output required by DUC block.
	input    wire				rate_interp_we,//Rate input write enable.
	input  	wire       		strobe_cic,   	//CIC interpolation strobe 
	input  	wire 				strobe_bb,    	//Baseband data strobe
	input  	wire [31:0] 	phase_cordic, 	//Generated Cordic phase.
	input		wire [3:0]  	log2_rate,
	
	//Data I/O.
	input	  wire			strobe_if,
	input   wire	[31:0] iq_bb_in,
	output  wire	[23:0] iq_if_out,
	
   //CPU Interface.
	input   wire  [11:0] rbusCtrl,  // Address and control lines(12 total) { addr[7:0], readEnable, writeEnable, dataStrobe, clkbus}
	inout   wire  [7:0]  rbusData	// Tri-state I/O data.
  );
  parameter MODE  =  4'hF; 		//Dynamic configuration enabled by default.

	//DDC Configuration Mode flags:
	`define CORDIC_ENABLED 3'h1
	`define CIC_ENABLED 3'h2
	`define HBF_ENABLED 3'h4
	`define DYNAMIC_CONFIG 4'h8
	
	wire [15:0] ihb_in = iq_bb_in[15:0];
	wire [15:0] qhb_in = iq_bb_in[31:16];
	
	wire [11:0] icor_in;
	wire [11:0] qcor_in;
	
	wire [15:0] icic_in;
	wire [15:0] qcic_in;
	wire [15:0] icic_out;
   wire [15:0] qcic_out;

	
	wire hb_strobe_out;
	wire cic_strobe_out;
	
//*****************************************************  
// Halfband filter 
//*****************************************************
generate if( MODE & `HBF_ENABLED )
  begin
  
  wire [15:0] ihbout_internal;
  wire [15:0] qhbout_internal;
  	wire [15:0] ihb_out;
	wire [15:0] qhb_out; 
	//Implement strobe that tighens up when data is ready so we don't
	//double clock.
	/*
	wire rdy;
	reg hbstrobeout;
	always @(posedge clock)
	begin
		if( reset) hbstrobeout <= 1'b0;
		else hbstrobeout <= (rdy & ~hbstrobeout);
	end
	assign dstrobe_out = hbstrobeout;
	*/
	
	// xbh_in --> |halfband interp|-->  xhb_out
	// where x is 'i' or 'q'
	 halfband_interp hb_i (
		.clk(clock), 				// input clk
		.nd(strobe_bb), 		// input nd
		.rfd(), 						// output rfd
		.rdy(), 						// output rdy
		.din(ihb_in), 				// input [15 : 0] din
		.dout(ihbout_internal)); 			// output [15 : 0] dout 
	  
	 halfband_interp hb_q(
		.clk(clock), 		    	// input clk
		.nd(strobe_bb),     	// input nd
		.rfd(), 				    	// output rfd
		.rdy(), 	 					// output rdy
		.din(qhb_in), 			 	// input [15 : 0] din
		.dout(qhbout_internal)); 			// output [15 : 0] dout 
		
		
	//Enable dynamic selection of bypass if DYNAMIC_CONFIG ENABLED, otherwise hardwire.
	wire bypassHbf    	= MODE[3] & cfg[5];
	assign icic_in 		= (bypassHbf) ? ihb_in : ihbout_internal;
	assign qcic_in 		= (bypassHbf) ? qhb_in : qhbout_internal;	 		
  end
else
  begin
    assign icic_in 		= ihb_in; 
    assign qcic_in 		= qhb_in;
  end
endgenerate
	
//*****************************************************  
// CIC Interpolator
//*****************************************************
generate if( MODE & `CIC_ENABLED)
  begin
  
	WcaCicInterp cic_i (
	 .clock(clock), 
    .reset(reset), 
    .enable(enable), 
    .strobe_cic(strobe_cic), 
    .strobe_if(strobe_if), 
    .log2_rate(log2_rate), 
    .data_in(icic_in), 
    .data_out(icic_out)
    );

	WcaCicInterp cic_q (
    .clock(clock), 
    .reset(reset), 
    .enable(enable), 
    .strobe_cic(strobe_cic), 
    .strobe_if(strobe_if), 
    .log2_rate(log2_rate), 
    .data_in(qcic_in), 
    .data_out(qcic_out)
    );
		
	//Enable dynamic selection of bypass if DYNAMIC_CONFIG ENABLED, otherwise hardwire.
	wire bypassCic    	=  MODE[3] & cfg[3];
   assign icor_in 		= (bypassCic) ? icic_in[15:4] : icic_out[15:4];
	assign qcor_in 		= (bypassCic) ? qcic_in[15:4] : qcic_out[15:4] ;

	end
else 
 begin 
    assign icor_in 			= icic_in[15:4] ;
    assign qcor_in 			= qcic_in[15:4] ;
  end   
endgenerate

//*****************************************************  
// CORDIC Frequency Up/Down Conversion
//*****************************************************
generate if( MODE & `CORDIC_ENABLED)
  begin
  wire [11:0] icordicout_internal;
  wire [11:0] qcordicout_internal;
  
    //Down conversion frequency must be in the 1Qn format (see cordic doc below).  2^n-1 = pi/2 and -2^n-1 = -pi/2
    //The Cordic only supports values between this range so the phase accumulator must produce values within this range.
    //We rotate quadrants and adjust phase to make work.
    reg [11:0] X0;
    reg [11:0] Y0;
    reg [11:0] A0;

    always @(posedge clock) 
     case(phase_cordic[31:30])
       2'b01 :  //pi/2 to pi   
        begin            
            X0 <= #1 -qcor_in;
            Y0 <= #1 icor_in;
            A0 <= #1 phase_cordic[31:20] - 12'h400;                    
        end
       2'b10 : //-pi/2 to -pi
        begin
            X0 <= #1 qcor_in;
            Y0 <= #1 -icor_in;
            A0 <= #1 phase_cordic[31:20] + 12'h400;                    
        end
       default:
        begin
            X0 <= #1 icor_in;
            Y0 <= #1 qcor_in;
            A0 <= #1 phase_cordic [31:20];
        end
    endcase

    // CORDIC gain is about 1.64 and formulation requires that numbers have the format of 1Qn format
    // where there are 1 sign bit plus 1 integer bits and n-1 fractional bits.  For a 12 bit 2's complement number
    // this means that n = 11: 1 sign bit, 1 integer bit, and 10 fractional. For 1Qn format, the maximum allowable 
    // range input is between -2^n-1 and 2^n-1; thus with 12 bits, the maximum range is  -2^10 to 2^10 (-1024 to 1024). 
    // In a 12 bit system, -1024 = -1.0 and 1024 = 1.0.  Converting between decimal and 1Qn is
    //
    //  1Qn = int(1024* float_val);
    //  float_val = 1Qn / 1024f;
    //
    //  2' Complement numbers need to be scaled to conform to this standard.  If value magnitude in X and Y are > 1024 overflow
    //   can occur. This implementation does not scale the input so need to be careful.
    //
    WcaCordic12  #(12,12,0) cordic
    ( 
       .ngreset(1'b1), .clock(clock),.reset(reset), .strobeData(strobe_if),
       .X0(X0), .Y0(Y0),.A0(A0),
       .XN(icordicout_internal), .YN(qcordicout_internal), .AN() 
    );	 
	 
	//Enable dynamic selection of bypass if DYNAMIC_CONFIG ENABLED, otherwise hardwire.
	wire bypassCordic    = MODE[3] & cfg[2];
	assign iq_if_out[11:0]  = (bypassCordic) ? icor_in : icordicout_internal;
	assign iq_if_out[23:12] = (bypassCordic) ? qcor_in : qcordicout_internal;

  end
else // bypass
  begin
    assign iq_if_out[11:0]  = icor_in;
    assign iq_if_out[23:12] = qcor_in;
  end
endgenerate 

endmodule // WcaUpConverter