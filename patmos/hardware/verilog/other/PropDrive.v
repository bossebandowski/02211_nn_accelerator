//
//   Copyright 2018 Aalborg University, Denamrk. 
//   All rights reserved.
//   Author: Shibarchi Majumder (sm@es.aau.dk)
//   
//   
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions are met:
//
//     1. Redistributions of source code must retain the above copyright notice,
//        author, this list of conditions and the following disclaimer.
//
//     2. Redistributions in binary form must reproduce the above copyright
//        notice, author, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ``AS IS'' AND ANY EXPRESS
//   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
//   NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
//   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
//   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//   The views and conclusions contained in the software and documentation are
//   those of the authors and should not be interpreted as representing official
//   policies, either expressed or implied, of the copyright holder.
//
//


`timescale 1ns / 1ps




module PropDrive (
                       // inputs:
                        address,
                        chipselect,
                        clk,
                        reset_n,
                        write_n,
                        writedata,

                       // outputs:
                        out_port,
                        readdata
                     )
;

  output           out_port;
  output  [ 31: 0] readdata;
  input   [  1: 0] address;
  input            chipselect;
  input            clk;
  input            reset_n;
  input            write_n;
  input   [ 31: 0] writedata;


wire             clk_en;
reg              data_out;
wire             out_port;
wire             read_mux_out;
wire    [ 31: 0] readdata;

reg [17:0] sample_clk;
reg [31:0] data;

parameter FREQ = 250;

  assign clk_en = 1;
  //s1, which is an e_avalon_slave
  assign read_mux_out = {1 {(address == 0)}} & data_out;
  always @(posedge clk or negedge reset_n)
    begin
	 
      if (reset_n == 0)
		begin
          data <= 0;
			 
		end
      else if (chipselect && ~write_n && (address == 0))
		begin
          
			 data <= (writedata*50);
			 
		end
			 
    end
	 
 always @(posedge clk)
 begin 
   sample_clk = sample_clk + 1;
   
	if (sample_clk < (data))
	begin 
		data_out<= 1;
	end
	else if (sample_clk == (data))
	begin 
	   data_out <= 0;
	end
	else if (sample_clk >= (50000000/FREQ))
	begin 
	   sample_clk <= 18'b0;
	end
	
 end


  assign readdata = {32'b0 | read_mux_out};
  assign out_port = data_out;

endmodule