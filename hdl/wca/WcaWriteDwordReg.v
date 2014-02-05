
// Name:  WcaWriteDwordReg.v
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

module WcaWriteDwordReg
( 
    input   wire  reset, 
    output  wire  [31:0] out,        //Output data.    

    //Internal Interface.
    input  wire  [11:0] rbusCtrl,  // Address and control lines(12 total) { addr[7:0], readEnable, writeEnable, dataStrobe, clkbus}
    inout   wire  [7:0]  rbusData	// Tri-state I/O data.
);
parameter my_addr = 0;

	wire addrValid = (my_addr == rbusCtrl[11:4]);
	wire write     = addrValid & rbusCtrl[2];   

	//Sequence Registers for 4 pulse write.
	reg [1:0] select;
	always @(posedge rbusCtrl[0])
	begin
		if( reset | ~addrValid)
			select <= 2'h0;
		else if(rbusCtrl[1] & addrValid)
			select <= select + 2'h1;
	end
	
	//Write to the selected register.
 	WcaRegCore8  sr3( .Data(rbusData), .Enable( write & select == 2'h3), .Aclr(reset), .Clock(rbusCtrl[0]), .Q(out[31:24]));	
	WcaRegCore8  sr2( .Data(rbusData), .Enable( write & select == 2'h2), .Aclr(reset), .Clock(rbusCtrl[0]), .Q(out[23:16]));	
	WcaRegCore8  sr1( .Data(rbusData), .Enable( write & select == 2'h1), .Aclr(reset), .Clock(rbusCtrl[0]), .Q(out[15:8]));	
	WcaRegCore8  sr0( .Data(rbusData), .Enable( write & select == 2'h0), .Aclr(reset), .Clock(rbusCtrl[0]), .Q(out[7:0]));	
endmodule //WcaWriteDwordReg