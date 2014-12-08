////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1995-2012 Xilinx, Inc.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////
//   ____  ____
//  /   /\/   /
// /___/  \  /    Vendor: Xilinx
// \   \   \/     Version: P.40xd
//  \   \         Application: netgen
//  /   /         Filename: halfband_decim.v
// /___/   /\     Timestamp: Sun Sep 08 21:55:30 2013
// \   \  /  \ 
//  \___\/\___\
//             
// Command	: -intstyle ise -w -sim -ofmt verilog ./tmp/_cg/halfband_decim.ngc ./tmp/_cg/halfband_decim.v 
// Device	: 6slx45csg484-3
// Input file	: ./tmp/_cg/halfband_decim.ngc
// Output file	: ./tmp/_cg/halfband_decim.v
// # of Modules	: 1
// Design Name	: halfband_decim
// Xilinx        : C:\Xilinx\14.3\ISE_DS\ISE\
//             
// Purpose:    
//     This verilog netlist is a verification model and uses simulation 
//     primitives which may not represent the true implementation of the 
//     device, however the netlist is functionally correct and should not 
//     be modified. This file cannot be synthesized and should only be used 
//     with supported simulation tools.
//             
// Reference:  
//     Command Line Tools User Guide, Chapter 23 and Synthesis and Simulation Design Guide, Chapter 6
//             
////////////////////////////////////////////////////////////////////////////////

