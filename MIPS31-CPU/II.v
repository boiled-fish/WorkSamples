`timescale 1ns / 1ps

module II(
    input [3:0] a,
    input [25:0] b,
    output [31:0] data_out
    );
    assign data_out = {a, b, 2'b00};
endmodule
