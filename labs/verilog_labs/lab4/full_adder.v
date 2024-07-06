`timescale 1ns / 1ps
//无使能的全加器
module full_adder(a, b, carry,sum);
    // 学习 parameter 的申明及使用
    parameter WIDTH = 8;
    input [WIDTH-1:0] a, b;
    output [WIDTH-1:0] sum;
    output [WIDTH-1:0] carry;
    
    assign { carry, sum } = a + b ;
endmodule