`timescale 1 ns/1 ps

module halfband_decim (
  rfd, rdy, nd, clk, dout, din
)/* synthesis syn_black_box syn_noprune=1 */;
  output rfd;
  output rdy;
  input nd;
  input clk;
  output [15 : 0] dout;
  input [15 : 0] din;
  
  // synthesis translate_off
  
  wire NlwRenamedSig_OI_rfd;
  wire \blk00000003/sig00000124 ;
  wire \blk00000003/sig00000123 ;
  wire \blk00000003/sig00000122 ;
  wire \blk00000003/sig00000121 ;
  wire \blk00000003/sig00000120 ;
  wire \blk00000003/sig0000011f ;
  wire \blk00000003/sig0000011e ;
  wire \blk00000003/sig0000011d ;
  wire \blk00000003/sig0000011c ;
  wire \blk00000003/sig0000011b ;
  wire \blk00000003/sig0000011a ;
  wire \blk00000003/sig00000119 ;
  wire \blk00000003/sig00000118 ;
  wire \blk00000003/sig00000117 ;
  wire \blk00000003/sig00000116 ;
  wire \blk00000003/sig00000115 ;
  wire \blk00000003/sig00000114 ;
  wire \blk00000003/sig00000113 ;
  wire \blk00000003/sig00000112 ;
  wire \blk00000003/sig00000111 ;
  wire \blk00000003/sig00000110 ;
  wire \blk00000003/sig0000010f ;
  wire \blk00000003/sig0000010e ;
  wire \blk00000003/sig0000010d ;
  wire \blk00000003/sig0000010c ;
  wire \blk00000003/sig0000010b ;
  wire \blk00000003/sig0000010a ;
  wire \blk00000003/sig00000109 ;
  wire \blk00000003/sig00000108 ;
  wire \blk00000003/sig00000107 ;
  wire \blk00000003/sig00000106 ;
  wire \blk00000003/sig00000105 ;
  wire \blk00000003/sig00000104 ;
  wire \blk00000003/sig00000103 ;
  wire \blk00000003/sig00000102 ;
  wire \blk00000003/sig00000101 ;
  wire \blk00000003/sig00000100 ;
  wire \blk00000003/sig000000ff ;
  wire \blk00000003/sig000000fe ;
  wire \blk00000003/sig000000fd ;
  wire \blk00000003/sig000000fc ;
  wire \blk00000003/sig000000fb ;
  wire \blk00000003/sig000000fa ;
  wire \blk00000003/sig000000f9 ;
  wire \blk00000003/sig000000f8 ;
  wire \blk00000003/sig000000f7 ;
  wire \blk00000003/sig000000f6 ;
  wire \blk00000003/sig000000f5 ;
  wire \blk00000003/sig000000f4 ;
  wire \blk00000003/sig000000f3 ;
  wire \blk00000003/sig000000f2 ;
  wire \blk00000003/sig000000f1 ;
  wire \blk00000003/sig000000f0 ;
  wire \blk00000003/sig000000ef ;
  wire \blk00000003/sig000000ee ;
  wire \blk00000003/sig000000ed ;
  wire \blk00000003/sig000000ec ;
  wire \blk00000003/sig000000eb ;
  wire \blk00000003/sig000000ea ;
  wire \blk00000003/sig000000e9 ;
  wire \blk00000003/sig000000e8 ;
  wire \blk00000003/sig000000e7 ;
  wire \blk00000003/sig000000e6 ;
  wire \blk00000003/sig000000e5 ;
  wire \blk00000003/sig000000e4 ;
  wire \blk00000003/sig000000e3 ;
  wire \blk00000003/sig000000e2 ;
  wire \blk00000003/sig000000e1 ;
  wire \blk00000003/sig000000e0 ;
  wire \blk00000003/sig000000df ;
  wire \blk00000003/sig000000de ;
  wire \blk00000003/sig000000dd ;
  wire \blk00000003/sig000000dc ;
  wire \blk00000003/sig000000db ;
  wire \blk00000003/sig000000da ;
  wire \blk00000003/sig000000d9 ;
  wire \blk00000003/sig000000d8 ;
  wire \blk00000003/sig000000d7 ;
  wire \blk00000003/sig000000d6 ;
  wire \blk00000003/sig000000d5 ;
  wire \blk00000003/sig000000d4 ;
  wire \blk00000003/sig000000d3 ;
  wire \blk00000003/sig000000d2 ;
  wire \blk00000003/sig000000d1 ;
  wire \blk00000003/sig000000d0 ;
  wire \blk00000003/sig000000cf ;
  wire \blk00000003/sig000000ce ;
  wire \blk00000003/sig000000cd ;
  wire \blk00000003/sig000000cc ;
  wire \blk00000003/sig000000cb ;
  wire \blk00000003/sig000000ca ;
  wire \blk00000003/sig000000c9 ;
  wire \blk00000003/sig000000c8 ;
  wire \blk00000003/sig000000c7 ;
  wire \blk00000003/sig000000c6 ;
  wire \blk00000003/sig000000c5 ;
  wire \blk00000003/sig000000c4 ;
  wire \blk00000003/sig000000c3 ;
  wire \blk00000003/sig000000c2 ;
  wire \blk00000003/sig000000c1 ;
  wire \blk00000003/sig000000c0 ;
  wire \blk00000003/sig000000bf ;
  wire \blk00000003/sig000000be ;
  wire \blk00000003/sig000000bd ;
  wire \blk00000003/sig000000bc ;
  wire \blk00000003/sig000000bb ;
  wire \blk00000003/sig000000ba ;
  wire \blk00000003/sig000000b9 ;
  wire \blk00000003/sig000000b8 ;
  wire \blk00000003/sig000000b7 ;
  wire \blk00000003/sig000000b6 ;
  wire \blk00000003/sig000000b5 ;
  wire \blk00000003/sig000000b4 ;
  wire \blk00000003/sig000000b3 ;
  wire \blk00000003/sig000000b2 ;
  wire \blk00000003/sig000000b1 ;
  wire \blk00000003/sig000000b0 ;
  wire \blk00000003/sig000000af ;
  wire \blk00000003/sig000000ae ;
  wire \blk00000003/sig000000ad ;
  wire \blk00000003/sig000000ac ;
  wire \blk00000003/sig000000ab ;
  wire \blk00000003/sig000000aa ;
  wire \blk00000003/sig000000a9 ;
  wire \blk00000003/sig000000a8 ;
  wire \blk00000003/sig000000a7 ;
  wire \blk00000003/sig000000a6 ;
  wire \blk00000003/sig000000a5 ;
  wire \blk00000003/sig000000a4 ;
  wire \blk00000003/sig000000a3 ;
  wire \blk00000003/sig000000a2 ;
  wire \blk00000003/sig000000a1 ;
  wire \blk00000003/sig000000a0 ;
  wire \blk00000003/sig0000009f ;
  wire \blk00000003/sig0000009e ;
  wire \blk00000003/sig0000009d ;
  wire \blk00000003/sig0000009c ;
  wire \blk00000003/sig0000009b ;
  wire \blk00000003/sig0000009a ;
  wire \blk00000003/sig00000099 ;
  wire \blk00000003/sig00000098 ;
  wire \blk00000003/sig00000097 ;
  wire \blk00000003/sig00000096 ;
  wire \blk00000003/sig00000095 ;
  wire \blk00000003/sig00000094 ;
  wire \blk00000003/sig00000093 ;
  wire \blk00000003/sig00000092 ;
  wire \blk00000003/sig00000091 ;
  wire \blk00000003/sig00000090 ;
  wire \blk00000003/sig0000008f ;
  wire \blk00000003/sig0000008e ;
  wire \blk00000003/sig0000008d ;
  wire \blk00000003/sig0000008c ;
  wire \blk00000003/sig0000008b ;
  wire \blk00000003/sig0000008a ;
  wire \blk00000003/sig00000089 ;
  wire \blk00000003/sig00000088 ;
  wire \blk00000003/sig00000087 ;
  wire \blk00000003/sig00000086 ;
  wire \blk00000003/sig00000085 ;
  wire \blk00000003/sig00000084 ;
  wire \blk00000003/sig00000083 ;
  wire \blk00000003/sig00000082 ;
  wire \blk00000003/sig00000081 ;
  wire \blk00000003/sig00000080 ;
  wire \blk00000003/sig0000007f ;
  wire \blk00000003/sig0000007e ;
  wire \blk00000003/sig0000007d ;
  wire \blk00000003/sig0000007c ;
  wire \blk00000003/sig0000007b ;
  wire \blk00000003/sig0000007a ;
  wire \blk00000003/sig00000079 ;
  wire \blk00000003/sig00000078 ;
  wire \blk00000003/sig00000077 ;
  wire \blk00000003/sig00000076 ;
  wire \blk00000003/sig00000075 ;
  wire \blk00000003/sig00000074 ;
  wire \blk00000003/sig00000073 ;
  wire \blk00000003/sig00000072 ;
  wire \blk00000003/sig00000071 ;
  wire \blk00000003/sig00000070 ;
  wire \blk00000003/sig0000006f ;
  wire \blk00000003/sig0000006e ;
  wire \blk00000003/sig0000006d ;
  wire \blk00000003/sig0000006c ;
  wire \blk00000003/sig0000006b ;
  wire \blk00000003/sig0000006a ;
  wire \blk00000003/sig00000069 ;
  wire \blk00000003/sig00000068 ;
  wire \blk00000003/sig00000067 ;
  wire \blk00000003/sig00000066 ;
  wire \blk00000003/sig00000065 ;
  wire \blk00000003/sig00000064 ;
  wire \blk00000003/sig00000063 ;
  wire \blk00000003/sig00000062 ;
  wire \blk00000003/sig00000061 ;
  wire \blk00000003/sig00000060 ;
  wire \blk00000003/sig0000005f ;
  wire \blk00000003/sig0000005e ;
  wire \blk00000003/sig0000005d ;
  wire \blk00000003/sig0000005c ;
  wire \blk00000003/sig0000005b ;
  wire \blk00000003/sig0000005a ;
  wire \blk00000003/sig00000059 ;
  wire \blk00000003/sig00000058 ;
  wire \blk00000003/sig00000057 ;
  wire \blk00000003/sig00000056 ;
  wire \blk00000003/sig00000055 ;
  wire \blk00000003/sig00000054 ;
  wire \blk00000003/sig00000053 ;
  wire \blk00000003/sig00000052 ;
  wire \blk00000003/sig00000051 ;
  wire \blk00000003/sig00000050 ;
  wire \blk00000003/sig0000004f ;
  wire \blk00000003/sig0000004d ;
  wire \blk00000003/sig0000004c ;
  wire \blk00000003/sig0000004b ;
  wire \blk00000003/sig0000004a ;
  wire \blk00000003/sig00000049 ;
  wire \blk00000003/sig00000048 ;
  wire \blk00000003/sig00000047 ;
  wire \blk00000003/sig00000046 ;
  wire \blk00000003/sig00000045 ;
  wire \blk00000003/sig00000044 ;
  wire \blk00000003/sig00000043 ;
  wire \blk00000003/sig00000042 ;
  wire \blk00000003/sig00000041 ;
  wire \blk00000003/sig00000040 ;
  wire \blk00000003/sig0000003f ;
  wire \blk00000003/sig0000003e ;
  wire \blk00000003/sig0000003d ;
  wire \blk00000003/sig0000003c ;
  wire \blk00000003/sig0000003b ;
  wire \blk00000003/sig0000003a ;
  wire \blk00000003/sig00000039 ;
  wire \blk00000003/sig00000038 ;
  wire \blk00000003/sig00000037 ;
  wire \blk00000003/sig00000036 ;
  wire \blk00000003/sig00000035 ;
  wire \blk00000003/sig00000034 ;
  wire \blk00000003/sig00000033 ;
  wire \blk00000003/sig00000032 ;
  wire \blk00000003/sig00000031 ;
  wire \blk00000003/sig00000030 ;
  wire \blk00000003/sig0000002f ;
  wire \blk00000003/sig0000002e ;
  wire \blk00000003/sig0000002d ;
  wire \blk00000003/sig0000002c ;
  wire \blk00000003/sig0000002b ;
  wire \blk00000003/sig0000002a ;
  wire \blk00000003/sig00000029 ;
  wire \blk00000003/sig00000028 ;
  wire \blk00000003/sig00000027 ;
  wire \blk00000003/sig00000026 ;
  wire \blk00000003/sig00000013 ;
  wire \blk00000003/blk00000061/sig0000015b ;
  wire \blk00000003/blk00000061/sig0000015a ;
  wire \blk00000003/blk00000061/sig00000159 ;
  wire \blk00000003/blk00000061/sig00000158 ;
  wire \blk00000003/blk00000061/sig00000157 ;
  wire \blk00000003/blk00000061/sig00000156 ;
  wire \blk00000003/blk00000061/sig00000155 ;
  wire \blk00000003/blk00000061/sig00000154 ;
  wire \blk00000003/blk00000061/sig00000153 ;
  wire \blk00000003/blk00000061/sig00000152 ;
  wire \blk00000003/blk00000061/sig00000151 ;
  wire \blk00000003/blk00000061/sig00000150 ;
  wire \blk00000003/blk00000061/sig0000014f ;
  wire \blk00000003/blk00000061/sig0000014e ;
  wire \blk00000003/blk00000061/sig0000014d ;
  wire \blk00000003/blk00000061/sig0000014c ;
  wire \blk00000003/blk00000082/sig00000192 ;
  wire \blk00000003/blk00000082/sig00000191 ;
  wire \blk00000003/blk00000082/sig00000190 ;
  wire \blk00000003/blk00000082/sig0000018f ;
  wire \blk00000003/blk00000082/sig0000018e ;
  wire \blk00000003/blk00000082/sig0000018d ;
  wire \blk00000003/blk00000082/sig0000018c ;
  wire \blk00000003/blk00000082/sig0000018b ;
  wire \blk00000003/blk00000082/sig0000018a ;
  wire \blk00000003/blk00000082/sig00000189 ;
  wire \blk00000003/blk00000082/sig00000188 ;
  wire \blk00000003/blk00000082/sig00000187 ;
  wire \blk00000003/blk00000082/sig00000186 ;
  wire \blk00000003/blk00000082/sig00000185 ;
  wire \blk00000003/blk00000082/sig00000184 ;
  wire \blk00000003/blk00000082/sig00000183 ;
  wire \blk00000003/blk000000a3/sig000001b5 ;
  wire \blk00000003/blk000000a3/sig000001b4 ;
  wire \blk00000003/blk000000a3/sig000001b3 ;
  wire \blk00000003/blk000000a3/sig000001b2 ;
  wire \blk00000003/blk000000a3/sig000001b1 ;
  wire \blk00000003/blk000000a3/sig000001b0 ;
  wire \blk00000003/blk000000a3/sig000001af ;
  wire \blk00000003/blk000000a3/sig000001ae ;
  wire \blk00000003/blk000000a3/sig000001ad ;
  wire \blk00000003/blk000000a3/sig000001ac ;
  wire \blk00000003/blk000000a3/sig000001ab ;
  wire \blk00000003/blk000000a3/sig000001aa ;
  wire \blk00000003/blk000000a3/sig000001a9 ;
  wire \blk00000003/blk000000a3/sig000001a8 ;
  wire \blk00000003/blk000000a3/sig000001a7 ;
  wire \blk00000003/blk000000c2/sig000001ea ;
  wire \blk00000003/blk000000c2/sig000001e9 ;
  wire \blk00000003/blk000000c2/sig000001e8 ;
  wire \blk00000003/blk000000c2/sig000001e7 ;
  wire \blk00000003/blk000000c2/sig000001e6 ;
  wire \blk00000003/blk000000c2/sig000001e5 ;
  wire \blk00000003/blk000000c2/sig000001e4 ;
  wire \blk00000003/blk000000c2/sig000001e3 ;
  wire \blk00000003/blk000000c2/sig000001e2 ;
  wire \blk00000003/blk000000c2/sig000001e1 ;
  wire \blk00000003/blk000000c2/sig000001e0 ;
  wire \blk00000003/blk000000c2/sig000001df ;
  wire \blk00000003/blk000000c2/sig000001de ;
  wire \blk00000003/blk000000c2/sig000001dd ;
  wire \blk00000003/blk000000c2/sig000001dc ;
  wire \blk00000003/blk000000c2/sig000001db ;
  wire \blk00000003/blk000000c2/sig000001da ;
  wire NLW_blk00000001_P_UNCONNECTED;
  wire NLW_blk00000002_G_UNCONNECTED;
  wire \NLW_blk00000003/blk00000153_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000151_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000014f_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000014d_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000014b_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000149_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000147_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000145_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000143_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000141_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000013f_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000013d_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000013b_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000139_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000137_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000135_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000133_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000131_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000012f_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000012d_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk000000e6_O_UNCONNECTED ;
  wire \NLW_blk00000003/blk000000e6_LO_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000005c_O_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000005c_LO_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000055_O_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000055_LO_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000004d_O_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000004d_LO_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000045_O_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000045_LO_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000003f_O_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000003f_LO_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000002b_Q_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000001a_LO_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000019_LO_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000015_O_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000014_LO_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000012_O_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000011_LO_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000010_LO_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000000f_LO_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000000d_O_UNCONNECTED ;
  wire \NLW_blk00000003/blk0000000c_LO_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000007_O_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000007_LO_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_CARRYOUTF_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_CARRYOUT_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_BCOUT<17>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_BCOUT<16>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_BCOUT<15>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_BCOUT<14>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_BCOUT<13>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_BCOUT<12>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_BCOUT<11>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_BCOUT<10>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_BCOUT<9>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_BCOUT<8>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_BCOUT<7>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_BCOUT<6>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_BCOUT<5>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_BCOUT<4>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_BCOUT<3>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_BCOUT<2>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_BCOUT<1>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_BCOUT<0>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<47>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<46>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<45>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<44>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<43>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<42>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<41>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<40>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<39>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<38>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<37>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<36>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<35>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<34>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<33>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<32>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<15>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<14>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<13>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<12>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<11>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<10>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<9>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<8>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<7>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<6>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<5>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<4>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<3>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<2>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<1>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_P<0>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<47>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<46>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<45>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<44>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<43>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<42>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<41>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<40>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<39>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<38>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<37>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<36>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<35>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<34>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<33>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<32>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<31>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<30>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<29>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<28>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<27>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<26>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<25>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<24>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<23>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<22>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<21>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<20>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<19>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<18>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<17>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<16>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<15>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<14>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<13>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<12>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<11>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<10>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<9>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<8>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<7>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<6>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<5>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<4>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<3>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<2>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<1>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_PCOUT<0>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<35>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<34>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<33>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<32>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<31>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<30>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<29>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<28>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<27>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<26>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<25>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<24>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<23>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<22>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<21>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<20>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<19>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<18>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<17>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<16>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<15>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<14>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<13>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<12>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<11>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<10>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<9>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<8>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<7>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<6>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<5>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<4>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<3>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<2>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<1>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000006_M<0>_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000061/blk00000081_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000061/blk00000080_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000061/blk0000007f_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000061/blk0000007e_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000061/blk0000007d_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000061/blk0000007c_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000061/blk0000007b_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000061/blk0000007a_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000061/blk00000079_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000061/blk00000078_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000061/blk00000077_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000061/blk00000076_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000061/blk00000075_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000061/blk00000074_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000061/blk00000073_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000061/blk00000072_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000082/blk000000a2_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000082/blk000000a1_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000082/blk000000a0_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000082/blk0000009f_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000082/blk0000009e_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000082/blk0000009d_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000082/blk0000009c_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000082/blk0000009b_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000082/blk0000009a_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000082/blk00000099_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000082/blk00000098_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000082/blk00000097_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000082/blk00000096_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000082/blk00000095_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000082/blk00000094_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk00000082/blk00000093_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk000000c2/blk000000e3_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk000000c2/blk000000e2_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk000000c2/blk000000e1_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk000000c2/blk000000e0_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk000000c2/blk000000df_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk000000c2/blk000000de_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk000000c2/blk000000dd_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk000000c2/blk000000dc_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk000000c2/blk000000db_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk000000c2/blk000000da_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk000000c2/blk000000d9_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk000000c2/blk000000d8_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk000000c2/blk000000d7_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk000000c2/blk000000d6_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk000000c2/blk000000d5_Q15_UNCONNECTED ;
  wire \NLW_blk00000003/blk000000c2/blk000000d4_Q15_UNCONNECTED ;
  wire [15 : 0] din_0;
  wire [15 : 0] dout_1;
  assign
    rfd = NlwRenamedSig_OI_rfd,
    dout[15] = dout_1[15],
    dout[14] = dout_1[14],
    dout[13] = dout_1[13],
    dout[12] = dout_1[12],
    dout[11] = dout_1[11],
    dout[10] = dout_1[10],
    dout[9] = dout_1[9],
    dout[8] = dout_1[8],
    dout[7] = dout_1[7],
    dout[6] = dout_1[6],
    dout[5] = dout_1[5],
    dout[4] = dout_1[4],
    dout[3] = dout_1[3],
    dout[2] = dout_1[2],
    dout[1] = dout_1[1],
    dout[0] = dout_1[0],
    din_0[15] = din[15],
    din_0[14] = din[14],
    din_0[13] = din[13],
    din_0[12] = din[12],
    din_0[11] = din[11],
    din_0[10] = din[10],
    din_0[9] = din[9],
    din_0[8] = din[8],
    din_0[7] = din[7],
    din_0[6] = din[6],
    din_0[5] = din[5],
    din_0[4] = din[4],
    din_0[3] = din[3],
    din_0[2] = din[2],
    din_0[1] = din[1],
    din_0[0] = din[0];
  VCC   blk00000001 (
    .P(NLW_blk00000001_P_UNCONNECTED)
  );
  GND   blk00000002 (
    .G(NLW_blk00000002_G_UNCONNECTED)
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000154  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000124 ),
    .Q(\blk00000003/sig0000010d )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000153  (
    .A0(\blk00000003/sig00000026 ),
    .A1(\blk00000003/sig00000013 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(\blk00000003/sig00000087 ),
    .Q(\blk00000003/sig00000124 ),
    .Q15(\NLW_blk00000003/blk00000153_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000152  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000123 ),
    .Q(\blk00000003/sig0000010c )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000151  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000026 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(\blk00000003/sig00000073 ),
    .Q(\blk00000003/sig00000123 ),
    .Q15(\NLW_blk00000003/blk00000151_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000150  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000122 ),
    .Q(\blk00000003/sig00000104 )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk0000014f  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000013 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(din_0[0]),
    .Q(\blk00000003/sig00000122 ),
    .Q15(\NLW_blk00000003/blk0000014f_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000014e  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000121 ),
    .Q(\blk00000003/sig00000103 )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk0000014d  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000013 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(din_0[1]),
    .Q(\blk00000003/sig00000121 ),
    .Q15(\NLW_blk00000003/blk0000014d_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000014c  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000120 ),
    .Q(\blk00000003/sig0000010e )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk0000014b  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000026 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(\blk00000003/sig0000008d ),
    .Q(\blk00000003/sig00000120 ),
    .Q15(\NLW_blk00000003/blk0000014b_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000014a  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig0000011f ),
    .Q(\blk00000003/sig00000102 )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000149  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000013 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(din_0[2]),
    .Q(\blk00000003/sig0000011f ),
    .Q15(\NLW_blk00000003/blk00000149_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000148  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig0000011e ),
    .Q(\blk00000003/sig00000101 )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000147  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000013 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(din_0[3]),
    .Q(\blk00000003/sig0000011e ),
    .Q15(\NLW_blk00000003/blk00000147_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000146  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig0000011d ),
    .Q(\blk00000003/sig000000ff )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000145  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000013 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(din_0[5]),
    .Q(\blk00000003/sig0000011d ),
    .Q15(\NLW_blk00000003/blk00000145_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000144  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig0000011c ),
    .Q(\blk00000003/sig000000fe )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000143  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000013 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(din_0[6]),
    .Q(\blk00000003/sig0000011c ),
    .Q15(\NLW_blk00000003/blk00000143_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000142  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig0000011b ),
    .Q(\blk00000003/sig00000100 )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000141  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000013 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(din_0[4]),
    .Q(\blk00000003/sig0000011b ),
    .Q15(\NLW_blk00000003/blk00000141_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000140  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig0000011a ),
    .Q(\blk00000003/sig000000fd )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk0000013f  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000013 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(din_0[7]),
    .Q(\blk00000003/sig0000011a ),
    .Q15(\NLW_blk00000003/blk0000013f_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000013e  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000119 ),
    .Q(\blk00000003/sig000000fc )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk0000013d  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000013 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(din_0[8]),
    .Q(\blk00000003/sig00000119 ),
    .Q15(\NLW_blk00000003/blk0000013d_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000013c  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000118 ),
    .Q(\blk00000003/sig000000fa )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk0000013b  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000013 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(din_0[10]),
    .Q(\blk00000003/sig00000118 ),
    .Q15(\NLW_blk00000003/blk0000013b_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000013a  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000117 ),
    .Q(\blk00000003/sig000000f9 )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000139  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000013 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(din_0[11]),
    .Q(\blk00000003/sig00000117 ),
    .Q15(\NLW_blk00000003/blk00000139_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000138  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000116 ),
    .Q(\blk00000003/sig000000fb )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000137  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000013 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(din_0[9]),
    .Q(\blk00000003/sig00000116 ),
    .Q15(\NLW_blk00000003/blk00000137_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000136  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000115 ),
    .Q(\blk00000003/sig000000f8 )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000135  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000013 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(din_0[12]),
    .Q(\blk00000003/sig00000115 ),
    .Q15(\NLW_blk00000003/blk00000135_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000134  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000114 ),
    .Q(\blk00000003/sig000000f7 )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000133  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000013 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(din_0[13]),
    .Q(\blk00000003/sig00000114 ),
    .Q15(\NLW_blk00000003/blk00000133_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000132  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000113 ),
    .Q(\blk00000003/sig000000f5 )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000131  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000013 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(din_0[15]),
    .Q(\blk00000003/sig00000113 ),
    .Q15(\NLW_blk00000003/blk00000131_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000130  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000112 ),
    .Q(rdy)
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk0000012f  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000013 ),
    .A2(\blk00000003/sig00000026 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(\blk00000003/sig0000009e ),
    .Q(\blk00000003/sig00000112 ),
    .Q15(\NLW_blk00000003/blk0000012f_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000012e  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000111 ),
    .Q(\blk00000003/sig000000f6 )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk0000012d  (
    .A0(\blk00000003/sig00000013 ),
    .A1(\blk00000003/sig00000013 ),
    .A2(\blk00000003/sig00000013 ),
    .A3(\blk00000003/sig00000013 ),
    .CE(\blk00000003/sig00000026 ),
    .CLK(clk),
    .D(din_0[14]),
    .Q(\blk00000003/sig00000111 ),
    .Q15(\NLW_blk00000003/blk0000012d_Q15_UNCONNECTED )
  );
  INV   \blk00000003/blk0000012c  (
    .I(\blk00000003/sig0000005b ),
    .O(\blk00000003/sig0000006b )
  );
  INV   \blk00000003/blk0000012b  (
    .I(\blk00000003/sig00000110 ),
    .O(\blk00000003/sig00000037 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000012a  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig0000007d ),
    .Q(\blk00000003/sig0000010f )
  );
  LUT4 #(
    .INIT ( 16'h88F8 ))
  \blk00000003/blk00000129  (
    .I0(nd),
    .I1(NlwRenamedSig_OI_rfd),
    .I2(\blk00000003/sig00000075 ),
    .I3(\blk00000003/sig00000098 ),
    .O(\blk00000003/sig00000097 )
  );
  LUT4 #(
    .INIT ( 16'h88F8 ))
  \blk00000003/blk00000128  (
    .I0(nd),
    .I1(NlwRenamedSig_OI_rfd),
    .I2(\blk00000003/sig00000073 ),
    .I3(\blk00000003/sig0000007c ),
    .O(\blk00000003/sig00000095 )
  );
  LUT4 #(
    .INIT ( 16'h8C00 ))
  \blk00000003/blk00000127  (
    .I0(nd),
    .I1(\blk00000003/sig00000099 ),
    .I2(NlwRenamedSig_OI_rfd),
    .I3(\blk00000003/sig0000007f ),
    .O(\blk00000003/sig00000091 )
  );
  LUT4 #(
    .INIT ( 16'h8C00 ))
  \blk00000003/blk00000126  (
    .I0(nd),
    .I1(\blk00000003/sig0000009a ),
    .I2(NlwRenamedSig_OI_rfd),
    .I3(\blk00000003/sig0000007f ),
    .O(\blk00000003/sig00000090 )
  );
  LUT4 #(
    .INIT ( 16'h8C00 ))
  \blk00000003/blk00000125  (
    .I0(nd),
    .I1(\blk00000003/sig0000009b ),
    .I2(NlwRenamedSig_OI_rfd),
    .I3(\blk00000003/sig0000007f ),
    .O(\blk00000003/sig0000008e )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000124  (
    .C(clk),
    .D(\blk00000003/sig00000089 ),
    .Q(\blk00000003/sig00000110 )
  );
  LUT1 #(
    .INIT ( 2'h2 ))
  \blk00000003/blk00000123  (
    .I0(\blk00000003/sig00000106 ),
    .O(\blk00000003/sig00000107 )
  );
  LUT1 #(
    .INIT ( 2'h2 ))
  \blk00000003/blk00000122  (
    .I0(\blk00000003/sig000000e4 ),
    .O(\blk00000003/sig000000e2 )
  );
  LUT1 #(
    .INIT ( 2'h2 ))
  \blk00000003/blk00000121  (
    .I0(\blk00000003/sig000000df ),
    .O(\blk00000003/sig000000d7 )
  );
  LUT1 #(
    .INIT ( 2'h2 ))
  \blk00000003/blk00000120  (
    .I0(\blk00000003/sig000000d5 ),
    .O(\blk00000003/sig000000c7 )
  );
  LUT1 #(
    .INIT ( 2'h2 ))
  \blk00000003/blk0000011f  (
    .I0(\blk00000003/sig000000b4 ),
    .O(\blk00000003/sig000000b1 )
  );
  LUT1 #(
    .INIT ( 2'h2 ))
  \blk00000003/blk0000011e  (
    .I0(\blk00000003/sig000000a9 ),
    .O(\blk00000003/sig000000a6 )
  );
  LUT1 #(
    .INIT ( 2'h2 ))
  \blk00000003/blk0000011d  (
    .I0(\blk00000003/sig0000005b ),
    .O(\blk00000003/sig0000006e )
  );
  LUT1 #(
    .INIT ( 2'h2 ))
  \blk00000003/blk0000011c  (
    .I0(\blk00000003/sig0000007c ),
    .O(\blk00000003/sig00000069 )
  );
  LUT1 #(
    .INIT ( 2'h2 ))
  \blk00000003/blk0000011b  (
    .I0(\blk00000003/sig0000010f ),
    .O(\blk00000003/sig00000063 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000011a  (
    .C(clk),
    .D(\blk00000003/sig00000108 ),
    .Q(\blk00000003/sig00000106 )
  );
  FD #(
    .INIT ( 1'b1 ))
  \blk00000003/blk00000119  (
    .C(clk),
    .D(\blk00000003/sig0000010b ),
    .Q(\blk00000003/sig00000105 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000118  (
    .C(clk),
    .D(\blk00000003/sig000000e3 ),
    .Q(\blk00000003/sig000000e4 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000117  (
    .C(clk),
    .D(\blk00000003/sig000000de ),
    .Q(\blk00000003/sig000000e1 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000116  (
    .C(clk),
    .D(\blk00000003/sig000000db ),
    .Q(\blk00000003/sig000000e0 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000115  (
    .C(clk),
    .D(\blk00000003/sig000000d8 ),
    .Q(\blk00000003/sig000000df )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000114  (
    .C(clk),
    .D(\blk00000003/sig000000c8 ),
    .Q(\blk00000003/sig000000d5 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000113  (
    .C(clk),
    .D(\blk00000003/sig000000d1 ),
    .Q(\blk00000003/sig000000d4 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000112  (
    .C(clk),
    .D(\blk00000003/sig000000ce ),
    .Q(\blk00000003/sig000000d3 )
  );
  FD #(
    .INIT ( 1'b1 ))
  \blk00000003/blk00000111  (
    .C(clk),
    .D(\blk00000003/sig000000cb ),
    .Q(\blk00000003/sig000000d2 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000110  (
    .C(clk),
    .D(\blk00000003/sig000000b8 ),
    .Q(\blk00000003/sig000000c6 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000010f  (
    .C(clk),
    .D(\blk00000003/sig000000c1 ),
    .Q(\blk00000003/sig000000c5 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000010e  (
    .C(clk),
    .D(\blk00000003/sig000000be ),
    .Q(\blk00000003/sig000000c4 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000010d  (
    .C(clk),
    .D(\blk00000003/sig000000bb ),
    .Q(\blk00000003/sig000000c3 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000010c  (
    .C(clk),
    .D(\blk00000003/sig000000ac ),
    .Q(\blk00000003/sig000000b5 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000010b  (
    .C(clk),
    .D(\blk00000003/sig000000b2 ),
    .Q(\blk00000003/sig000000b4 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000010a  (
    .C(clk),
    .D(\blk00000003/sig000000af ),
    .Q(\blk00000003/sig000000b3 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000109  (
    .C(clk),
    .D(\blk00000003/sig000000a1 ),
    .Q(\blk00000003/sig000000aa )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000108  (
    .C(clk),
    .D(\blk00000003/sig000000a7 ),
    .Q(\blk00000003/sig000000a9 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000107  (
    .C(clk),
    .D(\blk00000003/sig000000a4 ),
    .Q(\blk00000003/sig000000a8 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000106  (
    .C(clk),
    .D(\blk00000003/sig0000005d ),
    .Q(\blk00000003/sig0000005b )
  );
  LUT2 #(
    .INIT ( 4'h6 ))
  \blk00000003/blk00000105  (
    .I0(\blk00000003/sig00000105 ),
    .I1(\blk00000003/sig0000007e ),
    .O(\blk00000003/sig0000010a )
  );
  LUT3 #(
    .INIT ( 8'h78 ))
  \blk00000003/blk00000104  (
    .I0(\blk00000003/sig00000083 ),
    .I1(\blk00000003/sig00000085 ),
    .I2(\blk00000003/sig000000e0 ),
    .O(\blk00000003/sig000000da )
  );
  LUT3 #(
    .INIT ( 8'h78 ))
  \blk00000003/blk00000103  (
    .I0(\blk00000003/sig00000083 ),
    .I1(\blk00000003/sig00000085 ),
    .I2(\blk00000003/sig000000e1 ),
    .O(\blk00000003/sig000000dd )
  );
  LUT2 #(
    .INIT ( 4'h6 ))
  \blk00000003/blk00000102  (
    .I0(\blk00000003/sig00000080 ),
    .I1(\blk00000003/sig000000d2 ),
    .O(\blk00000003/sig000000ca )
  );
  LUT3 #(
    .INIT ( 8'hB4 ))
  \blk00000003/blk00000101  (
    .I0(\blk00000003/sig0000008f ),
    .I1(\blk00000003/sig00000080 ),
    .I2(\blk00000003/sig000000d3 ),
    .O(\blk00000003/sig000000cd )
  );
  LUT3 #(
    .INIT ( 8'hB4 ))
  \blk00000003/blk00000100  (
    .I0(\blk00000003/sig0000008f ),
    .I1(\blk00000003/sig00000080 ),
    .I2(\blk00000003/sig000000d4 ),
    .O(\blk00000003/sig000000d0 )
  );
  LUT2 #(
    .INIT ( 4'h7 ))
  \blk00000003/blk000000ff  (
    .I0(\blk00000003/sig0000008f ),
    .I1(\blk00000003/sig00000080 ),
    .O(\blk00000003/sig000000c2 )
  );
  LUT3 #(
    .INIT ( 8'hF8 ))
  \blk00000003/blk000000fe  (
    .I0(\blk00000003/sig0000008f ),
    .I1(\blk00000003/sig00000080 ),
    .I2(\blk00000003/sig000000c4 ),
    .O(\blk00000003/sig000000bd )
  );
  LUT3 #(
    .INIT ( 8'hF8 ))
  \blk00000003/blk000000fd  (
    .I0(\blk00000003/sig0000008f ),
    .I1(\blk00000003/sig00000080 ),
    .I2(\blk00000003/sig000000c3 ),
    .O(\blk00000003/sig000000ba )
  );
  LUT3 #(
    .INIT ( 8'hF8 ))
  \blk00000003/blk000000fc  (
    .I0(\blk00000003/sig0000008f ),
    .I1(\blk00000003/sig00000080 ),
    .I2(\blk00000003/sig000000c6 ),
    .O(\blk00000003/sig000000b7 )
  );
  LUT3 #(
    .INIT ( 8'hBC ))
  \blk00000003/blk000000fb  (
    .I0(\blk00000003/sig0000008f ),
    .I1(\blk00000003/sig00000080 ),
    .I2(\blk00000003/sig000000c5 ),
    .O(\blk00000003/sig000000c0 )
  );
  LUT3 #(
    .INIT ( 8'h6A ))
  \blk00000003/blk000000fa  (
    .I0(\blk00000003/sig000000b3 ),
    .I1(\blk00000003/sig00000073 ),
    .I2(\blk00000003/sig0000007c ),
    .O(\blk00000003/sig000000ae )
  );
  LUT3 #(
    .INIT ( 8'hA6 ))
  \blk00000003/blk000000f9  (
    .I0(\blk00000003/sig000000b5 ),
    .I1(\blk00000003/sig00000073 ),
    .I2(\blk00000003/sig0000007c ),
    .O(\blk00000003/sig000000ab )
  );
  LUT3 #(
    .INIT ( 8'h6A ))
  \blk00000003/blk000000f8  (
    .I0(\blk00000003/sig000000a8 ),
    .I1(\blk00000003/sig00000075 ),
    .I2(\blk00000003/sig00000098 ),
    .O(\blk00000003/sig000000a3 )
  );
  LUT3 #(
    .INIT ( 8'h9A ))
  \blk00000003/blk000000f7  (
    .I0(\blk00000003/sig000000aa ),
    .I1(\blk00000003/sig00000098 ),
    .I2(\blk00000003/sig00000075 ),
    .O(\blk00000003/sig000000a0 )
  );
  LUT2 #(
    .INIT ( 4'h2 ))
  \blk00000003/blk000000f6  (
    .I0(\blk00000003/sig0000007c ),
    .I1(\blk00000003/sig0000008d ),
    .O(\blk00000003/sig0000009c )
  );
  LUT2 #(
    .INIT ( 4'h8 ))
  \blk00000003/blk000000f5  (
    .I0(\blk00000003/sig0000007c ),
    .I1(\blk00000003/sig00000068 ),
    .O(\blk00000003/sig0000009d )
  );
  LUT2 #(
    .INIT ( 4'hB ))
  \blk00000003/blk000000f4  (
    .I0(nd),
    .I1(NlwRenamedSig_OI_rfd),
    .O(\blk00000003/sig0000007a )
  );
  LUT3 #(
    .INIT ( 8'h40 ))
  \blk00000003/blk000000f3  (
    .I0(\blk00000003/sig000000a8 ),
    .I1(\blk00000003/sig000000a9 ),
    .I2(\blk00000003/sig000000aa ),
    .O(\blk00000003/sig00000076 )
  );
  LUT3 #(
    .INIT ( 8'h40 ))
  \blk00000003/blk000000f2  (
    .I0(\blk00000003/sig000000b3 ),
    .I1(\blk00000003/sig000000b4 ),
    .I2(\blk00000003/sig000000b5 ),
    .O(\blk00000003/sig00000074 )
  );
  LUT2 #(
    .INIT ( 4'h8 ))
  \blk00000003/blk000000f1  (
    .I0(NlwRenamedSig_OI_rfd),
    .I1(nd),
    .O(\blk00000003/sig0000009f )
  );
  LUT2 #(
    .INIT ( 4'h1 ))
  \blk00000003/blk000000f0  (
    .I0(\blk00000003/sig00000105 ),
    .I1(\blk00000003/sig00000106 ),
    .O(\blk00000003/sig00000066 )
  );
  LUT2 #(
    .INIT ( 4'h6 ))
  \blk00000003/blk000000ef  (
    .I0(\blk00000003/sig0000005b ),
    .I1(\blk00000003/sig0000007c ),
    .O(\blk00000003/sig0000005c )
  );
  LUT5 #(
    .INIT ( 32'hA8F0F8F0 ))
  \blk00000003/blk000000ee  (
    .I0(nd),
    .I1(\blk00000003/sig00000093 ),
    .I2(\blk00000003/sig00000073 ),
    .I3(NlwRenamedSig_OI_rfd),
    .I4(\blk00000003/sig0000007f ),
    .O(\blk00000003/sig00000094 )
  );
  LUT4 #(
    .INIT ( 16'h3A2A ))
  \blk00000003/blk000000ed  (
    .I0(\blk00000003/sig00000093 ),
    .I1(nd),
    .I2(NlwRenamedSig_OI_rfd),
    .I3(\blk00000003/sig0000007f ),
    .O(\blk00000003/sig00000092 )
  );
  LUT3 #(
    .INIT ( 8'h54 ))
  \blk00000003/blk000000ec  (
    .I0(\blk00000003/sig00000096 ),
    .I1(\blk00000003/sig0000007c ),
    .I2(\blk00000003/sig0000008d ),
    .O(\blk00000003/sig0000008c )
  );
  LUT3 #(
    .INIT ( 8'h54 ))
  \blk00000003/blk000000eb  (
    .I0(\blk00000003/sig0000008d ),
    .I1(\blk00000003/sig0000008b ),
    .I2(\blk00000003/sig00000086 ),
    .O(\blk00000003/sig0000008a )
  );
  LUT3 #(
    .INIT ( 8'hF2 ))
  \blk00000003/blk000000ea  (
    .I0(\blk00000003/sig0000010d ),
    .I1(\blk00000003/sig0000010e ),
    .I2(\blk00000003/sig0000010c ),
    .O(\blk00000003/sig00000060 )
  );
  LUT3 #(
    .INIT ( 8'h51 ))
  \blk00000003/blk000000e9  (
    .I0(\blk00000003/sig0000010c ),
    .I1(\blk00000003/sig0000010d ),
    .I2(\blk00000003/sig0000010e ),
    .O(\blk00000003/sig0000005f )
  );
  LUT3 #(
    .INIT ( 8'hA2 ))
  \blk00000003/blk000000e8  (
    .I0(\blk00000003/sig0000010c ),
    .I1(\blk00000003/sig0000010d ),
    .I2(\blk00000003/sig0000010e ),
    .O(\blk00000003/sig0000005e )
  );
  MUXCY_L   \blk00000003/blk000000e7  (
    .CI(\blk00000003/sig00000013 ),
    .DI(\blk00000003/sig00000106 ),
    .S(\blk00000003/sig00000107 ),
    .LO(\blk00000003/sig00000109 )
  );
  MUXCY_D   \blk00000003/blk000000e6  (
    .CI(\blk00000003/sig00000109 ),
    .DI(\blk00000003/sig00000105 ),
    .S(\blk00000003/sig0000010a ),
    .O(\NLW_blk00000003/blk000000e6_O_UNCONNECTED ),
    .LO(\NLW_blk00000003/blk000000e6_LO_UNCONNECTED )
  );
  XORCY   \blk00000003/blk000000e5  (
    .CI(\blk00000003/sig00000109 ),
    .LI(\blk00000003/sig0000010a ),
    .O(\blk00000003/sig0000010b )
  );
  XORCY   \blk00000003/blk000000e4  (
    .CI(\blk00000003/sig00000013 ),
    .LI(\blk00000003/sig00000107 ),
    .O(\blk00000003/sig00000108 )
  );
  MUXCY_L   \blk00000003/blk00000060  (
    .CI(\blk00000003/sig00000081 ),
    .DI(\blk00000003/sig000000e4 ),
    .S(\blk00000003/sig000000e2 ),
    .LO(\blk00000003/sig000000dc )
  );
  XORCY   \blk00000003/blk0000005f  (
    .CI(\blk00000003/sig00000081 ),
    .LI(\blk00000003/sig000000e2 ),
    .O(\blk00000003/sig000000e3 )
  );
  MUXCY_L   \blk00000003/blk0000005e  (
    .CI(\blk00000003/sig000000dc ),
    .DI(\blk00000003/sig000000e1 ),
    .S(\blk00000003/sig000000dd ),
    .LO(\blk00000003/sig000000d9 )
  );
  MUXCY_L   \blk00000003/blk0000005d  (
    .CI(\blk00000003/sig000000d9 ),
    .DI(\blk00000003/sig000000e0 ),
    .S(\blk00000003/sig000000da ),
    .LO(\blk00000003/sig000000d6 )
  );
  MUXCY_D   \blk00000003/blk0000005c  (
    .CI(\blk00000003/sig000000d6 ),
    .DI(\blk00000003/sig000000df ),
    .S(\blk00000003/sig000000d7 ),
    .O(\NLW_blk00000003/blk0000005c_O_UNCONNECTED ),
    .LO(\NLW_blk00000003/blk0000005c_LO_UNCONNECTED )
  );
  XORCY   \blk00000003/blk0000005b  (
    .CI(\blk00000003/sig000000dc ),
    .LI(\blk00000003/sig000000dd ),
    .O(\blk00000003/sig000000de )
  );
  XORCY   \blk00000003/blk0000005a  (
    .CI(\blk00000003/sig000000d9 ),
    .LI(\blk00000003/sig000000da ),
    .O(\blk00000003/sig000000db )
  );
  XORCY   \blk00000003/blk00000059  (
    .CI(\blk00000003/sig000000d6 ),
    .LI(\blk00000003/sig000000d7 ),
    .O(\blk00000003/sig000000d8 )
  );
  MUXCY_L   \blk00000003/blk00000058  (
    .CI(\blk00000003/sig00000013 ),
    .DI(\blk00000003/sig000000d5 ),
    .S(\blk00000003/sig000000c7 ),
    .LO(\blk00000003/sig000000cf )
  );
  MUXCY_L   \blk00000003/blk00000057  (
    .CI(\blk00000003/sig000000cf ),
    .DI(\blk00000003/sig000000d4 ),
    .S(\blk00000003/sig000000d0 ),
    .LO(\blk00000003/sig000000cc )
  );
  MUXCY_L   \blk00000003/blk00000056  (
    .CI(\blk00000003/sig000000cc ),
    .DI(\blk00000003/sig000000d3 ),
    .S(\blk00000003/sig000000cd ),
    .LO(\blk00000003/sig000000c9 )
  );
  MUXCY_D   \blk00000003/blk00000055  (
    .CI(\blk00000003/sig000000c9 ),
    .DI(\blk00000003/sig000000d2 ),
    .S(\blk00000003/sig000000ca ),
    .O(\NLW_blk00000003/blk00000055_O_UNCONNECTED ),
    .LO(\NLW_blk00000003/blk00000055_LO_UNCONNECTED )
  );
  XORCY   \blk00000003/blk00000054  (
    .CI(\blk00000003/sig000000cf ),
    .LI(\blk00000003/sig000000d0 ),
    .O(\blk00000003/sig000000d1 )
  );
  XORCY   \blk00000003/blk00000053  (
    .CI(\blk00000003/sig000000cc ),
    .LI(\blk00000003/sig000000cd ),
    .O(\blk00000003/sig000000ce )
  );
  XORCY   \blk00000003/blk00000052  (
    .CI(\blk00000003/sig000000c9 ),
    .LI(\blk00000003/sig000000ca ),
    .O(\blk00000003/sig000000cb )
  );
  XORCY   \blk00000003/blk00000051  (
    .CI(\blk00000003/sig00000013 ),
    .LI(\blk00000003/sig000000c7 ),
    .O(\blk00000003/sig000000c8 )
  );
  MUXCY_L   \blk00000003/blk00000050  (
    .CI(\blk00000003/sig000000b6 ),
    .DI(\blk00000003/sig000000c6 ),
    .S(\blk00000003/sig000000b7 ),
    .LO(\blk00000003/sig000000bf )
  );
  MUXCY_L   \blk00000003/blk0000004f  (
    .CI(\blk00000003/sig000000bf ),
    .DI(\blk00000003/sig000000c5 ),
    .S(\blk00000003/sig000000c0 ),
    .LO(\blk00000003/sig000000bc )
  );
  MUXCY_L   \blk00000003/blk0000004e  (
    .CI(\blk00000003/sig000000bc ),
    .DI(\blk00000003/sig000000c4 ),
    .S(\blk00000003/sig000000bd ),
    .LO(\blk00000003/sig000000b9 )
  );
  MUXCY_D   \blk00000003/blk0000004d  (
    .CI(\blk00000003/sig000000b9 ),
    .DI(\blk00000003/sig000000c3 ),
    .S(\blk00000003/sig000000ba ),
    .O(\NLW_blk00000003/blk0000004d_O_UNCONNECTED ),
    .LO(\NLW_blk00000003/blk0000004d_LO_UNCONNECTED )
  );
  MUXCY   \blk00000003/blk0000004c  (
    .CI(\blk00000003/sig00000013 ),
    .DI(\blk00000003/sig00000026 ),
    .S(\blk00000003/sig000000c2 ),
    .O(\blk00000003/sig000000b6 )
  );
  XORCY   \blk00000003/blk0000004b  (
    .CI(\blk00000003/sig000000bf ),
    .LI(\blk00000003/sig000000c0 ),
    .O(\blk00000003/sig000000c1 )
  );
  XORCY   \blk00000003/blk0000004a  (
    .CI(\blk00000003/sig000000bc ),
    .LI(\blk00000003/sig000000bd ),
    .O(\blk00000003/sig000000be )
  );
  XORCY   \blk00000003/blk00000049  (
    .CI(\blk00000003/sig000000b9 ),
    .LI(\blk00000003/sig000000ba ),
    .O(\blk00000003/sig000000bb )
  );
  XORCY   \blk00000003/blk00000048  (
    .CI(\blk00000003/sig000000b6 ),
    .LI(\blk00000003/sig000000b7 ),
    .O(\blk00000003/sig000000b8 )
  );
  MUXCY_L   \blk00000003/blk00000047  (
    .CI(\blk00000003/sig00000013 ),
    .DI(\blk00000003/sig000000b5 ),
    .S(\blk00000003/sig000000ab ),
    .LO(\blk00000003/sig000000b0 )
  );
  MUXCY_L   \blk00000003/blk00000046  (
    .CI(\blk00000003/sig000000b0 ),
    .DI(\blk00000003/sig000000b4 ),
    .S(\blk00000003/sig000000b1 ),
    .LO(\blk00000003/sig000000ad )
  );
  MUXCY_D   \blk00000003/blk00000045  (
    .CI(\blk00000003/sig000000ad ),
    .DI(\blk00000003/sig000000b3 ),
    .S(\blk00000003/sig000000ae ),
    .O(\NLW_blk00000003/blk00000045_O_UNCONNECTED ),
    .LO(\NLW_blk00000003/blk00000045_LO_UNCONNECTED )
  );
  XORCY   \blk00000003/blk00000044  (
    .CI(\blk00000003/sig000000b0 ),
    .LI(\blk00000003/sig000000b1 ),
    .O(\blk00000003/sig000000b2 )
  );
  XORCY   \blk00000003/blk00000043  (
    .CI(\blk00000003/sig000000ad ),
    .LI(\blk00000003/sig000000ae ),
    .O(\blk00000003/sig000000af )
  );
  XORCY   \blk00000003/blk00000042  (
    .CI(\blk00000003/sig00000013 ),
    .LI(\blk00000003/sig000000ab ),
    .O(\blk00000003/sig000000ac )
  );
  MUXCY_L   \blk00000003/blk00000041  (
    .CI(\blk00000003/sig00000013 ),
    .DI(\blk00000003/sig000000aa ),
    .S(\blk00000003/sig000000a0 ),
    .LO(\blk00000003/sig000000a5 )
  );
  MUXCY_L   \blk00000003/blk00000040  (
    .CI(\blk00000003/sig000000a5 ),
    .DI(\blk00000003/sig000000a9 ),
    .S(\blk00000003/sig000000a6 ),
    .LO(\blk00000003/sig000000a2 )
  );
  MUXCY_D   \blk00000003/blk0000003f  (
    .CI(\blk00000003/sig000000a2 ),
    .DI(\blk00000003/sig000000a8 ),
    .S(\blk00000003/sig000000a3 ),
    .O(\NLW_blk00000003/blk0000003f_O_UNCONNECTED ),
    .LO(\NLW_blk00000003/blk0000003f_LO_UNCONNECTED )
  );
  XORCY   \blk00000003/blk0000003e  (
    .CI(\blk00000003/sig000000a5 ),
    .LI(\blk00000003/sig000000a6 ),
    .O(\blk00000003/sig000000a7 )
  );
  XORCY   \blk00000003/blk0000003d  (
    .CI(\blk00000003/sig000000a2 ),
    .LI(\blk00000003/sig000000a3 ),
    .O(\blk00000003/sig000000a4 )
  );
  XORCY   \blk00000003/blk0000003c  (
    .CI(\blk00000003/sig00000013 ),
    .LI(\blk00000003/sig000000a0 ),
    .O(\blk00000003/sig000000a1 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000003b  (
    .C(clk),
    .D(\blk00000003/sig0000009f ),
    .Q(\blk00000003/sig00000086 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000003a  (
    .C(clk),
    .D(\blk00000003/sig0000009d ),
    .Q(\blk00000003/sig0000009e )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000039  (
    .C(clk),
    .D(\blk00000003/sig0000009c ),
    .Q(\blk00000003/sig00000088 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000038  (
    .C(clk),
    .CE(\blk00000003/sig0000007f ),
    .D(\blk00000003/sig0000007c ),
    .Q(\blk00000003/sig0000009b )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000037  (
    .C(clk),
    .CE(\blk00000003/sig0000007f ),
    .D(\blk00000003/sig00000068 ),
    .Q(\blk00000003/sig0000009a )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000036  (
    .C(clk),
    .CE(\blk00000003/sig0000007f ),
    .D(\blk00000003/sig00000096 ),
    .Q(\blk00000003/sig00000099 )
  );
  FDSE #(
    .INIT ( 1'b1 ))
  \blk00000003/blk00000035  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig0000007b ),
    .S(\blk00000003/sig00000013 ),
    .Q(NlwRenamedSig_OI_rfd)
  );
  FDRE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000034  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000079 ),
    .R(\blk00000003/sig00000013 ),
    .Q(\blk00000003/sig00000098 )
  );
  FDRE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000033  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000097 ),
    .R(\blk00000003/sig00000013 ),
    .Q(\blk00000003/sig00000075 )
  );
  FDRE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000032  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000072 ),
    .R(\blk00000003/sig00000013 ),
    .Q(\blk00000003/sig0000007c )
  );
  FDRE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000031  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000070 ),
    .R(\blk00000003/sig00000013 ),
    .Q(\blk00000003/sig00000096 )
  );
  FDRE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000030  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig0000006c ),
    .R(\blk00000003/sig00000013 ),
    .Q(\blk00000003/sig00000068 )
  );
  FDRE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000002f  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000095 ),
    .R(\blk00000003/sig00000013 ),
    .Q(\blk00000003/sig00000073 )
  );
  FDRE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000002e  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000094 ),
    .R(\blk00000003/sig00000013 ),
    .Q(\blk00000003/sig0000007f )
  );
  FDRE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000002d  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000092 ),
    .R(\blk00000003/sig00000013 ),
    .Q(\blk00000003/sig00000093 )
  );
  FDRE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000002c  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000091 ),
    .R(\blk00000003/sig00000013 ),
    .Q(\blk00000003/sig00000082 )
  );
  FDRE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000002b  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000090 ),
    .R(\blk00000003/sig00000013 ),
    .Q(\NLW_blk00000003/blk0000002b_Q_UNCONNECTED )
  );
  FDRE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000002a  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig0000008e ),
    .R(\blk00000003/sig00000013 ),
    .Q(\blk00000003/sig00000084 )
  );
  FDRE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000029  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig0000008e ),
    .R(\blk00000003/sig00000013 ),
    .Q(\blk00000003/sig0000008f )
  );
  FDRE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000028  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig0000008c ),
    .R(\blk00000003/sig00000013 ),
    .Q(\blk00000003/sig0000008d )
  );
  FDRE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000027  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig0000008a ),
    .R(\blk00000003/sig00000013 ),
    .Q(\blk00000003/sig0000008b )
  );
  FDRE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000026  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000088 ),
    .R(\blk00000003/sig00000013 ),
    .Q(\blk00000003/sig00000089 )
  );
  FDRE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000025  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000086 ),
    .R(\blk00000003/sig00000013 ),
    .Q(\blk00000003/sig00000087 )
  );
  FDRE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000024  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000084 ),
    .R(\blk00000003/sig00000013 ),
    .Q(\blk00000003/sig00000085 )
  );
  FDRE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000023  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000082 ),
    .R(\blk00000003/sig00000013 ),
    .Q(\blk00000003/sig00000083 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000022  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000080 ),
    .Q(\blk00000003/sig00000081 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000021  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig0000007f ),
    .Q(\blk00000003/sig00000080 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000020  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig0000007d ),
    .Q(\blk00000003/sig0000007e )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000001f  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig0000007c ),
    .Q(\blk00000003/sig0000007d )
  );
  FDSE #(
    .INIT ( 1'b1 ))
  \blk00000003/blk0000001e  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/sig00000065 ),
    .S(\blk00000003/sig00000013 ),
    .Q(\blk00000003/sig00000062 )
  );
  MUXCY   \blk00000003/blk0000001d  (
    .CI(\blk00000003/sig00000078 ),
    .DI(\blk00000003/sig00000026 ),
    .S(\blk00000003/sig0000007a ),
    .O(\blk00000003/sig0000007b )
  );
  XORCY   \blk00000003/blk0000001c  (
    .CI(\blk00000003/sig00000077 ),
    .LI(\blk00000003/sig00000013 ),
    .O(\blk00000003/sig00000079 )
  );
  MUXCY   \blk00000003/blk0000001b  (
    .CI(\blk00000003/sig00000077 ),
    .DI(\blk00000003/sig00000013 ),
    .S(\blk00000003/sig00000026 ),
    .O(\blk00000003/sig00000078 )
  );
  MUXCY_D   \blk00000003/blk0000001a  (
    .CI(\blk00000003/sig00000075 ),
    .DI(\blk00000003/sig00000013 ),
    .S(\blk00000003/sig00000076 ),
    .O(\blk00000003/sig00000077 ),
    .LO(\NLW_blk00000003/blk0000001a_LO_UNCONNECTED )
  );
  MUXCY_D   \blk00000003/blk00000019  (
    .CI(\blk00000003/sig00000073 ),
    .DI(\blk00000003/sig00000013 ),
    .S(\blk00000003/sig00000074 ),
    .O(\blk00000003/sig00000071 ),
    .LO(\NLW_blk00000003/blk00000019_LO_UNCONNECTED )
  );
  MUXCY   \blk00000003/blk00000018  (
    .CI(\blk00000003/sig00000071 ),
    .DI(\blk00000003/sig00000013 ),
    .S(\blk00000003/sig00000026 ),
    .O(\blk00000003/sig0000006d )
  );
  XORCY   \blk00000003/blk00000017  (
    .CI(\blk00000003/sig00000071 ),
    .LI(\blk00000003/sig00000013 ),
    .O(\blk00000003/sig00000072 )
  );
  XORCY   \blk00000003/blk00000016  (
    .CI(\blk00000003/sig0000006f ),
    .LI(\blk00000003/sig00000013 ),
    .O(\blk00000003/sig00000070 )
  );
  MUXCY   \blk00000003/blk00000015  (
    .CI(\blk00000003/sig0000006f ),
    .DI(\blk00000003/sig00000013 ),
    .S(\blk00000003/sig00000026 ),
    .O(\NLW_blk00000003/blk00000015_O_UNCONNECTED )
  );
  MUXCY_D   \blk00000003/blk00000014  (
    .CI(\blk00000003/sig0000006d ),
    .DI(\blk00000003/sig00000013 ),
    .S(\blk00000003/sig0000006e ),
    .O(\blk00000003/sig0000006f ),
    .LO(\NLW_blk00000003/blk00000014_LO_UNCONNECTED )
  );
  XORCY   \blk00000003/blk00000013  (
    .CI(\blk00000003/sig0000006a ),
    .LI(\blk00000003/sig00000013 ),
    .O(\blk00000003/sig0000006c )
  );
  MUXCY   \blk00000003/blk00000012  (
    .CI(\blk00000003/sig0000006a ),
    .DI(\blk00000003/sig00000013 ),
    .S(\blk00000003/sig00000026 ),
    .O(\NLW_blk00000003/blk00000012_O_UNCONNECTED )
  );
  MUXCY_D   \blk00000003/blk00000011  (
    .CI(\blk00000003/sig00000026 ),
    .DI(\blk00000003/sig00000013 ),
    .S(\blk00000003/sig0000006b ),
    .O(\blk00000003/sig00000067 ),
    .LO(\NLW_blk00000003/blk00000011_LO_UNCONNECTED )
  );
  MUXCY_D   \blk00000003/blk00000010  (
    .CI(\blk00000003/sig00000067 ),
    .DI(\blk00000003/sig00000068 ),
    .S(\blk00000003/sig00000069 ),
    .O(\blk00000003/sig0000006a ),
    .LO(\NLW_blk00000003/blk00000010_LO_UNCONNECTED )
  );
  MUXCY_D   \blk00000003/blk0000000f  (
    .CI(\blk00000003/sig00000026 ),
    .DI(\blk00000003/sig00000013 ),
    .S(\blk00000003/sig00000066 ),
    .O(\blk00000003/sig00000061 ),
    .LO(\NLW_blk00000003/blk0000000f_LO_UNCONNECTED )
  );
  XORCY   \blk00000003/blk0000000e  (
    .CI(\blk00000003/sig00000064 ),
    .LI(\blk00000003/sig00000013 ),
    .O(\blk00000003/sig00000065 )
  );
  MUXCY   \blk00000003/blk0000000d  (
    .CI(\blk00000003/sig00000064 ),
    .DI(\blk00000003/sig00000013 ),
    .S(\blk00000003/sig00000026 ),
    .O(\NLW_blk00000003/blk0000000d_O_UNCONNECTED )
  );
  MUXCY_D   \blk00000003/blk0000000c  (
    .CI(\blk00000003/sig00000061 ),
    .DI(\blk00000003/sig00000062 ),
    .S(\blk00000003/sig00000063 ),
    .O(\blk00000003/sig00000064 ),
    .LO(\NLW_blk00000003/blk0000000c_LO_UNCONNECTED )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000000b  (
    .C(clk),
    .D(\blk00000003/sig00000060 ),
    .Q(\blk00000003/sig0000003a )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk0000000a  (
    .C(clk),
    .D(\blk00000003/sig0000005f ),
    .Q(\blk00000003/sig00000039 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000009  (
    .C(clk),
    .D(\blk00000003/sig0000005e ),
    .Q(\blk00000003/sig00000038 )
  );
  XORCY   \blk00000003/blk00000008  (
    .CI(\blk00000003/sig00000013 ),
    .LI(\blk00000003/sig0000005c ),
    .O(\blk00000003/sig0000005d )
  );
  MUXCY_D   \blk00000003/blk00000007  (
    .CI(\blk00000003/sig00000013 ),
    .DI(\blk00000003/sig0000005b ),
    .S(\blk00000003/sig0000005c ),
    .O(\NLW_blk00000003/blk00000007_O_UNCONNECTED ),
    .LO(\NLW_blk00000003/blk00000007_LO_UNCONNECTED )
  );
  DSP48A1 #(
    .A0REG ( 0 ),
    .A1REG ( 1 ),
    .B0REG ( 1 ),
    .B1REG ( 1 ),
    .CARRYINREG ( 0 ),
    .CARRYINSEL ( "OPMODE5" ),
    .CARRYOUTREG ( 0 ),
    .CREG ( 1 ),
    .DREG ( 1 ),
    .MREG ( 1 ),
    .OPMODEREG ( 1 ),
    .PREG ( 1 ),
    .RSTTYPE ( "SYNC" ))
  \blk00000003/blk00000006  (
    .CECARRYIN(\blk00000003/sig00000026 ),
    .RSTC(\blk00000003/sig00000013 ),
    .RSTCARRYIN(\blk00000003/sig00000013 ),
    .CED(\blk00000003/sig00000026 ),
    .RSTD(\blk00000003/sig00000013 ),
    .CEOPMODE(\blk00000003/sig00000026 ),
    .CEC(\blk00000003/sig00000013 ),
    .CARRYOUTF(\NLW_blk00000003/blk00000006_CARRYOUTF_UNCONNECTED ),
    .RSTOPMODE(\blk00000003/sig00000013 ),
    .RSTM(\blk00000003/sig00000013 ),
    .CLK(clk),
    .RSTB(\blk00000003/sig00000013 ),
    .CEM(\blk00000003/sig00000026 ),
    .CEB(\blk00000003/sig00000026 ),
    .CARRYIN(\blk00000003/sig00000013 ),
    .CEP(\blk00000003/sig00000026 ),
    .CEA(\blk00000003/sig00000026 ),
    .CARRYOUT(\NLW_blk00000003/blk00000006_CARRYOUT_UNCONNECTED ),
    .RSTA(\blk00000003/sig00000013 ),
    .RSTP(\blk00000003/sig00000013 ),
    .B({\blk00000003/sig00000027 , \blk00000003/sig00000027 , \blk00000003/sig00000027 , \blk00000003/sig00000028 , \blk00000003/sig00000029 , 
\blk00000003/sig0000002a , \blk00000003/sig0000002b , \blk00000003/sig0000002c , \blk00000003/sig0000002d , \blk00000003/sig0000002e , 
\blk00000003/sig0000002f , \blk00000003/sig00000030 , \blk00000003/sig00000031 , \blk00000003/sig00000032 , \blk00000003/sig00000033 , 
\blk00000003/sig00000034 , \blk00000003/sig00000035 , \blk00000003/sig00000036 }),
    .BCOUT({\NLW_blk00000003/blk00000006_BCOUT<17>_UNCONNECTED , \NLW_blk00000003/blk00000006_BCOUT<16>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_BCOUT<15>_UNCONNECTED , \NLW_blk00000003/blk00000006_BCOUT<14>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_BCOUT<13>_UNCONNECTED , \NLW_blk00000003/blk00000006_BCOUT<12>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_BCOUT<11>_UNCONNECTED , \NLW_blk00000003/blk00000006_BCOUT<10>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_BCOUT<9>_UNCONNECTED , \NLW_blk00000003/blk00000006_BCOUT<8>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_BCOUT<7>_UNCONNECTED , \NLW_blk00000003/blk00000006_BCOUT<6>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_BCOUT<5>_UNCONNECTED , \NLW_blk00000003/blk00000006_BCOUT<4>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_BCOUT<3>_UNCONNECTED , \NLW_blk00000003/blk00000006_BCOUT<2>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_BCOUT<1>_UNCONNECTED , \NLW_blk00000003/blk00000006_BCOUT<0>_UNCONNECTED }),
    .PCIN({\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , 
\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , 
\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , 
\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , 
\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , 
\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , 
\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , 
\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , 
\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , 
\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 }),
    .C({\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , 
\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , 
\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , 
\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , 
\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , 
\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , 
\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , 
\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , 
\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , 
\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 }),
    .P({\NLW_blk00000003/blk00000006_P<47>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<46>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_P<45>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<44>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<43>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_P<42>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<41>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<40>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_P<39>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<38>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<37>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_P<36>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<35>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<34>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_P<33>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<32>_UNCONNECTED , dout_1[15], dout_1[14], dout_1[13], dout_1[12], 
dout_1[11], dout_1[10], dout_1[9], dout_1[8], dout_1[7], dout_1[6], dout_1[5], dout_1[4], dout_1[3], dout_1[2], dout_1[1], dout_1[0], 
\NLW_blk00000003/blk00000006_P<15>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<14>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<13>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_P<12>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<11>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<10>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_P<9>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<8>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<7>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_P<6>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<5>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<4>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_P<3>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<2>_UNCONNECTED , \NLW_blk00000003/blk00000006_P<1>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_P<0>_UNCONNECTED }),
    .OPMODE({\blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000013 , \blk00000003/sig00000037 , \blk00000003/sig00000038 , 
\blk00000003/sig00000013 , \blk00000003/sig00000039 , \blk00000003/sig0000003a }),
    .D({\blk00000003/sig0000003b , \blk00000003/sig0000003b , \blk00000003/sig0000003b , \blk00000003/sig0000003c , \blk00000003/sig0000003d , 
\blk00000003/sig0000003e , \blk00000003/sig0000003f , \blk00000003/sig00000040 , \blk00000003/sig00000041 , \blk00000003/sig00000042 , 
\blk00000003/sig00000043 , \blk00000003/sig00000044 , \blk00000003/sig00000045 , \blk00000003/sig00000046 , \blk00000003/sig00000047 , 
\blk00000003/sig00000048 , \blk00000003/sig00000049 , \blk00000003/sig0000004a }),
    .PCOUT({\NLW_blk00000003/blk00000006_PCOUT<47>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<46>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<45>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<44>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<43>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<42>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<41>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<40>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<39>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<38>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<37>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<36>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<35>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<34>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<33>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<32>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<31>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<30>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<29>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<28>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<27>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<26>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<25>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<24>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<23>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<22>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<21>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<20>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<19>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<18>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<17>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<16>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<15>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<14>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<13>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<12>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<11>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<10>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<9>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<8>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<7>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<6>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<5>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<4>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<3>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<2>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_PCOUT<1>_UNCONNECTED , \NLW_blk00000003/blk00000006_PCOUT<0>_UNCONNECTED }),
    .A({\blk00000003/sig0000004b , \blk00000003/sig0000004b , \blk00000003/sig0000004b , \blk00000003/sig0000004c , \blk00000003/sig0000004d , 
\blk00000003/sig0000004c , \blk00000003/sig0000004f , \blk00000003/sig00000050 , \blk00000003/sig00000051 , \blk00000003/sig00000052 , 
\blk00000003/sig00000053 , \blk00000003/sig00000054 , \blk00000003/sig00000055 , \blk00000003/sig00000056 , \blk00000003/sig00000057 , 
\blk00000003/sig00000058 , \blk00000003/sig00000059 , \blk00000003/sig0000005a }),
    .M({\NLW_blk00000003/blk00000006_M<35>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<34>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_M<33>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<32>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<31>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_M<30>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<29>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<28>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_M<27>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<26>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<25>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_M<24>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<23>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<22>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_M<21>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<20>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<19>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_M<18>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<17>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<16>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_M<15>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<14>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<13>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_M<12>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<11>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<10>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_M<9>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<8>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<7>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_M<6>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<5>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<4>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_M<3>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<2>_UNCONNECTED , \NLW_blk00000003/blk00000006_M<1>_UNCONNECTED , 
\NLW_blk00000003/blk00000006_M<0>_UNCONNECTED })
  );
  VCC   \blk00000003/blk00000005  (
    .P(\blk00000003/sig00000026 )
  );
  GND   \blk00000003/blk00000004  (
    .G(\blk00000003/sig00000013 )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000061/blk00000081  (
    .A0(\blk00000003/sig000000d5 ),
    .A1(\blk00000003/sig000000d4 ),
    .A2(\blk00000003/sig000000d3 ),
    .A3(\blk00000003/sig000000d2 ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig000000e5 ),
    .Q(\blk00000003/blk00000061/sig0000015b ),
    .Q15(\NLW_blk00000003/blk00000061/blk00000081_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000061/blk00000080  (
    .A0(\blk00000003/sig000000d5 ),
    .A1(\blk00000003/sig000000d4 ),
    .A2(\blk00000003/sig000000d3 ),
    .A3(\blk00000003/sig000000d2 ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig000000e6 ),
    .Q(\blk00000003/blk00000061/sig0000015a ),
    .Q15(\NLW_blk00000003/blk00000061/blk00000080_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000061/blk0000007f  (
    .A0(\blk00000003/sig000000d5 ),
    .A1(\blk00000003/sig000000d4 ),
    .A2(\blk00000003/sig000000d3 ),
    .A3(\blk00000003/sig000000d2 ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig000000e7 ),
    .Q(\blk00000003/blk00000061/sig00000159 ),
    .Q15(\NLW_blk00000003/blk00000061/blk0000007f_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000061/blk0000007e  (
    .A0(\blk00000003/sig000000d5 ),
    .A1(\blk00000003/sig000000d4 ),
    .A2(\blk00000003/sig000000d3 ),
    .A3(\blk00000003/sig000000d2 ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig000000e8 ),
    .Q(\blk00000003/blk00000061/sig00000158 ),
    .Q15(\NLW_blk00000003/blk00000061/blk0000007e_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000061/blk0000007d  (
    .A0(\blk00000003/sig000000d5 ),
    .A1(\blk00000003/sig000000d4 ),
    .A2(\blk00000003/sig000000d3 ),
    .A3(\blk00000003/sig000000d2 ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig000000e9 ),
    .Q(\blk00000003/blk00000061/sig00000157 ),
    .Q15(\NLW_blk00000003/blk00000061/blk0000007d_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000061/blk0000007c  (
    .A0(\blk00000003/sig000000d5 ),
    .A1(\blk00000003/sig000000d4 ),
    .A2(\blk00000003/sig000000d3 ),
    .A3(\blk00000003/sig000000d2 ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig000000ea ),
    .Q(\blk00000003/blk00000061/sig00000156 ),
    .Q15(\NLW_blk00000003/blk00000061/blk0000007c_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000061/blk0000007b  (
    .A0(\blk00000003/sig000000d5 ),
    .A1(\blk00000003/sig000000d4 ),
    .A2(\blk00000003/sig000000d3 ),
    .A3(\blk00000003/sig000000d2 ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig000000eb ),
    .Q(\blk00000003/blk00000061/sig00000155 ),
    .Q15(\NLW_blk00000003/blk00000061/blk0000007b_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000061/blk0000007a  (
    .A0(\blk00000003/sig000000d5 ),
    .A1(\blk00000003/sig000000d4 ),
    .A2(\blk00000003/sig000000d3 ),
    .A3(\blk00000003/sig000000d2 ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig000000ec ),
    .Q(\blk00000003/blk00000061/sig00000154 ),
    .Q15(\NLW_blk00000003/blk00000061/blk0000007a_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000061/blk00000079  (
    .A0(\blk00000003/sig000000d5 ),
    .A1(\blk00000003/sig000000d4 ),
    .A2(\blk00000003/sig000000d3 ),
    .A3(\blk00000003/sig000000d2 ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig000000ed ),
    .Q(\blk00000003/blk00000061/sig00000153 ),
    .Q15(\NLW_blk00000003/blk00000061/blk00000079_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000061/blk00000078  (
    .A0(\blk00000003/sig000000d5 ),
    .A1(\blk00000003/sig000000d4 ),
    .A2(\blk00000003/sig000000d3 ),
    .A3(\blk00000003/sig000000d2 ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig000000ee ),
    .Q(\blk00000003/blk00000061/sig00000152 ),
    .Q15(\NLW_blk00000003/blk00000061/blk00000078_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000061/blk00000077  (
    .A0(\blk00000003/sig000000d5 ),
    .A1(\blk00000003/sig000000d4 ),
    .A2(\blk00000003/sig000000d3 ),
    .A3(\blk00000003/sig000000d2 ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig000000ef ),
    .Q(\blk00000003/blk00000061/sig00000151 ),
    .Q15(\NLW_blk00000003/blk00000061/blk00000077_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000061/blk00000076  (
    .A0(\blk00000003/sig000000d5 ),
    .A1(\blk00000003/sig000000d4 ),
    .A2(\blk00000003/sig000000d3 ),
    .A3(\blk00000003/sig000000d2 ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig000000f0 ),
    .Q(\blk00000003/blk00000061/sig00000150 ),
    .Q15(\NLW_blk00000003/blk00000061/blk00000076_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000061/blk00000075  (
    .A0(\blk00000003/sig000000d5 ),
    .A1(\blk00000003/sig000000d4 ),
    .A2(\blk00000003/sig000000d3 ),
    .A3(\blk00000003/sig000000d2 ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig000000f1 ),
    .Q(\blk00000003/blk00000061/sig0000014f ),
    .Q15(\NLW_blk00000003/blk00000061/blk00000075_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000061/blk00000074  (
    .A0(\blk00000003/sig000000d5 ),
    .A1(\blk00000003/sig000000d4 ),
    .A2(\blk00000003/sig000000d3 ),
    .A3(\blk00000003/sig000000d2 ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig000000f2 ),
    .Q(\blk00000003/blk00000061/sig0000014e ),
    .Q15(\NLW_blk00000003/blk00000061/blk00000074_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000061/blk00000073  (
    .A0(\blk00000003/sig000000d5 ),
    .A1(\blk00000003/sig000000d4 ),
    .A2(\blk00000003/sig000000d3 ),
    .A3(\blk00000003/sig000000d2 ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig000000f3 ),
    .Q(\blk00000003/blk00000061/sig0000014d ),
    .Q15(\NLW_blk00000003/blk00000061/blk00000073_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000061/blk00000072  (
    .A0(\blk00000003/sig000000d5 ),
    .A1(\blk00000003/sig000000d4 ),
    .A2(\blk00000003/sig000000d3 ),
    .A3(\blk00000003/sig000000d2 ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig000000f4 ),
    .Q(\blk00000003/blk00000061/sig0000014c ),
    .Q15(\NLW_blk00000003/blk00000061/blk00000072_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000061/blk00000071  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000061/sig0000015b ),
    .Q(\blk00000003/sig00000027 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000061/blk00000070  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000061/sig0000015a ),
    .Q(\blk00000003/sig00000028 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000061/blk0000006f  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000061/sig00000159 ),
    .Q(\blk00000003/sig00000029 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000061/blk0000006e  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000061/sig00000158 ),
    .Q(\blk00000003/sig0000002a )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000061/blk0000006d  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000061/sig00000157 ),
    .Q(\blk00000003/sig0000002b )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000061/blk0000006c  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000061/sig00000156 ),
    .Q(\blk00000003/sig0000002c )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000061/blk0000006b  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000061/sig00000155 ),
    .Q(\blk00000003/sig0000002d )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000061/blk0000006a  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000061/sig00000154 ),
    .Q(\blk00000003/sig0000002e )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000061/blk00000069  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000061/sig00000153 ),
    .Q(\blk00000003/sig0000002f )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000061/blk00000068  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000061/sig00000152 ),
    .Q(\blk00000003/sig00000030 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000061/blk00000067  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000061/sig00000151 ),
    .Q(\blk00000003/sig00000031 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000061/blk00000066  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000061/sig00000150 ),
    .Q(\blk00000003/sig00000032 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000061/blk00000065  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000061/sig0000014f ),
    .Q(\blk00000003/sig00000033 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000061/blk00000064  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000061/sig0000014e ),
    .Q(\blk00000003/sig00000034 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000061/blk00000063  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000061/sig0000014d ),
    .Q(\blk00000003/sig00000035 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000061/blk00000062  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000061/sig0000014c ),
    .Q(\blk00000003/sig00000036 )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000082/blk000000a2  (
    .A0(\blk00000003/sig000000c6 ),
    .A1(\blk00000003/sig000000c5 ),
    .A2(\blk00000003/sig000000c4 ),
    .A3(\blk00000003/sig000000c3 ),
    .CE(\blk00000003/sig00000087 ),
    .CLK(clk),
    .D(\blk00000003/sig000000f5 ),
    .Q(\blk00000003/blk00000082/sig00000192 ),
    .Q15(\NLW_blk00000003/blk00000082/blk000000a2_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000082/blk000000a1  (
    .A0(\blk00000003/sig000000c6 ),
    .A1(\blk00000003/sig000000c5 ),
    .A2(\blk00000003/sig000000c4 ),
    .A3(\blk00000003/sig000000c3 ),
    .CE(\blk00000003/sig00000087 ),
    .CLK(clk),
    .D(\blk00000003/sig000000f6 ),
    .Q(\blk00000003/blk00000082/sig00000191 ),
    .Q15(\NLW_blk00000003/blk00000082/blk000000a1_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000082/blk000000a0  (
    .A0(\blk00000003/sig000000c6 ),
    .A1(\blk00000003/sig000000c5 ),
    .A2(\blk00000003/sig000000c4 ),
    .A3(\blk00000003/sig000000c3 ),
    .CE(\blk00000003/sig00000087 ),
    .CLK(clk),
    .D(\blk00000003/sig000000f7 ),
    .Q(\blk00000003/blk00000082/sig00000190 ),
    .Q15(\NLW_blk00000003/blk00000082/blk000000a0_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000082/blk0000009f  (
    .A0(\blk00000003/sig000000c6 ),
    .A1(\blk00000003/sig000000c5 ),
    .A2(\blk00000003/sig000000c4 ),
    .A3(\blk00000003/sig000000c3 ),
    .CE(\blk00000003/sig00000087 ),
    .CLK(clk),
    .D(\blk00000003/sig000000f8 ),
    .Q(\blk00000003/blk00000082/sig0000018f ),
    .Q15(\NLW_blk00000003/blk00000082/blk0000009f_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000082/blk0000009e  (
    .A0(\blk00000003/sig000000c6 ),
    .A1(\blk00000003/sig000000c5 ),
    .A2(\blk00000003/sig000000c4 ),
    .A3(\blk00000003/sig000000c3 ),
    .CE(\blk00000003/sig00000087 ),
    .CLK(clk),
    .D(\blk00000003/sig000000f9 ),
    .Q(\blk00000003/blk00000082/sig0000018e ),
    .Q15(\NLW_blk00000003/blk00000082/blk0000009e_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000082/blk0000009d  (
    .A0(\blk00000003/sig000000c6 ),
    .A1(\blk00000003/sig000000c5 ),
    .A2(\blk00000003/sig000000c4 ),
    .A3(\blk00000003/sig000000c3 ),
    .CE(\blk00000003/sig00000087 ),
    .CLK(clk),
    .D(\blk00000003/sig000000fa ),
    .Q(\blk00000003/blk00000082/sig0000018d ),
    .Q15(\NLW_blk00000003/blk00000082/blk0000009d_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000082/blk0000009c  (
    .A0(\blk00000003/sig000000c6 ),
    .A1(\blk00000003/sig000000c5 ),
    .A2(\blk00000003/sig000000c4 ),
    .A3(\blk00000003/sig000000c3 ),
    .CE(\blk00000003/sig00000087 ),
    .CLK(clk),
    .D(\blk00000003/sig000000fb ),
    .Q(\blk00000003/blk00000082/sig0000018c ),
    .Q15(\NLW_blk00000003/blk00000082/blk0000009c_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000082/blk0000009b  (
    .A0(\blk00000003/sig000000c6 ),
    .A1(\blk00000003/sig000000c5 ),
    .A2(\blk00000003/sig000000c4 ),
    .A3(\blk00000003/sig000000c3 ),
    .CE(\blk00000003/sig00000087 ),
    .CLK(clk),
    .D(\blk00000003/sig000000fc ),
    .Q(\blk00000003/blk00000082/sig0000018b ),
    .Q15(\NLW_blk00000003/blk00000082/blk0000009b_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000082/blk0000009a  (
    .A0(\blk00000003/sig000000c6 ),
    .A1(\blk00000003/sig000000c5 ),
    .A2(\blk00000003/sig000000c4 ),
    .A3(\blk00000003/sig000000c3 ),
    .CE(\blk00000003/sig00000087 ),
    .CLK(clk),
    .D(\blk00000003/sig000000fd ),
    .Q(\blk00000003/blk00000082/sig0000018a ),
    .Q15(\NLW_blk00000003/blk00000082/blk0000009a_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000082/blk00000099  (
    .A0(\blk00000003/sig000000c6 ),
    .A1(\blk00000003/sig000000c5 ),
    .A2(\blk00000003/sig000000c4 ),
    .A3(\blk00000003/sig000000c3 ),
    .CE(\blk00000003/sig00000087 ),
    .CLK(clk),
    .D(\blk00000003/sig000000fe ),
    .Q(\blk00000003/blk00000082/sig00000189 ),
    .Q15(\NLW_blk00000003/blk00000082/blk00000099_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000082/blk00000098  (
    .A0(\blk00000003/sig000000c6 ),
    .A1(\blk00000003/sig000000c5 ),
    .A2(\blk00000003/sig000000c4 ),
    .A3(\blk00000003/sig000000c3 ),
    .CE(\blk00000003/sig00000087 ),
    .CLK(clk),
    .D(\blk00000003/sig000000ff ),
    .Q(\blk00000003/blk00000082/sig00000188 ),
    .Q15(\NLW_blk00000003/blk00000082/blk00000098_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000082/blk00000097  (
    .A0(\blk00000003/sig000000c6 ),
    .A1(\blk00000003/sig000000c5 ),
    .A2(\blk00000003/sig000000c4 ),
    .A3(\blk00000003/sig000000c3 ),
    .CE(\blk00000003/sig00000087 ),
    .CLK(clk),
    .D(\blk00000003/sig00000100 ),
    .Q(\blk00000003/blk00000082/sig00000187 ),
    .Q15(\NLW_blk00000003/blk00000082/blk00000097_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000082/blk00000096  (
    .A0(\blk00000003/sig000000c6 ),
    .A1(\blk00000003/sig000000c5 ),
    .A2(\blk00000003/sig000000c4 ),
    .A3(\blk00000003/sig000000c3 ),
    .CE(\blk00000003/sig00000087 ),
    .CLK(clk),
    .D(\blk00000003/sig00000101 ),
    .Q(\blk00000003/blk00000082/sig00000186 ),
    .Q15(\NLW_blk00000003/blk00000082/blk00000096_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000082/blk00000095  (
    .A0(\blk00000003/sig000000c6 ),
    .A1(\blk00000003/sig000000c5 ),
    .A2(\blk00000003/sig000000c4 ),
    .A3(\blk00000003/sig000000c3 ),
    .CE(\blk00000003/sig00000087 ),
    .CLK(clk),
    .D(\blk00000003/sig00000102 ),
    .Q(\blk00000003/blk00000082/sig00000185 ),
    .Q15(\NLW_blk00000003/blk00000082/blk00000095_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000082/blk00000094  (
    .A0(\blk00000003/sig000000c6 ),
    .A1(\blk00000003/sig000000c5 ),
    .A2(\blk00000003/sig000000c4 ),
    .A3(\blk00000003/sig000000c3 ),
    .CE(\blk00000003/sig00000087 ),
    .CLK(clk),
    .D(\blk00000003/sig00000103 ),
    .Q(\blk00000003/blk00000082/sig00000184 ),
    .Q15(\NLW_blk00000003/blk00000082/blk00000094_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk00000082/blk00000093  (
    .A0(\blk00000003/sig000000c6 ),
    .A1(\blk00000003/sig000000c5 ),
    .A2(\blk00000003/sig000000c4 ),
    .A3(\blk00000003/sig000000c3 ),
    .CE(\blk00000003/sig00000087 ),
    .CLK(clk),
    .D(\blk00000003/sig00000104 ),
    .Q(\blk00000003/blk00000082/sig00000183 ),
    .Q15(\NLW_blk00000003/blk00000082/blk00000093_Q15_UNCONNECTED )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000082/blk00000092  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000082/sig00000192 ),
    .Q(\blk00000003/sig0000003b )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000082/blk00000091  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000082/sig00000191 ),
    .Q(\blk00000003/sig0000003c )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000082/blk00000090  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000082/sig00000190 ),
    .Q(\blk00000003/sig0000003d )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000082/blk0000008f  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000082/sig0000018f ),
    .Q(\blk00000003/sig0000003e )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000082/blk0000008e  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000082/sig0000018e ),
    .Q(\blk00000003/sig0000003f )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000082/blk0000008d  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000082/sig0000018d ),
    .Q(\blk00000003/sig00000040 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000082/blk0000008c  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000082/sig0000018c ),
    .Q(\blk00000003/sig00000041 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000082/blk0000008b  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000082/sig0000018b ),
    .Q(\blk00000003/sig00000042 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000082/blk0000008a  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000082/sig0000018a ),
    .Q(\blk00000003/sig00000043 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000082/blk00000089  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000082/sig00000189 ),
    .Q(\blk00000003/sig00000044 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000082/blk00000088  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000082/sig00000188 ),
    .Q(\blk00000003/sig00000045 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000082/blk00000087  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000082/sig00000187 ),
    .Q(\blk00000003/sig00000046 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000082/blk00000086  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000082/sig00000186 ),
    .Q(\blk00000003/sig00000047 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000082/blk00000085  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000082/sig00000185 ),
    .Q(\blk00000003/sig00000048 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000082/blk00000084  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000082/sig00000184 ),
    .Q(\blk00000003/sig00000049 )
  );
  FDE #(
    .INIT ( 1'b0 ))
  \blk00000003/blk00000082/blk00000083  (
    .C(clk),
    .CE(\blk00000003/sig00000026 ),
    .D(\blk00000003/blk00000082/sig00000183 ),
    .Q(\blk00000003/sig0000004a )
  );
  LUT4 #(
    .INIT ( 16'h0614 ))
  \blk00000003/blk000000a3/blk000000c1  (
    .I0(\blk00000003/sig000000e4 ),
    .I1(\blk00000003/sig000000e0 ),
    .I2(\blk00000003/sig000000df ),
    .I3(\blk00000003/sig000000e1 ),
    .O(\blk00000003/blk000000a3/sig000001af )
  );
  LUT4 #(
    .INIT ( 16'h01D0 ))
  \blk00000003/blk000000a3/blk000000c0  (
    .I0(\blk00000003/sig000000e4 ),
    .I1(\blk00000003/sig000000e1 ),
    .I2(\blk00000003/sig000000e0 ),
    .I3(\blk00000003/sig000000df ),
    .O(\blk00000003/blk000000a3/sig000001b4 )
  );
  LUT4 #(
    .INIT ( 16'h4556 ))
  \blk00000003/blk000000a3/blk000000bf  (
    .I0(\blk00000003/sig000000df ),
    .I1(\blk00000003/sig000000e1 ),
    .I2(\blk00000003/sig000000e0 ),
    .I3(\blk00000003/sig000000e4 ),
    .O(\blk00000003/blk000000a3/sig000001ae )
  );
  LUT4 #(
    .INIT ( 16'h4562 ))
  \blk00000003/blk000000a3/blk000000be  (
    .I0(\blk00000003/sig000000df ),
    .I1(\blk00000003/sig000000e1 ),
    .I2(\blk00000003/sig000000e4 ),
    .I3(\blk00000003/sig000000e0 ),
    .O(\blk00000003/blk000000a3/sig000001b2 )
  );
  LUT4 #(
    .INIT ( 16'h2426 ))
  \blk00000003/blk000000a3/blk000000bd  (
    .I0(\blk00000003/sig000000e0 ),
    .I1(\blk00000003/sig000000df ),
    .I2(\blk00000003/sig000000e1 ),
    .I3(\blk00000003/sig000000e4 ),
    .O(\blk00000003/blk000000a3/sig000001b3 )
  );
  LUT4 #(
    .INIT ( 16'h111A ))
  \blk00000003/blk000000a3/blk000000bc  (
    .I0(\blk00000003/sig000000df ),
    .I1(\blk00000003/sig000000e4 ),
    .I2(\blk00000003/sig000000e0 ),
    .I3(\blk00000003/sig000000e1 ),
    .O(\blk00000003/blk000000a3/sig000001ad )
  );
  LUT4 #(
    .INIT ( 16'h3128 ))
  \blk00000003/blk000000a3/blk000000bb  (
    .I0(\blk00000003/sig000000e4 ),
    .I1(\blk00000003/sig000000df ),
    .I2(\blk00000003/sig000000e1 ),
    .I3(\blk00000003/sig000000e0 ),
    .O(\blk00000003/blk000000a3/sig000001b1 )
  );
  LUT4 #(
    .INIT ( 16'h0929 ))
  \blk00000003/blk000000a3/blk000000ba  (
    .I0(\blk00000003/sig000000e4 ),
    .I1(\blk00000003/sig000000e1 ),
    .I2(\blk00000003/sig000000df ),
    .I3(\blk00000003/sig000000e0 ),
    .O(\blk00000003/blk000000a3/sig000001ab )
  );
  LUT4 #(
    .INIT ( 16'h0145 ))
  \blk00000003/blk000000a3/blk000000b9  (
    .I0(\blk00000003/sig000000e4 ),
    .I1(\blk00000003/sig000000e0 ),
    .I2(\blk00000003/sig000000e1 ),
    .I3(\blk00000003/sig000000df ),
    .O(\blk00000003/blk000000a3/sig000001a7 )
  );
  LUT4 #(
    .INIT ( 16'h0118 ))
  \blk00000003/blk000000a3/blk000000b8  (
    .I0(\blk00000003/sig000000e4 ),
    .I1(\blk00000003/sig000000df ),
    .I2(\blk00000003/sig000000e0 ),
    .I3(\blk00000003/sig000000e1 ),
    .O(\blk00000003/blk000000a3/sig000001a8 )
  );
  LUT4 #(
    .INIT ( 16'h2614 ))
  \blk00000003/blk000000a3/blk000000b7  (
    .I0(\blk00000003/sig000000e1 ),
    .I1(\blk00000003/sig000000df ),
    .I2(\blk00000003/sig000000e0 ),
    .I3(\blk00000003/sig000000e4 ),
    .O(\blk00000003/blk000000a3/sig000001ac )
  );
  LUT3 #(
    .INIT ( 8'h42 ))
  \blk00000003/blk000000a3/blk000000b6  (
    .I0(\blk00000003/sig000000df ),
    .I1(\blk00000003/sig000000e0 ),
    .I2(\blk00000003/sig000000e1 ),
    .O(\blk00000003/blk000000a3/sig000001b5 )
  );
  LUT4 #(
    .INIT ( 16'h511F ))
  \blk00000003/blk000000a3/blk000000b5  (
    .I0(\blk00000003/sig000000df ),
    .I1(\blk00000003/sig000000e4 ),
    .I2(\blk00000003/sig000000e1 ),
    .I3(\blk00000003/sig000000e0 ),
    .O(\blk00000003/blk000000a3/sig000001aa )
  );
  LUT4 #(
    .INIT ( 16'h411B ))
  \blk00000003/blk000000a3/blk000000b4  (
    .I0(\blk00000003/sig000000df ),
    .I1(\blk00000003/sig000000e4 ),
    .I2(\blk00000003/sig000000e0 ),
    .I3(\blk00000003/sig000000e1 ),
    .O(\blk00000003/blk000000a3/sig000001a9 )
  );
  LUT4 #(
    .INIT ( 16'h4504 ))
  \blk00000003/blk000000a3/blk000000b3  (
    .I0(\blk00000003/sig000000df ),
    .I1(\blk00000003/sig000000e0 ),
    .I2(\blk00000003/sig000000e4 ),
    .I3(\blk00000003/sig000000e1 ),
    .O(\blk00000003/blk000000a3/sig000001b0 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000a3/blk000000b2  (
    .C(clk),
    .D(\blk00000003/blk000000a3/sig000001b5 ),
    .Q(\blk00000003/sig0000004b )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000a3/blk000000b1  (
    .C(clk),
    .D(\blk00000003/blk000000a3/sig000001b4 ),
    .Q(\blk00000003/sig0000004d )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000a3/blk000000b0  (
    .C(clk),
    .D(\blk00000003/blk000000a3/sig000001b3 ),
    .Q(\blk00000003/sig0000004c )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000a3/blk000000af  (
    .C(clk),
    .D(\blk00000003/blk000000a3/sig000001b2 ),
    .Q(\blk00000003/sig0000004f )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000a3/blk000000ae  (
    .C(clk),
    .D(\blk00000003/blk000000a3/sig000001b1 ),
    .Q(\blk00000003/sig00000050 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000a3/blk000000ad  (
    .C(clk),
    .D(\blk00000003/blk000000a3/sig000001b0 ),
    .Q(\blk00000003/sig00000051 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000a3/blk000000ac  (
    .C(clk),
    .D(\blk00000003/blk000000a3/sig000001af ),
    .Q(\blk00000003/sig00000052 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000a3/blk000000ab  (
    .C(clk),
    .D(\blk00000003/blk000000a3/sig000001ae ),
    .Q(\blk00000003/sig00000053 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000a3/blk000000aa  (
    .C(clk),
    .D(\blk00000003/blk000000a3/sig000001ad ),
    .Q(\blk00000003/sig00000054 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000a3/blk000000a9  (
    .C(clk),
    .D(\blk00000003/blk000000a3/sig000001ac ),
    .Q(\blk00000003/sig00000055 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000a3/blk000000a8  (
    .C(clk),
    .D(\blk00000003/blk000000a3/sig000001ab ),
    .Q(\blk00000003/sig00000056 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000a3/blk000000a7  (
    .C(clk),
    .D(\blk00000003/blk000000a3/sig000001aa ),
    .Q(\blk00000003/sig00000057 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000a3/blk000000a6  (
    .C(clk),
    .D(\blk00000003/blk000000a3/sig000001a9 ),
    .Q(\blk00000003/sig00000058 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000a3/blk000000a5  (
    .C(clk),
    .D(\blk00000003/blk000000a3/sig000001a8 ),
    .Q(\blk00000003/sig00000059 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000a3/blk000000a4  (
    .C(clk),
    .D(\blk00000003/blk000000a3/sig000001a7 ),
    .Q(\blk00000003/sig0000005a )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk000000c2/blk000000e3  (
    .A0(\blk00000003/sig00000106 ),
    .A1(\blk00000003/sig00000105 ),
    .A2(\blk00000003/blk000000c2/sig000001da ),
    .A3(\blk00000003/blk000000c2/sig000001da ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig0000003b ),
    .Q(\blk00000003/blk000000c2/sig000001ea ),
    .Q15(\NLW_blk00000003/blk000000c2/blk000000e3_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk000000c2/blk000000e2  (
    .A0(\blk00000003/sig00000106 ),
    .A1(\blk00000003/sig00000105 ),
    .A2(\blk00000003/blk000000c2/sig000001da ),
    .A3(\blk00000003/blk000000c2/sig000001da ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig0000003c ),
    .Q(\blk00000003/blk000000c2/sig000001e9 ),
    .Q15(\NLW_blk00000003/blk000000c2/blk000000e2_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk000000c2/blk000000e1  (
    .A0(\blk00000003/sig00000106 ),
    .A1(\blk00000003/sig00000105 ),
    .A2(\blk00000003/blk000000c2/sig000001da ),
    .A3(\blk00000003/blk000000c2/sig000001da ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig0000003d ),
    .Q(\blk00000003/blk000000c2/sig000001e8 ),
    .Q15(\NLW_blk00000003/blk000000c2/blk000000e1_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk000000c2/blk000000e0  (
    .A0(\blk00000003/sig00000106 ),
    .A1(\blk00000003/sig00000105 ),
    .A2(\blk00000003/blk000000c2/sig000001da ),
    .A3(\blk00000003/blk000000c2/sig000001da ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig0000003e ),
    .Q(\blk00000003/blk000000c2/sig000001e7 ),
    .Q15(\NLW_blk00000003/blk000000c2/blk000000e0_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk000000c2/blk000000df  (
    .A0(\blk00000003/sig00000106 ),
    .A1(\blk00000003/sig00000105 ),
    .A2(\blk00000003/blk000000c2/sig000001da ),
    .A3(\blk00000003/blk000000c2/sig000001da ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig0000003f ),
    .Q(\blk00000003/blk000000c2/sig000001e6 ),
    .Q15(\NLW_blk00000003/blk000000c2/blk000000df_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk000000c2/blk000000de  (
    .A0(\blk00000003/sig00000106 ),
    .A1(\blk00000003/sig00000105 ),
    .A2(\blk00000003/blk000000c2/sig000001da ),
    .A3(\blk00000003/blk000000c2/sig000001da ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig00000040 ),
    .Q(\blk00000003/blk000000c2/sig000001e5 ),
    .Q15(\NLW_blk00000003/blk000000c2/blk000000de_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk000000c2/blk000000dd  (
    .A0(\blk00000003/sig00000106 ),
    .A1(\blk00000003/sig00000105 ),
    .A2(\blk00000003/blk000000c2/sig000001da ),
    .A3(\blk00000003/blk000000c2/sig000001da ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig00000041 ),
    .Q(\blk00000003/blk000000c2/sig000001e4 ),
    .Q15(\NLW_blk00000003/blk000000c2/blk000000dd_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk000000c2/blk000000dc  (
    .A0(\blk00000003/sig00000106 ),
    .A1(\blk00000003/sig00000105 ),
    .A2(\blk00000003/blk000000c2/sig000001da ),
    .A3(\blk00000003/blk000000c2/sig000001da ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig00000042 ),
    .Q(\blk00000003/blk000000c2/sig000001e3 ),
    .Q15(\NLW_blk00000003/blk000000c2/blk000000dc_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk000000c2/blk000000db  (
    .A0(\blk00000003/sig00000106 ),
    .A1(\blk00000003/sig00000105 ),
    .A2(\blk00000003/blk000000c2/sig000001da ),
    .A3(\blk00000003/blk000000c2/sig000001da ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig00000043 ),
    .Q(\blk00000003/blk000000c2/sig000001e2 ),
    .Q15(\NLW_blk00000003/blk000000c2/blk000000db_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk000000c2/blk000000da  (
    .A0(\blk00000003/sig00000106 ),
    .A1(\blk00000003/sig00000105 ),
    .A2(\blk00000003/blk000000c2/sig000001da ),
    .A3(\blk00000003/blk000000c2/sig000001da ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig00000044 ),
    .Q(\blk00000003/blk000000c2/sig000001e1 ),
    .Q15(\NLW_blk00000003/blk000000c2/blk000000da_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk000000c2/blk000000d9  (
    .A0(\blk00000003/sig00000106 ),
    .A1(\blk00000003/sig00000105 ),
    .A2(\blk00000003/blk000000c2/sig000001da ),
    .A3(\blk00000003/blk000000c2/sig000001da ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig00000045 ),
    .Q(\blk00000003/blk000000c2/sig000001e0 ),
    .Q15(\NLW_blk00000003/blk000000c2/blk000000d9_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk000000c2/blk000000d8  (
    .A0(\blk00000003/sig00000106 ),
    .A1(\blk00000003/sig00000105 ),
    .A2(\blk00000003/blk000000c2/sig000001da ),
    .A3(\blk00000003/blk000000c2/sig000001da ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig00000046 ),
    .Q(\blk00000003/blk000000c2/sig000001df ),
    .Q15(\NLW_blk00000003/blk000000c2/blk000000d8_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk000000c2/blk000000d7  (
    .A0(\blk00000003/sig00000106 ),
    .A1(\blk00000003/sig00000105 ),
    .A2(\blk00000003/blk000000c2/sig000001da ),
    .A3(\blk00000003/blk000000c2/sig000001da ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig00000047 ),
    .Q(\blk00000003/blk000000c2/sig000001de ),
    .Q15(\NLW_blk00000003/blk000000c2/blk000000d7_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk000000c2/blk000000d6  (
    .A0(\blk00000003/sig00000106 ),
    .A1(\blk00000003/sig00000105 ),
    .A2(\blk00000003/blk000000c2/sig000001da ),
    .A3(\blk00000003/blk000000c2/sig000001da ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig00000048 ),
    .Q(\blk00000003/blk000000c2/sig000001dd ),
    .Q15(\NLW_blk00000003/blk000000c2/blk000000d6_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk000000c2/blk000000d5  (
    .A0(\blk00000003/sig00000106 ),
    .A1(\blk00000003/sig00000105 ),
    .A2(\blk00000003/blk000000c2/sig000001da ),
    .A3(\blk00000003/blk000000c2/sig000001da ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig00000049 ),
    .Q(\blk00000003/blk000000c2/sig000001dc ),
    .Q15(\NLW_blk00000003/blk000000c2/blk000000d5_Q15_UNCONNECTED )
  );
  SRLC16E #(
    .INIT ( 16'h0000 ))
  \blk00000003/blk000000c2/blk000000d4  (
    .A0(\blk00000003/sig00000106 ),
    .A1(\blk00000003/sig00000105 ),
    .A2(\blk00000003/blk000000c2/sig000001da ),
    .A3(\blk00000003/blk000000c2/sig000001da ),
    .CE(\blk00000003/sig0000007e ),
    .CLK(clk),
    .D(\blk00000003/sig0000004a ),
    .Q(\blk00000003/blk000000c2/sig000001db ),
    .Q15(\NLW_blk00000003/blk000000c2/blk000000d4_Q15_UNCONNECTED )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000c2/blk000000d3  (
    .C(clk),
    .D(\blk00000003/blk000000c2/sig000001ea ),
    .Q(\blk00000003/sig000000e5 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000c2/blk000000d2  (
    .C(clk),
    .D(\blk00000003/blk000000c2/sig000001e9 ),
    .Q(\blk00000003/sig000000e6 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000c2/blk000000d1  (
    .C(clk),
    .D(\blk00000003/blk000000c2/sig000001e8 ),
    .Q(\blk00000003/sig000000e7 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000c2/blk000000d0  (
    .C(clk),
    .D(\blk00000003/blk000000c2/sig000001e7 ),
    .Q(\blk00000003/sig000000e8 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000c2/blk000000cf  (
    .C(clk),
    .D(\blk00000003/blk000000c2/sig000001e6 ),
    .Q(\blk00000003/sig000000e9 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000c2/blk000000ce  (
    .C(clk),
    .D(\blk00000003/blk000000c2/sig000001e5 ),
    .Q(\blk00000003/sig000000ea )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000c2/blk000000cd  (
    .C(clk),
    .D(\blk00000003/blk000000c2/sig000001e4 ),
    .Q(\blk00000003/sig000000eb )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000c2/blk000000cc  (
    .C(clk),
    .D(\blk00000003/blk000000c2/sig000001e3 ),
    .Q(\blk00000003/sig000000ec )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000c2/blk000000cb  (
    .C(clk),
    .D(\blk00000003/blk000000c2/sig000001e2 ),
    .Q(\blk00000003/sig000000ed )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000c2/blk000000ca  (
    .C(clk),
    .D(\blk00000003/blk000000c2/sig000001e1 ),
    .Q(\blk00000003/sig000000ee )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000c2/blk000000c9  (
    .C(clk),
    .D(\blk00000003/blk000000c2/sig000001e0 ),
    .Q(\blk00000003/sig000000ef )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000c2/blk000000c8  (
    .C(clk),
    .D(\blk00000003/blk000000c2/sig000001df ),
    .Q(\blk00000003/sig000000f0 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000c2/blk000000c7  (
    .C(clk),
    .D(\blk00000003/blk000000c2/sig000001de ),
    .Q(\blk00000003/sig000000f1 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000c2/blk000000c6  (
    .C(clk),
    .D(\blk00000003/blk000000c2/sig000001dd ),
    .Q(\blk00000003/sig000000f2 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000c2/blk000000c5  (
    .C(clk),
    .D(\blk00000003/blk000000c2/sig000001dc ),
    .Q(\blk00000003/sig000000f3 )
  );
  FD #(
    .INIT ( 1'b0 ))
  \blk00000003/blk000000c2/blk000000c4  (
    .C(clk),
    .D(\blk00000003/blk000000c2/sig000001db ),
    .Q(\blk00000003/sig000000f4 )
  );
  GND   \blk00000003/blk000000c2/blk000000c3  (
    .G(\blk00000003/blk000000c2/sig000001da )
  );

// synthesis translate_on

endmodule

// synthesis translate_off

`ifndef GLBL
`define GLBL

`timescale  1 ps / 1 ps

