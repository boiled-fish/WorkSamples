`timescale 1ns / 1ns
module extend18 (
    input [15:0] a,
    output [31:0] b
    );
    assign b = {{14{a[15]}}, a, 2'b00};
endmodule