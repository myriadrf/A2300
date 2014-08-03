// Name:  main.v
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

`timescale 1ns / 1ps
`include "../wca/hal/WcaHalRegisterDefs.vh" //grab register addresses.

module main
(
	//Clocks and General control. 
	clkref1, clkref2, clkref3, c3_sys_clk,
	
	//CPU Micro interface.
	cpu_clock, cpu_astrobe, cpu_rstrobe, cpu_wstrobe, cpu_dbus, cpu_reset, cpu_interrupt,

	//Lime #1 interface.
   rf1_txclk, rf1_txiqsel, rf1_txen, rf1_txd, rf1_rxclk, rf1_rxiqsel, rf1_rxen, rf1_rxd,  

	//Lime #2 interface.
   rf2_txclk, rf2_txiqsel, rf2_txen, rf2_txd, rf2_rxclk, rf2_rxiqsel, rf2_rxen, rf2_rxd,    

	//RF Selection Switches.
	rf_Wb1_RxSelect,  rf_Wb1_nRxSelect,
	rf_Wb2_RxSelect,  rf_Wb2_nRxSelect,
	rf_Ism_TxSelect,  rf_Ism_nTxSelect,
	rf_Ism_ExtSelect, rf_Ism_nExtSelect,  
	rf_Gps_ExtSelect, rf_Gps_nExtSelect,
	rf_Gps_DcBiasSelect,
	
	//GP LEDs.
	led_0, led_1, led_2, led_3, /* led_red, led_green, NOT USED. */

/*
	//Cypress USB FX3 Interface. (5 bit Addressing Mode.)
   fx3_clk, fx3_nSlcs, fx3_nPktEnd, fx3_nEpSwitch, fx3_flagA, fx3_flagB, 
	fx3_addr, fx3_data, fx3_nSlWr, fx3_nSlRd, fx3_nSlOe, fx3_gpio,
*/	

	//Cypress USB FX3 Interface. (2 bit Addressing Mode.)
   fx3_clk, fx3_nSlcs, fx3_nPktEnd, fx3_flagA, fx3_flagB, fx3_flagC, fx3_flagD,
	fx3_addr, fx3_data, fx3_nSlWr, fx3_nSlRd, fx3_nSlOe, fx3_gpio,
	
	//Expansion Port Interface.
	ep_io,

	//DDR2 SDRAM Memory Interface
	mcb3_dram_a,   mcb3_dram_ba,    mcb3_dram_ras_n,  mcb3_dram_cas_n,  mcb3_dram_we_n, mcb3_dram_odt,
	mcb3_dram_cke, mcb3_dram_ck,    mcb3_dram_ck_n, 
	mcb3_dram_dq,  mcb3_dram_dqs,  mcb3_dram_dqs_n, mcb3_dram_udqs, mcb3_dram_udqs_n,
	mcb3_dram_dm, mcb3_dram_udm,		
	
	mcb3_rzq, mcb3_zio
);
	 //clock references.
	input clkref1, clkref2, clkref3, c3_sys_clk;

	//CPU micro interface.
	input 	cpu_astrobe, cpu_clock, cpu_rstrobe, cpu_wstrobe, cpu_reset;
	output 	cpu_interrupt;
	inout 	[7:0] cpu_dbus;

	// Lime #1 interface.
	input 	rf1_rxiqsel;
	input 	[11:0] rf1_rxd;
   output 	rf1_rxen;
   output 	rf1_rxclk;
   output 	rf1_txen;
   output 	rf1_txiqsel;
   output 	rf1_txclk;
   output 	[11:0] rf1_txd;
	
	//Lime #2 interface
	input 	rf2_rxiqsel;
	input 	[11:0] rf2_rxd;
   output 	rf2_rxen;
   output 	rf2_rxclk;
   output 	rf2_txen;
   output 	rf2_txiqsel;
   output 	rf2_txclk;
   output 	[11:0] rf2_txd;
	
	//RF Selection Switches
	output rf_Wb1_RxSelect;
	output rf_Wb1_nRxSelect;
	output rf_Wb2_RxSelect;  
	output rf_Wb2_nRxSelect;  
	output rf_Ism_TxSelect;
	output rf_Ism_nTxSelect;
	output rf_Ism_ExtSelect;
	output rf_Ism_nExtSelect;
	output rf_Gps_ExtSelect;
	output rf_Gps_nExtSelect;
	output rf_Gps_DcBiasSelect;

	//LEDs
	output led_0;
	output led_1;
	output led_2;
	output led_3;
