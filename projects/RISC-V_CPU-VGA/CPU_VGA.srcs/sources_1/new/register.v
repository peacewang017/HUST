`timescale 1ns / 1ps
module register(clk, rst, en, d_in, d_out);
    parameter WIDTH = 32;   
    input clk, rst, en;     
    input [WIDTH-1:0] d_in;  
    output [WIDTH-1:0] d_out; 
    reg [31:0] ram;        

    initial ram = 0;         

    always @(posedge clk) begin
        if (rst)
            ram <= 0;        
        else if (en)
            ram <= d_in;     
        else
            ram = ram;        
    end

    assign d_out = ram;       
endmodule