#include "pmm.h"
#include "util/functions.h"
#include "riscv.h"
#include "config.h"
#include "util/string.h"
#include "memlayout.h"
#include "spike_interface/spike_utils.h"
#include "kernel/sync_utils.h"

// 自旋锁，避免申请页面冲突
static spinlock_t page_lock = { 0 };

// _end is defined in kernel/kernel.lds, it marks the ending (virtual) address of PKE kernel
extern char _end[];
// g_mem_size is defined in spike_interface/spike_memory.c, it indicates the size of our
// (emulated) spike machine. g_mem_size's value is obtained when initializing HTIF.
extern uint64 g_mem_size;

static uint64 free_mem_start_addr; //beginning address of free memory
static uint64 free_mem_end_addr; //end address of free memory (not included)

typedef struct node {
	struct node *next;
} list_node;

// g_free_mem_list is the head of the list of free physical memory pages
static list_node g_free_mem_list;

//
// actually creates the freepage list. each page occupies 4KB (PGSIZE), i.e., small page.
// PGSIZE is defined in kernel/riscv.h, ROUNDUP is defined in util/functions.h.
//
static void create_freepage_list(uint64 start, uint64 end)
{
	g_free_mem_list.next = 0;
	for (uint64 p = ROUNDUP(start, PGSIZE); p + PGSIZE < end; p += PGSIZE)
		free_page((void *)p);
}

//
// place a physical page at *pa to the free list of g_free_mem_list (to reclaim the page)
//
void free_page(void *pa)
{
	spin_lock(&page_lock);
	if (((uint64)pa % PGSIZE) != 0 || (uint64)pa < free_mem_start_addr ||
	    (uint64)pa >= free_mem_end_addr)
		panic("free_page 0x%lx \n", pa);

	// insert a physical page to g_free_mem_list
	list_node *n = (list_node *)pa;
	// sprint("free pa: 0x%lx\n", (uint64)n);
	n->next = g_free_mem_list.next;
	g_free_mem_list.next = n;
	spin_unlock(&page_lock);
}

//
// takes the first free page from g_free_mem_list, and returns (allocates) it.
// Allocates only ONE page!
//
void *alloc_page(void)
{
	spin_lock(&page_lock);
	uint64 hartid = read_tp();
	list_node *n = g_free_mem_list.next;
	// sprint("alloc pa: 0x%lx\n", (uint64)n);
	if (n) {
		g_free_mem_list.next = n->next;
	}
	spin_unlock(&page_lock);
	return (void *)n;
}

// 只允许在内核态使用，申请的两个页面地址连续
void *alloc_two_page(void)
{
	spin_lock(&page_lock);
	list_node *n = g_free_mem_list.next;
	while (n->next->next &&
	       (uint64)(n->next) - PGSIZE != (uint64)(n->next->next)) {
		n = n->next;
	}
	if (!n->next->next) {
		panic("alloc two page fault!");
	}
	void *ret = (void *)n->next->next;
	n->next = n->next->next->next;
	spin_unlock(&page_lock);
	return ret;
}

//
// pmm_init() establishes the list of free physical pages according to available
// physical memory space.
//
void pmm_init()
{
	// start of kernel program segment
	uint64 g_kernel_start = KERN_BASE;
	uint64 g_kernel_end = (uint64)&_end;

	uint64 pke_kernel_size = g_kernel_end - g_kernel_start;
	sprint("PKE kernel start 0x%lx, PKE kernel end: 0x%lx, PKE kernel size: 0x%lx .\n",
	       g_kernel_start, g_kernel_end, pke_kernel_size);

	// free memory starts from the end of PKE kernel and must be page-aligined
	free_mem_start_addr = ROUNDUP(g_kernel_end, PGSIZE);

	// recompute g_mem_size to limit the physical memory space that our riscv-pke kernel
	// needs to manage
	g_mem_size = MIN(PKE_MAX_ALLOWABLE_RAM, g_mem_size);
	if (g_mem_size < pke_kernel_size)
		panic("Error when recomputing physical memory size (g_mem_size).\n");

	free_mem_end_addr = g_mem_size + DRAM_BASE;
	sprint("free physical memory address: [0x%lx, 0x%lx] \n",
	       free_mem_start_addr, free_mem_end_addr - 1);

	sprint("kernel memory manager is initializing ...\n");
	// create the list of free pages
	create_freepage_list(free_mem_start_addr, free_mem_end_addr);
}