//	output led_red; -- NOT USED CURRENTLY
//	output led_green;

	// Cypress USB FX3 Interface.
	input fx3_flagA;
	input fx3_flagB;
	input fx3_flagC; 		  // Uncomment for 2 bit addressing.
	input fx3_flagD;		  // Uncomment   for 2 bit addressing.
	output [1:0] fx3_addr; // [1:0] for 2 bit addressing, [4:0] for 5 bit addressing.
	output fx3_clk;
	//output fx3_nEpSwitch; //Uncomment for 5 bit addressing
	output fx3_nPktEnd;
	output fx3_nSlcs;
	output fx3_nSlRd;
	output fx3_nSlWr;
	output fx3_nSlOe;
	inout [3:0]  fx3_gpio;
	inout [31:0] fx3_data;
	
	// 8 bit expansion port.
   inout [7:0] ep_io;

	//DDR2 SDRAM Memory Interface 
	output [12:0] 	mcb3_dram_a; 			// Address Bus
	output [2:0]  	mcb3_dram_ba; 			// Bank Address
	output        	mcb3_dram_ras_n; 		// Row address strobe.
	output        	mcb3_dram_cas_n; 		// Column address strobe.
	output			mcb3_dram_we_n;		// Write Enable.
	output			mcb3_dram_odt;			// Output is the on-die termination signal.

	output 		  	mcb3_dram_cke; 		// Clock Enable.
	output			mcb3_dram_ck; 			// Differential output clock.
	output			mcb3_dram_ck_n;		// Differential output clock (180).
	
	inout	 [15:0]	mcb3_dram_dq;  		// Bidirectional Data bus.
	inout				mcb3_dram_dqs;		   // Bidirectional Data strobe (Lower byte).
	inout				mcb3_dram_dqs_n;		// Bidirectional Data Strobe complement (Lower byte).
	inout				mcb3_dram_udqs;		// Bidirectional Data strobe (Upper byte).
	inout				mcb3_dram_udqs_n;		// Bidirectional Data Strobe complement (Upper byte).
	output			mcb3_dram_dm;			// Lower Data Byte Mask
	output			mcb3_dram_udm;			// uppoer Data Byte Mask		
	
	inout				mcb3_rzq;				//The RZQ pin should have a resistor value of 2R from the pin to ground, where R is the desired input termination
	inout				mcb3_zio;				//No connct signal used with the soft calibration module

	
//*****************************************************************************************
//WCA HAL Implementation
//*****************************************************************************************
   wire clock_data, clock_limeif, clock_dsp;
	wire [7:0]	mstrCtrl;
   wire [11:0] rbusCtrl;
   wire [7:0] rbusData;
	wire [7:0]  evtsig;
   wire cpu_reset;
	wire [23:0] rx0_iq; 
	wire [23:0] rx1_iq;
	wire [23:0] tx0_iq; 
	wire [23:0] tx1_iq;

	wire rx0_strobe, tx0_strobe;
	wire rx1_strobe, tx1_strobe;
	
	//Port Interface
	wire  [31:0]  	pifData;	 // 32 bit port data bus.
	wire  [3:0]   	pifCtrl;  	 // {addr[NBITS_ADDR:0] Port control, iocmd[1:0] }
	wire   [6:0]	pifStatus;  // I/O Status  {fifo_full[0], fifo_empty[0], ioState[3:0], clk}	
   
//*****************************************************************************
// Clock Generation
//*****************************************************************************

/*
	//Generator creates Highspeed data clock (100 MHz)
   clk_gen clkgen_data( .fpga_clk(clkref1), .clk_dsp(clock_dsp), 
		.clk_data(clock_data), .clk_sampling(clock_limeif), .RESET(cpu_reset));
*/

	//Generator creates Highspeed data clock (100 MHz)
   clk_gen clkgen_data( .fpga_clk(clkref2), .clk_dsp(clock_data), .RESET(cpu_reset));
							
	//Generate base sampling clock (128 MHz, 64 MHz)
	//128 MHz is the fundamental clock for all DSP
	//64  MHz drives the Lime interfaces.
   clk_gen32Samp  clkgen_sampling(.fpga_clk(clkref1),  .RESET(cpu_reset), 
											 .clk_dsp(clock_dsp), .clk_sampling(clock_limeif));

