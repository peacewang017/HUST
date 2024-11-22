`timescale 1ns / 1ps
module rom(Addr, Dout);
    input [9:0] Addr;
    output reg [31:0] Dout;
    reg [31:0] rom[1023:0];

    initial begin
        $readmemh("C:\\Users\\12456\\Desktop\\sources\\CPU_VGA\\mem.txt", rom);//abs path
    end

    always @(Addr) begin
        Dout <= rom[Addr];
    end
endmodule
