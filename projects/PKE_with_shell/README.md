# Proxy Kernel

基于 Spike emulator 和 RISC-V Proxy Kernel

## 1 单元测试
有以下单元测试：

- /bin/app_semaphore
- /bin/app_semaphore2
- /bin/app_singlepageheap
- /bin/app_singlepageheap2
- /bin/app_wait
- /bin/app_yield
- /bin/app_link
- /bin/app_loogloop
- /bin/app_cow

## 2 shell 模式
输入`spike -p2 obj/riscv-pke /bin/app_shell`，可以使用一些简单的 linux 指令（如 ls, mkdir, cd, touch, cat, link 等）




