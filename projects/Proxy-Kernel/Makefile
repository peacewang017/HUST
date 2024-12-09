# we assume that the utilities from RISC-V cross-compiler (i.e., riscv64-unknown-elf-gcc and etc.)
# are in your system PATH. To check if your environment satisfies this requirement, simple use 
# `which` command as follows:
# $ which riscv64-unknown-elf-gcc
# if you have an output path, your environment satisfy our requirement.

# ---------------------	macros --------------------------
CROSS_PREFIX 	:= riscv64-unknown-elf-
CC 				:= $(CROSS_PREFIX)gcc
AR 				:= $(CROSS_PREFIX)ar
RANLIB        	:= $(CROSS_PREFIX)ranlib

SRC_DIR        	:= .
OBJ_DIR 		:= obj
SPROJS_INCLUDE 	:= -I.  

HOSTFS_ROOT := hostfs_root
ifneq (,)
  march := -march=
  is_32bit := $(findstring 32,$(march))
  mabi := -mabi=$(if $(is_32bit),ilp32,lp64)
endif

CFLAGS        := -Wall -Werror  -fno-builtin -nostdlib -D__NO_INLINE__ -mcmodel=medany -g -Og -std=gnu99 -Wno-unused -Wno-attributes -fno-delete-null-pointer-checks -fno-PIE $(march)
COMPILE       	:= $(CC) -MMD -MP $(CFLAGS) $(SPROJS_INCLUDE)

