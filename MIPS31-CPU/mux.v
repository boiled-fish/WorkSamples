`timescale 1ns / 1ns
module mux(
    input [31:0] a,
    input [31:0] b,
    input choose,
    output reg [31:0] z
    );
    always @(*)
    begin
    case(choose)
        1'b1:z <= b;
        1'b0:z <= a;
   endcase
   end
endmodule