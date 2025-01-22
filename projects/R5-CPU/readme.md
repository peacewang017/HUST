# 华中科技大学计算机组成原理课程设计

在基于 Xlinx FPGA 开发板的哈佛架构、RISC-V 指令集 CPU 上运行一个 C 语言贪吃蛇游戏，使用 VGA 显示屏作为输出。

## RISC-V CPU 模块
支持指令：
```
add sub and or slt sltu addi andi ori xori slti slli srli srai lw sw ecall beq bne jal jalr xor auipc lhu bgeu blt lui mul div
```

## VGA 模块
将内存 0x008 开始的 16\*12 个字节映射到屏幕的 1024\*768 个像素点。

## 贪吃蛇模块
/snake_game 下，snake_game.c 为源代码，snake_game.s 为手动替换不支持指令后的汇编，mem.txt 为最终导入的 hex 文件。
