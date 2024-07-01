`timescale 1ns / 1ps
module comparator(a, b, is_equal);
    parameter WIDTH = 8;
    
    input [WIDTH-1:0] a, b;
    output is_equal;
    
    assign is_equal = (a == b) ? 1'b1 : 1'b0;
endmodule