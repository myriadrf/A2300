//////////////////////////////////////////////////////////////////////////////////
// Company: Loctronix Corporation
// Engineer: Michael B. Mathews
// 
// Create Date:    21:44:12 06/12/2013 
// Design Name: 
// Module Name:    4ChTxRx.v
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//  Container implementent 4 channel transmit and receive paths.
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
`include "2ChRxRegisterDefs.vh"

`define  IDENTIFIER	16'h0001		//2 CH Transceiver Identifier.
`define  VERSION		16'h0106		//[ver].[rev]
`define  PORTCAPS		16'h0300		//2 RX / 0 TX porta defined.
`define  PORT_COUNT  4
`define  PORT0_ADDR  2'h0  //TX Port 0 is EP 8h 
`define  PORT1_ADDR  2'h1	//Rx Port 1 is EP 88h
`define  PORT2_ADDR  2'h2	//TX Port 2 is EP 9h
`define  PORT3_ADDR  2'h3	//RX Port 3 is EP 89h
`define  PORT0_CONFIG 0 //
`define  PORT_ADDR_MODE_SEQUENTIAL  0
`define  PORT_ADDR_MODE_FIXED		  1

//Following macros define the port mode and fixed address
//Changing these changes what is active.
`define PORT_MODE 		`PORT_ADDR_MODE_SEQUENTIAL
`define PORT_FIXED_ADDR `PORT1_ADDR

//DEFINE DDC / DUC Register addresses using WCA Component structure.
//This is defined in the 4CHTxRxRegisterDefs.vh.
`define DUC0_CTRL  (`WCACOMP_DSP_DUC0 + `DSP_DDUC_CTRL)
`define DUC0_FREQ  (`WCACOMP_DSP_DUC0 + `DSP_DDUC_PHASERATE)
`define DUC0_INTEG (`WCACOMP_DSP_DUC0 + `DSP_DDUC_SAMPRATE)
`define DDC0_CTRL  (`WCACOMP_DSP_DDC0 + `DSP_DDUC_CTRL)
`define DDC0_FREQ  (`WCACOMP_DSP_DDC0 + `DSP_DDUC_PHASERATE)
`define DDC0_DECIM (`WCACOMP_DSP_DDC0 + `DSP_DDUC_SAMPRATE)
`define DUC1_CTRL  (`WCACOMP_DSP_DUC1 + `DSP_DDUC_CTRL)
`define DUC1_FREQ  (`WCACOMP_DSP_DUC1 + `DSP_DDUC_PHASERATE)
`define DUC1_INTEG (`WCACOMP_DSP_DUC1 + `DSP_DDUC_SAMPRATE)
`define DDC1_CTRL  (`WCACOMP_DSP_DDC1 + `DSP_DDUC_CTRL)
`define DDC1_FREQ  (`WCACOMP_DSP_DDC1 + `DSP_DDUC_PHASERATE)
`define DDC1_DECIM (`WCACOMP_DSP_DDC1 + `DSP_DDUC_SAMPRATE)


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
	assign ep_io = 8'h0;
							
