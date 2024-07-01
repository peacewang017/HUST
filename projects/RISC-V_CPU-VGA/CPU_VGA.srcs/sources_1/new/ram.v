`timescale 1ns / 1ps

module ram(
    input [9:0] Addr, Addr2,
    input [31:0] Din,
    input CLK, MemWrite,
    input rst,
    input button_a, button_w, button_d, button_s,
    input v_sync,
    output [31:0] Dout, Dout2
);

    reg [31:0] RAM[1023:0]; 
    integer i;
    
    initial begin
        for (i = 0; i <= 1023; i = i + 1) begin
            RAM[i] = 0;
        end
    end

    always @(posedge CLK) begin
        if (MemWrite) begin
            RAM[Addr] <= Din;   
        end

        if (button_a == 1 && button_w == 0 && button_d == 0 && button_s == 0) begin
            RAM[0] <= 32'h00000001;
        end
        else if (button_a == 0 && button_w == 1 && button_d == 0 && button_s == 0) begin
            RAM[0] <= 32'h00000002;
        end
        else if (button_a == 0 && button_w == 0 && button_d == 1 && button_s == 0) begin
            RAM[0] <= 32'h00000003;
        end
        else if (button_a == 0 && button_w == 0 && button_d == 0 && button_s == 1) begin
            RAM[0] <= 32'h00000004;
        end
        
        if(rst) begin
            for (i = 0; i <= 1023; i = i + 1) begin
                RAM[i] <= 0;
            end
        end

        if(v_sync) begin
            RAM[1] <= 0;
        end

        if(v_sync == 0) begin
            RAM[1] <= 1;
        end
    end

    assign Dout = RAM[Addr];
    assign Dout2 = RAM[Addr2];

endmodule