`timescale 1ns / 1ns
module mux5(
    input [4:0] a,
    input [4:0] b,
    input [1:0] choose,
    output [4:0] z
    );
    reg [4:0] temp_z;
    always @(*)
    begin
    case(choose)
        2'b00:temp_z <= a;
        2'b01:temp_z <= b;
        2'b10:temp_z <= 5'b11111;
        2'b11:temp_z <= 5'b11111;
        default:temp_z <= 5'bz;
    endcase
    end
    assign z = temp_z;
endmodule