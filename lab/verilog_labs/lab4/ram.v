module ram( read_addr, clk, q);
  parameter DATA_WIDTH = 8;
  parameter ADDR_WIDTH = 3;

  input clk;
  input [ADDR_WIDTH-1:0] read_addr;
  output reg [DATA_WIDTH-1:0] q;

  reg [DATA_WIDTH-1:0] ram[2**ADDR_WIDTH-1:0];

  initial begin                            
    $readmemh("C:/code/verilog/lab4/ram_init.txt", ram);	end
   
  always @(*) begin
    q <= ram[read_addr];
  end

endmodule