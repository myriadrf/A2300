`timescale 1ns / 1ps

// Name:  WcaRegisterBus.v
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

module WcaRegisterBus
  ( 
    //Module control.
    input reset, 
    
    // DMA Bus interface.
    input csel, 
    input cpuclock,
    input ASTB, 
    input nOE,
    input nWE, 
    input wire [5:0]  abus,
    inout wire [15:0] dbus, 

    //Internal Interface.
    output  wire  [12:0] rbusCtrl,     //DMA address and control lines { addr[7:0], nAddrStrobe, nReadStrobe, nWriteStrobe, cpuclk }
    inout   wire  [15:0] rbusData    
  );  
  
	assign dmaData = dbus;
	assign dmaCtrl[13:10] = 4'd0;
	assign dmaCtrl[9:4]   = abus[5:0];
	assign dmaCtrl[3]     = ASTB | csel;
	assign dmaCtrl[2]     = nOE;
	assign dmaCtrl[1]     = nWE;
	assign dmaCtrl[0]     = cpuclock;
              
endmodule // dma_interface