//*****************************************************************************
// WCA HAL
//*****************************************************************************
	
	//Implement Mater control register.
	//	Core configuration register.  
	//
	// bit#  |  Description
	//-------|----------------------------------------------------------
	//	  0		ResetRf0 - Performs a reset on RF0.
	//	  1		ResetRf1 - Performs a reset on RF1.
	//   2      ResetContainer - Performs a reset on HalContainer functions.
	//   3      ResetUsbIF - Performs a reset on the USB Interface.  Holding high places
	//				the interface in an idle state (disabled).
	//	  4      ClearRf0Rf1 - Clears state of RF0 and RF1 without changing configuration
	//				This synchronizes internal counters in order to eliminate interchannel biases.
	
	WcaWriteByteReg #(`WCAHAL_MSTR_CTRL) wr_mstr_ctrl
								(.reset(cpu_reset), 
								.out( mstrCtrl), 
								.rbusCtrl(rbusCtrl), .rbusData(rbusData) );
	
	//Highjack expansion port for now. TODO REMOVE ME.
	//assign ep_io = { rbusCtrl[5:1], cpu_rstrobe, cpu_wstrobe, cpu_astrobe};
	
	//CPU Micro Register interface implementation.
   WcaRegBus  cpu_if ( .clock(clock_data),  .address(cpu_astrobe), .reset(cpu_reset), 
							  .read(cpu_rstrobe), .write(cpu_wstrobe), .dbus(cpu_dbus), 
                       .rbusCtrl(rbusCtrl),   .rbusData(rbusData));
				  
	//CPU Micro Interrupt interface implementation.							
	WcaInterruptIF #(`WCAHAL_EVENT) cpu_interrupt_if ( .clock( clock_data), .reset(cpu_reset), 
					   .evtsig( evtsig),
						.cpu_interrupt(cpu_interrupt),
						.rbusCtrl(rbusCtrl),.rbusData(rbusData) );
	

	//Create the inverted version of the selection signal to match switch
	//requirements. SKY13335-381LF SPDT SWITCH
	assign rf_Wb1_nRxSelect  = ~rf_Wb1_RxSelect;
	assign rf_Wb2_nRxSelect  = ~rf_Wb2_RxSelect;  
	
	//20130801 MBM - Invert the ISM Band Select Lines so 
	//state matches the specification.  These lines states, like the 
	//GPS below were backwards on the schematic so this fixes the issue.
	wire ism_txselect, ism_extselect;
	assign rf_Ism_nTxSelect  = ism_txselect;
	assign rf_Ism_TxSelect   = ~ism_txselect;
	assign rf_Ism_nExtSelect = ism_extselect;
	assign rf_Ism_ExtSelect  = ~ism_extselect;
	
	//20130729 MBM - Invert the GPS select lines so Internal GPS 
	//is the default setting.  This is the desired spec and propagates
	//through all the docuementation.  Was easiest to fix here.
	wire gps_select;
	assign rf_Gps_nExtSelect = gps_select;  
	assign rf_Gps_ExtSelect = ~gps_select; //The affirmative external select is the opposite of the register settings.


	//Create synchronized clear of lime interfaces.
	wire enRx1, enRx2, enTx1, enTx2;
	wire aclrLime;
	WcaSynchEdgeDetect seClearLime ( .clk(clock_dsp), .in(mstrCtrl[4]),  .rising( aclrLime) );

	// Lime RF#0 Interface implementation.
   WcaLimeIF  #( `WCAHAL_RF0_CTRL, `WCAHAL_RF0_RSSI, `WCAHAL_RF0_BIAS)
				  lime0_if ( .clock_dsp(clock_dsp), .clock_rx(clock_limeif), .clock_tx(clock_limeif), 
							 .reset(cpu_reset | mstrCtrl[0]), .aclr( aclrLime),
                      .rbusCtrl(rbusCtrl),.rbusData(rbusData),
                      .rf_rxclk(rf1_rxclk), .rf_rxiqsel(rf1_rxiqsel), .rf_rxen(rf1_rxen), .rf_rxdata(rf1_rxd[11:0]), 
                      .rf_txclk(rf1_txclk), .rf_txiqsel(rf1_txiqsel), .rf_txen(rf1_txen), .rf_txdata(rf1_txd[11:0]), 
                      .rx_strobe( rx0_strobe),  .rx_iq(rx0_iq), .tx_strobe( tx0_strobe), .tx_iq( tx0_iq ) ); 
	
	// Lime RF#1 Interface implementation						 
   WcaLimeIF   #( `WCAHAL_RF1_CTRL, `WCAHAL_RF1_RSSI, `WCAHAL_RF1_BIAS)
					lime1_if ( .clock_dsp(clock_dsp), .clock_rx(clock_limeif), .clock_tx(clock_limeif), 
							 .reset(cpu_reset | mstrCtrl[1]), .aclr( aclrLime),
                      .rbusCtrl(rbusCtrl), .rbusData(rbusData),
                      .rf_rxclk(rf2_rxclk), .rf_rxiqsel(rf2_rxiqsel), .rf_rxen(rf2_rxen), .rf_rxdata(rf2_rxd[11:0]), 
                      .rf_txclk(rf2_txclk), .rf_txiqsel(rf2_txiqsel), .rf_txen(rf2_txen), .rf_txdata(rf2_txd[11:0]), 
                      .rx_strobe( rx1_strobe),  .rx_iq(rx1_iq), .tx_strobe( tx1_strobe), .tx_iq(tx1_iq) );

