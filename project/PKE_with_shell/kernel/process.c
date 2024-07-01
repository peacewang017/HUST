/*
 * Utility functions for process management. 
 *
 * Note: in Lab1, only one process (i.e., our user application) exists. Therefore, 
 * PKE OS at this stage will set "current" to the loaded user application, and also
 * switch to the old "current" process after trap handling.
 */

#include "riscv.h"
#include "strap.h"
#include "config.h"
#include "process.h"
#include "elf.h"
#include "string.h"
#include "vmm.h"
#include "pmm.h"
#include "memlayout.h"
#include "sched.h"
#include "spike_interface/spike_utils.h"
#include "util/functions.h"

//Two functions defined in kernel/usertrap.S
extern char smode_trap_vector[];
extern void return_to_user(trapframe *, uint64 satp);

// trap_sec_start points to the beginning of S-mode trap segment (i.e., the entry point
// of S-mode trap vector).
extern char trap_sec_start[];

// process pool. added @lab3_1
process procs[NCPU][NPROC];

// current points to the currently running user-mode application.
process *current[NCPU] = { NULL };

// 初始化
uint64 g_ufree_page[NCPU];

//
// switch to a user-mode process
//
void switch_to(process *proc)
{
	int hartid = (int)read_tp();
	assert(proc);
	current[hartid] = proc;

	// write the smode_trap_vector (64-bit func. address) defined in kernel/strap_vector.S
	// to the stvec privilege register, such that trap handler pointed by smode_trap_vector
	// will be triggered when an interrupt occurs in S mode.
	write_csr(stvec, (uint64)smode_trap_vector);

	// set up trapframe values (in process structure) that smode_trap_vector will need when
	// the process next re-enters the kernel.
	proc->trapframe->kernel_sp = proc->kstack; // process's kernel stack
	proc->trapframe->kernel_satp = read_csr(satp); // kernel page table
	proc->trapframe->kernel_trap = (uint64)smode_trap_handler;
	proc->trapframe->regs.tp = hartid;

	// SSTATUS_SPP and SSTATUS_SPIE are defined in kernel/riscv.h
	// set S Previous Privilege mode (the SSTATUS_SPP bit in sstatus register) to User mode.
	unsigned long x = read_csr(sstatus);
	x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
	x |= SSTATUS_SPIE; // enable interrupts in user mode

	// write x back to 'sstatus' register to enable interrupts, and sret destination mode.
	write_csr(sstatus, x);

	// set S Exception Program Counter (sepc register) to the elf entry pc.
	write_csr(sepc, proc->trapframe->epc);

	// make user page table. macro MAKE_SATP is defined in kernel/riscv.h. added @lab2_1
	uint64 user_satp = MAKE_SATP(proc->pagetable);

	// return_to_user() is defined in kernel/strap_vector.S. switch to user mode with sret.
	// note, return_to_user takes two parameters @ and after lab2_1.
	// print_proc_info(current);
	return_to_user(proc->trapframe, user_satp);
}

//
// initialize process pool (the procs[] array). added @lab3_1
//
void init_proc_pool()
{
	int hartid = (int)read_tp();
	memset(procs[hartid], 0, sizeof(process) * NPROC);

	for (int i = 0; i < NPROC; ++i) {
		procs[hartid][i].status = FREE;
		procs[hartid][i].pid = i;
		procs[hartid][i].parent = NULL;
		procs[hartid][i].waiting_pid = -1;
		procs[hartid][i].queue_next =
			NULL; // 在 ready 和 blocked 队列中用
		procs[hartid][i].sem_index = -1;
		procs[hartid][i].num_malloc = 0;
		procs[hartid][i].num_page = 0;
	}
}

