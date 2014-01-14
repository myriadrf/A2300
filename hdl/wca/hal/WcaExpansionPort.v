`timescale 1ns / 1ps
// Name:  WcaExpansionPort.v
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
module WcaExpansionPort( clk, reset, ctrl1, ctrl2, epin, epout );
	input clk;
	input reset;
	input ctrl1;
	input ctrl2;
	input  [2:0] epin;
	output [2:0] epout;
	
	reg [5:0] buff;

	assign epout = buff;

	//Latch the flag state.
	always @(posedge clk)
	begin
		if(reset)
			buff <= 6'h0;
		else if( ctrl1 & ctrl2)
			buff <= epin;
	end	


endmodule
