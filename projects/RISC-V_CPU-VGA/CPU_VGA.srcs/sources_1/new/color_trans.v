module color_trans(ram_data_out,vdata);
    input [31:0] ram_data_out;
    output reg [11:0] vdata;
    always @(*) begin
        if(ram_data_out==32'h00000001) begin
            vdata <= 12'hfff;
        end
        else if(ram_data_out==32'h00000002) begin
            vdata <= 12'h00f;
        end
        else if(ram_data_out==32'h00000000) begin
            vdata <= 12'h444;
        end
        else begin
            vdata <= 12'h000;
        end
    end
endmodule