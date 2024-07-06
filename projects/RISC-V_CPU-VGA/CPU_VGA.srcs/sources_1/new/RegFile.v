`timescale 1ns / 1ps
module RegFile(clk,we,r1_addr,r2_addr,w_addr,d_in,r1,r2);
    input clk,we;
    input [4:0]r1_addr,r2_addr,w_addr;
    input [31:0]d_in;
    output reg [31:0]r1,r2;
    reg [31:0]memory[31:0];
    
    integer i;
    initial begin
        for (i=0;i<=31;i=i+1) begin
            memory[i] = 0;
        end
    end
    
    always @(posedge clk) begin
        if(we == 1 && w_addr!=0) begin
            memory[w_addr] = d_in;
        end
    end

    //always @(*)??????????????
    always @(*) begin
        r1 = memory[r1_addr];
        r2 = memory[r2_addr];
    end
endmodule