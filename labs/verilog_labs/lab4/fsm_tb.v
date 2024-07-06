`timescale 1ns / 1ps
module fsm_tb();
    reg clk , rst , start;
    wire SUM_SEL , NEXT_SEL , A_SEL , LD_SUM , LD_NEXT , NEXT_ZERO , DONE;
    wire [31:0] sum_out;
    fsm_controller FSM1(.clk(clk),
                        .rst_n(rst),
                        .start(start),
                        .next_zero(NEXT_ZERO),
                        .LD_SUM(LD_SUM),
                        .LD_NEXT(LD_NEXT),
                        .SUM_SEL(SUM_SEL),
                        .NEXT_SEL(NEXT_SEL),
                        .A_SEL(A_SEL),
                        .DONE(DONE));

    datapath DP1(.clk(clk),
             .rst(rst),
             .SUM_SEL(SUM_SEL),
             .NEXT_SEL(NEXT_SEL),
             .A_SEL(A_SEL),
             .LD_SUM(LD_SUM),
             .LD_NEXT(LD_NEXT),
             .NEXT_ZERO(NEXT_ZERO),
             .sum_out(sum_out));


    always #5 clk<=~clk;

    initial begin
    clk<=0;
    rst<=0;
    start<=0;
  
    #15 rst<=1;
    #15 start<=1;
    end

endmodule
