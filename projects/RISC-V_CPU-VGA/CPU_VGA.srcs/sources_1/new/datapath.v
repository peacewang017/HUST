`timescale 1ns / 1ps

`include "alu.v"
`include "adder.v"
`include "controller.v"
`include "extender.v"
`include "FPGADigit.v"
`include "mux.v"
`include "ram.v"
`include "RegFile.v"
`include "register.v"
`include "rom.v"
`include "shifter.v"

module datapath(CLK,Go,RST,button_a,button_w,button_d,button_s,AN,SEG,Addr2,Dout2,v_sync);
    input CLK;
    input Go;
    input RST;
    input button_a,button_w,button_d,button_s;
    input [9:0]Addr2;
    input v_sync;
    output [31:0]Dout2;
    output [7:0]AN,SEG;
    
    wire [31:0]LedData;
    wire CLK_N;
    wire [31:0]PC,IR;
    wire [4:0]OP;
    wire [4:0]Funct;
    wire [3:0]AluOP;
    wire Beq,Bne,MemToReg,MemWrite,AluSrcB,RegWrite,S_type,Bgeu,LHU,ecall,AUIPC,JAL,JALR,J_type,LUI,BLT;
    wire PCjump;
    wire [4:0]IR_R1_addr,IR_R2_addr;
    wire [4:0]R1_addr,R2_addr,W_addr;
    wire [11:0]I_immediate,S_immediate;
    wire [11:0]IorS_immediate;
    wire [31:0]IorS_immediate_ex;
    wire [31:0]U_immediate,B_immediate,J_immediate;
    wire [11:0]B_immediate_12;
    wire [19:0]J_immediate_20;
    wire [31:0]B_immediate_shifted,J_immediate_shifted;
    wire [31:0]R1out,R2out;
    wire [31:0]Bin;
    wire Equal,bigger_or_equal;
    wire [31:0]Aluresult;
    wire high_double_byte;
    wire [31:0]temp1,temp2,temp3,temp4,temp5,temp6,temp7;
    wire [31:0]MDin,RDin;
    wire [31:0]mem_out,mem_out_low,mem_out_high,mem_out_ex;
    wire [31:0]Dataout;
    wire [31:0]PC_add4,PC_addU,PC_addB,PC_addJ,JALR_addr;
    wire [31:0]PCnext;
    wire [31:0]R2_show;
    wire [31:0]cycle_num;
    wire less,BLT_jump;
    wire halt;
    
    assign OP = IR[6:2];
    assign Funct = {IR[30],IR[25],IR[14:12]};
    assign IR_R1_addr = IR[19:15];
    assign IR_R2_addr = IR[24:20];
    assign W_addr = IR[11:7];
    assign I_immediate = IR[31:20];
    assign S_immediate = {IR[31:25],IR[11:7]};
    assign B_immediate_12 = {IR[31],IR[7],IR[30:25],IR[11:8]};
    assign J_immediate_20 = {IR[31],IR[19:12],IR[20],IR[30:21]};
    assign U_immediate = {IR[31:12],12'b000000000000};
    assign J_type = JAL|JALR;
    assign high_double_byte = Aluresult[1];
    assign PCjump = (Beq&Equal) | (Bne&(~Equal)) | (Bgeu&bigger_or_equal);
    assign JALR_addr = Aluresult & 32'hfffffffe;
    assign halt = ((R1out==32'h0000000a)&ecall)&(~Go);
    assign MDin = R2out;
    assign LedData = R2_show;
    assign less = ~bigger_or_equal;
    assign BLT_jump = less & BLT;

    //
    counter_32 cycle_count(
        .clk(CLK_N),
        .out(cycle_num),
        .halt(halt),
        .RST(RST)
    );


    divider #(100000) divider2(
        .clk(CLK),
        .clk_N(CLK_N)
    );
    
    register #(32) PC_reg(
        .clk(CLK_N),
        .rst(RST),
        .en(~halt),
        .d_in(PCnext),
        .d_out(PC)
    );
    rom rom_1(
        .Addr(PC[11:2]),
        .Dout(IR)
    );
    controller controller1(
        .Funct(Funct),
        .OpCode(OP),
        .ALUOP(AluOP),
        .MemtoReg(MemToReg),
        .MemWrite(MemWrite),
        .ALU_src(AluSrcB),
        .RegWrite(RegWrite),
        .ecall(ecall),
        .S_Type(S_type),
        .BEQ(Beq),
        .BNE(Bne),
        .Jal(JAL),
        .jalr(JALR),
        .AUIPC(AUIPC),
        .LHU(LHU),
        .BGEU(Bgeu),
        .LUI(LUI),
        .BLT(BLT)
    );
    mux2 #(5) mux2_1(
        .out(R1_addr),
        .in0(IR_R1_addr),
        .in1(5'h11),
        .sel(ecall)
    );
    mux2 #(5) mux2_2(
        .out(R2_addr),
        .in0(IR_R2_addr),
        .in1(5'h0a),
        .sel(ecall)
    );
    mux2 #(12) mux2_3(
        .out(IorS_immediate),
        .in0(I_immediate),
        .in1(S_immediate),
        .sel(S_type)
    );
    extender_sign #(.in_WIDTH(12),.out_WIDTH(32)) extender1(
        .in(IorS_immediate),
        .out(IorS_immediate_ex)
    );
    mux2 #(32) mux2_4(
        .out(Bin),
        .in0(R2out),
        .in1(IorS_immediate_ex),
        .sel(AluSrcB)
    );
    RegFile regfile1(
        .clk(CLK_N),
        .we(RegWrite),
        .r1_addr(R1_addr),
        .r2_addr(R2_addr),
        .w_addr(W_addr),
        .d_in(temp5),
        .r1(R1out),
        .r2(R2out)
    );
    alu alu1(
        .x(R1out),
        .y(Bin),
        .alu_op(AluOP),
        .equal(Equal),
        .bigger_or_equal(bigger_or_equal),
        .result(Aluresult)
    );
    ram ram1(
        .Addr(Aluresult[11:2]),
        .Din(MDin),
        .CLK(CLK_N),
        .MemWrite(MemWrite),
        .Dout(mem_out),
        .button_a(button_a),
        .button_d(button_d),
        .button_s(button_s),
        .button_w(button_w),
        .Addr2(Addr2),
        .Dout2(Dout2),
        .rst(RST),
        .v_sync(v_sync)
    );
    extender_0 #(.in_WIDTH(16),.out_WIDTH(32)) extender2(
        .in(mem_out[15:0]),
        .out(mem_out_low)
    );
    extender_0 #(.in_WIDTH(16),.out_WIDTH(32)) extender3(
        .in(mem_out[31:16]),
        .out(mem_out_high)
    );
    mux2 #(32) mux2_5(
        .out(mem_out_ex),
        .in0(mem_out_low),
        .in1(mem_out_high),
        .sel(high_double_byte)
    );
    mux2 #(32) mux2_6(
        .out(Dataout),
        .in0(mem_out),
        .in1(mem_out_ex),
        .sel(LHU)
    );
    extender_sign #(.in_WIDTH(12),.out_WIDTH(32)) extender4(
        .in(B_immediate_12),
        .out(B_immediate)
    );
    extender_sign #(.in_WIDTH(20),.out_WIDTH(32)) extender5(
        .in(J_immediate_20),
        .out(J_immediate)
    );

    //
    mux2 #(32) mux2_7(
        .out(temp1),
        .in0(Aluresult),
        .in1(Dataout),
        .sel(MemToReg)
    );
    mux2 #(32) mux2_8(
        .out(temp2),
        .in0(temp1),
        .in1(PC_add4),
        .sel(J_type)
    );
    adder adder1(
        .a(PC),
        .b(U_immediate),
        .out(PC_addU)
    );
    mux2 #(32) mux2_9(
        .out(RDin),
        .in0(temp2),
        .in1(PC_addU),
        .sel(AUIPC)
    );

    //
    adder adder2(
        .a(PC),
        .b(32'h00000004),
        .out(PC_add4)
    );
    log_shifter_left #(32,5) shifter1(
        .in(B_immediate),
        .out(B_immediate_shifted),
        .shiftAmount(5'h01)
    );
    adder adder3(
        .a(PC),
        .b(B_immediate_shifted),
        .out(PC_addB)
    );
    mux2 #(32) mux2_10(
        .out(temp3),
        .in0(PC_add4),
        .in1(PC_addB),
        .sel(PCjump)
    );
    log_shifter_left #(32,5) shifter2(
        .in(J_immediate),
        .out(J_immediate_shifted),
        .shiftAmount(5'h01)
    );
    adder adder4(
        .a(PC),
        .b(J_immediate_shifted),
        .out(PC_addJ)
    );
    mux2 #(32) mux2_11(
        .out(temp4),
        .in0(temp3),
        .in1(PC_addJ),
        .sel(JAL)
    );
    mux2 #(32) mux2_12(
        .out(temp6),
        .in0(temp4),
        .in1(JALR_addr),
        .sel(JALR)
    );
    mux2 #(32) mux2_13(
        .out(temp5),
        .in0(RDin),
        .in1(U_immediate),
        .sel(LUI)
    );
    mux2 #(32) mux2_14(
        .out(PCnext),
        .in0(temp6),
        .in1(temp7),
        .sel(BLT_jump)
    );
    adder adder5(
        .a(PC),
        .b(B_immediate_shifted),
        .out(temp7)
    );

    //
    register #(32) datashow_reg(
        .clk(CLK_N),
        .rst(RST),
        .en(ecall&(~(R1out==32'h0000000a))),
        .d_in(R2out),
        .d_out(R2_show)        
    );
    FPGADigit FPGADigit1(
        .LedData(LedData),
        .CLK(CLK),
        .SEG(SEG),
        .AN(AN),
        .RST(RST)
    );
endmodule