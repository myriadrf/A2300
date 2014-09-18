//////////////////////////////////////////////////////////////////////////////////
// Company: Loctronix Corporation
// Engineer: Michael B. Mathews
// 
// Create Date:    21:44:12 02/24/2014 
// Design Name: 
// Module Name:    HardwareTxExample.v
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//  Container hdl generated frequency sweep transmitter.
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps
`include "../wca/hal/WcaHalRegisterDefs.vh" //grab register addresses.
`include "4ChTxRxRegisterDefs.vh"

`define  IDENTIFIER		16'h0C01		//Custom identifier.
`define  VERSION		16'h0100		//[ver].[rev]
`define  PORTCAPS		16'h0101		//2 RX / 1 TX ports defined.
`define  PORT_COUNT  2
`define  PORT0_ADDR  2'h0  //TX Port 0 is EP 8h 
`define  PORT1_ADDR  2'h1	//Rx Port 1 is EP 88h

`define  PORT_ADDR_MODE_SEQUENTIAL  0
`define  PORT_ADDR_MODE_FIXED		  1

//Following macros define the port mode and fixed address
//Changing these changes what is active.
`define PORT_MODE 		`PORT_ADDR_MODE_SEQUENTIAL
`define PORT_FIXED_ADDR `PORT0_ADDR

