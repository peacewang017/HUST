`timescale 1ns / 1ps
module mux_21(a, b, sel, out);
    parameter WIDTH = 8;
    
    input [WIDTH-1:0] a, b;
    input sel;
    output reg [WIDTH-1:0] out;
    
    always @ (*) begin
        out = sel == 0 ? a : b;
    end
    
endmodule
