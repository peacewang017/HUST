module divide_and_count(clk , out)
    input clk;
    output [2:0] out;
    wire clk_N;

    divider(.clk(clk) , .clk_N(clk_N));
    counter(.clk(clk_N) , .out(out[2:0]));
endmodule 