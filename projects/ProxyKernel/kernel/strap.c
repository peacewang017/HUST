/*
 * Utility functions for trap handling in Supervisor mode.
 */

#include "riscv.h"
#include "process.h"
#include "strap.h"
#include "syscall.h"
#include "pmm.h"
#include "vmm.h"
#include "sched.h"
#include "util/functions.h"

#include "spike_interface/spike_utils.h"

//
// handling the syscalls. will call do_syscall() defined in kernel/syscall.c
//
static void handle_syscall(trapframe *tf)
{
	// tf->epc points to the address that our computer will jump to after the trap handling.
	// for a syscall, we should return to the NEXT instruction after its handling.
	// in RV64G, each instruction occupies exactly 32 bits (i.e., 4 Bytes)
	tf->epc += 4;
	// TODO (lab1_1): remove the panic call below, and call do_syscall (defined in
	// kernel/syscall.c) to conduct real operations of the kernel side for a syscall.
	// IMPORTANT: return value should be returned to user app, or else, you will encounter
	// problems in later experiments!
	tf->regs.a0 = do_syscall(tf->regs.a0, tf->regs.a1, tf->regs.a2,
				 tf->regs.a3, tf->regs.a4, tf->regs.a5,
				 tf->regs.a6, tf->regs.a7);
}

//
// global variable that store the recorded "ticks". added @lab1_3
static uint64 g_ticks = 0;

//
// added @lab1_3
//
// 在这里，我们只让 1 号核打印
void handle_mtimer_trap()
{
	int hartid = read_tp();

	if (hartid == 1) {
		if (g_ticks == 1) {
			sprint("timer: 1 min\n");
		} else {
			sprint("timer: %d mins\n", (g_ticks + 1) / 2);
		}
	}
	// TODO (lab1_3): increase g_ticks to record this "tick", and then clear the "SIP"
	// field in sip register.
	// hint: use write_csr to disable the SIP_SSIP bit in sip.
	g_ticks++;
	write_csr(sip, 0);
}

//
// the page fault handler. added @lab2_3. parameters:
// sepc: the pc when fault happens;
// stval: the virtual address that causes pagefault when being accessed.
//
void handle_user_page_fault(uint64 mcause, uint64 sepc, uint64 stval)
{
	int hartid = (int)read_tp();
	sprint("handle_page_fault: %lx\n", stval);
	switch (mcause) {
	case CAUSE_STORE_PAGE_FAULT:
		// TODO (lab2_3): implement the operations that solve the page fault to
		// dynamically increase application stack.
		// hint: first allocate a new physical page, and then, maps the new page to the
		// virtual address that causes the page fault.
		map_pages((pagetable_t)current[hartid]->pagetable,
			  ROUNDDOWN(stval, PGSIZE), PGSIZE,
			  (uint64)alloc_page(),
			  prot_to_type(PROT_READ | PROT_WRITE, 1));
		break;
	default:
		panic("unknown page fault.\n");
		break;
	}
}

//
// implements round-robin scheduling. added @lab3_3
//
void rrsched()
{
	int hartid = (int)read_tp();
	// TODO (lab3_3): implements round-robin scheduling.
	// hint: increase the tick_count member of current process by one, if it is bigger than
	// TIME_SLICE_LEN (means it has consumed its time slice), change its status into READY,
	// place it in the rear of ready queue, and finally schedule next process to run.
	if (current[hartid]->tick_count + 1 >= TIME_SLICE_LEN) {
		current[hartid]->tick_count = 0;
		current[hartid]->status = READY;
		insert_to_ready_queue(current[hartid]);
		schedule();
	} else {
		current[hartid]->tick_count++;
	}
	return;
}

//
// kernel/smode_trap.S will pass control to smode_trap_handler, when a trap happens
// in S-mode.
//
void smode_trap_handler(void)
{
	int hartid = (int)read_tp();
	// make sure we are in User mode before entering the trap handling.
	// we will consider other previous case in lab1_3 (interrupt).
	if ((read_csr(sstatus) & SSTATUS_SPP) != 0)
		panic("usertrap: not from user mode");

	assert(current[hartid]);
	// save user process counter.
	current[hartid]->trapframe->epc = read_csr(sepc);

	// if the cause of trap is syscall from user application.
	// read_csr() and CAUSE_USER_ECALL are macros defined in kernel/riscv.h
	uint64 cause = read_csr(scause);

	// use switch-case instead of if-else, as there are many cases since lab2_3.
	switch (cause) {
	case CAUSE_USER_ECALL:
		handle_syscall(current[hartid]->trapframe);
		break;
	case CAUSE_MTIMER_S_TRAP:
		handle_mtimer_trap();
		// invoke round-robin scheduler. added @lab3_3
		rrsched();
		break;
	case CAUSE_STORE_PAGE_FAULT:
	case CAUSE_LOAD_PAGE_FAULT:
		// the address of missing page is stored in stval
		// call handle_user_page_fault to process page faults
		handle_user_page_fault(cause, read_csr(sepc), read_csr(stval));
		break;
	default:
		sprint("smode_trap_handler(): unexpected scause %p\n",
		       read_csr(scause));
		sprint("            sepc=%p stval=%p\n", read_csr(sepc),
		       read_csr(stval));
		panic("unexpected exception happened.\n");
		break;
	}

	// continue (come back to) the execution of current process.
	switch_to(current[hartid]);
}