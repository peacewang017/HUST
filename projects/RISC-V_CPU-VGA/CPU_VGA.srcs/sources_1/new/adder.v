`timescale 1ns / 1ps
module adder(a,b,out);
    input [31:0]a,b;
    output [31:0] out;
    assign out = a+b;
endmodule