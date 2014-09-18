`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   17:04:55 07/15/2014
// Design Name:   cic_decim
// Module Name:   U:/GitHubA2300_ltxdev/hdl/test/cic_decim_stimulus.v
// Project Name:  Asr2300FpgaCore_2chrx
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: cic_decim
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module cic_decim_stimulus;

parameter DSPCLK_PERIOD =   1000.0/128.0; //128 MHz

	// Inputs
	reg sclr;
	wire nd;
	reg reset;
	reg dspclk;
	reg rate_we;
	reg [11:0] din;
	reg [12:0] rate;

	// Outputs
	wire rfd;
	wire rdy;
	wire [15:0] dout;

	//create 32 MHz strobed
	reg [1:0] strobe_nd;
		always @(posedge dspclk)
	begin
		if(reset) strobe_nd <= 4'h0;
		else 		 strobe_nd <= strobe_nd + 4'h1;
	end
	assign nd = strobe_nd == 0;


	// Instantiate the Unit Under Test (UUT)
	cic_decim uut (
		.sclr(sclr | reset), 
		.rfd(rfd), 
		.rdy(rdy), 
		.nd(nd), 
		.clk(dspclk), 
		.rate_we(rate_we), 
		.dout(dout), 
		.din(din), 
		.rate(rate)
	);

	initial begin
		// Initialize Inputs
		sclr = 0;
		dspclk = 1;
		rate_we = 0;
		din = 12'h010;
		rate = 12'h4;

		// Wait 100 ns for global reset to finish
		#100	reset = 1;
		#(DSPCLK_PERIOD*4)reset = 0;
		#(DSPCLK_PERIOD)  rate_we = 1;
		#(DSPCLK_PERIOD)  rate_we = 0;
		
		#(DSPCLK_PERIOD*120)  sclr = 1;
      #(DSPCLK_PERIOD)      sclr = 0;  
      #(DSPCLK_PERIOD*64)   rate_we = 1;  
      #(DSPCLK_PERIOD)      rate_we = 0;  
		// Add stimulus here
      #(DSPCLK_PERIOD*75)   rate_we = 1;  
      #(DSPCLK_PERIOD)      rate_we = 0;  
	end
	
//////////////////////////////////////////////////////////////////////
// Clocks
//////////////////////////////////////////////////////////////////////			
always @(dspclk)
    #(DSPCLK_PERIOD / 2.0) dspclk<= !dspclk;

	
endmodule

