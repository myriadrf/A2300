// Name:  WcaWriteWordReg.v
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

module WcaWriteWordReg
  ( 
    input   wire  reset, 
    output  wire  [15:0] out,        //Output data.    
	 output  wire  		 nd,			 //new data available indicator.	

    //Internal Interface.
    input  wire  [11:0] rbusCtrl,  // Address and control lines(12 total) { addr[7:0], readEnable, writeEnable, dataStrobe, clkbus}
    inout   wire  [7:0]  rbusData	// Tri-state I/O data.
   );
   parameter my_addr = 0;

	wire addrValid = (my_addr == rbusCtrl[11:4]);
	wire write     = addrValid & rbusCtrl[2];   
	assign nd = write & select == 1'h1; 

	//Alternate Register for two pulse read.
	reg select;
	always @(posedge rbusCtrl[0])
	begin
		if( reset | ~addrValid)
			select <= 1'h0;
		else if(rbusCtrl[1] & addrValid) 
			select <= ~select;
	end

	WcaRegCore8  sr1( .Data(rbusData), .Enable( write & select == 1'h1), .Aclr(reset), .Clock(rbusCtrl[0]), .Q(out[15:8]));	
	WcaRegCore8  sr0( .Data(rbusData), .Enable( write & select == 1'h0), .Aclr(reset), .Clock(rbusCtrl[0]), .Q(out[7:0]));	
endmodule // WcaWriteWordReg