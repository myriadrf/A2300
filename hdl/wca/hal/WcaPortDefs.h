// Name:  WcaPortDefs.v
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

//*******************************************************************
//*******************************************************************
//WCA PORT Definitions	 
//*******************************************************************
//*******************************************************************
`define PIFCMD_IDLE 		2'h0
`define PIFCMD_READ 		2'h1
`define PIFCMD_WRITE 	2'h2

//Burst IF State Machine States
`define PIFSTAT_IDLE 			3'h0
`define PIFSTAT_READ_BEGIN  	3'h1 
`define PIFSTAT_READ 		 	3'h2
`define PIFSTAT_WRITE_BEGIN 	3'h3
`define PIFSTAT_WRITE 		 	3'h4
`define PIFSTAT_WAITEND			3'h5

`define READ_WAIT_END_CYCLES  2'h3
`define WRITE_WAIT_END_CYCLES	2'h3
`define READ_BEGIN_CYCLES		2'h1