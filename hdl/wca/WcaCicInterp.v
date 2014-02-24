`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    10:11:43 02/19/2014 
// Design Name: 
// Module Name:    WcaCicInterp 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
// CIC Interpolator, 3-Stages
module WcaCicInterp
(
	input wire clock,
	input wire reset,
	input wire enable,

	input wire strobe_cic,
	input wire strobe_if,
	
	input wire [3:0] log2_rate,

	input  wire [15:0] data_in,
	output wire [15:0] data_out
);

//integ2 => #1 integ2 + {{8{integ1[15]}}, integ1}

// Comb (differentiator) registers
reg [24+15:0] comb1;
reg [24+15:0] comb2;
reg [24+15:0] comb3;

// Pipeline registers for combs
reg [24+15:0] pipe1;
reg [24+15:0] pipe2;
reg [24+15:0] pipe3;

// Integrating (accumulator) registers
// Each integrator stage can grow by as much as 8 bits
// assuming a max interpolation rate of 256
reg [24+15:0] integ1;  
reg [24+15:0] integ2;
reg [24+ 15:0] integ3;


//***********************************
// Cascading Combs with Pipeline Regs
//***********************************
always @(posedge clock)
	if(reset | ~enable)
		begin
			pipe1 <= #1 0;
			pipe2 <= #1 0;
			pipe3 <= #1 0;
			comb1 <= #1 0;
			comb2 <= #1 0;
			comb3 <= #1 0;	
		end
	else if (strobe_cic)
		begin
			comb1 <= #1 data_in;
			pipe1 <= #1 data_in - comb1;
			comb2 <= #1 pipe1;
			pipe2 <= #1 pipe1 - comb2;
			comb3 <= #1 pipe2;
			pipe3 <= #1 pipe2 - comb3;
		end

//***********************************
// Cascading Integrators
//***********************************
reg [15:0] data_store;

always @(posedge clock)
	if(reset | ~enable)
		begin
			integ1 <= #1 0;
			integ2 <= #1 0;
			integ3 <= #1 0;
		end
	else if (strobe_if)
		begin
			integ1 <= #1 integ1 + pipe3;//{{8{pipe3[15]}}, pipe3};
			integ2 <= #1 integ2 + integ1;//{{8{integ1[23]}}, integ1};
			integ3 <= #1 integ3 + integ2;//{{8{integ2[31]}}, integ2};
			
			// Scale the output by log2 of interpolation rate
			case(log2_rate)
				4'h1:  data_store = integ3[18:3];
				4'h2:  data_store = integ3[21:6];
				4'h3:  data_store = integ3[24:9];
				4'h4:  data_store = integ3[27:12];
				4'h5:  data_store = integ3[30:15];
				4'h6:  data_store = integ3[33:18];
				4'h7:  data_store = integ3[36:21];
				4'h8:  data_store = integ3[39:24];
				default: data_store = integ3[15:0];
			endcase
		end
		
// Assign the final value coming out of CIC
assign data_out = data_store;

endmodule

