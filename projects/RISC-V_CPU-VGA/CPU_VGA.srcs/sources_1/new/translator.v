module translator(vaddr_x , vaddr_y , Addr2);
    input [10:0] vaddr_x,vaddr_y;
    output reg [9:0] Addr2;
    
    wire [10:0] board_y,board_x;

    assign board_y = (vaddr_x / 64)+1;
    assign board_x = (vaddr_y / 64)+1;

    always @(*) begin
        Addr2 <= (8+((board_x-1)*16+board_y)*4)/4;
    end
endmodule