// Name:  WcaDownConverter.v
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

//  12 bit DDC Decimator with half-band filter output.
module WcaDownConverter
  (
    input              clock,			// input Clock 
    input              reset,			// input reset	
    input              enable,		// input enable
	 input				  aclr,			// clears state but not configuration.
    input              dstrobe_in,	// input dstrobe_in
	 input  wire [3:0]  cfgflags,		// input dynamic configuration mode. 
	 input  wire [23:0] iq_in,			// input [23:0] diq_in
    output wire [31:0] iq_out,		// ouput [31:0] diq_out
	 output wire 		  dstrobe_out, // ouput dstrobe_out
    input   wire  [11:0] rbusCtrl,  // Address and control lines(12 total) { addr[7:0], readEnable, writeEnable, dataStrobe, clkbus}
    inout   wire  [7:0]  rbusData	// Tri-state I/O data.	 
  );

   parameter IF_FREQ_ADDR 		= 0;
   parameter DECIM_RATE_ADDR 	= 1;
   parameter MODE  =  4'hF; 		//Dynamic configuration enabled by default.
	
	//DDC Configuration Mode flags:
   `define CORDIC_ENABLED 3'h1
	`define CIC_ENABLED 3'h2
	`define HBF_ENABLED 3'h4
	`define DYNAMIC_CONFIG 4'h8

	wire [11:0] icor_in = iq_in[11:0];
	wire [11:0] qcor_in = iq_in[23:12];
	wire [11:0] icic_in;
	wire [11:0] qcic_in;
	wire [15:0] ihb_in;
	wire [15:0] qhb_in;
	wire hbstrobe_in;
	wire clearAll = reset | aclr;
	
