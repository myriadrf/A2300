`timescale 1ns / 1ps

// Name:  WcaSynchEdgeDetect.v
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

module WcaSynchEdgeDetect(
    input wire clk,
    input wire in,
    output wire out,
    output wire rising,
    output wire falling
    );
	reg  [2:0] sr;
	always @(posedge clk)
	begin
		sr[0] <= in;
		sr[1] <= sr[0];
		sr[2] <= sr[1];
	end
	
	assign rising  = sr[1] & ~sr[2];
	assign falling = ~sr[1] & sr[2];
	assign out = sr[1];

endmodule
