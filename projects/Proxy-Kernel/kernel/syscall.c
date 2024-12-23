/*
 * contains the implementation of all syscalls.
 */

#include <stdint.h>
#include <errno.h>

#include "util/types.h"
#include "syscall.h"
#include "string.h"
#include "process.h"
#include "util/functions.h"
#include "pmm.h"
#include "vmm.h"
#include "sched.h"
#include "proc_file.h"

#include "spike_interface/spike_utils.h"

//
// implement the SYS_user_print syscall
//
ssize_t sys_user_print(const char *buf, size_t n)
{
	int hartid = (int)read_tp();
	// buf is now an address in user space of the given app's user stack,
	// so we have to transfer it into phisical address (kernel is running in direct mapping).
	assert(current[hartid]);
	char *pa = (char *)user_va_to_pa(
		(pagetable_t)(current[hartid]->pagetable), (void *)buf);
	sprint(pa);
	return 0;
}

ssize_t sys_user_scan(char *buf)
{
	int hartid = (int)read_tp();
	assert(current[hartid]);
	char *pa = (char *)user_va_to_pa(
		(pagetable_t)(current[hartid]->pagetable), (void *)buf);
	spike_file_read(stdin, pa, 256);
	return 0;
}

//
// implement the SYS_user_exit syscall
//
// 这里我们只允许一号核进行调度
ssize_t sys_user_exit(uint64 code)
{
	int hartid = (int)read_tp();
	// reclaim the current process, and reschedule.
	sprint("User exit with code:%d.\n", code);
	free_process(current[hartid]);
	schedule();
	return 0;
}

//
uint64 sys_user_allocate_page()
{
	int hartid = (int)read_tp();
	void *pa = alloc_page();
	uint64 va;
	// if there are previously reclaimed pages, use them first (this does not change the
	// size of the heap)
	if (current[hartid]->user_heap.free_pages_count > 0) {
		va = current[hartid]->user_heap.free_pages_address
			     [--current[hartid]->user_heap.free_pages_count];
		assert(va < current[hartid]->user_heap.heap_top);
	} else {
		// otherwise, allocate a new page (this increases the size of the heap by one page)
		va = current[hartid]->user_heap.heap_top;
		current[hartid]->user_heap.heap_top += PGSIZE;

		current[hartid]->mapped_info[HEAP_SEGMENT].npages++;
	}
	user_vm_map((pagetable_t)current[hartid]->pagetable, va, PGSIZE,
		    (uint64)pa, prot_to_type(PROT_WRITE | PROT_READ, 1));

	return va;
}

//
// reclaim a page, indicated by "va".
//
uint64 sys_user_free_page(uint64 va)
{
	int hartid = (int)read_tp();
	user_vm_unmap((pagetable_t)current[hartid]->pagetable, va, PGSIZE, 1);
	// add the reclaimed page to the free page list
	current[hartid]->user_heap.free_pages_address
		[current[hartid]->user_heap.free_pages_count++] = va;
	return 0;
}

//
// kerenl entry point of naive_fork
//
ssize_t sys_user_fork()
{
	int hartid = (int)read_tp();
	sprint("User call fork.\n");
	return do_fork(current[hartid]);
}

//
// kerenl entry point of yield.
//
ssize_t sys_user_yield()
{
	int hartid = (int)read_tp();
	current[hartid]->status = READY;
	from_blocked_to_ready(current[hartid]);
	schedule();
	return 0;
}

//
// open file
//
ssize_t sys_user_open(char *pathva, int flags)
{
	int hartid = (int)read_tp();
	char *pathpa = (char *)user_va_to_pa(
		(pagetable_t)(current[hartid]->pagetable), pathva);
	return do_open(pathpa, flags);
}

//
// read file
//
ssize_t sys_user_read(int fd, char *bufva, uint64 count)
{
	int hartid = (int)read_tp();
	int i = 0;
	while (i < count) { // count can be greater than page size
		uint64 addr = (uint64)bufva + i;
		uint64 pa = lookup_pa((pagetable_t)current[hartid]->pagetable,
				      addr);
		uint64 off = addr - ROUNDDOWN(addr, PGSIZE);
		uint64 len = count - i < PGSIZE - off ? count - i :
							PGSIZE - off;
		uint64 r = do_read(fd, (char *)pa + off, len);
		i += r;
		if (r < len)
			return i;
	}
	return count;
}

