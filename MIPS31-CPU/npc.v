`timescale 1ns / 1ps

module npc(
    input [31:0] pc,
    input reset,
    output [31:0] data_out
    );
    assign data_out = reset ? pc: (pc + 4);
endmodule
