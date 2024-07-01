module divider(clk, clk_N);
input clk;                      // 系统时钟
output clk_N;                   // 分频后的时钟
parameter N = 100_000_000;      // 1Hz的时钟,N=fclk/fclk_N
reg [31:0] counter;             /* 计数器变量，通过计数实现分频。
                                   当计数器从0计数到(N/2-1)时，
                                   输出时钟翻转，计数器清零 */

initial begin
    count = 0;
    clk_N = 0;
end       
                            
always @(posedge clk)  begin   
    count <= count+1;
    if(count >= (N/2)-1) begin
        clk_N <= ~clk_N;
        count <= 0;
    end                      
end                           
endmodule
