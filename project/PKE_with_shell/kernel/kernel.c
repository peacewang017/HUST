/*
 * Supervisor-mode startup codes
 */

#include "riscv.h"
#include "string.h"
#include "elf.h"
#include "process.h"
#include "pmm.h"
#include "vmm.h"
#include "sched.h"
#include "memlayout.h"
#include "spike_interface/spike_utils.h"
#include "util/types.h"
#include "vfs.h"
#include "rfs.h"
#include "ramdev.h"
#include "kernel/sync_utils.h"

//
// trap_sec_start points to the beginning of S-mode trap segment (i.e., the entry point of
// S-mode trap vector). added @lab2_1
//
extern char trap_sec_start[];

//
// turn on paging. added @lab2_1
//
void enable_paging()
{
	// write the pointer to kernel page (table) directory into the CSR of "satp".
	write_csr(satp, MAKE_SATP(g_kernel_pagetable));

	// refresh tlb to invalidate its content.
	flush_tlb();
}

typedef union {
	uint64 buf[MAX_CMDLINE_ARGS];
	char *argv[MAX_CMDLINE_ARGS];
} arg_buf;

//
// returns the number (should be 1) of string(s) after PKE kernel in command line.
// and store the string(s) in arg_bug_msg.
//
static size_t parse_args(arg_buf *arg_bug_msg)
{
	// HTIFSYS_getmainvars frontend call reads command arguments to (input) *arg_bug_msg
	long r = frontend_syscall(HTIFSYS_getmainvars, (uint64)arg_bug_msg,
				  sizeof(*arg_bug_msg), 0, 0, 0, 0, 0);
	kassert(r == 0);

	size_t pk_argc = arg_bug_msg->buf[0];
	uint64 *pk_argv = &arg_bug_msg->buf[1];

	int arg =
		1; // skip the PKE OS kernel string, leave behind only the application name
	for (size_t i = 0; arg + i < pk_argc; i++)
		arg_bug_msg->argv[i] = (char *)(uintptr_t)pk_argv[arg + i];

	//returns the number of strings after PKE kernel in command line
	return pk_argc - arg;
}

//
// load the elf, and construct a "process" (with only a trapframe).
// load_bincode_from_host_elf is defined in elf.c
//
// 用来载入第一个用户程序
process *load_user_program()
{
	int hartid = read_tp();
	process *proc;

	proc = alloc_process();
	sprint("Hartid %d: User application is loading.\n", hartid);

	arg_buf arg_bug_msg;

	// retrieve command line arguements
	size_t argc = parse_args(&arg_bug_msg);
	if (!argc)
		panic("You need to specify the application program!\n");

	int user_app_argc = argc - 1;
	char **user_app_argv;
	user_app_argv = (char **)(arg_bug_msg.argv + 1);

	load_bincode_from_vfs_elf(proc, arg_bug_msg.argv[0], user_app_argc,
				  user_app_argv);
	return proc;
}

process *load_cycle_program()
{
	int hartid = read_tp();
	process *proc;

	proc = alloc_process();
	sprint("User application is loading.\n");

	load_bincode_from_vfs_elf(proc, "/bin/app_cycle", 0, NULL);
	return proc;
}

//
// s_start: S-mode entry point of riscv-pke OS kernel.
//
int s_start(void)
{
	int hartid = (int)read_tp();
	sprint("Hartid = %d: Enter supervisor mode...\n", hartid);
	// in the beginning, we use Bare mode (direct) memory mapping as in lab1.
	// but now, we are going to switch to the paging mode @lab2_1.
	// note, the code still works in Bare mode when calling pmm_init() and kern_vm_init().
	write_csr(satp, 0);

	static volatile int sync_counter = 0;

	g_ufree_page[hartid] = USER_FREE_ADDRESS_START;

	if (hartid == 0) {
		pmm_init();
		kern_vm_init();
	}

	sync_barrier(&sync_counter, NCPU);

	// now, switch to paging mode by turning on paging (SV39)
	enable_paging();

	// the code now formally works in paging mode, meaning the page table is now in use.
	sprint("kernel page table is on \n");

	// added @lab3_1
	init_proc_pool();

	static volatile int sync_counter_2 = 0;

	// init file system, added @lab4_1
	if (hartid == 0) {
		fs_init();
	}

	sync_barrier(&sync_counter_2, NCPU);

	sprint("Switch to user mode...\n");
	// the application code (elf) is first loaded into memory, and then put into execution
	// added @lab3_1
	if (hartid == 0) {
		insert_to_ready_queue(load_user_program());
	} else {
		insert_to_ready_queue(load_cycle_program());
	}

	schedule();

	// we should never reach here.
	return 0;
}
