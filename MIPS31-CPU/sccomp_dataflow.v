`timescale 1ns / 1ps

module sccomp_dataflow(
    input clk_in,
    input reset,
    output [31:0] inst,
    output [31:0] pc
    );
    wire [10:0] data_addr;
    wire [31:0] inst_addr;
    wire IM_R, DM_cs, DM_w, DM_r;
    wire [31:0] wdata, rdata, r;
    
    assign inst_addr = pc - 32'h0040_0000;
    assign data_addr = (r - 32'h10010000) /4;
    
    mips_31_mars_simulate imem (inst_addr[12:2], inst);
    Dram dmem (
        clk_in, 
        DM_cs, DM_w, DM_r, 
        data_addr, wdata, rdata
        );
    cpu sccpu(
        clk_in, reset, inst, rdata, 
        IM_R, 
        DM_cs, DM_r, DM_w, 
        pc, r, wdata
        );
endmodule
