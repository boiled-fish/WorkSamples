`timescale 1ns / 1ns
module extend16(
    input [15:0] a,
    input sign,             //1±íÊ¾ÓÐ·ûºÅ
    output [31:0] b
    );
    assign b = sign ? {{16{a[15]}}, a} : {16'b0, a};
endmodule