// MBM 140728 - Restored clock control to the lime chips.	This may cause interchannel delay bias.  Need to fix
// 				 with more consistent fix that works sequential and non-sequential.
//	assign rf1_rxen = enRx1;
//	assign rf2_rxen = enRx1;
//	ODDR2 oddr1_rxclk	( .D0(1'b1), .D1(1'b0), .CE(enRx1),	.C0(clock_limeif),	.C1(~clock_limeif), .Q(rf1_rxclk) );
//	ODDR2 oddr2_rxclk	( .D0(1'b1), .D1(1'b0), .CE(enRx1),	.C0(clock_limeif),	.C1(~clock_limeif), .Q(rf2_rxclk) );	

	// Push out top level synchronization signals 
	wire [7:0] ep_io_container; // { 2'h0, clearSynch, enableSynch, clearRx1, clearRx0, rx1_strobeWrite, rx0_strobeWrite};
	assign ep_io = { ep_io_container[2:0], aclrLime, rx1_strobe, rx0_strobe, rf2_rxiqsel, rf1_rxiqsel};
	
	
	//Pass elements into WcaContainer for DSP logic..
	WcaContainer container( .reset( cpu_reset | mstrCtrl[2]), .clockData( clock_data), .clockDsp( clock_dsp),
			//RF0 Interface 
			.rx0_iq(rx0_iq ), .tx0_iq(tx0_iq ), .rx0_strobe(rx0_strobe ), 	.tx0_strobe(tx0_strobe ),
			//RF1 Interface 
			.rx1_iq(rx1_iq ),	.tx1_iq(tx1_iq ),	.rx1_strobe(rx1_strobe ),	.tx1_strobe(tx1_strobe ),
			//LED Interface 
			.ledSelect( {led_3, led_2, led_1, led_0} ), 
			
			//RF Control Interface.
			.rfSelect({rf_Wb2_RxSelect,  rf_Wb1_RxSelect,  ism_txselect,  
						ism_extselect, gps_select, rf_Gps_DcBiasSelect}), 
						
			//Expansion Port Interface.			
			.ep_io( ep_io_container),

			//USB Interface 
			.pifData( pifData),
			.pifCtrl( pifCtrl),
			.pifStatus( pifStatus),

			//CPU Interface
			.evtsig( evtsig), 
			.rbusCtrl(rbusCtrl),.rbusData(rbusData)
			);
		 