#---------------------	utils -----------------------
UTIL_CPPS 	:= util/*.c

UTIL_CPPS  := $(wildcard $(UTIL_CPPS))
UTIL_OBJS  :=  $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(UTIL_CPPS)))


UTIL_LIB   := $(OBJ_DIR)/util.a

#---------------------	kernel -----------------------
KERNEL_LDS  	:= kernel/kernel.lds
KERNEL_CPPS 	:= \
	kernel/*.c \
	kernel/machine/*.c \
	kernel/util/*.c

KERNEL_ASMS 	:= \
	kernel/*.S \
	kernel/machine/*.S \
	kernel/util/*.S

KERNEL_CPPS  	:= $(wildcard $(KERNEL_CPPS))
KERNEL_ASMS  	:= $(wildcard $(KERNEL_ASMS))
KERNEL_OBJS  	:=  $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(KERNEL_CPPS)))
KERNEL_OBJS  	+=  $(addprefix $(OBJ_DIR)/, $(patsubst %.S,%.o,$(KERNEL_ASMS)))

KERNEL_TARGET = $(OBJ_DIR)/riscv-pke


#---------------------	spike interface library -----------------------
SPIKE_INF_CPPS 	:= spike_interface/*.c

SPIKE_INF_CPPS  := $(wildcard $(SPIKE_INF_CPPS))
SPIKE_INF_OBJS 	:=  $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(SPIKE_INF_CPPS)))


SPIKE_INF_LIB   := $(OBJ_DIR)/spike_interface.a


#---------------------	user   -----------------------
USER_CPPS 		:= user/app_shell.c user/user_lib.c

USER_OBJS  		:= $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(USER_CPPS)))

USER_TARGET 	:= $(HOSTFS_ROOT)/bin/app_shell

USER_S_CPPS 		:= user/app_cycle.c user/user_lib.c

USER_S_OBJS  		:= $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(USER_S_CPPS)))

USER_S_TARGET 	:= $(HOSTFS_ROOT)/bin/app_cycle

USER_A_CPPS 		:= user/app_wait.c user/user_lib.c

USER_A_OBJS  		:= $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(USER_A_CPPS)))

USER_A_TARGET 	:= $(HOSTFS_ROOT)/bin/app_wait

USER_U_CPPS 		:= user/app_singlepageheap.c user/user_lib.c

USER_U_OBJS  		:= $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(USER_U_CPPS)))

USER_U_TARGET 	:= $(HOSTFS_ROOT)/bin/app_singlepageheap

USER_Z_CPPS 		:= user/app_singlepageheap2.c user/user_lib.c

USER_Z_OBJS  		:= $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(USER_Z_CPPS)))

USER_Z_TARGET 	:= $(HOSTFS_ROOT)/bin/app_singlepageheap2

USER_X_CPPS 		:= user/app_semaphore.c user/user_lib.c

USER_X_OBJS  		:= $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(USER_X_CPPS)))

USER_X_TARGET 	:= $(HOSTFS_ROOT)/bin/app_semaphore

USER_Y_CPPS 		:= user/app_semaphore2.c user/user_lib.c

USER_Y_OBJS  		:= $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(USER_Y_CPPS)))

USER_Y_TARGET 	:= $(HOSTFS_ROOT)/bin/app_semaphore2

USER_T_CPPS 		:= user/app_yield.c user/user_lib.c

USER_T_OBJS  		:= $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(USER_T_CPPS)))

USER_T_TARGET 	:= $(HOSTFS_ROOT)/bin/app_yield

USER_M_CPPS 		:= user/app_link.c user/user_lib.c

USER_M_OBJS  		:= $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(USER_M_CPPS)))

USER_M_TARGET 	:= $(HOSTFS_ROOT)/bin/app_link

USER_W_CPPS 		:= user/app_longloop.c user/user_lib.c

USER_W_OBJS  		:= $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(USER_W_CPPS)))

USER_W_TARGET 	:= $(HOSTFS_ROOT)/bin/app_loogloop

USER_COW_CPPS 		:= user/app_cow.c user/user_lib.c

USER_COW_OBJS  		:= $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(USER_COW_CPPS)))

USER_COW_TARGET 	:= $(HOSTFS_ROOT)/bin/app_cow

#------------------------targets------------------------
$(OBJ_DIR):
	@-mkdir -p $(OBJ_DIR)	
	@-mkdir -p $(dir $(UTIL_OBJS))
	@-mkdir -p $(dir $(SPIKE_INF_OBJS))
	@-mkdir -p $(dir $(KERNEL_OBJS))
	@-mkdir -p $(dir $(USER_OBJS))
	@-mkdir -p $(dir $(USER_S_OBJS))
	@-mkdir -p $(dir $(USER_A_OBJS))
	@-mkdir -p $(dir $(USER_U_OBJS))
	@-mkdir -p $(dir $(USER_Z_OBJS))
	@-mkdir -p $(dir $(USER_X_OBJS))
	@-mkdir -p $(dir $(USER_Y_OBJS))
	@-mkdir -p $(dir $(USER_T_OBJS))
	@-mkdir -p $(dir $(USER_M_OBJS))
	@-mkdir -p $(dir $(USER_W_OBJS))
	@-mkdir -p $(dir $(USER_COW_OBJS))
	
$(OBJ_DIR)/%.o : %.c
	@echo "compiling" $<
	@$(COMPILE) -c $< -o $@

$(OBJ_DIR)/%.o : %.S
	@echo "compiling" $<
	@$(COMPILE) -c $< -o $@

$(UTIL_LIB): $(OBJ_DIR) $(UTIL_OBJS)
	@echo "linking " $@	...	
	@$(AR) -rcs $@ $(UTIL_OBJS) 
	@echo "Util lib has been build into" \"$@\"
	
$(SPIKE_INF_LIB): $(OBJ_DIR) $(UTIL_OBJS) $(SPIKE_INF_OBJS)
	@echo "linking " $@	...	
	@$(AR) -rcs $@ $(SPIKE_INF_OBJS) $(UTIL_OBJS)
	@echo "Spike lib has been build into" \"$@\"

$(KERNEL_TARGET): $(OBJ_DIR) $(UTIL_LIB) $(SPIKE_INF_LIB) $(KERNEL_OBJS) $(KERNEL_LDS)
	@echo "linking" $@ ...
	@$(COMPILE) $(KERNEL_OBJS) $(UTIL_LIB) $(SPIKE_INF_LIB) -o $@ -T $(KERNEL_LDS)
	@echo "PKE core has been built into" \"$@\"

$(USER_TARGET): $(OBJ_DIR) $(UTIL_LIB) $(USER_OBJS)
	@echo "linking" $@	...	
	-@mkdir -p $(HOSTFS_ROOT)/bin
	@$(COMPILE) --entry=main $(USER_OBJS) $(UTIL_LIB) -o $@
	@echo "User app has been built into" \"$@\"
	@cp $@ $(OBJ_DIR)

$(USER_S_TARGET): $(OBJ_DIR) $(UTIL_LIB) $(USER_S_OBJS)
	@echo "linking" $@	...	
	-@mkdir -p $(HOSTFS_ROOT)/bin
	@$(COMPILE) --entry=main $(USER_S_OBJS) $(UTIL_LIB) -o $@
	@echo "User app has been built into" \"$@\"

$(USER_A_TARGET): $(OBJ_DIR) $(UTIL_LIB) $(USER_A_OBJS)
	@echo "linking" $@	...	
	-@mkdir -p $(HOSTFS_ROOT)/bin
	@$(COMPILE) --entry=main $(USER_A_OBJS) $(UTIL_LIB) -o $@
	@echo "User app has been built into" \"$@\"

$(USER_U_TARGET): $(OBJ_DIR) $(UTIL_LIB) $(USER_U_OBJS)
	@echo "linking" $@	...	
	-@mkdir -p $(HOSTFS_ROOT)/bin
	@$(COMPILE) --entry=main $(USER_U_OBJS) $(UTIL_LIB) -o $@
	@echo "User app has been built into" \"$@\"

$(USER_Z_TARGET): $(OBJ_DIR) $(UTIL_LIB) $(USER_Z_OBJS)
	@echo "linking" $@	...	
	-@mkdir -p $(HOSTFS_ROOT)/bin
	@$(COMPILE) --entry=main $(USER_Z_OBJS) $(UTIL_LIB) -o $@
	@echo "User app has been built into" \"$@\"

$(USER_X_TARGET): $(OBJ_DIR) $(UTIL_LIB) $(USER_X_OBJS)
	@echo "linking" $@	...	
	-@mkdir -p $(HOSTFS_ROOT)/bin
	@$(COMPILE) --entry=main $(USER_X_OBJS) $(UTIL_LIB) -o $@
	@echo "User app has been built into" \"$@\"

$(USER_Y_TARGET): $(OBJ_DIR) $(UTIL_LIB) $(USER_Y_OBJS)
	@echo "linking" $@	...	
	-@mkdir -p $(HOSTFS_ROOT)/bin
	@$(COMPILE) --entry=main $(USER_Y_OBJS) $(UTIL_LIB) -o $@
	@echo "User app has been built into" \"$@\"

$(USER_T_TARGET): $(OBJ_DIR) $(UTIL_LIB) $(USER_T_OBJS)
	@echo "linking" $@	...	
	-@mkdir -p $(HOSTFS_ROOT)/bin
	@$(COMPILE) --entry=main $(USER_T_OBJS) $(UTIL_LIB) -o $@
	@echo "User app has been built into" \"$@\"

$(USER_M_TARGET): $(OBJ_DIR) $(UTIL_LIB) $(USER_M_OBJS)
	@echo "linking" $@	...	
	-@mkdir -p $(HOSTFS_ROOT)/bin
	@$(COMPILE) --entry=main $(USER_M_OBJS) $(UTIL_LIB) -o $@
	@echo "User app has been built into" \"$@\"

$(USER_W_TARGET): $(OBJ_DIR) $(UTIL_LIB) $(USER_W_OBJS)
	@echo "linking" $@	...	
	-@mkdir -p $(HOSTFS_ROOT)/bin
	@$(COMPILE) --entry=main $(USER_W_OBJS) $(UTIL_LIB) -o $@
	@echo "User app has been built into" \"$@\"

$(USER_COW_TARGET): $(OBJ_DIR) $(UTIL_LIB) $(USER_COW_OBJS)
	@echo "linking" $@	...	
	-@mkdir -p $(HOSTFS_ROOT)/bin
	@$(COMPILE) --entry=main $(USER_COW_OBJS) $(UTIL_LIB) -o $@
	@echo "User app has been built into" \"$@\"

-include $(wildcard $(OBJ_DIR)/*/*.d)
-include $(wildcard $(OBJ_DIR)/*/*/*.d)

