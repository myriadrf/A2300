`timescale 1ns / 1ps
`include "hal/WcaPortDefs.h" //grab register addresses.

// Name:  WcaInputPort.v
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

module WcaReadPort(
	 input 	wire				reset,

	 input 	wire 			 	rd_clk,	// Clock read input..
	 input   wire         	rd_en,  	// Enables reading.
	 output  wire  [31:0] 	rd_out,	// Read Data output.	
	
	 output  wire   			empty,	// Active high indicates buffer is empty.	 
	 output  wire   			full,		// Active high indicates buffer is empty.

	 //Port Controller interface.	 
	 inout  [31:0]  						pifData,		// 32 bit port interface data bus.
	 input  wire [(NBITS_ADDR+2):0] 	portCtrl, 	// Port State Control bits {addr[NBITS_ADDR:0], read, write, clk }
	 output wire [1:0]				  	portCmd  	// Port Command ID
    );

parameter ADDR_PORT = 0;
parameter NBITS_ADDR = 2;

	wire wr_en   = (ADDR_PORT == portCtrl[NBITS_ADDR+2:3]) & rbusCtrl[1];
	
	//Request a read of data from the FIFO Slave interface.
	assign portCmd = (full) ? `PIFCMD_READ : `PIFCMD_IDLE;

	FiFo512Core32W32R your_instance_name (
	  .rst(rst), // input rst
	  .wr_clk(portCtrl[0]), // input wr_clk
	  .rd_clk(rd_clk), // input rd_clk
	  .din(pifData), // input [31 : 0] din
	  .wr_en(wr_en), // input wr_en
	  .rd_en(rd_en), // input rd_en
	  .dout(rd_out), // output [31 : 0] dout
	  .full(full), // output full
	  .empty(empty) // output empty
	);

endmodule
