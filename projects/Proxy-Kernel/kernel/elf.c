/*
 * routines that scan and load a (host) Executable and Linkable Format (ELF) file
 * into the (emulated) memory.
 */

#include "elf.h"
#include "string.h"
#include "riscv.h"
#include "vmm.h"
#include "pmm.h"
#include "vfs.h"
#include "util/functions.h"
#include "spike_interface/spike_utils.h"

//
// the implementation of allocater. allocates memory space for later segment loading.
//
static void *elf_alloc_mb(elf_ctx *ctx, uint64 elf_pa, uint64 elf_va,
			  uint64 size)
{
	elf_info *msg = (elf_info *)ctx->info;
	// we assume that size of proram segment is smaller than a page.

	kassert(size < MAX_PAGE_PER_ELF * PGSIZE);
	int n = ROUNDUP(size, PGSIZE) / PGSIZE;
	void *pa;
	switch (n) {
	case 1:
		pa = alloc_page();
		if (pa == 0) {
			panic("uvmalloc mem alloc failed\n");
		}
		memset((void *)pa, 0, PGSIZE);
		user_vm_map((pagetable_t)msg->p->pagetable, elf_va, PGSIZE,
			    (uint64)pa,
			    prot_to_type(PROT_WRITE | PROT_READ | PROT_EXEC,
					 1));
		break;
	case 2:
		pa = alloc_two_page();
		if (pa == 0) {
			panic("uvmalloc mem alloc failed\n");
		}
		memset((void *)pa, 0, 2 * PGSIZE);
		user_vm_map((pagetable_t)msg->p->pagetable, elf_va, 2 * PGSIZE,
			    (uint64)pa,
			    prot_to_type(PROT_WRITE | PROT_READ | PROT_EXEC,
					 1));
		break;
	default:
		panic("elf > 2 * PGSIZE not allowed.\n");
		return NULL;
	}
	return pa;
}

//
// actual file reading, using the vfs file interface.
//
static uint64 elf_fpread(elf_ctx *ctx, void *dest, uint64 nb, uint64 offset)
{
	elf_info *msg = (elf_info *)ctx->info;
	// call spike file utility to load the content of elf file into memory.
	// spike_file_pread will read the elf file (msg->f) from offset to memory (indicated by
	// *dest) for nb bytes.
	vfs_lseek(msg->f, offset, SEEK_SET);
	return vfs_read(msg->f, dest, nb);
}

//
// init elf_ctx, a data structure that loads the elf.
//
elf_status elf_init(elf_ctx *ctx, void *info)
{
	ctx->info = info;

	// load the elf header
	if (elf_fpread(ctx, &ctx->ehdr, sizeof(ctx->ehdr), 0) !=
	    sizeof(ctx->ehdr))
		return EL_EIO;

	// check the signature (magic value) of the elf
	if (ctx->ehdr.magic != ELF_MAGIC)
		return EL_NOTELF;

	return EL_OK;
}

