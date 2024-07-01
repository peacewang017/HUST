`timescale 1ns / 1ps
module datapath(clk, rst, SUM_SEL, NEXT_SEL, A_SEL, LD_SUM, LD_NEXT, NEXT_ZERO, sum_out);
    input clk,rst,SUM_SEL,NEXT_SEL,A_SEL,LD_SUM,LD_NEXT;    
    output NEXT_ZERO;
    output [31:0] sum_out;

    wire [31:0] Mem_data;
    wire [31:0] A1_result,A2_result;
    wire [31:0] R2_result;
    wire [31:0] S1_result,S2_result;
    wire [4:0] S3_result;
    wire [31:0] Sum_overflow,Addr_overflow;     
    
    full_adder #(32) A1 (sum_out,Mem_data,Sum_overflow,A1_result);
    full_adder #(32) A2 (1,R2_result,Addr_overflow,A2_result);

    register #(32) R1 (clk,rst,LD_SUM,S1_result,sum_out);
    register #(32) R2 (clk,rst,LD_NEXT,S2_result,R2_result);

    mux_21 #(32) S1 (0,A1_result,SUM_SEL,S1_result);
    mux_21 #(32) S2 (0,Mem_data,NEXT_SEL,S2_result);
    mux_21 #(32) S3 (R2_result,A2_result,A_SEL,S3_result);

    comparator #(32) C1 (0,S2_result,NEXT_ZERO);

    ram #(32,5) MEMORY (S3_result,clk,Mem_data);
endmodule