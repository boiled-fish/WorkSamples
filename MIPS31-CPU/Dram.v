`timescale 1ns / 1ps

module Dram(
    input clk,
    input DM_cs,
    input DM_w,
    input DM_r,
    input [10:0] addr,
    input [31:0] data_in,
    output [31:0] data_out
    );
    
    reg [31:0] mem [0:31];
    
    assign data_out = (DM_r && DM_cs) ? mem[addr] : 32'hz;

    always@(posedge clk) begin
        if(DM_w && DM_cs) begin
            mem[addr] <= data_in;             
        end
    end
endmodule

