`timescale 1ns / 1ps
`include "hal/WcaPortDefs.h" //grab register addresses.

// Name:  WcaPortWrite.v
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

module WcaPortWrite(
	 input 	wire				reset,
	 input 	wire				port_enable, //Enables/Disable port writing.
	 
	 input 	wire 			 	wr_clk,	// Clock input to fifo.
	 input   wire         	wr_en,  	//Allows input if specified.
	 input   wire  [31:0] 	wr_in,	// Clock data input.	
	
	 output  wire   			empty,	// Active high indicates buffer is empty.	 
	 output  wire   			full,		// Active high indicates buffer is full.
	 
	 output  wire   			prog_empty,		// Active high indicates buffer is almost empty at or below 160 samples.
	 output  wire   			prog_full,		// Active high indicates buffer is almost full at or above 352 samples.	 

	 //Port Controller interface.	 
	 inout  [31:0]  						pifData,		// 32 bit port interface data bus.
	 input  wire [(NBITS_ADDR+2):0] 	portCtrl, 	// Port State Control bits {addr[NBITS_ADDR:0], read, write, clk }
	 output wire [1:0]				  	portCmd  	// Port Command ID
 
    );
parameter ADDR_PORT  = 0;
parameter NBITS_ADDR = 2;

parameter TEST_PATTERN = 0;

	wire [31:0] dout;
	wire isAddr  = (ADDR_PORT == portCtrl[NBITS_ADDR+2:3]);	
	wire rd_en   = isAddr & portCtrl[1];

	//Request a read of data from the FIFO Slave interface.
	assign 		portCmd = (isAddr) ? ((prog_full & port_enable) ? `PIFCMD_WRITE : `PIFCMD_IDLE) : 2'bz;

	//Buffer the data.
	FiFo512Core32W32R fifo512 (
	  .rst(reset), // input rst
	  .wr_clk(wr_clk), // input wr_clk
	  .rd_clk(portCtrl[0]), // input rd_clk
	  .din(wr_in), // input [31 : 0] din
	  .wr_en(wr_en & port_enable), // input wr_en
	  .rd_en(rd_en), // input rd_en
 	  .dout(dout), // output [31 : 0] dout
	  .full(full), // output full
	  .empty(empty), // output empty
	  .prog_full(prog_full),  // output prog_full  threhold at  388  DWORDS
	  .prog_empty(prog_empty) // output prog_empty threshold at 256 DWORDS	   
	);	
 
	
generate if( TEST_PATTERN == 1)	
 begin
	//Place data on the buss if reading.
	assign 		pifData =  ( rd_en )   ?  ctr : 32'bz;	

	//Writes a sequence of interface out the Port, use for USB Troubleshooting.
	reg [31:0] ctr;
	always @(posedge portCtrl[0])
	begin
		if( reset)
			ctr <= 32'h0;
		else if( rd_en )
			ctr <= ctr + 32'h1;
   end
 end	
else
 begin	
	
	//Place data on the buss if reading.
	assign 		pifData =  ( rd_en )   ?  dout : 32'bz;		
 
end
endgenerate

endmodule