//
// write file
//
ssize_t sys_user_write(int fd, char *bufva, uint64 count)
{
	int hartid = (int)read_tp();
	int i = 0;
	while (i < count) { // count can be greater than page size
		uint64 addr = (uint64)bufva + i;
		uint64 pa = lookup_pa((pagetable_t)current[hartid]->pagetable,
				      addr);
		uint64 off = addr - ROUNDDOWN(addr, PGSIZE);
		uint64 len = count - i < PGSIZE - off ? count - i :
							PGSIZE - off;
		uint64 r = do_write(fd, (char *)pa + off, len);
		i += r;
		if (r < len)
			return i;
	}
	return count;
}

//
// lseek file
//
ssize_t sys_user_lseek(int fd, int offset, int whence)
{
	return do_lseek(fd, offset, whence);
}

//
// read vinode
//
ssize_t sys_user_stat(int fd, struct istat *istat)
{
	int hartid = (int)read_tp();
	struct istat *pistat = (struct istat *)user_va_to_pa(
		(pagetable_t)(current[hartid]->pagetable), istat);
	return do_stat(fd, pistat);
}

//
// read disk inode
//
ssize_t sys_user_disk_stat(int fd, struct istat *istat)
{
	int hartid = (int)read_tp();
	struct istat *pistat = (struct istat *)user_va_to_pa(
		(pagetable_t)(current[hartid]->pagetable), istat);
	return do_disk_stat(fd, pistat);
}

//
// close file
//
ssize_t sys_user_close(int fd)
{
	return do_close(fd);
}

//
// lib call to opendir
//
ssize_t sys_user_opendir(char *pathva)
{
	int hartid = (int)read_tp();
	char *pathpa = (char *)user_va_to_pa(
		(pagetable_t)(current[hartid]->pagetable), pathva);
	return do_opendir(pathpa);
}

//
// lib call to readdir
//
ssize_t sys_user_readdir(int fd, struct dir *vdir)
{
	int hartid = (int)read_tp();
	struct dir *pdir = (struct dir *)user_va_to_pa(
		(pagetable_t)(current[hartid]->pagetable), vdir);
	return do_readdir(fd, pdir);
}

//
// lib call to mkdir
//
ssize_t sys_user_mkdir(char *pathva)
{
	int hartid = (int)read_tp();
	char *pathpa = (char *)user_va_to_pa(
		(pagetable_t)(current[hartid]->pagetable), pathva);
	return do_mkdir(pathpa);
}

//
// lib call to closedir
//
ssize_t sys_user_closedir(int fd)
{
	return do_closedir(fd);
}

//
// lib call to link
//
ssize_t sys_user_link(char *vfn1, char *vfn2)
{
	int hartid = (int)read_tp();
	char *pfn1 = (char *)user_va_to_pa(
		(pagetable_t)(current[hartid]->pagetable), (void *)vfn1);
	char *pfn2 = (char *)user_va_to_pa(
		(pagetable_t)(current[hartid]->pagetable), (void *)vfn2);
	return do_link(pfn1, pfn2);
}

//
// lib call to unlink
//
ssize_t sys_user_unlink(char *vfn)
{
	int hartid = (int)read_tp();
	char *pfn = (char *)user_va_to_pa(
		(pagetable_t)(current[hartid]->pagetable), (void *)vfn);
	return do_unlink(pfn);
}

ssize_t sys_user_exec(char *path, char *agmt)
{
	int hartid = (int)read_tp();
	char *pa_path =
		user_va_to_pa((pagetable_t)(current[hartid]->pagetable), path);
	char *pa_agmt =
		user_va_to_pa((pagetable_t)(current[hartid]->pagetable), agmt);
	return do_exec(pa_path, pa_agmt);
}

ssize_t sys_user_wait(int pid)
{
	return do_wait(pid);
}

