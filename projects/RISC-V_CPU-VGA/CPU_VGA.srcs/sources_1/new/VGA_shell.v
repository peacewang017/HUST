`timescale 1ns / 1ps

module VGA_shell(
   clk,
   rst,
   Addr2,
   Dout2,
   h_sync,
   v_sync,
   vga
);
   input clk,rst;
   input [31:0]Dout2;
   output [9:0]Addr2;
   output h_sync,v_sync;
   output [11:0] vga;

   wire [10:0] vaddr_x,vaddr_y;
   wire [11:0]v_data;
   wire clk_vga;

   clk_wiz_0 clk_wiz (.clk_in1(clk),.clk_out1(clk_vga));

   color_trans color_trans1(
    .ram_data_out(Dout2),
    .vdata(v_data)
   );

   vga_display vga_display1(
    .clk(clk_vga),
    .rst(rst),
    .vdata(v_data),
    .vaddr_x(vaddr_x),
    .vaddr_y(vaddr_y),
    .h_sync(h_sync),
    .v_sync(v_sync),
    .vga(vga)
   );

   translator translator1(
    .vaddr_x(vaddr_x),
    .vaddr_y(vaddr_y),
    .Addr2(Addr2)
   );
endmodule
