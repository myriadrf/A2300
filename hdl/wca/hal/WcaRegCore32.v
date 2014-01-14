`timescale 1ns / 1ps

// Name:  WcaRegCore32.v
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

module WcaRegCore32 (Data,Enable,Aclr,Clock,Q);
	input [31:0] Data;
	input Enable, Aclr, Clock;
	output [31:0] Q;
	reg [31:0] Q;

	always @(posedge Clock)
	begin
		if( Aclr)
			Q <= 31'h0;
		else if( Enable)
			Q <= Data;
	end
endmodule