//
// allocate an empty process, init its vm space. returns the pointer to
// process strcuture. added @lab3_1
//
process *alloc_process()
{
	int hartid = (int)read_tp();
	// locate the first usable process structure
	int i;

	for (i = 0; i < NPROC; i++)
		if (procs[hartid][i].status == FREE)
			break;

	if (i >= NPROC) {
		panic("cannot find any free process structure.\n");
		return 0;
	}

	// init proc[i]'s vm space
	procs[hartid][i].trapframe =
		(trapframe *)alloc_page(); //trapframe, used to save context
	memset(procs[hartid][i].trapframe, 0, sizeof(trapframe));

	// page directory
	procs[hartid][i].pagetable = (pagetable_t)alloc_page();
	memset((void *)procs[hartid][i].pagetable, 0, PGSIZE);

	procs[hartid][i].kstack =
		(uint64)alloc_page() + PGSIZE; //user kernel stack top
	uint64 user_stack =
		(uint64)alloc_page(); //phisical address of user stack bottom
	procs[hartid][i].trapframe->regs.sp =
		USER_STACK_TOP; //virtual address of user stack top
	procs[hartid][i].trapframe->regs.tp =
		hartid; // 由哪个核载入，由哪个核执行

	// allocates a page to record memory regions (segments)
	procs[hartid][i].mapped_info = (mapped_region *)alloc_page();
	memset(procs[hartid][i].mapped_info, 0, PGSIZE);

	// map user stack in userspace
	user_vm_map((pagetable_t)procs[hartid][i].pagetable,
		    USER_STACK_TOP - PGSIZE, PGSIZE, user_stack,
		    prot_to_type(PROT_WRITE | PROT_READ, 1));
	procs[hartid][i].mapped_info[STACK_SEGMENT].va =
		USER_STACK_TOP - PGSIZE;
	procs[hartid][i].mapped_info[STACK_SEGMENT].npages = 1;
	procs[hartid][i].mapped_info[STACK_SEGMENT].seg_type = STACK_SEGMENT;

	// map trapframe in user space (direct mapping as in kernel space).
	user_vm_map((pagetable_t)procs[hartid][i].pagetable,
		    (uint64)procs[hartid][i].trapframe, PGSIZE,
		    (uint64)procs[hartid][i].trapframe,
		    prot_to_type(PROT_WRITE | PROT_READ, 0));
	procs[hartid][i].mapped_info[CONTEXT_SEGMENT].va =
		(uint64)procs[hartid][i].trapframe;
	procs[hartid][i].mapped_info[CONTEXT_SEGMENT].npages = 1;
	procs[hartid][i].mapped_info[CONTEXT_SEGMENT].seg_type =
		CONTEXT_SEGMENT;

	// map S-mode trap vector section in user space (direct mapping as in kernel space)
	// we assume that the size of usertrap.S is smaller than a page.
	user_vm_map((pagetable_t)procs[hartid][i].pagetable,
		    (uint64)trap_sec_start, PGSIZE, (uint64)trap_sec_start,
		    prot_to_type(PROT_READ | PROT_EXEC, 0));
	procs[hartid][i].mapped_info[SYSTEM_SEGMENT].va =
		(uint64)trap_sec_start;
	procs[hartid][i].mapped_info[SYSTEM_SEGMENT].npages = 1;
	procs[hartid][i].mapped_info[SYSTEM_SEGMENT].seg_type = SYSTEM_SEGMENT;

	sprint("in alloc_proc. user frame 0x%lx, user stack 0x%lx, user kstack 0x%lx \n",
	       procs[hartid][i].trapframe, procs[hartid][i].trapframe->regs.sp,
	       procs[hartid][i].kstack);

	// initialize the process's heap manager
	procs[hartid][i].user_heap.heap_top = USER_FREE_ADDRESS_START;
	procs[hartid][i].user_heap.heap_bottom = USER_FREE_ADDRESS_START;
	procs[hartid][i].user_heap.free_pages_count = 0;

	// map user heap in userspace
	procs[hartid][i].mapped_info[HEAP_SEGMENT].va = USER_FREE_ADDRESS_START;
	procs[hartid][i].mapped_info[HEAP_SEGMENT].npages =
		0; // no pages are mapped to heap yet.
	procs[hartid][i].mapped_info[HEAP_SEGMENT].seg_type = HEAP_SEGMENT;

	procs[hartid][i].total_mapped_region = 4;

	// initialize files_struct
	procs[hartid][i].pfiles = init_proc_file_management();
	sprint("in alloc_proc. build proc_file_management successfully.\n");

	// return after initialization.
	return &procs[hartid][i];
}

//
// reclaim a process. added @lab3_1
//
int free_process(process *proc)
{
	// we set the status to ZOMBIE, but cannot destruct its vm space immediately.
	// since proc can be current process, and its user kernel stack is currently in use!
	// but for proxy kernel, it (memory leaking) may NOT be a really serious issue,
	// as it is different from regular OS, which needs to run 7x24.
	proc->status = ZOMBIE;
	clear_malloc_dir(proc);
	clear_page_dir(proc);
	remove_from_blocked_queue(proc);
	remove_from_ready_queue(proc);
	return 0;
}

