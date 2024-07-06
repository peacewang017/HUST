#ifndef _PROC_H_
#define _PROC_H_

#include "riscv.h"
#include "proc_file.h"

typedef struct trapframe_t {
	// space to store context (all common registers)
	/* offset:0   */ riscv_regs regs;

	// process's "user kernel" stack
	/* offset:248 */ uint64 kernel_sp;
	// pointer to smode_trap_handler
	/* offset:256 */ uint64 kernel_trap;
	// saved user process counter
	/* offset:264 */ uint64 epc;

	// kernel page table. added @lab2_1
	/* offset:272 */ uint64 kernel_satp;
} trapframe;

// riscv-pke kernel supports at most 32 processes
#define NPROC 32
// maximum number of pages in a process's heap
#define MAX_HEAP_PAGES 32

// in better malloc
#define MAX_MALLOC_IN_HEAP 100

// 用来记录堆空间中已经alloc并map过的page,va->pa的map关系
typedef struct page_dentry {
	uint64 va_page;
	uint64 pa_page;
} page_dentry;

// 用来记录malloc的块的虚拟地址起点及终点
typedef struct malloc_dentry {
	uint64 va_start;
	uint64 va_end;
} malloc_dentry;

// possible status of a process
enum proc_status {
	FREE, // unused state
	READY, // ready state
	RUNNING, // currently running
	BLOCKED, // waiting for something
	ZOMBIE, // terminated but not reclaimed yet
};

// types of a segment
enum segment_type {
	STACK_SEGMENT = 0, // runtime stack segment
	CONTEXT_SEGMENT, // trapframe segment
	SYSTEM_SEGMENT, // system segment
	HEAP_SEGMENT, // runtime heap segment
	CODE_SEGMENT, // ELF segment
	DATA_SEGMENT, // ELF segment
};

// the VM regions mapped to a user process
typedef struct mapped_region {
	uint64 va; // mapped virtual address
	uint32 npages; // mapping_info is unused if npages == 0
	uint32 seg_type; // segment type, one of the segment_types
} mapped_region;

typedef struct process_heap_manager {
	// points to the last free page in our simple heap.
	uint64 heap_top;
	// points to the bottom of our simple heap.
	uint64 heap_bottom;

	// the address of free pages in the heap
	uint64 free_pages_address[MAX_HEAP_PAGES];
	// the number of free pages in the heap
	uint32 free_pages_count;
} process_heap_manager;

// the extremely simple definition of process, used for begining labs of PKE
typedef struct process_t {
	// pointing to the stack used in trap handling.
	uint64 kstack;
	// user page table
	pagetable_t pagetable;
	// trapframe storing the context of a (User mode) process.
	trapframe *trapframe;

	// points to a page that contains mapped_regions. below are added @lab3_1
	mapped_region *mapped_info;
	// next free mapped region in mapped_info
	int total_mapped_region;

	// heap management
	process_heap_manager user_heap;

	// process id
	uint64 pid;
	// process status
	int status;
	// parent process
	struct process_t *parent;
	// next queue element
	struct process_t *queue_next;

	// accounting. added @lab3_3
	int tick_count;

	// file system. added @lab4_1
	proc_file_management *pfiles;

	// for wait()
	int waiting_pid;

	// 目录存储map过的page，需要按照虚拟地址排序
	page_dentry page_dir[MAX_HEAP_PAGES];
	int num_page;
	// 目录存储malloc的区域，用虚拟地址起止表示
	malloc_dentry malloc_dir[MAX_MALLOC_IN_HEAP];
	int num_malloc;

	// when blocked
	int sem_index;
} process;

// switch to run user app
void switch_to(process *);

// initialize process pool (the procs[] array)
void init_proc_pool();
// allocate an empty process, init its vm space. returns its pid
process *alloc_process();
// reclaim a process, destruct its vm space and free physical pages.
int free_process(process *proc);
void clear_process(process *proc);

void print_proc_info(process *p);
// fork a child from parent
int do_fork(process *parent);
int do_exec(char *path, char *agmt);
int do_wait(int pid);
uint64 do_better_malloc(int n);

int alloc_n_page(int n);
void add_to_page_dir(uint64 va_page, uint64 pa_page);
void sort_page_dir();
void add_to_malloc_dir(uint64 va_start, uint64 va_end);
void sort_malloc_dir();
int find_malloc_dir(uint64 va);
void remove_from_malloc_dir(int index);
void do_better_free(uint64 va);

void clear_page_dir(process *proc);
void clear_malloc_dir(process *proc);

// current running process
extern process *current[NCPU];

// 堆底页面的虚地址
extern uint64 g_ufree_page[NCPU];

#endif