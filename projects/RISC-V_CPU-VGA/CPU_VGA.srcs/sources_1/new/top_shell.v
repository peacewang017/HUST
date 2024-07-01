`timescale 1ns / 1ps
module top_shell(
    clk,
    go,
    rst,
    button_a,
    button_w,
    button_d,
    button_s,
    v_sync,
    h_sync,
    AN,
    SEG,
    vga
    );
    input clk;
    input go;
    input rst;
    input button_a,button_w,button_d,button_s;
    output v_sync,h_sync;
    output [7:0]AN,SEG;
    output [11:0]vga;
    
    wire [9:0] Addr2;
    wire [31:0]Dout2;
    
    datapath datapath_1(
        .CLK(clk),
        .Go(go),
        .RST(rst),
        .button_a(button_a),
        .button_w(button_w),
        .button_d(button_d),
        .button_s(button_s),
        .AN(AN),
        .SEG(SEG),
        .Addr2(Addr2),
        .Dout2(Dout2),
        .v_sync(v_sync)
    );
    
    VGA_shell VGA_shell1(
        .clk(clk),
        .rst(rst),
        .Addr2(Addr2),
        .Dout2(Dout2),
        .h_sync(h_sync),
        .v_sync(v_sync),
        .vga(vga)
    );
endmodule
