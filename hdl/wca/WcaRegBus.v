// Name:  WcaRegBus.v
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

module WcaRegBus
  ( 
	 input clock, 	//System Clock.
    //Module control.
    input reset, 
    
    // DMA Bus interface.
    input address,  	//address Enable
    input write,	//Write Enable
    input read, 	//Read Enable
    inout wire [7:0] dbus,
	 
    //Internal Interface.
    output  wire  [11:0] rbusCtrl,  // Address and control lines(12 total) { addr[7:0], readEnable, writeEnable, dataStrobe, clkbus}
    inout   wire  [7:0]  rbusData	// Tri-state I/O data.
  );  
  
	wire wSync, wStart, wEnd;
	wire rSync, rStart, rEnd;
	wire sSync, aStart, aEnd;
	
	//Contstruct the Control Buss.
	//assign rbusCtrl = { AddressReg, rSync, wStart, rEnd | wEnd, clock};
	assign rbusCtrl[11:4]  = AddressReg;
	assign rbusCtrl[3]     = rSync;
	assign rbusCtrl[2]     = wStart;
	assign rbusCtrl[1]     = rEnd | wEnd;
	//assign rbusCtrl[0]     = clock;
	WcaPassthrough pthruclk(.a(clock), .b(rbusCtrl[0]));	
	
	//Synchronize all input signals. Introduces 2 clock cycle latency.
	WcaSynchEdgeDetect sedAddr ( .clk(clock), .in(address),  .out(aSync), .rising( aStart), .falling( aEnd));
	WcaSynchEdgeDetect sedRead ( .clk(clock), .in(read),     .out(rSync), .rising( rStart), .falling( rEnd));
	WcaSynchEdgeDetect sedWrite( .clk(clock), .in(write),    .out(wSync), .rising( wStart), .falling( wEnd));
	
   //Store Address in register on falling edge of address enable.
   reg  [7:0] AddressReg;
	always @(posedge clock)
	begin
		if( reset)
			AddressReg <= 8'd0;
		else if( aStart)
			AddressReg <= dbus;
	end
		
   //Map the input signals to the internal busses.
	//assign rbusData
	WcaPassthrough #(.width(8)) pthruData(.a(dbus), .b(rbusData));
	

endmodule 


 
