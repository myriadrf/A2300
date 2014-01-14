`timescale 1ns / 1ps

// Name:  WcaWriteFifo8W32R.v
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

module WcaWriteFifo32(
	 input   wire         reset,      //Active Hi
	 
	 input 	wire 			 rd_clk,		// Clock read input..
	 input   wire         rd_en,  	// Enables reading.
	 output  wire  [31:0] out,	   	// Read Data output.	
	
	 output  wire   empty,				 // Active high indicates buffer is empty.	 
	 output  wire   full,				 // Active high indicates buffer is empty.
	
   //data Interface.
    input   wire  [11:0] rbusCtrl,  // Address and control lines(12 total) { addr[7:0], readEnable, writeEnable, dataStrobe, clkbus}
    inout   wire  [7:0]  rbusData	// Tri-state I/O data.
    );
parameter my_addr = 0;

	wire wr_busy   = (my_addr == rbusCtrl[11:4]) & rbusCtrl[2];
	wire wr_en     =  wr_busy & rbusCtrl[1];   

	FifoCore8W32R fifo8W32Rcore (
	  .rst(reset), // input rst
	  .wr_clk(rbusCtrl[0]), // input wr_clk
	  .rd_clk(rd_clk), // input rd_clk
	  .din(rbusData), // input [7 : 0] din
	  .wr_en(wr_busy), // input wr_en
	  .rd_en(rd_en ), // input rd_en
	  .dout(out), // output [31 : 0] dout
	  .full(full), // output full
	  .empty(empty), // output empty
	  .prog_full(),
	  .prog_empty()
	);	 
	

endmodule