ssize_t sys_user_rcwd(char *path)
{
	int hartid = (int)read_tp();
	char *path_pa = (char *)user_va_to_pa(current[hartid]->pagetable, path);
	return do_rcwd(path_pa);
}

ssize_t sys_user_ccwd(char *path)
{
	int hartid = (int)read_tp();
	char *path_pa = (char *)user_va_to_pa(current[hartid]->pagetable, path);
	return do_ccwd(path_pa);
}

uint64 sys_user_better_allocate_page(int n)
{
	uint64 va = do_better_malloc(n);
	return va;
}

uint64 sys_user_better_free_page(uint64 va)
{
	do_better_free(va);
	return 0;
}

int sys_user_sem_new(int init_num)
{
	return add_to_semaphores(init_num);
}

int sys_user_sem_P(int index)
{
	return P_semaphore(index);
}

int sys_user_sem_V(int index)
{
	return V_semaphore(index);
}

ssize_t sys_user_printpa(uint64 va)
{
	int hartid = read_tp();
	uint64 pa = (uint64)user_va_to_pa(
		(pagetable_t)(current[hartid]->pagetable), (void *)va);
	sprint("0x%x\n", pa);
	return 0;
}

//
// [a0]: the syscall number; [a1] ... [a7]: arguments to the syscalls.
// returns the code of success, (e.g., 0 means success, fail for otherwise)
//
long do_syscall(long a0, long a1, long a2, long a3, long a4, long a5, long a6,
		long a7)
{
	int hartid = (int)read_tp();
	// sprint("entering kernel: proc %d inserted to ready queue\n", current[hartid]->pid);
	insert_to_ready_queue(current[hartid]);
	switch (a0) {
	case SYS_user_print:
		return sys_user_print((const char *)a1, a2);
	case SYS_user_exit:
		return sys_user_exit(a1);
	case SYS_user_allocate_page:
		return sys_user_allocate_page();
	case SYS_user_free_page:
		return sys_user_free_page(a1);
	case SYS_user_fork:
		return sys_user_fork();
	case SYS_user_yield:
		return sys_user_yield();
	case SYS_user_open:
		return sys_user_open((char *)a1, a2);
	case SYS_user_read:
		return sys_user_read(a1, (char *)a2, a3);
	case SYS_user_write:
		return sys_user_write(a1, (char *)a2, a3);
	case SYS_user_lseek:
		return sys_user_lseek(a1, a2, a3);
	case SYS_user_stat:
		return sys_user_stat(a1, (struct istat *)a2);
	case SYS_user_disk_stat:
		return sys_user_disk_stat(a1, (struct istat *)a2);
	case SYS_user_close:
		return sys_user_close(a1);
	case SYS_user_opendir:
		return sys_user_opendir((char *)a1);
	case SYS_user_readdir:
		return sys_user_readdir(a1, (struct dir *)a2);
	case SYS_user_mkdir:
		return sys_user_mkdir((char *)a1);
	case SYS_user_closedir:
		return sys_user_closedir(a1);
	case SYS_user_link:
		return sys_user_link((char *)a1, (char *)a2);
	case SYS_user_unlink:
		return sys_user_unlink((char *)a1);
	case SYS_user_exec:
		return sys_user_exec((char *)a1, (char *)a2);
	case SYS_user_wait:
		return sys_user_wait(a1);
	case SYS_user_scan:
		return sys_user_scan((char *)a1);
	case SYS_user_rcwd:
		return sys_user_rcwd((char *)a1);
	case SYS_user_ccwd:
		return sys_user_ccwd((char *)a1);
	case SYS_user_better_allocate_page:
		return sys_user_better_allocate_page((int)a1);
	case SYS_user_better_free_page:
		return sys_user_better_free_page((int)a1);
	case SYS_user_sem_new:
		return sys_user_sem_new((int)a1);
	case SYS_user_sem_P:
		return sys_user_sem_P((int)a1);
	case SYS_user_sem_V:
		return sys_user_sem_V((int)a1);
	case SYS_user_printpa:
		return sys_user_printpa(a1);
	default:
		panic("Unknown syscall %ld \n", a0);
	}
}
