`include "fpga_registers.v"

// Name:  WcaAdc.v
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
module WcaAdc
  (
    input              clock_sample,
    input              clock_rf_data,
    input              reset,
    input              enable,

     //DMA Interface .
    input   wire  [13:0] dmaCtrl,           //DMA address and control lines { addr[9:0], nAddrStrobe, nReadStrobe, nWriteStrobe, cpuclock }
    inout   wire  [15:0] dmaData,           //Tri-state I/O data.

    //RF Chip Interface.  2 Receive channels, 1 transmit.
    input wire         rx1_iqsel,
    input wire [11:0]  rx1_data,
    input wire         rx2_iqsel,
    input wire [11:0]  rx2_data,
    
    output reg [11:0]  tx1_data,
    output wire        tx1_iqsel,

    output wire         rf1_rxen,
    output wire         rf1_txen,
    output wire         rf2_rxen,

    output wire         rf_rxclk,           //Lime receive ADC clock.
    output wire         rf_txclk,           //Lime transmit ADC clock.

    //DAC Output dma_data 
    input  wire [11:0] tx_i,
    input  wire [11:0] tx_q,


    //Buffer ADC input dma_data.
    output reg  [11:0] rx0_i,
    output reg  [11:0] rx0_q, 
    output reg  [11:0] rx1_i,
    output reg  [11:0] rx1_q

  );


    //RF Interface. (WRITE)
    // bit#  |  Description
    //-------|----------------------------------------------------------
    //  0-1     RX Input Mode
    //           0 = txd Loopback (counter in UT version)
    //           1 = rf1_rx input.
    //           2 = rf2_rx input.
    //	         3 = Fixed Test Pattern  DSP0 i = AA0, q = BB1; DSP2 (if available) i=CC0, q=DD1;
    //
    //  2       "rf1_rxen" output line - set to 1 to enable, 0 to disable. Enables disables the 
    //          Lime chip #1 receive ADC function. If RF Interfane not enabled in FR_MCTRL_0, 
    //          this is disabled.
    //
    //  3       "rf1_txen" output pin state - set to 1 to enable, 0 to disable  Enables / disables the 
    //          Lime chip #1 transmit DAC function. If RF interface not enabled in FR_MCTRL_0, 
    //          this is disabled.
    //
    //  4       "rf2_rxen" output pin state - set to 1 to enable, 0 to disable. Enables / disables the 
    //          Lime chip #2 receive ADC function.  If RF interface not enabled in FR_MCTRL_0, 
    //          this is disabled.
    //
    //  5       "rf_rxclk" enable (1) / (0).  If enabled the rf_rxclk pin will turn on and provide clock
    //          signals to the receive functions of the Lime chips.  If disabled, clock output will be low.
    //          If RF interface not enabled in FR_MCTRL_0, clock output is disabled.
    //
    //  6       "rf_txclk" enable (1) / (0).  If enabled the tx_rxclk pin will turn on and provide clock
    //          signals to the transmit functions of Lime chip #1.  If disabled, clock output will be low.
    //          If RF interface not enabled in FR_MCTRL_0, clock output is disabled.
    //
    //  7-15    Reserved.
    // 


    wire [15:0] rf_ctrl;
    WcaWriteWordReg #(`FR_RF_CTRL) sr_rf_ctrl
    (.reset(reset), .out( rf_ctrl), .dmaCtrl(dmaCtrl), .dmaData(dmaData) );


    //------------------------------------------------------------
    // Lime chip control.
    //------------------------------------------------------------   
    assign rf1_rxen  = enable & rf_ctrl[2];   
    assign rf1_txen  = enable & rf_ctrl[3];
    assign rf2_rxen  = enable & rf_ctrl[4];

    assign rf_rxclk  = (enable & rf_ctrl[5]) ? clock_rf_data : 1'b0;
    assign rf_txclk  = (enable & rf_ctrl[6]) ? clock_rf_data : 1'b0;
    assign tx1_iqsel = (rf1_txen)            ? clock_sample  : 1'b0;

    //------------------------------------------------------------
    // Rx1 and Rx2 Interface.Selector.
    //------------------------------------------------------------   
    //Break out receive dma_data into separate I/Q paths
    //so we can process in dsp.
    always @(posedge clock_rf_data)
    begin
        case( { enable, rf_ctrl[1:0]} )
         3'b100 : // 0 is loop back.tx back on to rx.
          begin
            if( clock_sample )
              begin
                rx0_i <= #1 tx_i[11:0];
                rx1_i <= #1 tx_i[11:0];
              end
            else
              begin
                rx0_q <= #1 tx_q[11:0];
                rx1_q <= #1 tx_q[11:0];
              end
          end
         3'b101 :  //1 is rx 1.
          begin
            if( rx1_iqsel )
                rx0_i <= #1 rx1_data;
            else
                rx0_q <= #1 rx1_data;

            if( rx2_iqsel )
                rx1_i <= #1 rx2_data;
            else
                rx1_q <= #1 rx2_data;
          end
         3'b110 : // 2 is rx 2.
          begin
            if( rx2_iqsel )
                rx0_i <= #1 rx2_data;
            else
                rx0_q <= #1 rx2_data;

            if( rx1_iqsel )
                rx1_i <= #1 rx1_data;
            else
                rx1_q <= #1 rx1_data;
          end
         3'b111 : //3 is test pattern.
          begin
             rx0_i <= #1 12'd256;
             rx0_q <= #1 -12'd256;
             rx1_i <= #1 12'd256;
             rx1_q <= #1 -12'd256;
          end
         default: //Default is nothing.
          begin
            rx0_i <= #1 12'd0;
            rx0_q <= #2 12'd0;
            rx1_i <= #2 12'd0;
            rx1_q <= #2 12'd0;
          end
        endcase
    end

    //------------------------------------------------------------
    // RSSI Implementation
    //------------------------------------------------------------   
    wire [7:0] rssi_q0;
    wire [7:0] rssi_i0;

    //Construct Inphase RSSI function
    rssi RssiInphase(
        .clock(clock_sample), .reset(reset), .enable(enable),
        .adc(rx0_i), 
        .rssi(rssi_i0) 
    );

    //RSSI Quadrature Function
    rssi RssiQuadrature(
        .clock(clock_sample), .reset(reset), .enable(enable),
        .adc(rx0_q), 
        .rssi(rssi_q0) 
    );

    WcaReadWordReg #(`FR_ADC_RSSI) AdcRssiReadRegister
    ( .reset(reset), .clockIn(clock_sample), .enableIn(1'b1), .in( {rssi_q0, rssi_i0} ),.dmaCtrl(dmaCtrl), .dmaData(dmaData));


    //------------------------------------------------------------
    // DC Bias Detection.
    //------------------------------------------------------------   
    rx_dcoffset #(`FR_ADC_IBIAS) OffsetInphase(
        .clock(clock_sample),  .enable(enable), .reset(reset), 
        .adc_in(rx0_i), 
        .dmaCtrl(dmaCtrl), .dmaData(dmaData)   
    );

    rx_dcoffset #(`FR_ADC_QBIAS) OffsetQuadrature(
        .clock(clock_sample),  .enable(enable), .reset(reset), 
        .adc_in(rx0_q), 
        .dmaCtrl(dmaCtrl), .dmaData(dmaData)   
    );


    //------------------------------------------------------------
    // Tx1 Data Output.
    //------------------------------------------------------------   
    always @(posedge clock_rf_data)
    begin
      if( rf1_txen)
        begin
            if( clock_sample )
                tx1_data <= #1  tx_i[11:0];
            else
                tx1_data <= #1  tx_q[11:0];
        end
      else
        tx1_data <= #1 15'd0;
    end

endmodule // adc_interface