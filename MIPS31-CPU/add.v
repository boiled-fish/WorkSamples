`timescale 1ns / 1ns
module add(
    input [31:0] a,
    input [31:0] b,
    output reg [31:0] r
    );
    always@(*)begin
        r = a + b;
    end
endmodule