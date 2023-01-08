module pcreg(
    input clk,
    input rst,
    input [31:0] data_in,
    output reg [31:0] data_out
    );
    parameter pc_base = 32'h00400000;
    always @(negedge clk or posedge rst) begin
        if (rst)
            data_out <= pc_base; 
        else
            data_out <= data_in;
    end
endmodule