//
// load the elf segments to memory regions.
//
elf_status elf_load(elf_ctx *ctx)
{
	// elf_prog_header structure is defined in kernel/elf.h
	elf_prog_header ph_addr;
	int i, off;

	// traverse the elf program segment headers
	for (i = 0, off = ctx->ehdr.phoff; i < ctx->ehdr.phnum;
	     i++, off += sizeof(ph_addr)) {
		// read segment headers
		if (elf_fpread(ctx, (void *)&ph_addr, sizeof(ph_addr), off) !=
		    sizeof(ph_addr))
			return EL_EIO;

		if (ph_addr.type != ELF_PROG_LOAD)
			continue;
		if (ph_addr.memsz < ph_addr.filesz)
			return EL_ERR;
		if (ph_addr.vaddr + ph_addr.memsz < ph_addr.vaddr)
			return EL_ERR;

		// allocate memory block before elf loading
		void *dest = elf_alloc_mb(ctx, ph_addr.vaddr, ph_addr.vaddr,
					  ph_addr.memsz);

		// actual loading
		if (elf_fpread(ctx, dest, ph_addr.memsz, ph_addr.off) !=
		    ph_addr.memsz)
			return EL_EIO;

		// record the vm region in proc->mapped_info.
		int j;
		for (j = 0; j < PGSIZE / sizeof(mapped_region);
		     j++) //seek the last mapped region
			if ((process *)(((elf_info *)(ctx->info))->p)
				    ->mapped_info[j]
				    .va == 0x0)
				break;

		((process *)(((elf_info *)(ctx->info))->p))->mapped_info[j].va =
			ph_addr.vaddr;
		((process *)(((elf_info *)(ctx->info))->p))
			->mapped_info[j]
			.npages = 1;

		// SEGMENT_READABLE, SEGMENT_EXECUTABLE, SEGMENT_WRITABLE are defined in kernel/elf.h
		if (ph_addr.flags == (SEGMENT_READABLE | SEGMENT_EXECUTABLE)) {
			((process *)(((elf_info *)(ctx->info))->p))
				->mapped_info[j]
				.seg_type = CODE_SEGMENT;
			sprint("CODE_SEGMENT added at mapped info offset:%d\n",
			       j);
		} else if (ph_addr.flags ==
			   (SEGMENT_READABLE | SEGMENT_WRITABLE)) {
			((process *)(((elf_info *)(ctx->info))->p))
				->mapped_info[j]
				.seg_type = DATA_SEGMENT;
			sprint("DATA_SEGMENT added at mapped info offset:%d\n",
			       j);
		} else
			panic("unknown program segment encountered, segment flag:%d.\n",
			      ph_addr.flags);

		((process *)(((elf_info *)(ctx->info))->p))
			->total_mapped_region++;
	}

	return EL_OK;
}

void load_bincode_from_vfs_elf(process *p, char *filename, int argc,
			       char *argv[])
{
	print_loadfile_info(filename, argc, argv);
	//elf loading. elf_ctx is defined in kernel/elf.h, used to track the loading process.
	elf_ctx elfloader;
	// elf_info is defined above, used to tie the elf file and its corresponding process.
	elf_info info;

	info.f = vfs_open(filename, O_RDONLY);
	if (IS_ERR_VALUE(info.f))
		panic("Fail on openning the input application program.\n");
	info.p = p;

	// init elfloader context. elf_init() is defined above.
	if (elf_init(&elfloader, &info) != EL_OK)
		panic("fail to init elfloader.\n");

	// load elf. elf_load() is defined above.
	if (elf_load(&elfloader) != EL_OK)
		panic("Fail on loading elf.\n");

	p->trapframe->epc = elfloader.ehdr.entry;

	// 设置argc，argv
	p->trapframe->regs.a0 = argc;
	p->trapframe->regs.a1 = load_argv_into_stack(p, argc, argv);

	// close the host spike file
	vfs_close(info.f);

	sprint("Application program entry point (virtual address): 0x%lx\n",
	       p->trapframe->epc);
}

uint64 load_argv_into_stack(process *p, int argc, char *argv[])
{
	// 首先加载所有参数
	uint64 va_sp = p->trapframe->regs.sp;
	uint64 pa_sp;
	uint64 p_argv[argc];

	// 在栈中记载参数字符串
	for (int i = 0; i < argc; i++) {
		va_sp -= MAX_ARGUMENT_LENGTH;
		pa_sp = (uint64)user_va_to_pa((pagetable_t)p->pagetable,
					      (void *)va_sp);
		memcpy((void *)pa_sp, argv[i], MAX_ARGUMENT_LENGTH);
		p_argv[i] = va_sp;
	}

	// 在栈中记载argv指针数组
	va_sp -= sizeof(p_argv);
	pa_sp = (uint64)user_va_to_pa((pagetable_t)p->pagetable, (void *)va_sp);
	if (argc != 0) {
		memcpy((void *)pa_sp, p_argv, sizeof(p_argv));
	}

	uint64 new_argv = va_sp;
	p->trapframe->regs.sp = va_sp;

	return new_argv;
}

void print_loadfile_info(char *filename, int argc, char *argv[])
{
	sprint("Application: %s ", filename);
	sprint("Argc: %d ", argc);
	sprint("Argv: ");
	for (int i = 0; i < argc; i++) {
		sprint("%s ", argv[i]);
	}
	sprint("\n");
	return;
}