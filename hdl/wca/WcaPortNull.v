`timescale 1ns / 1ps
`include "hal/WcaPortDefs.h" //grab register addresses.

// Name:  WcaPortNull.v
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

module WcaPortNull(
	 //Port Controller interface.	 
	 inout  [31:0]  						pifData,		// 32 bit port interface data bus.
	 input  wire [(NBITS_ADDR+2):0] 	portCtrl, 	// Port State Control bits {addr[NBITS_ADDR:0], read, write, clk }
	 output wire [1:0]				  	portCmd  	// Port Command ID
    );

parameter ADDR_PORT = 0;
parameter NBITS_ADDR = 2;
	wire   isAddr  = (ADDR_PORT == portCtrl[NBITS_ADDR+2:3]);
	assign portCmd =  (isAddr ) ? `PIFCMD_IDLE : 2'bz;
endmodule
