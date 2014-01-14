
// Name:  WcaDCOffset.v
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

module WcaDcOffset (
    input clock, 
    input reset, 
    input strobe,	 
	 input  iqSel,

    input  signed [11:0] sig_in,    //signal input.
	 output signed [11:0] dcoffset,  //DC Offset measurement / scaled to 16 bits for higher resolution.
	 output signed [11:0] sigout 	 //Adjusted output signal with DC bias removed.
  );

/*	//Implement a scaled integrator. 
	reg signed [31:0] integrator[0:1];

	assign dcoffset = integrator[iqSel][31:20];
   assign sigout   = sig_in - dcoffset;
	wire  [31:0] update = integrator[iqSel][31:0] + {{(20){sigout[11]}},sigout}; 
		
   always @(posedge clock)
	 begin
     if(reset)
	  begin 
       integrator[0] <= #1 32'd0;
		 integrator[1] <= #1 32'd0;
	  end
     else if(strobe) 
		 if( iqSel) integrator[1][31:0] <= #1 update; 
		 else       integrator[0][31:0] <= #1 update; 
	 end */
	 
/*	//Implement a scaled integrator. 
	reg  signed [25:0] integrator;

	assign dcoffset = integrator[25:14];
   assign sigout   = (iqSel) ? (sig_in - dcoffset) : dcoffset;
	wire  signed [25:0] update = integrator + {{(14){sigout[11]}},sigout}; 
		
   always @(posedge clock)
	 begin
     if(reset)
	  begin 
		 integrator <= #1 16'd0;
	  end
     else if(iqSel) 
		 integrator<= #1 update; 
	 end 
*/
	//Implement a scaled integrator. 
	reg  signed [25:0] integrator[1:0];

	assign dcoffset = integrator[iqSel][25:14];
   assign sigout   = sig_in - dcoffset;
	wire  signed [25:0] update = integrator[iqSel] + {{(14){sigout[11]}},sigout}; 
		
   always @(negedge clock)
	 begin
     if(reset)
	  begin 
		 integrator[0] <= #1 26'd0;
		 integrator[1] <= #1 26'd0;
	  end
     else if(strobe) 
		 integrator[iqSel] <= #1 update; 
	 end 

/*	reg  signed [25:0] integrator;

	assign dcoffset = integrator[25:14];
   assign sigout   = (iqSel) ? (sig_in - dcoffset) : sig_in;
	wire  signed [25:0] update = integrator + {{(14){sigout[11]}},sigout}; 
		
   always @(negedge clock)
	 begin
     if(reset)
	  begin 
		 integrator <= #1 16'd0;
	  end
     else if(iqSel) 
		 integrator<= #1 update; 
	 end */

endmodule // WcaDcOffset


