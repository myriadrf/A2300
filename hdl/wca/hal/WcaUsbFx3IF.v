`timescale 1ns / 1ps
`include "WcaPortDefs.h" //grab register addresses.
`define  BURST_ZERO  9'h0

// Name:  WcaUsbFx3IF.v
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

module WcaUsbFx3IF(
	 input clk_in,					//Interface Clock Input
    input reset,					//resets the interfaces.
	 
	 
	 //Cypress Interface 
	 input flagA,					//Flag A  -- Current Thread Full
	 input flagB,					//Flag B	 -- Current Thread Empty
	 input flagC,					//Flag C
	 input flagD,					//Flag D
    //output nEpSwitch,			// Thread assert switch 
	 output nSlcs,					//FX-3 Chip Select (assert low), must be zero to control the Fx-3
    output nPktEnd,				//Packet end signal (active low) to write a short packet to FX3 slave FIFO.
    output [NBITS_ADDR-1:0] addr,			// [1:0] 2 bit addressing mode, [4:0] 5 bit address mode.
    output nSlWr,					//Fx-3 Write Strobe (active low) assert to perform write transfers from FPGA  to FX3 FIFO
    output nSlRd,				 	//Fx-3 Read  Strobe (active low) assert to perform read transfers to FPGA from FX3 FIFO
    output nSlOe,					//Fx-3 Output Enable (active low) assert causes FX-3 to drive data.  Asserts before Read
    output clk,					//Data bus clock.
    inout [31:0] data,			//32bit data bus.
	 inout [3:0]  gpio,			//Available GPIO Lines
	 
	 //Port Interface
	 inout  [31:0]  pifData,					// 32 bit port interface data bus.
	 input  [(NBITS_ADDR+1):0]  pifCtrl,  // {addr[NBITS_ADDR:0], iocmd[1:0] }
	 output [6:0]	 pifStatus					// I/O Status  {fifo_full[0], fifo_empty[0], ioBusy, ioState[2:0], clk}
	 
    );

	parameter NBITS_ADDR = 2;
	parameter [9:0] BURST_COUNT	 = 9'h80; //default 128 (80h) DWORDS (512 bytes) USB 2.0, can increase to 256 (100h) DWORDS  
														 // (1024 bytes) for optimal USB 3.0 performance.
	`define  LATENCY_ZERO 2'h0
	`define  LATENCY_ONE  2'h1
	
	//Registers and wires
   reg             nSlCs_i		= 1'b1 ;
   reg             nPktEnd_i	= 1'b1 ;
	reg             nSlRd_i 	= 1'b1;
	reg				 nSlOe_i 	= 1'b1;
   reg             nSlWr_i		= 1'b1 ;
  
   reg [2:0] 	    ioState = `PIFSTAT_IDLE;
	reg				 ioBusy	= 1'b0; //indicates IO State machine is busy.
	
	
   //Counters for state machine.
	reg [8:0]		 ctBurst   = `BURST_ZERO;  //Burst is maximum 512 (4 byte samples) = 2048 bytes
	reg [1:0]		 ctLatency = `LATENCY_ZERO; 
	
	//**************************************************
	// Status and Control
	//**************************************************

	//Set up iostatus.
	assign pifStatus[6] 	= flagA;  //Current Thread Full
	assign pifStatus[5] 	= flagB;	 //Current Thread Empty
	assign pifStatus[4] 	= ioBusy;	 //Current Thread Empty
	assign pifStatus[3:1] = ioState;
	WcaPassthrough pthruClk(.a(clk_in), .b(pifStatus[0]));	

	//Set up address.
	assign addr = pifCtrl[NBITS_ADDR+1:2];

	//**************************************************
	// Burst I/O State Machine 
	//**************************************************
	assign nSlcs 	= nSlCs_i;
	assign nPktEnd = nPktEnd_i;
	assign nSlRd   = nSlRd_i;
	assign nSlOe   = nSlOe_i;
	assign nSlWr   = nSlWr_i;
  
	
	always @(posedge clk_in)   
		begin: PIFSTAT_State_Machine     
			if( reset)
				begin
					ioBusy  <= 1'b0;
					ioState <= `PIFSTAT_IDLE;
				end
			else
				//State machine Generates FX3 Slave interface signals.
				case (ioState)        
					`PIFSTAT_IDLE:
						begin
							nPktEnd_i 	<= 1'b1;
							nSlOe_i 		<= 1'b1;
							nSlRd_i 		<= 1'b1;
							nSlWr_i 		<= 1'b1;
							nSlCs_i 		<= 1'b0;
							ctBurst 		<= `BURST_ZERO;
							ioBusy 		<= 1'b0;
							
							//FLAG A denotes full/empty (active low) on the current thread (full==write, empty==read);
							//If command is read and not empty buffer, start read op.
							if( (pifCtrl[1:0] == `PIFCMD_READ) && flagA)    ioState <= `PIFSTAT_READ_BEGIN;
							//If command is write and not full buffer, start write op.
							else if( (pifCtrl[1:0] == `PIFCMD_WRITE) && flagA) ioState <= `PIFSTAT_WRITE_BEGIN;
							//else stay in this state.
							else ioState <= `PIFSTAT_IDLE;
						end
					`PIFSTAT_READ_BEGIN:
						begin
							nPktEnd_i 	<= 1'b1;
							nSlOe_i 		<= 1'b0;
							nSlRd_i 		<= 1'b0;
							nSlWr_i 		<= 1'b1;
							nSlCs_i 		<= 1'b0;
							ctBurst 		<= `BURST_ZERO;
							if(~ioBusy)
								begin
									ioBusy <= 1'b1; //Now doing a read, can't interrupt.
									ctLatency <= `READ_BEGIN_CYCLES; //2 cycles latency -1.
								end
							else //Stay in Begin Read until we passed latency.
								begin
									ctLatency 	<= ctLatency - `LATENCY_ONE;
									ioBusy 		<= 1'b1;
									
									if( ctLatency == `LATENCY_ZERO) ioState <= `PIFSTAT_READ;
									else ioState <= `PIFSTAT_READ_BEGIN;
								end
						end
					`PIFSTAT_READ: //Read a burst of data.
						begin
							nPktEnd_i 	<= 1'b1;
							nSlOe_i 		<= 1'b0;
							nSlRd_i 		<= 1'b0;
							nSlWr_i 		<= 1'b1;
							nSlCs_i 		<= 1'b0;
							ioBusy		<= 1'b1;
							
							if( ctBurst < BURST_COUNT- 9'h1)
								begin
									ctBurst <= ctBurst + 9'h1;
									ioState <= `PIFSTAT_READ;
								end
							else //Transition to Idle.
								begin
									ioState 		<= `PIFSTAT_WAITEND;
									ctLatency	<= `READ_WAIT_END_CYCLES;									
								end
						end
					`PIFSTAT_WRITE_BEGIN:
						begin
							nPktEnd_i 	<= 1'b1;
							nSlOe_i 		<= 1'b1;
							nSlRd_i 		<= 1'b1;
							nSlWr_i 		<= 1'b0;
							nSlCs_i 		<= 1'b0;
							ctBurst 		<= `BURST_ZERO;
							ioState 		<= `PIFSTAT_WRITE;
							ioBusy		<= 1'b1;							
							/*
							if( ~ioBusy)
								begin
									ioBusy <= 1'b1;  //Now doing a Write, can't interrupt.
									ctLatency = `LATENCY_ZERO;
								end
							else //Stay in Begin Write until we passed latency.
								begin
									ctLatency 	<= ctLatency - `LATENCY_ONE;
									ioBusy 		<= 1'b1;
									
									if( ctLatency == `LATENCY_ZERO) ioState <= PIFSTAT_WRITE;
									else ioState <= PIFSTAT_WRITE_BEGIN;
								end
							*/
						end
					`PIFSTAT_WRITE: //Write a burst of data.
						begin
							nPktEnd_i 	<= 1'b1;
							nSlOe_i 		<= 1'b1;
							nSlRd_i 		<= 1'b1;
							nSlCs_i 		<= 1'b0;
							ioBusy		<= 1'b1;
							if( ctBurst < BURST_COUNT- 9'h1)
								begin
									ctBurst <= ctBurst + 9'h1;
									nSlWr_i 		<= 1'b0;
									ioState <= `PIFSTAT_WRITE;
								end
							else //Transition to Idle.
								begin
									ioState 		<= `PIFSTAT_WAITEND;
									ctLatency	<= `WRITE_WAIT_END_CYCLES;									
									nSlWr_i 		<= 1'b1;
								end
						end
					`PIFSTAT_WAITEND:
					    begin
						 	nPktEnd_i 	<= 1'b1;
							nSlOe_i 		<= 1'b1; 
							nSlRd_i 		<= 1'b1;
							nSlWr_i 		<= 1'b1;
							nSlCs_i 		<= 1'b0;
							ctBurst 		<= `BURST_ZERO;
							ioBusy 		<= 1'b1;
							if( ctLatency == `LATENCY_ZERO) ioState <= `PIFSTAT_IDLE;
							else ctLatency <= ctLatency - `LATENCY_ONE;
						end
					default:
						begin
							ioBusy  <= 1'b0;
							ioState <= `PIFSTAT_IDLE;
						end
				endcase
		end
						
	//Tie the GPIO to high impedance since they are not used.
	assign gpio =  4'bz;
	
	//************************************
	// CLOCK BUFFER
	//************************************
	
	//Buffer the clock output so internal clock is not driving output pin.
	ODDR2 oddr_clk
	(
		.D0(1'b1),
		.D1(1'b0),
		.C0 (clk_in),
		.C1(~clk_in),
		.Q(clk)
	);
	
	//************************************
	// Data Bus
	//************************************
	
   //Map the input signals to the internal busses.
	//assign rbusData
	WcaPassthrough #(.width(32)) pthruData(.a(data), .b(pifData));
	
endmodule


