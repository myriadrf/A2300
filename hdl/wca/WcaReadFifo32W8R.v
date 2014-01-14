`timescale 1ns / 1ps

// Name:  WcaReadFifo32W8R.v
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

module WcaReadFifo32(
	 input   wire         reset,      //Active Hi
	 
	 input 	wire 			 clock_in,		 // Clock input to fifo.
	 input   wire         enable_in,  //Allows input if specified.
	 input   wire  [31:0] in,			 // Clock data input.	
	
	 output  wire   empty,				 // Active high indicates buffer is empty.	 
	 output  wire   full,				 // Active high indicates buffer is empty.
	 
	
   //data Interface.
    input   wire  [11:0] rbusCtrl,  // Address and control lines(12 total) { addr[7:0], readEnable, writeEnable, dataStrobe, clkbus}
    inout   wire  [7:0]  rbusData	// Tri-state I/O data.

	
    );
	 parameter my_addr = 0;

	wire [7:0] dout;
	wire addrValid = (my_addr == rbusCtrl[11:4]);

	//Place data on the buss if reading.
	assign rbusData =  ( addrValid & rbusCtrl[3] )   ? dout : 8'bz;

	FifoCore32w8r fifo32w8r(
	  .rst(reset), // input rst
	  .wr_clk(clock_in), // input wr_clk
	  .rd_clk(rbusCtrl[0]), // input rd_clk
	  .din(in), // input [31 : 0] din
	  .wr_en(enable_in), // input wr_en
	  .rd_en(rbusCtrl[1] & addrValid), // input rd_en
	  .dout(dout), // output [7 : 0] dout
	  .full(full), // output full
	  .empty(empty), // output empty
	  .prog_full(), // output prog_full
	  .prog_empty() // output prog_empty
	);
	
endmodule
