// Name:  WcaPhaseGen.v
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

module WcaPhaseGen ( clock, reset, aclr, enable, strobe, phase, rbusCtrl, rbusData);
	parameter FREQADDR 	= 0;
	parameter RESOLUTION = 32;

	input    clock, reset, aclr, enable, strobe;

	//CPU Interface.
	input   wire  [11:0] 		  rbusCtrl;  // Address and control lines(12 total) { addr[7:0], readEnable, writeEnable, dataStrobe, clkbus}
	inout   wire  [7:0]  		  rbusData;  // Tri-state I/O data.
	output  reg [RESOLUTION-1:0] phase;

	wire [RESOLUTION-1:0] freq;
	
	WcaWriteDwordReg #(FREQADDR) dwreg_phasefreq(.reset(reset), .out( freq), .rbusCtrl(rbusCtrl), .rbusData(rbusData) );		

	//implement our own counting setting register.
	always @(posedge clock)
	begin
	  if(reset | aclr)
			phase <= #1 32'b0;
	  else if(enable & strobe)
			phase <= #1 phase + freq;
	end
endmodule // phase_acc