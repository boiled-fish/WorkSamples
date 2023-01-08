module regfile(
    input clk,
    input rst,
    input RF_w,
    input [4:0] rsc,
    input [4:0] rtc,
    input [4:0] rdc,
    input [31:0] rd,
    output [31:0] rs,
    output [31:0] rt 
    );
    reg [31:0] array_reg [31:0];
    reg [5:0] i; 
    
    assign rs = array_reg[rsc];
    assign rt = array_reg[rtc];

    always @(negedge clk or posedge rst) begin
        if (rst)
            for(i=0; i<32; i = i + 1)
                array_reg[i] <= 0;
        else if (RF_w && rdc != 0)
            array_reg[rdc] <= rd;
    end
endmodule