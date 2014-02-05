`timescale 1ns / 1ps
// Name:  WcaInterruptIF.v
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
module WcaInterruptIF
(
    input  wire         clock,              //High speed logic clock
    input  wire         reset,              //Reset functions
	 input  wire  [7:0]  evtsig,		  		  //Up to eight events supported.
	 
    output wire         cpu_interrupt,       // signals the CPU an interrupt occurred. 
	 
	 input  wire  [11:0] rbusCtrl,   // Address and control lines { addr[7:0], readStrobe, writeStrobe, cpuclk }
	 inout   wire  [7:0] rbusData    // Tri-state I/O data.		 
 );
 parameter CTRL_ADDR=0;  //Interrupt register control address.


//****************************************************
//* Event control
//****************************************************
//  MASK WRITE REGISTER( WriteByteRegister)  
//    Write to this register to set the Event mask.  Register is also used to 
// 	clear an event by writing a 0 and 1 to the respective event bit.
//
// bit#  |  Description
//-------|----------------------------------------------------------
//   0     Enable / Disable Event #0
//   1     Enable / Disable Event #1
//   2     Enable / Disable Event #2
//   3     Enable / Disable Event #3
//   4     Enable / Disable Event #4
//   5     Enable / Disable Event #5
//   6     Enable / Disable Event #6
//   7     Enable / Disable Event #7
// 
//  STATUS READ REGISTER( ReadByteRegister)  
//    Read this register to get the Event status.  
// bit#  |  Description
//-------|----------------------------------------------------------
//   0     Event #0 Triggered
//   1     Event #1 Triggered
//   2     Event #2 Triggered
//   3     Event #3 Triggered
//   4     Event #4 Triggered
//   5     Event #5 Triggered
//   6     Event #6 Triggered
//   7     Event #7 Triggered
 
 wire [7:0]  evtMask;
 wire [7:0]  evtStatus;
 wire [7:0]  evtUpdate;
 wire      	 evt;

	 //Combine all signals into general event indicator.
	 assign evt  = (evtsig[7] & evtMask[7]) | (evtsig[6] & evtMask[6])  
					 | (evtsig[5] & evtMask[5]) | (evtsig[4] & evtMask[4]) 
					 | (evtsig[3] & evtMask[3]) | (evtsig[2] & evtMask[2]) 
					 | (evtsig[1] & evtMask[1]) | (evtsig[0] & evtMask[0]) ;
 

	//Get the current interrupt state.
	assign cpu_interrupt = evtStatus[7] | evtStatus[6]
								| evtStatus[5] | evtStatus[4]
								| evtStatus[3] | evtStatus[2]
								| evtStatus[1] | evtStatus[0];

	//Calculate the latest event state update.
	assign  evtUpdate = (evtStatus | evtsig) & evtMask;
	
	//Get Event Enable Mask.
	WcaWriteByteReg #(CTRL_ADDR ) wstatreg
	(.reset(reset), .out( evtMask), .rbusCtrl(rbusCtrl), .rbusData(rbusData) );

	//Latch current event state into the read status register. 
	//Override standard disable write on read function to allow
	//interrupts to happen while register is being processed.
	WcaReadByteReg #(CTRL_ADDR, 1'b1) rstatreg
	( .reset(reset), .clock(clock), .enableIn(1'b1), .in(evtUpdate), 
	.Q( evtStatus), .rbusCtrl(rbusCtrl), .rbusData(rbusData) );

endmodule
