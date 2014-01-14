`timescale 1ns / 1ps

// Name:  WcaDspCounter.v
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

module WcaDspCounter(
		input clock,
		input reset,		// Resets the counter.
		input	bUp,   		//(1) Counts up, (0) counts down.
		input data,			//Data value to load.
		input load,			//(1) loads the data.
		output count		//Current Count value.
    );
	 parameter MAXBITS = 24;

	wire clock, reset, bUp, load;
	wire [MAXBITS : 0] data;
	wire [MAXBITS : 0] count;
	
	//Implment internal counter.
	DspCounter24 ( .clk(clock), .up( bUp), .load( load | reset), .l( data), .q( count) );


endmodule