`define TX0_MODE 4'hB // TODO: Halfband filter (IPCore) needs to be replaced. Left out for now.

//DEFINE DDC / DUC Register addresses using WCA Component structure.
//This is defined in the 4CHTxRxRegisterDefs.vh.
`define DUC0_CTRL  (`WCACOMP_DSP_DUC0 + `DSP_DDUC_CTRL)
`define DUC0_FREQ  (`WCACOMP_DSP_DUC0 + `DSP_DDUC_PHASERATE)
`define DUC0_INTERP (`WCACOMP_DSP_DUC0 + `DSP_DDUC_SAMPRATE)
`define DDC0_CTRL  (`WCACOMP_DSP_DDC0 + `DSP_DDUC_CTRL)
`define DDC0_FREQ  (`WCACOMP_DSP_DDC0 + `DSP_DDUC_PHASERATE)
`define DDC0_DECIM (`WCACOMP_DSP_DDC0 + `DSP_DDUC_SAMPRATE)

module WcaContainer
(
	input   wire	reset,       //Active Hi
	input   wire	clockData,   //Runs control logics and Data I/O  
	input	  wire   clockDsp,	 //Fundamental clock of Baseband DSP data. All sample clocks derive from this.
	
	//RF0
	input		wire	[23:0] rx0_iq,
	output  	wire	[23:0] tx0_iq,
	input   	wire  		 rx0_strobe,   //Strobe of rx0 baseband data.
	input  	wire			 tx0_strobe,	//Strobe of tx0 baseband data.	
	 
	 
	//RF1
	input   	wire	[23:0] rx1_iq,
	output  	wire	[23:0] tx1_iq,
	input   	wire  		 rx1_strobe, 	//Strobe of rx1 baseband data.
	input  	wire			 tx1_strobe,	//Strobe of tx1 baseband data.

	//LED
	output	wire  [3:0]	 ledSelect, // {led_3, led_2, led_1, led_0}
	output  	wire  [5:0]  rfSelect,	// {rf_Wb2_RxSelect,  rf_Wb1_RxSelect, 
												//  rf_Ism_TxSelect,  rf_Ism_ExtSelect, 
												//	 rf_Gps_ExtSelect, rf_Gps_DcBiasSelect}
												
	//USB Port Interface
	inout  [31:0]  						pifData,	  // 32 bit port interface data bus.	 
	output  wire [(NBITS_ADDR+1):0]  pifCtrl,   // {addr[NBITS_ADDR:0], iocmd[1:0] }
	input   wire [6:0]	 				pifStatus,  // Interface status  {fifo_full[0], fifo_empty[0], ioState[3:0], clk}												
											
	// 8 bit expansion port.
	inout [7:0] ep_io,
	
	output   wire [7:0] 	 evtsig,				   // Event Indicators.
												
   //CPU Interface.
    input   wire  [11:0] rbusCtrl,  // Address and control lines(12 total) { addr[7:0], readEnable, writeEnable, dataStrobe, clkbus}
    inout   wire  [7:0]  rbusData	// Tri-state I/O data.
	 
);
parameter NBITS_ADDR = 2;

//*******************************
// Standard Registers
//*******************************

	//Register returns the waveform id.	
	WcaReadWordReg  #(`WCAHAL_IDWAVEFORM) waveformid ( .clock( clockData), .reset( reset),
							.enableIn( 1'b1), .in(`IDENTIFIER),
							.rbusCtrl(rbusCtrl),.rbusData(rbusData));
	
	//Register returns the version identifier.
	WcaReadWordReg  #(`WCAHAL_VERSION) version ( .clock( clockData), .reset( reset), 	
							.enableIn( 1'b1), .in(`VERSION),
							.rbusCtrl(rbusCtrl),.rbusData(rbusData));

//*******************************
// I/O Control Registers
//*******************************
	//RF Selection Switches
	WcaWriteByteReg #(`WCAHAL_RFCONFIG) wr_rf_select
							(.reset(reset), 
							.out( rfSelect ), 
							.rbusCtrl(rbusCtrl), .rbusData(rbusData) );

//*******************************************
// Expansion Port
//*******************************************
	
   //Ground the expansion port interface.
	//wire dspClkout;
	//assign dpsClkout = clockDsp;
	
	//assign ep_io = 8'hFF;
	
//*******************************************
// USB PORT Interface  Control  
//*******************************************
	wire  [4:0]  portCtrl;
	wire  [1:0]  portCmd;	
	wire  		 clearSynch; //Wire clears all fifos when synch enabled.
	wire  		 enableSynch; // Wire enables all fifos with the synch bit set in the configuration register.
			
	//Assign the LEDs to indicate which ports are enabled [ rx1, rx0, tx1, tx0]
	assign ledSelect = { rx1_full, tx1_empty, rx0_full, tx0_empty}; 							
	//assign ledSelect = { tx0_duc_bbstrobe, tx0_cfg[0], tx0_full, tx0_empty}; 							
	
	//Register returns the version identifier.
	WcaReadWordReg  #(`WCAHAL_PORTCAPS)    rr_port_caps 
						   (.clock( clockData), .reset( reset), 	
							.enableIn( 1'b1),    .in(`PORTCAPS),
							.rbusCtrl(rbusCtrl),.rbusData(rbusData));

	
	//Controller manages reading and writing port data with the USB Interface.  Several
	//Addressing modes are supported depending on needs.
	WcaPortController #(`PORT_COUNT, NBITS_ADDR, `PORT_MODE, `PORT_FIXED_ADDR) port_controller(
		.reset(reset),
		.enable(1'b1),
		.portCtrl( portCtrl), //  Port State Control bits {addr[NBITS_ADDR:0], read, write, clk }
		.portCmd( portCmd),  //  Port Command ID

		//Port Interface
		.pifCtrl(pifCtrl),   // {addr[NBITS_ADDR:0], iocmd[1:0] }
		.pifStatus(pifStatus)  // Interface status  {fifo_full[0], fifo_empty[0], ioState[3:0], clk}
	 );

//*******************************************
// Event mapping
//*******************************************
	wire evt_fifoerr;

	//Assign events Upper four events are not assigned.
	//assign evtsig = { 3'h0, evt_fifoerr, rx1_full, tx1_empty, rx0_full, tx0_empty}; 

	//Generate an event if the fifos.full.
	//assign evt_fifoerr = rx1_full | tx1_empty | rx0_full | tx0_empty;

//******************************************
// Channel TX0 Implementation.
//*******************************************
   	wire tx0_full, tx0_empty, tx0_afull, tx0_aempty;
	wire [7:0]  tx0_cfg;			
	wire [31:0] tx0_phasecordic; 	


	// Control register.    
	// bit#  |  Description
	// -------|----------------------------------------------------------
	//  0	      (1)Enable / (0)Disable Component USB port streaming.
	//  1	      Clear FIFOS and resets counters. 
	WcaWriteByteReg #(`DUC0_CTRL) wr_config
							(.reset(reset), 
							.out( tx0_cfg), 
							.rbusCtrl(rbusCtrl), .rbusData(rbusData) );	
	
	//Generate Phase for frequency sweep.
	WcaSweepGen #(`DUC0_FREQ, 32) phase_generator
	(
	.clock(clock), .reset(reset), .aclr(tx0_cfg[1]), .enable(tx0_cfg[0]), .strobe(tx0_strobe),
	.rbusCtrl(rbusCtrl), .rbusData(rbusData), 
	.phase(tx0_phasecordic)
	);	
	
	//Hard code cordic inputs so we get a nice clean sinusoid.
	wire [11:0] icor_in = 12'd1024;
	wire [11:0] qcor_in = 12'd1024;	
  
    //Frequency must be in the 1Qn format (see cordic doc below).  2^n-1 = pi/2 and -2^n-1 = -pi/2
    //The Cordic only supports values between this range so the phase accumulator must produce values within this range.
    //We rotate quadrants and adjust phase to make work.
    reg [11:0] X0;
    reg [11:0] Y0;
    reg [11:0] A0;

    always @(posedge clock) 
     case({tx0_strobe, tx0_phasecordic[31:30]})
       3'b101 :  //pi/2 to pi   
        begin            
            X0 <= #1 -qcor_in;
            Y0 <= #1 icor_in;
            A0 <= #1 tx0_phasecordic[31:20] - 12'h400;                    
        end
       3'b110 : //-pi/2 to -pi
        begin
            X0 <= #1 qcor_in;
            Y0 <= #1 -icor_in;
            A0 <= #1 tx0_phasecordic[31:20] + 12'h400;                    
        end
       3'b100: // 0 to pi/2
			begin
            X0 <= #1 icor_in;
            Y0 <= #1 qcor_in;
            A0 <= #1 tx0_phasecordic [31:20];
        end
       3'b111: // 0 to -pi/2
			begin
            X0 <= #1 icor_in;
            Y0 <= #1 qcor_in;
            A0 <= #1 tx0_phasecordic [31:20];
        end
    endcase

    // CORDIC gain is about 1.64 and formulation requires that numbers have the format of 1Qn format
    // where there are 1 sign bit plus 1 integer bits and n-1 fractional bits.  For a 12 bit 2's complement number
    // this means that n = 11: 1 sign bit, 1 integer bit, and 10 fractional. For 1Qn format, the maximum allowable 
    // range input is between -2^n-1 and 2^n-1; thus with 12 bits, the maximum range is  -2^10 to 2^10 (-1024 to 1024). 
    // In a 12 bit system, -1024 = -1.0 and 1024 = 1.0.  Converting between decimal and 1Qn is
    //
    //  1Qn = int(1024* float_val);
    //  float_val = 1Qn / 1024f;
    //
    //  2' Complement numbers need to be scaled to conform to this standard.  If value magnitude in X and Y are > 1024 overflow
    //   can occur. This implementation does not scale the input so need to be careful.
    //
    WcaCordic12  #(12,12,0) cordic
    ( 
       .ngreset(1'b1), .clock(clock),.reset(reset | aclr), .strobeData(tx0_strobe),
       .X0(X0), .Y0(Y0),.A0(A0),
       .XN(tx0_iq[11:0]), .YN(tx0_iq[23:12]), .AN() 
    );	 
	
	//Disable transmit Port 0, since we are not modulating information.
	WcaPortNull #(.ADDR_PORT(`PORT0_ADDR)) tx0_port_null 
	( 
		 .pifData(pifData),		// 32 bit port interface data bus.
		 .portCtrl(portCtrl), 	// Port State Control bits {addr[NBITS_ADDR:0], read, write, clk }
		 .portCmd(portCmd)  	// Port Command ID
	);			
		
	
//******************************************
// Channel RX0 Implementation.
//*******************************************
	wire rx0_full, rx0_empty, rx0_afull, rx0_aempty;
	wire [31:0] rx0_out;
	wire [12:0] rx0_rate;
	wire		rx0_strobewrite;

	// DDC0_CTRL (WcaWriteByteRegister)
	wire [7:0] 	ddc0_ctrl;
	wire		clearRx0  = ddc0_ctrl[1];
	wire 		enableRx0 = ddc0_ctrl[0];
	
	WcaWriteByteReg #(`DDC0_CTRL) wr_ddc0_ctrl
							(.reset(reset), 
							.out( ddc0_ctrl), 
							.rbusCtrl(rbusCtrl), .rbusData(rbusData) );	


	//Implement DDC with Dynamic configuration enabled.
	WcaDownConverter #(`DDC0_FREQ, `DDC0_DECIM) rx0_ddc
	(	
		.clock( clockDsp),					// input Clock  
		.reset( reset ),						// input reset	
		.aclr( clearRx0),						// input clears state not configuration
		.enable( enableRx0),					// input enable
		.dstrobe_in(rx0_strobe),			// input dstrobe_in
		.cfgflags( ddc0_ctrl[5:2]),	   // input cfgflags (dynamic mode enabled).
		.iq_in(rx0_iq),						// input [23:0] diq_in
		.iq_out(rx0_out),						// ouput [31:0] diq_out
		.dstrobe_out(rx0_strobewrite), 	// ouput dstrobe_out
		.rbusCtrl(rbusCtrl), 
		.rbusData(rbusData) 	
	);	

	//Assign Port Address #1 as the DDC RX0 Output.
	WcaPortWrite #(.ADDR_PORT(`PORT1_ADDR)) rx0_port_write
	(
		.reset(reset | clearRx0),
		.port_enable( enableRx0),
		.wr_clk(clockDsp),			   // Clock input to fifo.
		.wr_en( rx0_strobewrite ), // Allows input if specified.	 
		.wr_in( rx0_out),			   // Clock data input.	
		.empty( rx0_empty),			// Active high indicates buffer is empty.	 
		.full(  rx0_full),			// Active high indicates buffer is empty.
		.prog_empty(rx0_aempty),
		.prog_full(rx0_afull),	 
		.pifData(pifData),			// 32 bit port interface data bus.
		.portCtrl(portCtrl), 		// Port State Control bits {addr[NBITS_ADDR:0], read, write, clk }
		.portCmd(portCmd)  			// Port Command ID
	);

endmodule

//******************************************
// Module implements a phase generator using a frequency sweep.
//******************************************
module SweepGen ( clock, reset, aclr, enable, strobe, phase, rbusCtrl, rbusData);
	parameter FREQADDR 	= 0;
	parameter RESOLUTION = 32;
	parameter freqStart  = 32'd0;
	parameter freqEnd    = 32'd536870912;
	parameter freqDelta  = 32'd1677722;
	
	input    clock, reset, aclr, enable, strobe;

	//Output
	output  reg [RESOLUTION-1:0] phase;
	reg [RESOLUTION-1:0] freq;

	//CPU Interface.
	input   wire  [11:0] 		  rbusCtrl;  // Address and control lines(12 total) { addr[7:0], readEnable, writeEnable, dataStrobe, clkbus}
	inout   wire  [7:0]  		  rbusData;  // Tri-state I/O data.
	
	//Can map the frequency to 32 bit register so host can set it.	
	//WcaWriteDwordReg #(FREQADDR) dwreg_phasefreq(.reset(reset), .out( freq), .rbusCtrl(rbusCtrl), .rbusData(rbusData) );		

	//implement our own counting setting register.
	always @(posedge clock)
	begin
	  if(reset | aclr)
			begin
				phase <= #1 32'b0;
				freq <=  #1 freqStart; 
			end
	  else if(enable & strobe)
		begin
			phase <= #1 phase + freq;
			freq <= #1 (freq >= freqEnd)? freqStart : (freq + freqDelta) ;
		end
	end
endmodule // phase_acc