.DEFAULT_GOAL := $(all)

all: $(KERNEL_TARGET) $(USER_TARGET) $(USER_S_TARGET) $(USER_A_TARGET) $(USER_U_TARGET) $(USER_Z_TARGET) $(USER_X_TARGET) $(USER_Y_TARGET) $(USER_T_TARGET) $(USER_M_TARGET) $(USER_W_TARGET) $(USER_COW_TARGET)   
.PHONY:all

run: $(KERNEL_TARGET) $(USER_TARGET) $(USER_S_TARGET) $(USER_A_TARGET) $(USER_U_TARGET) $(USER_Z_TARGET) $(USER_X_TARGET) $(USER_Y_TARGET) $(USER_T_TARGET) $(USER_M_TARGET) $(USER_W_TARGET) $(USER_COW_TARGET)
	@echo "********************HUST PKE********************"
	spike $(KERNEL_TARGET) /bin/app_shell

# need openocd!
gdb:$(KERNEL_TARGET) $(USER_TARGET)
	spike --rbb-port=9824 -H $(KERNEL_TARGET) $(USER_TARGET) &
	@sleep 1
	openocd -f ./.spike.cfg &
	@sleep 1
	riscv64-unknown-elf-gdb -command=./.gdbinit

# clean gdb. need openocd!
gdb_clean:
	@-kill -9 $$(lsof -i:9824 -t)
	@-kill -9 $$(lsof -i:3333 -t)
	@sleep 1

objdump:
	riscv64-unknown-elf-objdump -d $(KERNEL_TARGET) > $(OBJ_DIR)/kernel_dump
	riscv64-unknown-elf-objdump -d $(USER_TARGET) > $(OBJ_DIR)/user_dump

cscope:
	find ./ -name "*.c" > cscope.files
	find ./ -name "*.h" >> cscope.files
	find ./ -name "*.S" >> cscope.files
	find ./ -name "*.lds" >> cscope.files
	cscope -bqk

format:
	@python ./format.py ./

clean:
	rm -fr ${OBJ_DIR} ${HOSTFS_ROOT}/bin
