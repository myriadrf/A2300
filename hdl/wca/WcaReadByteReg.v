// Name:  WcaReadByteReg.v
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

module WcaReadByteReg
( 
	 input   wire         reset,       //Active Hi
	 input   wire         clock,       //System clock, should be synchronous with WcaRegbus
	 input   wire         enableIn,    //Allows input if specified.
	 input   wire  [7:0]  in, 			  //Register Input.
	 output  wire  [7:0]  Q,			  //Current state of the register.
	 //Internal Interface.
    input   wire  [11:0] rbusCtrl,  // Address and control lines(12 total) { addr[7:0], readEnable, writeEnable, dataStrobe, clkbus}
    inout   wire  [7:0]  rbusData	// Tri-state I/O data.
);
parameter my_addr = 0;
parameter AllowWriteOnRead = 1'b0;


	wire addrValid = (my_addr == rbusCtrl[11:4]);
	wire read      = addrValid & rbusCtrl[3];
	wire enable    = enableIn & (~addrValid | AllowWriteOnRead); 

	// Only allow latching when addres is not valid.   If preparing for a read, everything must be stable.
	WcaRegCore8  sr( .Data(in), .Enable( enable ), .Aclr(reset), .Clock(clock), .Q(Q));
	
	//Place data on the buss if reading.
	assign rbusData = (read ) ? Q : 8'bz;
endmodule // WcaReadByteReg
