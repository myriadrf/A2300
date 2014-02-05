`timescale 1ns / 1ps

// Name:  WcaStrobe.v
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

module WcaDspStrobe(
		clock,
		reset,		 //Resets the counter.
		strobe_in,   //
		enable,
		rate,			 //Strobe rate.
		strobe_out,	 //strobe signal.
		count			 //Optional Strobe Counter output.
    );
parameter INITIAL_VAL   = 24'd0;
parameter INCREMENT_VAL = 24'h1;

	input wire clock, reset, enable, strobe_in;
	input wire [23: 0] rate;
	output wire strobe_out;
	output reg [23:0] count;
   
	assign strobe_out = count == rate;
   always @(posedge clock)
     if(reset | ~enable | strobe_out)
       count <= #1 INITIAL_VAL;
     else if( strobe_in)
	    count <= #1 count + INCREMENT_VAL;	


	//Implment internal counter.
	/*
	DspCounter24 dspctcor( .clk(clock), .up( 1'b0), .load( strobe | reset), .l( rate), .q( count) );
*/

endmodule