//
// implements fork syscal in kernel. added @lab3_1
// basic idea here is to first allocate an empty process (child), then duplicate the
// context and data segments of parent process to the child, and lastly, map other
// segments (code, system) of the parent to child. the stack segment remains unchanged
// for the child.
//
int do_fork(process *parent)
{
	int hartid = read_tp();
	sprint("Will fork a child from parent %d.\n", parent->pid);
	process *child = alloc_process();

	for (int i = 0; i < parent->total_mapped_region; i++) {
		// browse parent's vm space, and copy its trapframe and data segments,
		// map its code segment.
		switch (parent->mapped_info[i].seg_type) {
		case CONTEXT_SEGMENT: {
			*child->trapframe = *parent->trapframe;
			break;
		}
		case STACK_SEGMENT: {
			memcpy((void *)lookup_pa(
				       child->pagetable,
				       child->mapped_info[STACK_SEGMENT].va),
			       (void *)lookup_pa(parent->pagetable,
						 parent->mapped_info[i].va),
			       PGSIZE);
			break;
		}
		case HEAP_SEGMENT: {
			int free_block_filter[MAX_HEAP_PAGES];
			memset(free_block_filter, 0, MAX_HEAP_PAGES);
			uint64 heap_bottom = parent->user_heap.heap_bottom;
			for (int i = 0; i < parent->user_heap.free_pages_count;
			     i++) {
				int index = (parent->user_heap
						     .free_pages_address[i] -
					     heap_bottom) /
					    PGSIZE;
				free_block_filter[index] = 1;
			}

			// copy and map the heap blocks
			for (uint64 heap_block =
				     current[hartid]->user_heap.heap_bottom;
			     heap_block < current[hartid]->user_heap.heap_top;
			     heap_block += PGSIZE) {
				if (free_block_filter[(heap_block - heap_bottom) /
						      PGSIZE]) // skip free blocks
					continue;

				// COW: just map (not cp) heap here
				uint64 child_pa = lookup_pa(parent->pagetable,
							    heap_block);
				user_vm_map((pagetable_t)child->pagetable,
					    heap_block, PGSIZE, child_pa,
					    prot_to_type(PROT_READ | PROT_COW,
							 1));
			}
			child->mapped_info[HEAP_SEGMENT].npages =
				parent->mapped_info[HEAP_SEGMENT].npages;

			// copy the heap manager from parent to child
			memcpy((void *)&child->user_heap,
			       (void *)&parent->user_heap,
			       sizeof(parent->user_heap));
			break;
		}
		case CODE_SEGMENT: {
			// TODO (lab3_1): implment the mapping of child code segment to parent's
			// code segment.
			// hint: the virtual address mapping of code segment is tracked in mapped_info
			// page of parent's process structure. use the information in mapped_info to
			// retrieve the virtual to physical mapping of code segment.
			// after having the mapping information, just map the corresponding virtual
			// address region of child to the physical pages that actually store the code
			// segment of parent process.
			// DO NOT COPY THE PHYSICAL PAGES, JUST MAP THEM.
			uint64 pa =
				lookup_pa(parent->pagetable,
					  parent->mapped_info[CODE_SEGMENT].va);
			user_vm_map(child->pagetable,
				    parent->mapped_info[CODE_SEGMENT].va,
				    parent->mapped_info[CODE_SEGMENT].npages *
					    PGSIZE,
				    pa, prot_to_type(PROT_EXEC | PROT_READ, 1));
			sprint("do_fork map code segment at pa:%lx of parent to child at va:%lx.\n",
			       pa, parent->mapped_info[CODE_SEGMENT].va);
			// after mapping, register the vm region (do not delete codes below!)
			child->mapped_info[child->total_mapped_region].va =
				parent->mapped_info[i].va;
			child->mapped_info[child->total_mapped_region].npages =
				parent->mapped_info[i].npages;
			child->mapped_info[child->total_mapped_region].seg_type =
				CODE_SEGMENT;
			child->total_mapped_region++;
			break;
		}
		case DATA_SEGMENT: {
			for (int j = 0; j < parent->mapped_info[i].npages;
			     j++) {
				uint64 addr = lookup_pa(
					parent->pagetable,
					parent->mapped_info[i].va + j * PGSIZE);
				char *newaddr = alloc_page();
				memcpy(newaddr, (void *)addr, PGSIZE);
				map_pages(child->pagetable,
					  parent->mapped_info[i].va +
						  j * PGSIZE,
					  PGSIZE, (uint64)newaddr,
					  prot_to_type(PROT_WRITE | PROT_READ,
						       1));
			}

			// after mapping, register the vm region (do not delete codes below!)
			child->mapped_info[child->total_mapped_region].va =
				parent->mapped_info[i].va;
			child->mapped_info[child->total_mapped_region].npages =
				parent->mapped_info[i].npages;
			child->mapped_info[child->total_mapped_region].seg_type =
				DATA_SEGMENT;
			child->total_mapped_region++;
			break;
		}
		}
	}

	child->status = READY;
	child->trapframe->regs.a0 = 0;
	child->parent = parent;
	insert_to_ready_queue(child);

	return child->pid;
}

