`timescale 1ns / 1ps
module alu(x,y,alu_op,equal,bigger_or_equal,result);
    input [31:0]x,y;
    input [3:0]alu_op;
    output reg [31:0]result;
    output reg equal,bigger_or_equal;
    reg [63:0]x_64,y_64;

    initial begin
        result = 0;
        equal = 0;
        bigger_or_equal = 0;
    end

    always @(*) begin
        equal = !(x^y);

        case(alu_op)
            0:  begin
                result = x << (y[4:0]);
            end
            1:  begin
                result = $signed(x) >>> (y[4:0]);
            end
            2:  begin
                result = x >> (y[4:0]);
            end
            3:  begin
                x_64 = {32'h00000000,x};
                y_64 = {32'h00000000,y};
                x_64 = x_64*y_64;
                result = x_64[31:0];
            end
            4:  begin
                result = x/y;
            end
            5:  begin
                result = x+y;
            end
            6:  begin
                result = x-y;
            end
            7:  begin
                result = x&y;
            end
            8:  begin
                result = x|y;
            end
            9:  begin
                result = x^y;
            end
            10: begin
                result = ~(x|y);
            end
            11: begin
                result = ($signed(x)<$signed(y)) ? 1:0;
                bigger_or_equal = ($signed(x)>=$signed(y)) ? 1:0;
            end
            12: begin
                result = ($unsigned(x)<$unsigned(y)) ? 1:0;
                bigger_or_equal = ($unsigned(x)>=$unsigned(y)) ? 1:0;
            end
            13: begin
                result = x%y;
            end
        endcase
    end
endmodule 