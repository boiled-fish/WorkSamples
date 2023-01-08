`timescale 1ns / 1ns
module operation(
    input clk,
    input zf,
    input [31:0] i,
    output PC_CLK,    
    output IM_R,     
    output M1,       
    output M2,        
    output M3,        
    output M4,        
    output M5,        
    output M6,       
    output M7, 
    output M8,      
    output M9,      
    output [3:0] aluc,    
    output RF_W,     
    output RF_CLK,  
    output DM_cs,
    output DM_w,   
    output DM_r,   
    output ext16_sign  
    );

    assign PC_CLK = clk;
    assign IM_R = 1;
    //j || jr || jal
    assign M1 = ~(i[16] || i[29] || i[30]);
    //beq || bne
    assign M2 = (i[24] & zf) || (i[25] & ~zf);
    //jr
    assign M3 = i[16];
    //sllv || srlv || srav
    assign M4 = i[13] || i[14] || i[15];
    //jal
    assign M5 = i[30];
    //addi || addiu || andi || ori || xori || lw || sw || slti || sliu || lui
    assign M6 = i[17] || i[18] || i[19] || i[20] || i[21] || i[22] || i[23] || i[26] || i[27] || i[28];
    //lw
    assign M7 = i[22];
    // !(sll || srl || sra || sllv || srav)
    assign M8 = ~(i[10] || i[11] || i[12] || i[13] || i[14] || i[15]);
    // 
    assign M9 = i[17] || i[18] || i[19] || i[20] || i[21] || i[22] || i[23] || i[26] || i[27] || i[28];
    
    //sub || subu || or || nor || slt || srl || srlv || ori || beq || bne || slti
    assign aluc[0] = i[2] || i[3] || i[5] || i[7] | i[8] || i[11] || i[14] || i[20] || i[24] || i[25] || i[26];
    //add || sub || xor || nor || slt || sltu || sll || sllv || addi || xori || lw || sw || beq || bne || slti || sltiu
    assign aluc[1] = i[0] || i[2] || i[6] || i[7] || i[8] || i[9] || i[10] || i[13] || i[17] || i[21] || i[22] || i[23] || i[24] || i[25] || i[26] || i[27];
    //and || or || xor || nor || sll || srl || sra || sllv || srlv || srav || andi || ori || xori
    assign aluc[2] = i[4] || i[5] || i[6] || i[7] || i[10] || i[11] || i[12] || i[13] || i[14] || i[15] || i[19] || i[20] || i[21];
    //slt || sltu || sll || srl || sra || sllv || srav || slti || sltiu || lui
    assign aluc[3] = i[8] || i[9] || i[10] || i[11] || i[12] || i[13] || i[14] || i[15] || i[26] || i[27] || i[28];
    //~(jr || sw || beq || bne || j
    assign RF_W = ~(i[16] || i[23] || i[24] || i[25] || i[29]);
    assign RF_CLK = clk;
    //sw
    assign DM_cs = i[22] || i[23];
    //lw
    assign DM_w = i[23];
    //sw
    assign DM_r = i[22];
    
    assign ext16_sign = (i[17] || i[18] || i[22] || i[23] || i[26]);
endmodule