//*****************************************************************************
// USB Interface
//*****************************************************************************

	// Cypress USB FX3 Interface implementation.
 
  //Uncomment for 5-bit addressing mode.
  //WcaUsbFx3IF  usb_if (.clk_in(clock_data), .reset(cpu_reset), 	.flagA(fx3_flagA), .flagB( fx3_flagB), 
  //                      .addr(fx3_addr[4:0]), .clk(fx3_clk), 		.nEpSwitch(fx3_nEpSwitch), .nPktEnd(fx3_nPktEnd), 
  //                      .nSlcs(fx3_nSlcs), 	.nSlOe(fx3_nSlOe), 		.nSlRd(fx3_nSlRd), 		.nSlWr(fx3_nSlWr), 
  //							  .data(fx3_data[31:0]), .gpio( fx3_gpio) );
//assign led_0 = fx3_addr[0];
//assign led_1 = fx3_flagA;
//assign led_2 = fx3_nSlOe;
//assign led_3 = fx3_nSlWr;

	//Uncomment for 2-bit addressing mode.
	WcaUsbFx3IF  usb_if (.clk_in(clock_data),   .reset(cpu_reset | mstrCtrl[3]), 	
								.flagA(fx3_flagA),     .flagB( fx3_flagB), 	.flagC(fx3_flagC), 		.flagD( fx3_flagD), 
								.addr(fx3_addr), 	     .clk(fx3_clk), 			.nPktEnd(fx3_nPktEnd), 
								.nSlcs(fx3_nSlcs), 	  .nSlOe(fx3_nSlOe), 	.nSlRd(fx3_nSlRd), 		.nSlWr(fx3_nSlWr), 
								.data(fx3_data[31:0]), .gpio( fx3_gpio),

								.pifData( pifData),
								.pifCtrl( pifCtrl),
								.pifStatus( pifStatus)							  
								);


//*****************************************************************************
// Memory Interface
//*****************************************************************************

