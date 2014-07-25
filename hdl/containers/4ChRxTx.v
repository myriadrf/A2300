//////////////////////////////////////////////////////////////////////////////////
// Company: Loctronix Corporation
// Engineer: Michael B. Mathews
// 
// Create Date:    21:44:12 02/24/2014 
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
`include "4ChTxRxRegisterDefs.vh"

`define  IDENTIFIER	16'h0002		//4 CH Transceiver Identifier.
`define  VERSION		16'h0103		//[ver].[rev]
`define  PORTCAPS		16'h0303		//2 RX / 2 TX ports defined.
`define  PORT_COUNT  4
`define  PORT0_ADDR  2'h0  //TX Port 0 is EP 8h 
`define  PORT1_ADDR  2'h1	//Rx Port 1 is EP 88h
`define  PORT2_ADDR  2'h2	//TX Port 2 is EP 9h
`define  PORT3_ADDR  2'h3	//RX Port 3 is EP 89h

`define  PORT_ADDR_MODE_SEQUENTIAL  0
`define  PORT_ADDR_MODE_FIXED		  1

//Following macros define the port mode and fixed address
//Changing these changes what is active.
`define PORT_MODE 		`PORT_ADDR_MODE_SEQUENTIAL
`define PORT_FIXED_ADDR `PORT0_ADDR

`define TX0_MODE 4'hB // TODO: Halfband filter (IPCore) needs to be replaced. Left out for now.
`define TX1_MODE 4'hB // TODO: Halfband filter (IPCore) needs to be replaced. Left out for now.

//DEFINE DDC / DUC Register addresses using WCA Component structure.
//This is defined in the 4CHTxRxRegisterDefs.vh.
`define DUC0_CTRL  (`WCACOMP_DSP_DUC0 + `DSP_DDUC_CTRL)
`define DUC0_FREQ  (`WCACOMP_DSP_DUC0 + `DSP_DDUC_PHASERATE)
`define DUC0_INTERP (`WCACOMP_DSP_DUC0 + `DSP_DDUC_SAMPRATE)
`define DDC0_CTRL  (`WCACOMP_DSP_DDC0 + `DSP_DDUC_CTRL)
`define DDC0_FREQ  (`WCACOMP_DSP_DDC0 + `DSP_DDUC_PHASERATE)
`define DDC0_DECIM (`WCACOMP_DSP_DDC0 + `DSP_DDUC_SAMPRATE)
`define DUC1_CTRL  (`WCACOMP_DSP_DUC1 + `DSP_DDUC_CTRL)
`define DUC1_FREQ  (`WCACOMP_DSP_DUC1 + `DSP_DDUC_PHASERATE)
`define DUC1_INTERP (`WCACOMP_DSP_DUC1 + `DSP_DDUC_SAMPRATE)
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
	//wire dspClkout;
	//assign dpsClkout = clockDsp;
	
	assign ep_io = 8'hFF;
	
