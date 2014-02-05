// Name:  WcaRssi.v
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

module WcaRssi (
    input clock, 
    input reset, 
    input strobe,
	input [11:0] adc, 
    output [7:0] rssi 
    );
   
   wire [11:0] abs_adc = adc[11] ? ~adc : adc;
   reg [23:0]  rssi_int;

   always @(posedge clock)
     if(reset )
       rssi_int <= #1 24'd0;
     else if (strobe)
       rssi_int <= #1 rssi_int + abs_adc - {1'b0,rssi_int[23:13]};

   assign      rssi = rssi_int[23:16];
    
endmodule // WcaRssi
