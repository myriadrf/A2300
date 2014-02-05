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
	input              clock,
	input              reset,
	input              enable,
	input              dstrobe_out,
	input              dstrobe_in,
	input   wire	[31:0] iq_in,
	output  wire	[23:0] iq_out,
	
   //CPU Interface.
	input   wire  [11:0] rbusCtrl,  // Address and control lines(12 total) { addr[7:0], readEnable, writeEnable, dataStrobe, clkbus}
	inout   wire  [7:0]  rbusData	// Tri-state I/O data.

  );
   parameter IF_FREQ_ADDR 		= 0;
   parameter INTEG_RATE_ADDR 	= 1;

   parameter CORDIC_ENABLE = 1; 		// CORDIC Enable/Disable.
   parameter CIC_ENABLE 	= 1;     // CIC Decimator Enable/Disable..
   parameter HBF_ENABLE 	= 1;     // Half-Band Filter Enable/Disable.
   
	wire [15:0] ihb_in = iq_in[15:0];
	wire [15:0] qhb_in = iq_in[31:16];
	wire [15:0] ihb_out;
	wire [15:0] qhb_out;
	wire [11:0] icic_out;
	wire [11:0] qcic_out;
	//wire hbstrobe_out;

//*****************************************************  
// Halfband filter 
//*****************************************************
generate if( HBF_ENABLE == 1)
  begin
   
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
		.nd(hbstrobe_in), 		// input nd
		.rfd(), 						// output rfd
		.rdy(dstrobe_out), 		// output rdy
		.din(ihb_in), 				// input [15 : 0] din
		.dout(iq_out[15:0])); 	// output [15 : 0] dout 
	  
	 halfband_decim hb_q(
		.clk(clock), 		    	// input clk
		.nd(hbstrobe_in),     	// input nd
		.rfd(), 				    	// output rfd
		.rdy(), 	 					// output rdy
		.din(qhb_in), 			 	// input [15 : 0] din
		.dout(iq_out[31:16])); 	// output [15 : 0] dout 
  end
else
  begin
    assign ihb_out 		= ihb_in; 
    assign qhb_out 		= qhb_in;
	 //assign hbstrobe_out = hbstrobe_in;
  end
endgenerate
	
//*****************************************************  
// CIC Decimator 
//*****************************************************
generate if( CIC_ENABLE == 1)
  begin

  wire [12:0] integ_rate;
  reg   rnd; 
  wire  newRateData;
  
	//24bit Data Rate Configuration Register.
	WcaWriteWordReg #(INTEG_RATE_ADDR) reg_integ_rate
	(.reset(reset), .out( integ_rate), .nd(newRateData),
	 .rbusCtrl(rbusCtrl), .rbusData(rbusData) );	

	//Synch the newRateData clock to a one pulse.
	always @(posedge clock)
	begin
		if( reset) rnd <= 1'b0;
		else rnd <= (~rnd & newRateData);
	end

	cic_decim cic_i (
	.sclr(reset),				// resets
	.din(icic_in), 		// input [11 : 0] din
	.nd(dstrobe_in),		// input nd
	.rate(integ_rate), 	// input [12 : 0] rate
	.rate_we(rnd), 		// input rate_we
	.clk(clock), 			// input clk
	.dout(ihb_in), 		// output [15 : 0] dout
	.rdy(hbstrobe_in), 	// output rdy
	.rfd()); 				// output rfd

	cic_decim cic_q (
	.sclr(reset),				// resets
	.din(qcic_in), 	 	// input [11 : 0] din
	.nd(dstrobe_in),		// input nd
	.rate(integ_rate), 	// input [12 : 0] rate
	.rate_we(rnd), 		// input rate_we
	.clk(clock), 			// input clk
	.dout(qhb_in), 		// output [15 : 0] dout
	.rdy(), 					// output rdy
	.rfd()); 				// output rfd
	end
else 
 begin
    assign icic_out 			= ihb_out[15:4];
    assign qcic_out 			= qhb_out[15:4];
  end   
endgenerate

//*****************************************************  
// CORDIC Frequency Up/Down Conversion
//*****************************************************
generate if( CORDIC_ENABLE == 1)
  begin
	wire [31:0] phase;

    //Generate Phase for frequency translation.
    WcaPhaseGen #(IF_FREQ_ADDR, 32) phase_generator
    (
      .clock(clock), .reset(reset), .aclr(1'b0), .enable(enable), .strobe(dstrobe_out),
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
     case(phase[31:30])
       2'b01 :  //pi/2 to pi   
        begin            
            X0 <= #1 -qcic_out;
            Y0 <= #1 icic_out;
            A0 <= #1 phase[31:20] - 12'h400;                    
        end
       2'b10 : //-pi/2 to -pi
        begin
            X0 <= #1 qcic_out;
            Y0 <= #1 -icic_out;
            A0 <= #1 phase[31:20] + 12'h400;                    
        end
       default:
        begin
            X0 <= #1 icic_out;
            Y0 <= #1 qcic_out;
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
       .ngreset(1'b1), .clock(clock),.reset(reset), .strobeData( dstrobe_out),
       .X0(X0), .Y0(Y0),.A0(A0),
       .XN(iq_out[11:0]), .YN(iq_out[23:12]), .AN() 
    );	 
  end
else // bypass
  begin
    assign iq_out[11:0]  = icic_out;
    assign iq_out[23:12] = qcic_out;
  end
endgenerate 

endmodule // WcaUpConverter