//TODO Figure out what the clock reference is supposed to be.
 WcaSdramIF # (
    .C3_P0_MASK_SIZE(4),
    .C3_P0_DATA_PORT_SIZE(32),
    .C3_P1_MASK_SIZE(4),
    .C3_P1_DATA_PORT_SIZE(32),
    .DEBUG_EN(0),
    .C3_MEMCLK_PERIOD(3200),
    .C3_CALIB_SOFT_IP("TRUE"),
    .C3_SIMULATION("FALSE"),
    .C3_RST_ACT_LOW(0),
    .C3_INPUT_CLK_TYPE("SINGLE_ENDED"),
    .C3_MEM_ADDR_ORDER("ROW_BANK_COLUMN"),
    .C3_NUM_DQ_PINS(16),
    .C3_MEM_ADDR_WIDTH(13),
    .C3_MEM_BANKADDR_WIDTH(3)
)
u_WcaSdramIF (

		.c3_sys_clk           (c3_sys_clk),
		.c3_sys_rst_i           (cpu_reset),                        

		.mcb3_dram_dq           (mcb3_dram_dq),  
		.mcb3_dram_a            (mcb3_dram_a),  
		.mcb3_dram_ba           (mcb3_dram_ba),
		.mcb3_dram_ras_n        (mcb3_dram_ras_n),                        
		.mcb3_dram_cas_n        (mcb3_dram_cas_n),                        
		.mcb3_dram_we_n         (mcb3_dram_we_n),                          
		.mcb3_dram_odt          (mcb3_dram_odt),
		.mcb3_dram_cke          (mcb3_dram_cke),                          
		.mcb3_dram_ck           (mcb3_dram_ck),                          
		.mcb3_dram_ck_n         (mcb3_dram_ck_n),       
		.mcb3_dram_dqs          (mcb3_dram_dqs),                          
		.mcb3_dram_dqs_n        (mcb3_dram_dqs_n),
		.mcb3_dram_udqs         (mcb3_dram_udqs),    // for X16 parts                        
		.mcb3_dram_udqs_n       (mcb3_dram_udqs_n),  // for X16 parts
		.mcb3_dram_udm          (mcb3_dram_udm),     // for X16 parts
		.mcb3_dram_dm           (mcb3_dram_dm),
		.c3_clk0		        (), //Clock output to drive input.
		.c3_rst0		        (), //Reset signal to do something with.


		.c3_calib_done          (),  //Signals when calibration is done.  Should wire to a WGA Core status register
		.mcb3_rzq               (mcb3_rzq),
			
		.mcb3_zio               (mcb3_zio),
				
		.c3_p0_cmd_clk                          (c3_p0_cmd_clk),
		.c3_p0_cmd_en                           (c3_p0_cmd_en),
		.c3_p0_cmd_instr                        (c3_p0_cmd_instr),
		.c3_p0_cmd_bl                           (c3_p0_cmd_bl),
		.c3_p0_cmd_byte_addr                    (c3_p0_cmd_byte_addr),
		.c3_p0_cmd_empty                        (c3_p0_cmd_empty),
		.c3_p0_cmd_full                         (c3_p0_cmd_full),
		.c3_p0_wr_clk                           (c3_p0_wr_clk),
		.c3_p0_wr_en                            (c3_p0_wr_en),
		.c3_p0_wr_mask                          (c3_p0_wr_mask),
		.c3_p0_wr_data                          (c3_p0_wr_data),
		.c3_p0_wr_full                          (c3_p0_wr_full),
		.c3_p0_wr_empty                         (c3_p0_wr_empty),
		.c3_p0_wr_count                         (c3_p0_wr_count),
		.c3_p0_wr_underrun                      (c3_p0_wr_underrun),
		.c3_p0_wr_error                         (c3_p0_wr_error),
		.c3_p0_rd_clk                           (c3_p0_rd_clk),
		.c3_p0_rd_en                            (c3_p0_rd_en),
		.c3_p0_rd_data                          (c3_p0_rd_data),
		.c3_p0_rd_full                          (c3_p0_rd_full),
		.c3_p0_rd_empty                         (c3_p0_rd_empty),
		.c3_p0_rd_count                         (c3_p0_rd_count),
		.c3_p0_rd_overflow                      (c3_p0_rd_overflow),
		.c3_p0_rd_error                         (c3_p0_rd_error),
		.c3_p2_cmd_clk                          (c3_p2_cmd_clk),
		.c3_p2_cmd_en                           (c3_p2_cmd_en),
		.c3_p2_cmd_instr                        (c3_p2_cmd_instr),
		.c3_p2_cmd_bl                           (c3_p2_cmd_bl),
		.c3_p2_cmd_byte_addr                    (c3_p2_cmd_byte_addr),
		.c3_p2_cmd_empty                        (c3_p2_cmd_empty),
		.c3_p2_cmd_full                         (c3_p2_cmd_full),
		.c3_p2_rd_clk                           (c3_p2_rd_clk),
		.c3_p2_rd_en                            (c3_p2_rd_en),
		.c3_p2_rd_data                          (c3_p2_rd_data),
		.c3_p2_rd_full                          (c3_p2_rd_full),
		.c3_p2_rd_empty                         (c3_p2_rd_empty),
		.c3_p2_rd_count                         (c3_p2_rd_count),
		.c3_p2_rd_overflow                      (c3_p2_rd_overflow),
		.c3_p2_rd_error                         (c3_p2_rd_error),
		.c3_p3_cmd_clk                          (c3_p3_cmd_clk),
		.c3_p3_cmd_en                           (c3_p3_cmd_en),
		.c3_p3_cmd_instr                        (c3_p3_cmd_instr),
		.c3_p3_cmd_bl                           (c3_p3_cmd_bl),
		.c3_p3_cmd_byte_addr                    (c3_p3_cmd_byte_addr),
		.c3_p3_cmd_empty                        (c3_p3_cmd_empty),
		.c3_p3_cmd_full                         (c3_p3_cmd_full),
		.c3_p3_wr_clk                           (c3_p3_wr_clk),
		.c3_p3_wr_en                            (c3_p3_wr_en),
		.c3_p3_wr_mask                          (c3_p3_wr_mask),
		.c3_p3_wr_data                          (c3_p3_wr_data),
		.c3_p3_wr_full                          (c3_p3_wr_full),
		.c3_p3_wr_empty                         (c3_p3_wr_empty),
		.c3_p3_wr_count                         (c3_p3_wr_count),
		.c3_p3_wr_underrun                      (c3_p3_wr_underrun),
		.c3_p3_wr_error                         (c3_p3_wr_error)
);

endmodule