//*****************************************************  
// CORDIC Frequency Up/Down Conversion
//*****************************************************
generate if( (MODE & `CORDIC_ENABLED))
 begin
	wire [11:0] icor_out;
	wire [11:0] qcor_out;
	wire [31:0] phase;

    //Generate Phase for frequency translation.
    WcaPhaseGen #(IF_FREQ_ADDR, 32) phase_generator
    (
      .clock(clock), .reset(reset), .aclr(aclr), .enable(enable), .strobe(dstrobe_in),
      .rbusCtrl(rbusCtrl), .rbusData(rbusData), 
      .phase(phase)
    );
	 
    //Down conversion frequency must be in the 1Qn format (see cordic doc below).  2^n-1 = pi/2 and -2^n-1 = -pi/2
    //The Cordic only supports values between this range so the phase accumulator must produce values within this range.
    //We rotate quadrants and adjust phase to make work.
    reg [11:0] X0;
    reg [11:0] Y0;
    reg [11:0] A0;

    always @(posedge clock) 
     case({dstrobe_in, phase[31:30]})
       3'b101 :  //pi/2 to pi   
        begin            
            X0 <= #1 -qcor_in;
            Y0 <= #1 icor_in;
            A0 <= #1 phase[31:20] - 12'h400;                    
        end
       3'b110 : //-pi/2 to -pi
        begin
            X0 <= #1 qcor_in;
            Y0 <= #1 -icor_in;
            A0 <= #1 phase[31:20] + 12'h400;                    
        end
       3'b100: // 0 to pi/2
			begin
            X0 <= #1 icor_in;
            Y0 <= #1 qcor_in;
            A0 <= #1 phase [31:20];
        end
       3'b111: // 0 to -pi/2
			begin
            X0 <= #1 icor_in;
            Y0 <= #1 qcor_in;
            A0 <= #1 phase [31:20];
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
       .ngreset(1'b1), .clock(clock),.reset(clearAll), .strobeData( dstrobe_in),
       .X0(X0), .Y0(Y0),.A0(A0),
       .XN(icor_out), .YN(qcor_out), .AN() 
    );	 
	 
	//Enable dynamic selection of bypass if DYNAMIC_CONFIG ENABLED, otherwise hardwire.
	wire bypassCordic    = MODE[3] & cfgflags[0];
	assign icic_in 		= (bypassCordic) ? icor_in : icor_out;
	assign qcic_in 		= (bypassCordic) ? qcor_in : qcor_out;	 
	 
  end
else // bypass
  begin
    assign icic_in = icor_in;
    assign qcic_in = qcor_in;
  end
endgenerate 

//*****************************************************  
// CIC Decimator 
//*****************************************************
	wire [12:0] decim_rate;
	wire  newRateData;
 
	//24bit Data Rate Configuration Register.
	WcaWriteWordReg #(DECIM_RATE_ADDR) reg_decim_rate
	(.reset(reset), .out( decim_rate), .nd(newRateData),
	 .rbusCtrl(rbusCtrl), .rbusData(rbusData) );	

generate if(MODE & `CIC_ENABLED)  
 begin
	wire [15:0] icic_out;
	wire [15:0] qcic_out;  
	wire  		cic_strobe_out;
	reg   rnd; 

	//Synch the newRateData clock to a one pulse.
	always @(posedge clock)
	begin
		if( reset) rnd <= 1'b0;
		else rnd <= (~rnd & newRateData);
	end

	cic_decim cic_i (
	.sclr(reset),			// resets
	.din(icic_in), 		// input [11 : 0] din
	.nd(dstrobe_in),		// input nd
	.rate(decim_rate), 	// input [12 : 0] rate
	.rate_we(rnd | aclr), // input rate_we
	.clk(clock), 			// input clk
	.dout(icic_out), 		// output [15 : 0] dout
	.rdy(cic_strobe_out),// output rdy
	.rfd()); 				// output rfd

	cic_decim cic_q (
	.sclr(reset),			// resets
	.din(qcic_in), 	 	// input [11 : 0] din
	.nd(dstrobe_in),		// input nd
	.rate(decim_rate), 	// input [12 : 0] rate
	.rate_we(rnd | aclr),// input rate_we
	.clk(clock), 			// input clk
	.dout(qcic_out), 		// output [15 : 0] dout
	.rdy(), 					// output rdy
	.rfd()); 				// output rfd
	
	//Enable dynamic selection of bypass if DYNAMIC_CONFIG ENABLED, otherwise hardwire.
	wire bypassCic = MODE[3] & cfgflags[1];
	wire bypassCicStrobe = MODE[3] & cfgflags[2];
	assign ihb_in 		= (bypassCic) ? {icic_in[11], icic_in[11], icic_in[11], icic_in[11], icic_in} : icic_out;
	assign qhb_in 		= (bypassCic) ? {qcic_in[11], qcic_in[11], qcic_in[11], qcic_in[11], qcic_in} : qcic_out;
	assign hbstrobe_in  = 	(bypassCicStrobe) ?  dstrobe_in : cic_strobe_out;
 end
else 
 begin
    assign ihb_in 		= {icic_in[11], icic_in[11], icic_in[11], icic_in[11], icic_in};
    assign qhb_in 		= {qcic_in[11], qcic_in[11], qcic_in[11], qcic_in[11], qcic_in};


	//Generate strobe given the rate provided by the user.
	WcaDspStrobe  lsdp_strobe_gen (
		.clock(clock),  	.reset(clearAll), .enable(enable), .strobe_in(dstrobe_in), 
		.rate(decim_rate), .strobe_out(hbstrobe_in), .count( )
	);	
 end   
endgenerate

//*****************************************************  
// Halfband filter 
//*****************************************************

generate if((MODE & `HBF_ENABLED))    
  begin
   wire [15:0]  i_hb_out;
	wire [15:0]  q_hb_out;
	wire  dstrobe_hb_out;  
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
	 halfband_decim hb_i (
		.clk(clock), 				// input clk
		.sclr(clearAll),
		.nd(hbstrobe_in), 		// input nd
		.rfd(), 						// output rfd
		.rdy(dstrobe_hb_out), 	// output rdy
		.din(ihb_in), 				// input [15 : 0] din
		.dout(i_hb_out)); 	// output [15 : 0] dout 
	  
	 halfband_decim hb_q(
		.clk(clock), 		    	// input clk
		.sclr(clearAll),
		.nd(hbstrobe_in),     	// input nd
		.rfd(), 				    	// output rfd
		.rdy(), 	 					// output rdy
		.din(qhb_in), 			 	// input [15 : 0] din
		.dout(q_hb_out)); 	// output [15 : 0] dout 
		
		//Enable dynamic selection of bypass if DYNAMIC_CONFIG ENABLED, otherwise hardwire.
		wire bypassHbf    	= MODE[3] & cfgflags[3];
		assign iq_out[15:0]	= (bypassHbf) ? ihb_in : i_hb_out ;	
		assign iq_out[31:16]	= (bypassHbf) ? qhb_in : q_hb_out ;			
		assign dstrobe_out 	= (bypassHbf) ? hbstrobe_in : dstrobe_hb_out;
  end
else
  begin
    assign iq_out[15:0]  = ihb_in;  //Map to high bits to preserve twos complement
    assign iq_out[31:16] = qhb_in;  //format. 
 	 assign dstrobe_out   = hbstrobe_in;
  end
endgenerate

endmodule 