//*******************************************
// USB PORT Interface  Control  
//*******************************************
	wire  [4:0]  portCtrl;
	wire  [1:0]  portCmd;	
		
	//Assign the LEDs to indicate which ports are enabled [ rx1, rx0, tx1, tx0]
	//assign ledSelect = { rx1_full, tx1_empty, rx0_full, tx0_empty}; 							
	assign ledSelect = { tx0_duc_bbstrobe, tx0_duc_cfg[0], tx0_full, tx0_empty}; 							
	
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

	//******************************************
	// Channel TX0 Implementation.
   //*******************************************
	
	wire tx0_full, tx0_empty, tx0_afull, tx0_aempty;
	wire [31:0] iq_port0;
	wire [23:0] bbtx0_iq;
	wire [7:0]  tx0_duc_cfg;			
	wire [12:0] tx0_duc_rateinterp;	
	wire			tx0_duc_rateinterp_we;	
	wire       	tx0_duc_cicstrobe;   	
	wire 			tx0_duc_bbstrobe;
	wire [31:0] tx0_duc_phasecordic; 	
	wire [3:0]  tx0_log2_rate;

	// tx0_cicStrobe strobes the data sent from the CIC filter to the CORDIC
	WcaDducController #(`DUC0_FREQ, `DUC0_INTERP, `DUC0_CTRL, `TX0_MODE	) tx0_DucController(
			 .clock(clockDsp),					//input clock
	       .reset(reset),						//input
			 .enable(tx0_duc_cfg[0]),						//input 
			 .strobe_if(tx0_strobe),
			 
			 //Bind controller signals.
			 .cfg( tx0_duc_cfg),								// output signal, read inside DDUC Controller
			 .rate_interp(tx0_duc_rateinterp),			// output interp rate for CIC in DUC
			 .rate_interp_we(tx0_duc_rateinterp_we),
			 .log2_rate(tx0_log2_rate),
			 
			 .strobe_cic(tx0_duc_cicstrobe),   	//output CIC baseband strobe rate
			 .strobe_bb( tx0_duc_bbstrobe),
			 .phase_cordic( tx0_duc_phasecordic),
			 
			 //CPU Interface.
			 .rbusCtrl(rbusCtrl),   		// input address and control lines(12 total) { addr[7:0], readEnable, writeEnable, dataStrobe, clkbus}
			 .rbusData(rbusData)			// inout Tri-state I/O data, 8 bits
	);

	
	WcaReadPort #(.ADDR_PORT(`PORT0_ADDR)) tx0_port_read(
	 .reset(reset | tx0_duc_cfg[1]),
	 .port_enable( tx0_duc_cfg[0]),
	 .rd_clk(clockDsp),						// Clock read input..
	 .rd_en(tx0_duc_bbstrobe),  			// Enables reading.
	 .rd_out(iq_port0),						// Read Data output.	
	 .empty(tx0_empty),						// Active high indicates buffer is empty.	 
	 .full (tx0_full),						// Active high indicates buffer is empty.
	 .prog_empty(tx0_aempty),
	 .prog_full(tx0_afull),
	 .pifData(pifData),						// 32 bit port interface data bus.
	 .portCtrl(portCtrl), 					// Port State Control bits {addr[NBITS_ADDR:0], read, write, clk }
	 .portCmd(portCmd)  						// Port Command ID
	 );


	//Offset and interpolate Baseband frequency.
	WcaUpConverter #(`TX0_MODE) tx0_duc
	(
		.clock( clockDsp),					// input Clock  
		.reset( reset),						// input reset	
		.enable(tx0_duc_cfg[0]),			// input enable
		
		.cfg( tx0_duc_cfg),
		.rate_interp(tx0_duc_rateinterp),
	   .rate_interp_we(tx0_duc_rateinterp_we),		
		.strobe_cic(tx0_duc_cicstrobe),		
		.strobe_bb(tx0_duc_bbstrobe),    	
		.phase_cordic( tx0_duc_phasecordic),
		.log2_rate(tx0_log2_rate),
		
		.strobe_if(tx0_strobe),  			// I.F. data output strobe (input).
		//.iq_bb_in({16'h0100, 16'h0100}),	// Baseband input data [31:0] 
		.iq_bb_in(iq_port0),					// Baseband input data [31:0] 
		.iq_if_out(tx0_iq),					// I.F. ouput [23:0] iq_out

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

	wire tx1_full, tx1_empty, tx1_afull, tx1_aempty;
	wire [31:0] iq_port1;
	wire [23:0] bbtx1_iq;

	wire [7:0]  tx1_duc_cfg;			
	wire [12:0] tx1_duc_rateinterp;	
	wire			tx1_duc_rateinterp_we;	
	wire       	tx1_duc_cicstrobe;   	
	wire 			tx1_duc_bbstrobe;
	wire [31:0] tx1_duc_phasecordic; 	
	wire [3:0]  tx1_log2_rate;

	WcaDducController #(`DUC1_FREQ, `DUC1_INTERP, `DUC1_CTRL, `TX1_MODE	) tx1_DucController(
			 .clock(clockDsp),								//input clock
	       .reset(reset),									//input
			 .enable(tx1_duc_cfg[0]),						//input 
			 .strobe_if(tx1_strobe),
			 
			 //Bind controller signals.
			 .cfg( tx1_duc_cfg),								// output signal, read inside DDUC Controller
			 .rate_interp(tx1_duc_rateinterp),			// output interp rate for CIC in DUC
			 .rate_interp_we(tx1_duc_rateinterp_we),
			 .log2_rate(tx1_log2_rate),
			 
			 .strobe_cic(tx1_duc_cicstrobe),   			//output CIC baseband strobe rate
			 .strobe_bb( tx1_duc_bbstrobe),
			 .phase_cordic( tx1_duc_phasecordic),
			 
			 //CPU Interface.
			 .rbusCtrl(rbusCtrl),   						// input address and control lines(12 total) { addr[7:0], readEnable, writeEnable, dataStrobe, clkbus}
			 .rbusData(rbusData)								// inout Tri-state I/O data, 8 bits
	);

	
	WcaReadPort #(.ADDR_PORT(`PORT2_ADDR)) tx1_port_read(
	 .reset(reset | tx1_duc_cfg[1]),
	 .port_enable( tx1_duc_cfg[0]),
	 .rd_clk(clockDsp),						// Clock read input..
	 .rd_en(tx1_duc_bbstrobe),  			// Enables reading.
	 .rd_out(iq_port1),						// Read Data output.	
	 .empty(tx1_empty),						// Active high indicates buffer is empty.	 
	 .full (tx1_full),						// Active high indicates buffer is empty.
	 .prog_empty(tx1_aempty),
	 .prog_full(tx1_afull),
	 .pifData(pifData),						// 32 bit port interface data bus.
	 .portCtrl(portCtrl), 					// Port State Control bits {addr[NBITS_ADDR:0], read, write, clk }
	 .portCmd(portCmd)  						// Port Command ID
	 );


	//Offset and interpolate Baseband frequency.
	WcaUpConverter #(`TX1_MODE) tx1_duc
	(
		.clock( clockDsp),					// input Clock  
		.reset( reset),						// input reset	
		.enable(tx1_duc_cfg[0]),			// input enable
		
		.cfg( tx1_duc_cfg),
		.rate_interp(tx1_duc_rateinterp),
	   .rate_interp_we(tx1_duc_rateinterp_we),		
		.strobe_cic(tx1_duc_cicstrobe),		
		.strobe_bb(tx1_duc_bbstrobe),    	
		.phase_cordic( tx1_duc_phasecordic),
		.log2_rate(tx1_log2_rate),
		
		.strobe_if(tx1_strobe),  			// I.F. data output strobe (input).
		.iq_bb_in(iq_port1),					// Baseband input data [31:0] 
		.iq_if_out(tx1_iq),					// I.F. ouput [23:0] iq_out

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
