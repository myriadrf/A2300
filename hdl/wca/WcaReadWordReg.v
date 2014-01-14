// Name:  WcaReadWordReg.v
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

module WcaReadWordReg 
( 
	 input   wire         reset,       //Active Hi
	 input   wire         clock,       //System clock, should be synchronous with WcaRegbus
	 input   wire         enableIn,    //Allows input if specified.
	 input   wire  [15:0] in, 
	 
   //Internal Interface.
    input   wire  [11:0] rbusCtrl,  // Address and control lines(12 total) { addr[7:0], readEnable, writeEnable, dataStrobe, clkbus}
    inout   wire  [7:0]  rbusData	// Tri-state I/O data.
);
parameter my_addr = 0;

	wire [7:0] Q0;
	wire [7:0] Q1;
	wire addrValid = (my_addr == rbusCtrl[11:4]);
	wire read      = addrValid & rbusCtrl[3];
	wire enable    = enableIn & ~addrValid; //Latch if address is us.


	//Alternate Register for two pulse read.
	reg select;
	always @(posedge rbusCtrl[0])
	begin
		if( reset | ~addrValid)
			select <= 1'h0;
		else if(rbusCtrl[1] & addrValid) 
			select <= ~select;
	end

	// Only allow latching when addres is not valid.   If preparing for a read, everything must be stable.
	WcaRegCore8  sr1( .Data(in[15:8]), .Enable( enable ), .Aclr(reset), .Clock(clock), .Q(Q1));
	WcaRegCore8  sr0( .Data(in[7:0]),  .Enable( enable ), .Aclr(reset), .Clock(clock), .Q(Q0));

	//Place data on the buss if reading.
	assign rbusData =  (read  & select)   ? Q1 : 8'bz;
	assign rbusData =  (read  & ~select)  ? Q0 : 8'bz;
      
endmodule // WcaReadWordReg