int do_exec(char *path, char *agmt)
{
	int hartid = (int)read_tp();
	clear_process(current[hartid]);
	char *argv[1];
	argv[0] = agmt;
	load_bincode_from_vfs_elf(current[hartid], path, 1, argv);
	schedule();
	return 0;
}

int do_wait(int pid)
{
	int hartid = (int)read_tp();
	int child_pid = -1;

	// case pid == -1
	if (pid == -1) {
		for (int i = 0; i < NPROC; i++) {
			if (procs[hartid][i].parent == current[hartid] &&
			    procs[hartid][i].status != FREE) {
				if (procs[hartid][i].status == ZOMBIE) {
					procs[hartid][i].status = FREE;
					return i;
				} else {
					child_pid = i;
					break;
				}
			}
		}

		if (child_pid == -1) {
			return -1;
		}

		// case pid = series number
	} else if (pid >= 0 && pid < NPROC) {
		if (procs[hartid][pid].parent == current[hartid] &&
		    procs[hartid][pid].status != FREE) {
			if (procs[hartid][pid].status == ZOMBIE) {
				procs[hartid][pid].status = FREE;
				return pid;
			} else {
				child_pid = pid;
			}
		} else {
			return -1;
		}

		if (child_pid == -1) {
			return -1;
		}

		// case invalid pid
	} else {
		return -1;
	}

	// handle with waiting child proc
	current[hartid]->waiting_pid = child_pid;
	from_ready_to_blocked(current[hartid]);
	// sprint("wait: proc %d from ready to blocked queue\n", current[hartid]->pid);
	schedule();
	return child_pid;
}

void print_proc_info(process *p)
{
	sprint("\n");
	sprint("-------------proc_info-------------\n");
	sprint("Pid: %d\n", p->pid);
	sprint("Waiting pid: %d\n", p->waiting_pid);
	sprint("Trapframe epc: 0x%lx, satp: 0x%lx, trap: 0x%lx\n",
	       p->trapframe->epc, p->trapframe->kernel_sp,
	       p->trapframe->kernel_trap);
	sprint("-----------------------------------\n");
	sprint("\n");
}

