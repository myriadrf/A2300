`timescale 1 ns/100 ps

// Name:  WcaCordic12.v
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


module WcaCordic12 (reset,ngreset,clock, strobeData, X0, Y0, A0, XN, YN, AN);
  parameter BIT_WIDTH   = 12;
  parameter ITERATIONS  = 12;
  parameter MODE        = 0; // vectoring mode: MODE=1, rotationg mode: MODE=0
  
  input clock, reset, ngreset,strobeData;
  input signed  [BIT_WIDTH-1:0] X0, Y0, A0;
  output signed [BIT_WIDTH-1:0] XN, YN, AN;
  
  wire signed [BIT_WIDTH-1:0] x_w [ITERATIONS-1:1];
  wire signed [BIT_WIDTH-1:0] y_w [ITERATIONS-1:1];
  wire signed [BIT_WIDTH-1:0] a_w [ITERATIONS-1:1];

    //Arctan LUT 
    localparam [143:0] arctanLUT = { 12'h0, 12'h1, 12'h1, 12'h3, 12'h5, 12'ha, 12'h14, 12'h29, 12'h51, 12'ha0, 12'h12e,  12'h200  };
  
    // to make initial connection with inputs
    CordicCalc #(0,MODE,BIT_WIDTH) pcc_0(.reset(reset), .ngreset(ngreset), .clock(clock), .strobeData( strobeData),
               .X0(X0), .Y0(Y0), .A0(A0), .aRom(arctanLUT[BIT_WIDTH-1:0]),
               .XN(x_w[1]), .YN(y_w[1]), .AN(a_w[1])  );
  
    genvar i;
    generate for(i=1; i<ITERATIONS-1; i=i+1) begin: pcc_1
      CordicCalc #(i,MODE,BIT_WIDTH) pcc_1(.reset(reset), .ngreset(ngreset), .clock(clock), .strobeData( strobeData),
               .X0(x_w[i]), .Y0(y_w[i]), .A0(a_w[i]), .aRom(arctanLUT[(i+1)*BIT_WIDTH-1:i*BIT_WIDTH]),
               .XN(x_w[i+1]), .YN(y_w[i+1]), .AN(a_w[i+1])  );
    end
	endgenerate 
	
    //make final connections with outputs
    CordicCalc #(ITERATIONS-1,MODE,BIT_WIDTH) pcc_2(.reset(reset),.ngreset(ngreset),.clock(clock), .strobeData( strobeData),
               .X0(x_w[ITERATIONS-1]), .Y0(y_w[ITERATIONS-1]), .A0(a_w[ITERATIONS-1]), 
               .aRom(arctanLUT[ITERATIONS*BIT_WIDTH-1:(ITERATIONS-1)*BIT_WIDTH]),
               .XN(XN), .YN(YN), .AN(AN)  );
endmodule

module CordicCalc(reset,ngreset,clock, strobeData, X0, Y0, A0, XN, YN, AN, aRom);
  parameter RSHIFT = 0;
  parameter MODE = 0;   // vectoring mode: MODE=1, rotationg mode: MODE=0
  parameter BIT_WIDTH = 12;

  input reset, ngreset, clock, strobeData;
  input  signed [BIT_WIDTH-1:0] X0, Y0, A0;
  input  signed [BIT_WIDTH-1:0] aRom;
  output signed [BIT_WIDTH-1:0] XN, YN, AN;
  wire d;

  reg signed [BIT_WIDTH-1:0] XN, YN, AN;

  assign d = (MODE==0)? A0[BIT_WIDTH-1] : ~Y0[BIT_WIDTH-1]; // Sin : Sqrt
  always @ (posedge clock or negedge ngreset) begin
    if(!ngreset) begin
      AN <= 0;
      XN <= 0;
      YN <= 0;
    end
    else
      if(reset) begin
        AN <=  0;
        XN <=  0;
        YN <=  0;
      end
      else
        if (d & strobeData)    begin 
          AN <=  A0 + aRom; 
          XN <=  X0 + (Y0>>>RSHIFT);
          YN <=  Y0 - (X0>>>RSHIFT);
        end
        else if (strobeData)      begin 
          AN <=  A0 - aRom; 
          XN <=  X0 - (Y0>>>RSHIFT);
          YN <=  Y0 + (X0>>>RSHIFT);
        end      // processing
  end  // always
  
endmodule