module glbl ();

    parameter ROC_WIDTH = 100000;
    parameter TOC_WIDTH = 0;

//--------   STARTUP Globals --------------
    wire GSR;
    wire GTS;
    wire GWE;
    wire PRLD;
    tri1 p_up_tmp;
    tri (weak1, strong0) PLL_LOCKG = p_up_tmp;

    wire PROGB_GLBL;
    wire CCLKO_GLBL;

    reg GSR_int;
    reg GTS_int;
    reg PRLD_int;

//--------   JTAG Globals --------------
    wire JTAG_TDO_GLBL;
    wire JTAG_TCK_GLBL;
    wire JTAG_TDI_GLBL;
    wire JTAG_TMS_GLBL;
    wire JTAG_TRST_GLBL;

    reg JTAG_CAPTURE_GLBL;
    reg JTAG_RESET_GLBL;
    reg JTAG_SHIFT_GLBL;
    reg JTAG_UPDATE_GLBL;
    reg JTAG_RUNTEST_GLBL;

    reg JTAG_SEL1_GLBL = 0;
    reg JTAG_SEL2_GLBL = 0 ;
    reg JTAG_SEL3_GLBL = 0;
    reg JTAG_SEL4_GLBL = 0;

    reg JTAG_USER_TDO1_GLBL = 1'bz;
    reg JTAG_USER_TDO2_GLBL = 1'bz;
    reg JTAG_USER_TDO3_GLBL = 1'bz;
    reg JTAG_USER_TDO4_GLBL = 1'bz;

    assign (weak1, weak0) GSR = GSR_int;
    assign (weak1, weak0) GTS = GTS_int;
    assign (weak1, weak0) PRLD = PRLD_int;

    initial begin
	GSR_int = 1'b1;
	PRLD_int = 1'b1;
	#(ROC_WIDTH)
	GSR_int = 1'b0;
	PRLD_int = 1'b0;
    end

    initial begin
	GTS_int = 1'b1;
	#(TOC_WIDTH)
	GTS_int = 1'b0;
    end

endmodule

`endif

// synthesis translate_on