void clear_process(process *proc)
{
	// init proc[i]'s vm space
	proc->trapframe =
		(trapframe *)alloc_page(); //trapframe, used to save context
	memset(proc->trapframe, 0, sizeof(trapframe));

	// page directory
	proc->pagetable = (pagetable_t)alloc_page();
	memset((void *)proc->pagetable, 0, PGSIZE);

	proc->kstack = (uint64)alloc_page() + PGSIZE; //user kernel stack top
	uint64 user_stack =
		(uint64)alloc_page(); //phisical address of user stack bottom
	proc->trapframe->regs.sp =
		USER_STACK_TOP; //virtual address of user stack top

	// allocates a page to record memory regions (segments)
	proc->mapped_info = (mapped_region *)alloc_page();
	memset(proc->mapped_info, 0, PGSIZE);

	// map user stack in userspace
	user_vm_map((pagetable_t)proc->pagetable, USER_STACK_TOP - PGSIZE,
		    PGSIZE, user_stack,
		    prot_to_type(PROT_WRITE | PROT_READ, 1));
	proc->mapped_info[STACK_SEGMENT].va = USER_STACK_TOP - PGSIZE;
	proc->mapped_info[STACK_SEGMENT].npages = 1;
	proc->mapped_info[STACK_SEGMENT].seg_type = STACK_SEGMENT;

	// map trapframe in user space (direct mapping as in kernel space).
	user_vm_map((pagetable_t)proc->pagetable, (uint64)proc->trapframe,
		    PGSIZE, (uint64)proc->trapframe,
		    prot_to_type(PROT_WRITE | PROT_READ, 0));
	proc->mapped_info[CONTEXT_SEGMENT].va = (uint64)proc->trapframe;
	proc->mapped_info[CONTEXT_SEGMENT].npages = 1;
	proc->mapped_info[CONTEXT_SEGMENT].seg_type = CONTEXT_SEGMENT;

	// map S-mode trap vector section in user space (direct mapping as in kernel space)
	// we assume that the size of usertrap.S is smaller than a page.
	user_vm_map((pagetable_t)proc->pagetable, (uint64)trap_sec_start,
		    PGSIZE, (uint64)trap_sec_start,
		    prot_to_type(PROT_READ | PROT_EXEC, 0));
	proc->mapped_info[SYSTEM_SEGMENT].va = (uint64)trap_sec_start;
	proc->mapped_info[SYSTEM_SEGMENT].npages = 1;
	proc->mapped_info[SYSTEM_SEGMENT].seg_type = SYSTEM_SEGMENT;

	// initialize the process's heap manager
	proc->user_heap.heap_top = USER_FREE_ADDRESS_START;
	proc->user_heap.heap_bottom = USER_FREE_ADDRESS_START;
	proc->user_heap.free_pages_count = 0;

	// map user heap in userspace
	proc->mapped_info[HEAP_SEGMENT].va = USER_FREE_ADDRESS_START;
	proc->mapped_info[HEAP_SEGMENT].npages =
		0; // no pages are mapped to heap yet.
	proc->mapped_info[HEAP_SEGMENT].seg_type = HEAP_SEGMENT;

	proc->total_mapped_region = 4;

	// initialize files_struct
	proc->pfiles = init_proc_file_management();
	sprint("in alloc_proc. build proc_file_management successfully.\n");

	proc->sem_index = -1;
	proc->waiting_pid = -1;
	clear_malloc_dir(proc);
	clear_page_dir(proc);
}

// 返回虚拟地址
uint64 do_better_malloc(int n)
{
	int hartid = read_tp();
	// 在没有已分配页面时
	if (current[hartid]->num_page == 0) {
		// 需要num_page页,alloc后map,加入表中
		int page_need = ROUNDUP(n, PGSIZE) / PGSIZE;
		if (alloc_n_page(page_need) == 0) {
			panic("page alloc error\n");
		}

		// 添加malloc表
		uint64 va_start = current[hartid]->page_dir[0].va_page;
		if (current[hartid]->num_malloc + 1 > MAX_MALLOC_IN_HEAP) {
			panic("malloc error\n");
		}
		add_to_malloc_dir(va_start, va_start + n);
		// print_malloc_dir();
		return va_start;
	}

	// 至少有一个已经分配的页，也就代表至少有一个未free的malloc(free时会检查并进行页回收)
	// 并且在free时，只free page_dir中开头和末尾的两项，保证va_page_start -> va_page_end是连续可用的空间
	uint64 va_page_start = current[hartid]->page_dir[0].va_page;
	uint64 va_page_end = current[hartid]
				     ->page_dir[current[hartid]->num_page - 1]
				     .va_page +
			     PGSIZE;

	// 首先检查头是否可用
	int front_offset =
		current[hartid]->malloc_dir[0].va_start - va_page_start;
	if (front_offset >= n) {
		add_to_malloc_dir(va_page_start, va_page_start + n);
		// print_malloc_dir();
		return va_page_start;
	}

	// 检查空隙是否可用
	if (current[hartid]->num_malloc >= 2) {
		for (int i = 0; i < current[hartid]->num_malloc - 1; i++) {
			int gap = current[hartid]->malloc_dir[i + 1].va_start -
				  current[hartid]->malloc_dir[i].va_end;
			if (gap >= n) {
				uint64 new_va_start =
					current[hartid]->malloc_dir[i].va_end;
				add_to_malloc_dir(new_va_start,
						  new_va_start + n);
				// print_malloc_dir();
				return new_va_start;
			}
		}
	}

	// 使用末尾，先填入malloc表，再去alloc page并map
	uint64 new_va_start =
		current[hartid]
			->malloc_dir[current[hartid]->num_malloc - 1]
			.va_end;
	add_to_malloc_dir(new_va_start, new_va_start + n);
	// print_malloc_dir();

	int rear_offset = va_page_end - new_va_start;
	if (rear_offset < n) {
		int page_need = ROUNDUP((n - rear_offset), PGSIZE) / PGSIZE;
		if (alloc_n_page(page_need) == 0) {
			panic("page alloc error\n");
		}
	}
	return new_va_start;
}

