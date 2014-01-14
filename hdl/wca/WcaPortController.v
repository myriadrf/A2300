`timescale 1ns / 1ps
`include "hal/WcaPortDefs.h" //grab register addresses.

// Name:  WcaPortController.v
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

module WcaPortController(
	 input 	wire							reset,
	 input	wire							enable,
	 output  wire [(NBITS_ADDR+2):0] 	portCtrl, //  Port State Control bits {addr[NBITS_ADDR:0], read, write, clk }
	 input   wire [1:0]				  		portCmd,  //  Port Command ID

	 //Port Interface
	 output  wire [(NBITS_ADDR+1):0] pifCtrl,   // {addr[NBITS_ADDR:0], iocmd[1:0] }
	 input   wire [6:0]	 				pifStatus  // I/O Status  {fifo_full[0], fifo_empty[0], ioBusy, ioState[2:0], clk}
    );

	parameter COUNT_PORTS = 1;
	parameter NBITS_ADDR = 2;
	parameter ADDR_MODE  = 0;
	parameter ADDR_OVERRIDE = 0;
	
	`define PORT_ADDR_MODE_SEQUENTIAL  0
	`define PORT_ADDR_MODE_FIXED		  1
	
	//State machine states.
	`define S_IDLE  			3'h0  //Idling state  Fixed addressing mode only.
	`define S_ADDR  			3'h0  //Addressing state.  Sequential addressing mode only.
	`define S_ADDR_WAIT1 	3'h1  //Addressing wait state #1. Sequential addressing mode only.
	`define S_ADDR_WAIT2 	3'h2  //Addressing wait state #2. Sequential addressing mode only.
	`define S_ADDR_WAIT3 	3'h3  //Addressing wait state #3. Sequential addressing mode only.
	`define S_CMD   			3'h4	//Command Initation
	`define S_CMD_WAIT1  	3'h5	//Wait for IF to settle #1
	`define S_CMD_WAIT2  	3'h6	//Wait for IF to settle #2
	`define S_WAIT_COMPLETE 3'h7	//Wait for Command Completion.
	
	`define ADDR_INCREMENT  2'h1; //Increment address by one.


	reg [NBITS_ADDR-1:0] addr;
	reg [2:0] cmdState;
	reg [1:0] cmd;
	wire read;  
	wire write;

	//Map Inputs to Outputs.
	assign read   = pifStatus[3:1] == `PIFSTAT_READ;
	assign write  = pifStatus[3:1] == `PIFSTAT_WRITE;
	assign pifCtrl = { addr, cmd};
	assign portCtrl[NBITS_ADDR+2: 1] = {addr, read, write};
	WcaPassthrough pthruClk(.a(pifStatus[0]), .b(portCtrl[0]));		

	//Control processing of port data based on count.
generate if( ADDR_MODE == `PORT_ADDR_MODE_SEQUENTIAL)
  begin
	always @(posedge pifStatus[0])
		begin :PIFCMD_STATE_MACHINE     
			if( reset)
				begin
					addr  <= COUNT_PORTS-1;
					cmdState <= `S_ADDR;
					cmd <= `PIFCMD_IDLE;
				end
			else
				//State machine performs addressing, command, and wait processing.
				case (cmdState)        
					`S_ADDR:
						begin
							cmd <= `PIFCMD_IDLE; 
							if( enable)
							 begin
								cmdState <= `S_ADDR_WAIT1;
								if( addr == COUNT_PORTS-1)	addr <= 0;
								else addr <= addr + `ADDR_INCREMENT;
							 end
						end
					`S_ADDR_WAIT1:  //Wait one for address to settle.
						begin
							cmdState <= `S_ADDR_WAIT2;
						end

					`S_ADDR_WAIT2:  //Wait another for address to settle.
						begin
							cmdState <= `S_ADDR_WAIT3;
						end
				
					`S_ADDR_WAIT3:  //Wait another for address to settle.
						begin
							cmdState <= `S_CMD;
						end					
					`S_CMD:
						begin
							cmd <= portCmd;
							if( portCmd != `PIFCMD_IDLE)
								begin
									cmdState <= `S_CMD_WAIT1;
								end
							else
								begin
									cmdState <= `S_ADDR;
								end
						end
					`S_CMD_WAIT1:  //Wait one clock for the IF to catch up.
						begin
							cmdState <= `S_CMD_WAIT2;
						end
					`S_CMD_WAIT2:  //Wait another clock for the IF to catch up.
						begin
							cmdState <= `S_WAIT_COMPLETE;
						end
					`S_WAIT_COMPLETE:
						begin
							cmd <= `PIFCMD_IDLE;
							if( ~pifStatus[4] )  //if no longer busy, return to idle.
								cmdState <= `S_ADDR;
						end
				endcase
		end
  end

//*******************************************************************************
// FIXED ADDRESSING MODE.
//*******************************************************************************
else if( ADDR_MODE == `PORT_ADDR_MODE_FIXED)
  begin
	
	always @(posedge pifStatus[0])
		begin :PIFCMD_STATE_MACHINE     
			if( reset)
				begin
					addr  <= ADDR_OVERRIDE;
					cmdState <= `S_IDLE;
					cmd <= `PIFCMD_IDLE;
				end
			else
				//State machine performs addressing, command, and wait processing.
				case (cmdState)        
					`S_IDLE:
						begin
							cmd <= `PIFCMD_IDLE; 
							if( enable)
								cmdState <= `S_CMD;
						end
					`S_CMD:
						begin
							cmd <= portCmd;
							if( portCmd != `PIFCMD_IDLE)
								begin
									cmdState <= `S_CMD_WAIT1;
								end
							else
								begin
									cmdState <= `S_IDLE;
								end
						end
					`S_CMD_WAIT1:  //Wait one clock for the IF to catch up.
						begin
							cmdState <= `S_CMD_WAIT2;
						end
					`S_CMD_WAIT2:  //Wait another clock for the IF to catch up.
						begin
							cmdState <= `S_WAIT_COMPLETE;
						end
					`S_WAIT_COMPLETE:
						begin
							cmd <= `PIFCMD_IDLE;
							if( ~pifStatus[4] )  //if no longer busy, return to idle.
								cmdState <= `S_ADDR;
						end

				endcase
		end
  end
endgenerate

endmodule
