`timescale 1ns / 1ps
module cpu(
    input         clk,
    input         reset,
    input  [31:0] inst,
    input  [31:0] rdata,
    output        IM_r,
    output        DM_cs,
    output        DM_r,
    output        DM_w,
    output [31:0] pc,
    output [31:0] addr,
    output [31:0] wdata
    );
    //npc
    wire [31:0] npc;
    //pc
    wire [31:0] pc_data;
    wire pc_clk;
    //regfiles
    wire [31:0] rd;
    wire [4:0] rdc;
    wire [4:0] rtc, rsc;
    wire RF_w;
    wire RF_clk;
    wire [31:0] rs, rt;
    //alu
    wire [31:0] op1, op2;
    wire [3:0] aluc;
    wire [31:0] r;
    //mux
    wire m1, m2, m3, m4, m5, m6, m7, m8, m9;
    wire [31:0] data1, data2, data3, data5, data7, data8, data9;
    wire [4:0] data4, data6;
    //sign
    wire zf, of, cf, nf;
    //states
    wire [31:0] choose;
    wire [31:0] ext5_data;
    wire ext16_sign;
    wire [31:0] ext16_data;
    wire [31:0] ext18_data;
    wire [31:0] add_data;
    wire [31:0] add8_data;
    wire [31:0] ii_data;
    
    assign npc = pc + 4;
    assign pc = pc_data;
    pcreg cpu_pc (
        pc_clk,
        reset, 
        data1,
        pc_data
        );
    //regfile
    assign rd = data5, rdc = data6;
    assign rsc = inst[25:21], rtc = inst[20:16];
    regfile cpu_ref (
        RF_clk, reset, RF_w, 
        rsc, rtc, rdc, rd, 
        rs, rt
        );
    //alu
    assign addr = r;
    assign wdata = rt;
    assign op1 = data8, op2 = data9;
    alu cpu_alu (
        op1, op2, aluc, 
        r, 
        zf, cf, nf, of
        );
    //mux
    mux mux1(data3, data2, m1, data1);
    mux mux2(npc, add_data, m2, data2);
    mux mux3(ii_data, rs, m3, data3);
    mux5 mux4 (inst[10:6], rs[4:0], {choose[30], m4}, data4);
    mux mux5(data7, add8_data, m5, data5);////////////
    mux5 mux6 (inst[15:11], inst[20:16], {choose[30], m6}, data6);
    mux mux7(r, rdata, m7, data7);
    mux mux8(ext5_data, rs, m8, data8);
    mux mux9(rt, ext16_data, m9, data9);
    //states
    instr_decoder choice (inst, choose);
    operation cpu_states (
        clk, zf, choose, pc_clk, IM_r,
        m1, m2, m3, m4, m5, m6, m7, m8, m9,
        aluc,
        RF_w, RF_clk, 
        DM_cs, DM_w, DM_r,
        ext16_sign
        );
    //ext5
    extend5 cpu_ext5_4 (data4, ext5_data);
    //ext16
    extend16 cpu_ext16 (inst[15:0], ext16_sign, ext16_data);
    //ext18
    extend18 cpu_ext18 (inst[15:0], ext18_data);
    //add
    add cpu_add (ext18_data, npc, add_data);
    //add8
    add8 cpu_add8 (pc_data, add8_data);
    //ii
    II cpu_ii (pc_data[31:28], inst[25:0], ii_data);

endmodule