int alloc_n_page(int n)
{
	int hartid = read_tp();
	if (current[hartid]->num_page + n > MAX_HEAP_PAGES) {
		return 0;
	}
	for (int i = 0; i < n; i++) {
		uint64 pa = (uint64)alloc_page();
		user_vm_map((pagetable_t)current[hartid]->pagetable,
			    g_ufree_page[hartid], PGSIZE, pa,
			    prot_to_type(PROT_WRITE | PROT_READ, 1));
		add_to_page_dir(g_ufree_page[hartid], pa);
		g_ufree_page[hartid] += PGSIZE;
	}
	return 1;
}

// 排序保证单增
void add_to_page_dir(uint64 va_page, uint64 pa_page)
{
	int hartid = read_tp();
	current[hartid]->page_dir[current[hartid]->num_page].va_page = va_page;
	current[hartid]->page_dir[current[hartid]->num_page].pa_page = pa_page;
	current[hartid]->num_page++;
	sort_page_dir();
}

void sort_page_dir()
{
	int hartid = read_tp();
	page_dentry temp;
	int n = current[hartid]->num_page;
	for (int i = 0; i < n - 1; i++) {
		for (int j = 0; j < n - i - 1; j++) {
			if (current[hartid]->page_dir[j].va_page >
			    current[hartid]->page_dir[j + 1].va_page) {
				temp = current[hartid]->page_dir[j];
				current[hartid]->page_dir[j] =
					current[hartid]->page_dir[j + 1];
				current[hartid]->page_dir[j + 1] = temp;
			}
		}
	}
}

void add_to_malloc_dir(uint64 va_start, uint64 va_end)
{
	int hartid = read_tp();
	current[hartid]->malloc_dir[current[hartid]->num_malloc].va_start =
		va_start;
	current[hartid]->malloc_dir[current[hartid]->num_malloc].va_end =
		va_end;
	current[hartid]->num_malloc++;
	sort_malloc_dir();
}

void sort_malloc_dir()
{
	int hartid = read_tp();
	malloc_dentry temp;
	int n = current[hartid]->num_malloc;
	for (int i = 0; i < n - 1; i++) {
		for (int j = 0; j < n - i - 1; j++) {
			if (current[hartid]->malloc_dir[j].va_start >
			    current[hartid]->malloc_dir[j + 1].va_start) {
				temp = current[hartid]->malloc_dir[j];
				current[hartid]->malloc_dir[j] =
					current[hartid]->malloc_dir[j + 1];
				current[hartid]->malloc_dir[j + 1] = temp;
			}
		}
	}
}

// -1 means not found
int find_malloc_dir(uint64 va)
{
	int hartid = read_tp();
	for (int i = 0; i < current[hartid]->num_malloc; i++) {
		if (current[hartid]->malloc_dir[i].va_start == va) {
			return i;
		}
	}
	return -1;
}

void remove_from_malloc_dir(int index)
{
	int hartid = read_tp();
	for (int i = index; i < current[hartid]->num_malloc - 1; i++) {
		current[hartid]->malloc_dir[i] =
			current[hartid]->malloc_dir[i + 1];
	}
	current[hartid]->num_malloc--;
}

void do_better_free(uint64 va)
{
	int index;
	if ((index = find_malloc_dir(va)) == -1) {
		panic("free error\n");
	}
	remove_from_malloc_dir(index);

	// malloc 放生后，我们先不返回页面，只有在进程 exit 后，统一 free 所有 malloc 占用的 page 并清空两个 dir
}

void clear_page_dir(process *proc)
{
	int hartid = read_tp();
	for (int i = 0; i < proc->num_page; i++) {
		user_vm_unmap((pagetable_t)proc->pagetable,
			      proc->page_dir[i].va_page, PGSIZE, 0);
		g_ufree_page[hartid] -= PGSIZE;
	}
	proc->num_page = 0;
}

void clear_malloc_dir(process *proc)
{
	proc->num_malloc = 0;
}