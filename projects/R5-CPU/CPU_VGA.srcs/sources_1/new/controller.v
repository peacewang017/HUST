`timescale 1ns / 1ps
module controller(Funct,OpCode,ALUOP,MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT);
    input[4:0] Funct;
    input[4:0] OpCode;
    output [3:0] ALUOP;
    output MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT;
    
    wire [6:0]Funct7;
    wire [2:0]Funct3;
    assign Funct3=Funct[2:0];
    assign Funct7={1'b0,Funct[4],4'b0000,Funct[3]};

    alu_controller controller1(
                .Funct7(Funct7),
                .Funct3(Funct3),
                .OpCode(OpCode),
                .ALUOP(ALUOP)
                );

    signal_controller controller2(
                .Funct7(Funct7),
                .Funct3(Funct3),
                .OpCode(OpCode),
                .MemtoReg(MemtoReg),
                .MemWrite(MemWrite),
                .ALU_src(ALU_src),
                .RegWrite(RegWrite),
                .ecall(ecall),
                .S_Type(S_Type),
                .BEQ(BEQ),
                .BNE(BNE),
                .Jal(Jal),
                .jalr(jalr),
                .AUIPC(AUIPC),
                .LHU(LHU),
                .BGEU(BGEU),
                .CSRRSI(CSRRSI),
                .CSRRCI(CSRRCI),
                .LUI(LUI),
                .BLT(BLT)
                );
endmodule

module alu_controller(Funct7,Funct3,OpCode,ALUOP);
    input[6:0] Funct7;
    input[2:0] Funct3;
    input[4:0] OpCode;
    output reg[3:0] ALUOP;
    always@(Funct7,Funct3,OpCode)begin
        if(Funct7==7'b0000000 && Funct3==3'b000 && OpCode==5'b01100) ALUOP=4'b0101;//add
        else if(Funct7==7'b0100000 && Funct3==3'b000 && OpCode==5'b01100) ALUOP=4'b0110;//sub
        else if(Funct7==7'b0000000 && Funct3==3'b111 && OpCode==5'b01100) ALUOP=4'b0111;//and
        else if(Funct7==7'b0000000 && Funct3==3'b110 && OpCode==5'b01100) ALUOP=4'b1000;//or
        else if(Funct7==7'b0000000 && Funct3==3'b010 && OpCode==5'b01100) ALUOP=4'b1011;//slt
        else if(Funct7==7'b0000000 && Funct3==3'b011 && OpCode==5'b01100) ALUOP=4'b1100;//sltu
        else if(Funct7==7'b0000000 && Funct3==3'b100 && OpCode==5'b01100) ALUOP=4'b1001;//xor
        else if(Funct7==7'b0000001 && Funct3==3'b000 && OpCode==5'b01100) ALUOP=4'b0011;//mul
        else if(Funct7==7'b0000001 && Funct3==3'b100 && OpCode==5'b01100) ALUOP=4'b0100;//div
        else if(Funct7==7'b0000001 && Funct3==3'b110 && OpCode==5'b01100) ALUOP=4'b1101;//rem
        else if(Funct3==3'b000 && OpCode==5'b00100) ALUOP=4'b0101;//addi
        else if(Funct3==3'b111 && OpCode==5'b00100) ALUOP=4'b0111;//andi
        else if(Funct3==3'b110 && OpCode==5'b00100) ALUOP=4'b1000;//ori
        else if(Funct3==3'b100 && OpCode==5'b00100) ALUOP=4'b1001;//xori
        else if(Funct3==3'b010 && OpCode==5'b00100) ALUOP=4'b1011;//slti
        else if(Funct7==7'b0000000 && Funct3==3'b001 && OpCode==5'b00100) ALUOP=4'b0000;//slli
        else if(Funct7==7'b0000000 && Funct3==3'b101 && OpCode==5'b00100) ALUOP=4'b0010;//srli
        else if(Funct7==7'b0100000 && Funct3==3'b101 && OpCode==5'b00100) ALUOP=4'b0001;//srai
        else if(Funct3==3'b010 && OpCode==5'b00000) ALUOP=4'b0101;//lw
        else if(Funct3==3'b010 && OpCode==5'b01000) ALUOP=4'b0101;//sw
        else if(OpCode==5'b11001) ALUOP=4'b0101;//jalr
        else if(Funct3==3'b000 && OpCode==5'b11000) ALUOP=4'b1011;//beq
        else if(Funct3==3'b001 && OpCode==5'b11000) ALUOP=4'b1011;//bne
        else if(Funct3==3'b101 && OpCode==5'b00000) ALUOP=4'b0101;//lhu
        else if(Funct3==3'b111 && OpCode==5'b11000) ALUOP=4'b1100;//bgeu
        else if(Funct3==4'b100 && OpCode==5'b11000) ALUOP=4'b1011;//blt
        else ALUOP=4'b0101;
    end
endmodule

module signal_controller(Funct7,Funct3,OpCode,MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT);
    input[6:0] Funct7;
    input[2:0] Funct3;
    input[4:0] OpCode;
    output reg MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT;

    always @(Funct7,Funct3,OpCode) begin
        if(Funct7==0 && Funct3 == 0 && OpCode == 5'hc) begin 
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00010000000000000;//add
        end
        else if(Funct7==32 && Funct3 == 0 && OpCode == 5'hc) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00010000000000000;//sub
        end
        else if(Funct7==0 && Funct3 == 7 && OpCode == 5'hc) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00010000000000000;//and
        end
        else if(Funct7==0 && Funct3 == 6 && OpCode == 5'hc) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00010000000000000;//or
        end
        else if(Funct7==0 && Funct3 == 2 && OpCode == 5'hc) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00010000000000000;//slt
        end
        else if(Funct7==1 && Funct3 == 4 && OpCode == 5'hc) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00010000000000000;//mul
        end
        else if(Funct7==1 && Funct3 == 0 && OpCode == 5'hc) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00010000000000000;//div
        end
        else if(Funct7==1 && Funct3 == 6 && OpCode == 5'hc) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00010000000000000;//rem
        end
        else if(Funct7==0 && Funct3 == 3 && OpCode == 5'hc) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00010000000000000;//sltu
        end
        else if(Funct3==0 && OpCode==5'h4) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00110000000000000;//addi
        end
        else if(Funct3==7 && OpCode==5'h4) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00110000000000000;//andi
        end
        else if(Funct3==6 && OpCode==5'h4) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00110000000000000;//ori
        end
        else if(Funct3==4 && OpCode==5'h4) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00110000000000000;//xori
        end
        else if(Funct3==2 && OpCode==5'h4) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00110000000000000;//slti
        end
        else if(Funct7==0 && Funct3==1 && OpCode==5'h4) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00110000000000000;//slli
        end
        else if(Funct7==0 && Funct3==5 && OpCode==5'h4) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00110000000000000;//srli
        end
        else if(Funct7==32 && Funct3==5 && OpCode==5'h4) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00110000000000000;//srai
        end
        else if(Funct3==2 && OpCode==5'h0) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b10110000000000000;//lw
        end
        else if(Funct3==2 && OpCode==5'h8) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b01100100000000000;//sw
        end
        else if(Funct7==0 && Funct3==0 && OpCode==5'h1c) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00001000000000000;//ecall
        end
        else if(Funct3==0 && OpCode==5'h18) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00000010000000000;//beq
        end
        else if(Funct3==1 && OpCode==5'h18) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00000001000000000;//bne
        end
        else if(OpCode==5'h1b) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00010000100000000;//jal
        end
        else if(Funct3==0 && OpCode==5'h19) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00010000010000000;//jalr
        end
        else if(Funct3==6 && OpCode==5'h1c) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00000000000001000;//csrrsi
        end
        else if(Funct3==7 && OpCode==5'h1c) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00000000000000100;//csrrci
        end
        else if(Funct7==0 && Funct3==4 && OpCode==5'hc) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00010000000000000;//xor
        end
        else if(OpCode==5'h5) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00010000001000000;//auipc
        end
        else if(Funct3==5 && OpCode==5'h0) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b10110000000100000;//lhu
        end
        else if(Funct3==7 && OpCode==5'h18) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00000000000010000;//bgeu
        end
        else if(OpCode==5'hd) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00010000000000010;//lui
        end
        else if(Funct3==4 && OpCode==5'h18) begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00000000000000001;//blt
        end
        else begin
            {MemtoReg,MemWrite,ALU_src,RegWrite,ecall,S_Type,BEQ,BNE,Jal,jalr,AUIPC,LHU,BGEU,CSRRSI,CSRRCI,LUI,BLT} = 17'b00000000000000000;
        end
    end
endmodule