//*******************************************
// USB PORT Interface  Control  
//*******************************************
	wire  [4:0]  portCtrl;
	wire  [1:0]  portCmd;	
		
	//Assign the LEDs to indicate which ports are enabled [ rx1, rx0, tx1, tx0]
	assign ledSelect = { rx1_full, rx1_empty, rx0_full, rx0_empty}; 							

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
	assign evtsig = { 3'h0, evt_fifoerr, rx1_full, tx1_empty, rx0_full, tx0_empty}; 

	//Generate an event if the fifos.full.
	assign evt_fifoerr = rx1_full | rx1_empty |
							   tx1_full | tx1_empty |
								rx0_full | rx0_empty |
								tx0_full | tx0_empty;

//*******************************************
// Low Speed Data Ports
//*******************************************
/*
	wire [23:0] lsdp_rate;
	wire [7:0]  lsdp_control;

	//8bit LSDP Control Register
	WcaWriteByteReg #(`WCAHAL_LSDP_CONTROL) wr_lsdp_control
							(.reset(reset), .out( lsdp_control), 
							 .rbusCtrl(rbusCtrl), .rbusData(rbusData) );	

	//24bit Data Rate Configuration Register.
	WcaWriteDwordReg #(`WCAHAL_LSDP_RATE) wr_lsdp_rate
							(.reset(reset), .out( lsdp_rate), 
							 .rbusCtrl(rbusCtrl), .rbusData(rbusData) );	
 
	//Keep track of fifo status.
	WcaReadByteReg #(`WCAHAL_LSDP_STATUS) lsdpstatus
	( .reset(reset), .clock(clkSample), .enableIn(1'b1), 
		.in({ 
				rx1_full, rx1_empty,
				tx1_full, tx1_empty,
				rx0_full, rx0_empty,
				tx0_full, tx0_empty
		}), 
	  .rbusCtrl(rbusCtrl), .rbusData(rbusData) );

	//Data Create one sample clock for now.
	assign clkLsdpSample = tx0_strobe; // Use a single clock for everything.
	
	//Generate lsdp strobe given the rate provided by the user.
	WcaDspStrobe  lsdp_strobe_gen (
		.clock(clkLsdpSample),  	.reset(reset), .enable(1'h1), 
		.rate(rate), .strobe(lsdp_strobe), .count( strobecount)
	);	
	  
	  
*/

	//******************************************
	// Channel TX0 Implementation.
   //*******************************************
	
	wire tx0_full, tx0_empty;
	wire [23:0] bbtx0_iq;
	assign tx0_full = 1'b0;
	assign tx0_empty = 1'b0;

/*	
	// DUC0_CTRL (WcaWriteByteRegister)
	wire [7:0] duc0_ctrl;
	WcaWriteWordReg #(`DUC0_CTRL) wr_duc0_ctrl
							(.reset(reset), 
							.out( duc0_ctrl), 
							.rbusCtrl(rbusCtrl), .rbusData(rbusData) );	
	
	WcaReadPort #(.ADDR_PORT(`PORT0_ADDR)) port_read0(
	 .reset(reset | duc0_ctrl[1]),
	 .port_enable( duc0_ctrl[0]),
	 .rd_clk(),	// Clock read input..
	 .rd_en(lsdp_strobe ),  	// Enables reading.
	 .rd_out({bbtx0_iq[7:4], tx0_iq[23:12],bbtx0_iq[3:0], tx0_iq[11:0]}),	// Read Data output.	
	 .empty(),	// Active high indicates buffer is empty.	 
	 .full (),	// Active high indicates buffer is empty.
	 .prog_empty(),
	 .prog_full(),
	 .pifData(pifData),		// 32 bit port interface data bus.
	 .portCtrl(portCtrl), 	// Port State Control bits {addr[NBITS_ADDR:0], read, write, clk }
	 .portCmd(portCmd)  	// Port Command ID
	 );
*/
	//Disable transmit Ports.
	WcaPortNull #(.ADDR_PORT(`PORT0_ADDR)) tx0_port_null 
	( 
		 .pifData(pifData),		// 32 bit port interface data bus.
		 .portCtrl(portCtrl), 	// Port State Control bits {addr[NBITS_ADDR:0], read, write, clk }
		 .portCmd(portCmd)  	// Port Command ID
	);			
	
	/*
	reg [31:00] counter;
	always @(posedge clockDsp)
	begin
		if( reset) counter <= 32'h01B001A0;
		else if( tx0_strobe)counter <= counter + 32'h01000100;
	end
	*/
	
	//Offset and interpolate Baseband frequency.
	WcaUpConverter #(`DUC0_FREQ, `DUC0_INTEG, 1,0,0) tx0_duc
	(
		.clock( clockDsp),					// input Clock  
		.reset( reset),						// input reset	
		.enable( 1'b1	/*portConfig[8]*/),	// input enable
		.dstrobe_in(tx0_strobe),			// input dstrobe_in
		.dstrobe_out(tx0_strobe),   		// input dstrobe_out
		.iq_in( /*counter */{16'hFE00, 16'h0200}),		// input [31:0] iq_in
		.iq_out(tx0_iq),						// ouput [23:0] iq_out

		.rbusCtrl(rbusCtrl), 
		.rbusData(rbusData) 	
	);	
	
	
	//******************************************
	// Channel RX0 Implementation.
   //*******************************************
	wire rx0_full, rx0_empty, rx0_afull, rx0_aempty;
	wire [31:0] rx0_out;
	wire [12:0] rx0_rate;
	wire			rx0_strobewrite;

	// DDC0_CTRL (WcaWriteByteRegister)
	wire [7:0] ddc0_ctrl;
	WcaWriteByteReg #(`DDC0_CTRL) wr_ddc0_ctrl
							(.reset(reset), 
							.out( ddc0_ctrl), 
							.rbusCtrl(rbusCtrl), .rbusData(rbusData) );	


	//Implement DDC with Dynamic configuration enabled.
	WcaDownConverter #(`DDC0_FREQ, `DDC0_DECIM) rx0_ddc
	(	
		.clock( clockDsp),					// input Clock  
		.reset( reset ),						// input reset	
		.enable( ddc0_ctrl[0]),				// input enable
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
		.reset(reset | ddc0_ctrl[1]),
		.port_enable( ddc0_ctrl[0]),
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

	//******************************************
	// Channel TX1 Implementation.
   //*******************************************
	wire   tx1_full, tx1_empty;
	assign tx1_full = 1'b0;
	assign tx1_empty = 1'b0;
/*
	// DUC1_CTRL (WcaWriteByteRegister)
	wire [7:0] duc1_ctrl;
	WcaWriteWordReg #(`DUC1_CTRL) wr_duc1_ctrl
							(.reset(reset), 
							.out( duc1_ctrl), 
							.rbusCtrl(rbusCtrl), .rbusData(rbusData) );	
*/

	//Disable transmit Ports.
	WcaPortNull #(.ADDR_PORT(`PORT2_ADDR)) tx1_port_null 
	( 
		.pifData(pifData),		// 32 bit port interface data bus.
		.portCtrl(portCtrl), 	// Port State Control bits {addr[NBITS_ADDR:0], read, write, clk }
		.portCmd(portCmd)  	// Port Command ID
	);			

	//Offset and interpolate Baseband frequency.
	WcaUpConverter #(`DUC1_FREQ, `DUC1_INTEG, 1,0,0) tx1_duc
	(
		.clock( clockDsp),					// input Clock  
		.reset( reset),						// input reset	
		.enable( 1'b1	/*portConfig[8]*/),	// input enable
		.dstrobe_in(tx1_strobe),			// input dstrobe_in
		.dstrobe_out(tx1_strobe),   		// input dstrobe_out
		.iq_in( /*counter */{16'hFE00, 16'h0200}),		// input [31:0] iq_in
		.iq_out(tx1_iq),						// ouput [23:0] iq_out

		.rbusCtrl(rbusCtrl), 
		.rbusData(rbusData) 	
	);	


	
	//******************************************
	// Channel RX1 Implementation.
   //*******************************************
	wire rx1_full, rx1_empty, rx1_aempty, rx1_afull;
	wire [31:0] rx1_out;
	wire [12:0] rx1_rate;
	wire			rx1_strobewrite;

	// DDC1_CTRL (WcaWriteByteRegister)
	wire [7:0] ddc1_ctrl;
	WcaWriteWordReg #(`DDC1_CTRL) wr_ddc1_ctrl
							(.reset(reset), 
							.out( ddc1_ctrl), 
							.rbusCtrl(rbusCtrl), .rbusData(rbusData) );	

	//Implement DDC 
	WcaDownConverter #(`DDC1_FREQ, `DDC1_DECIM) rx1_ddc
	(
		.clock( clockDsp),					// input Clock  
		.reset( reset ),	// input reset	
		.enable( ddc1_ctrl[0]),				// input enable
		.dstrobe_in(rx1_strobe),			// input dstrobe_in
		.cfgflags( ddc1_ctrl[5:2]),	   // input cfgflags (dynamic mode enabled).		
		.iq_in(rx1_iq),						// input [23:0] diq_in
		.iq_out(rx1_out),						// ouput [31:0] diq_out
		.dstrobe_out(rx1_strobewrite), 	// ouput dstrobe_out
		.rbusCtrl(rbusCtrl), 
		.rbusData(rbusData) 	
	);	

	//Assign Port Address #1 as the DDC RX0 Output.
	WcaPortWrite #(.ADDR_PORT(`PORT3_ADDR)) rx1_port_write
	(
		.reset(reset | ddc1_ctrl[1]),
		.port_enable( ddc1_ctrl[0]),
		.wr_clk(clockDsp),			   // Clock input to fifo.
		.wr_en( rx1_strobewrite ), 	// write.	 
		.wr_in( rx1_out),			   	// Clock data input.	
		.empty( rx1_empty),				// Active high indicates buffer is empty.	 
		.full(  rx1_full),				// Active high indicates buffer is empty.
		.prog_empty(rx1_aempty),
		.prog_full(rx1_afull),	 
		.pifData(pifData),			// 32 bit port interface data bus.
		.portCtrl(portCtrl), 		// Port State Control bits {addr[NBITS_ADDR:0], read, write, clk }
		.portCmd(portCmd)  			// Port Command ID
	);
endmodule