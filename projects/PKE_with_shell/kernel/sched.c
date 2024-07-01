/*
 * implementing the scheduler
 */

// 对于PKE进程的调度，我们这样进行操作
// 1. 用户态中断进入内核态时，将current置为ready，并插入ready队列尾
// 2. 内核态返回用户态时，从ready队列首取出一个进程设为current，并将其从ready队列删除
// 3. wait时，current进入内核态先被放入ready队列队尾，接着执行wait后被移除并放入blocked队列，并设置waiting_pid字段
// 4. fork时，申请一个进程，放入ready队列末尾
// 5. free时，current进入内核态先被放入ready队列队尾，接着执行wait后被移除
// 6. schedule时，首先扫描所有blocked队列里的进程，如果等待子进程已是zombie状态，则重置waiting_pid字段，将等待中的进程从blocked队列删除并加入ready队列,但并不回收子进程

#include "sched.h"
#include "spike_interface/spike_utils.h"

// 在这里，不同于pthread中共享内存的线程，我们使两个核独立的运行，各自具有自己的进程调度队列
process *ready_queue_head[NCPU] = { NULL };
process *blocked_queue_head[NCPU] = { NULL };
extern process procs[NCPU][NPROC];

static int semaphores[MAX_SEMAPHORE_NUM];
int num_semaphores;

//
// insert a process, proc, into the END of ready queue.
//
void insert_to_ready_queue(process *proc)
{
	int hartid = read_tp();
	// sprint("%d inserted to ready\n", proc->pid);
	// sprint( "going to insert process %d to ready queue.\n", proc->pid );
	// if the queue is empty in the beginning
	if (ready_queue_head[hartid] == NULL) {
		proc->status = READY;
		proc->queue_next = NULL;
		ready_queue_head[hartid] = proc;
		return;
	}

	// ready queue is not empty
	process *p;
	// browse the ready queue to see if proc is already in-queue
	for (p = ready_queue_head[hartid]; p->queue_next != NULL;
	     p = p->queue_next)
		if (p == proc)
			return; //already in queue

	// p points to the last element of the ready queue
	if (p == proc)
		return;
	p->queue_next = proc;
	proc->status = READY;
	proc->queue_next = NULL;

	return;
}

// 插入到末尾
void insert_to_blocked_queue(process *proc)
{
	// sprint("%d inserted to blocked\n", proc->pid);
	int hartid = read_tp();
	// sprint( "going to insert process %d to blocked queue.\n", proc->pid );
	// if the queue is empty in the beginning
	if (blocked_queue_head[hartid] == NULL) {
		proc->status = BLOCKED;
		proc->queue_next = NULL;
		blocked_queue_head[hartid] = proc;
		return;
	}

	// blocked queue is not empty
	process *p;
	// browse the blocked queue to see if proc is already in-queue
	for (p = blocked_queue_head[hartid]; p->queue_next != NULL;
	     p = p->queue_next)
		if (p == proc)
			return; //already in queue

	// p points to the last element of the blocked queue
	if (p == proc)
		return;

	p->queue_next = proc;
	proc->status = BLOCKED;
	proc->queue_next = NULL;

	return;
}

void remove_from_ready_queue(process *proc)
{
	// sprint("%d removed from ready\n", proc->pid);
	int hartid = (int)read_tp();
	if (ready_queue_head[hartid] == NULL) {
		return;
	}
	if (ready_queue_head[hartid] == proc) {
		ready_queue_head[hartid] = ready_queue_head[hartid]->queue_next;
		return;
	}
	for (process *p = ready_queue_head[hartid]; p->queue_next != NULL;
	     p = p->queue_next) {
		if (p->queue_next == proc) {
			p->queue_next = p->queue_next->queue_next;
			break;
		}
	}
}

void remove_from_blocked_queue(process *proc)
{
	// sprint("%d removed from blocked\n", proc->pid);
	int hartid = read_tp();
	if (blocked_queue_head[hartid] == NULL) {
		return;
	}
	if (blocked_queue_head[hartid] == proc) {
		blocked_queue_head[hartid] =
			blocked_queue_head[hartid]->queue_next;
		return;
	}
	for (process *p = blocked_queue_head[hartid]; p->queue_next != NULL;
	     p = p->queue_next) {
		if (p->queue_next == proc) {
			p->queue_next = p->queue_next->queue_next;
			break;
		}
	}
}

void from_ready_to_blocked(process *proc)
{
	remove_from_ready_queue(proc);
	insert_to_blocked_queue(proc);
	return;
}

void from_blocked_to_ready(process *proc)
{
	remove_from_blocked_queue(proc);
	insert_to_ready_queue(proc);
	return;
}

int add_to_semaphores(int init_num)
{
	if (num_semaphores + 1 > MAX_SEMAPHORE_NUM) {
		panic("error in alloc semaphore\n");
	}
	int index = num_semaphores;
	semaphores[num_semaphores] = init_num;
	num_semaphores++;
	return index;
}

int P_semaphore(int index)
{
	int hartid = read_tp();
	if (semaphores[index] >= 1) {
		semaphores[index]--;
	} else {
		// 因为index号信号量，进入阻塞队列
		current[hartid]->sem_index = index;
		from_ready_to_blocked(current[hartid]);
		schedule();
	}
	return 0;
}

int V_semaphore(int index)
{
	int hartid = read_tp();
	semaphores[index]++;
	for (process *p = blocked_queue_head[hartid]; p != NULL;
	     p = p->queue_next) {
		if (semaphores[p->sem_index] >= 1) {
			// do wake up
			semaphores[p->sem_index]--;
			from_blocked_to_ready(p);
		}
	}
	return 0;
}

//
// choose a proc from the ready queue, and put it to run.
// note: schedule() does not take care of previous current process. If the current
// process is still runnable, you should place it into the ready queue (by calling
// ready_queue_insert), and then call schedule().
//

void schedule()
{
	int hartid = (int)read_tp();
	// 检查父进程等待的子进程是否已经zombie，如果是，则将父进程重新投入运行（重置waiting_pid, status）
	// 对于 zombie 态，我们的系统不回收进程及进程页
	if (blocked_queue_head[hartid]) {
		for (process *p = blocked_queue_head[hartid]; p != NULL;
		     p = p->queue_next) {
			if (procs[hartid][p->waiting_pid].status == ZOMBIE) {
				// procs[hartid][p->waiting_pid].status = FREE;
				p->waiting_pid = -1;
				from_blocked_to_ready(p);
			}
		}
	}

	if (!ready_queue_head[hartid]) {
		// by default, if there are no ready process, and all processes are in the status of
		// FREE and ZOMBIE, we should shutdown the emulated RISC-V machine.
		int should_shutdown = 1;

		for (int i = 0; i < NPROC; i++)
			if ((procs[hartid][i].status != FREE) &&
			    (procs[hartid][i].status != ZOMBIE)) {
				should_shutdown = 0;
				sprint("ready queue empty, but process %d is not in free/zombie state:%d\n",
				       i, procs[hartid][i].status);
			}

		if (should_shutdown) {
			sprint("no more ready processes, system shutdown now.\n");
			shutdown(0);
		} else {
			panic("Not handled: we should let system wait for unfinished processes.\n");
		}
	}

	current[hartid] = ready_queue_head[hartid];
	assert(current[hartid]->status == READY);
	ready_queue_head[hartid] = ready_queue_head[hartid]->queue_next;

	current[hartid]->status = RUNNING;
	// sprint("leaving kernel: proc %d removed from ready queue\n", current[hartid]->pid);
	sprint("going to schedule process %d to run.\n", current[hartid]->pid);
	// print_proc_info(current);
	switch_to(current[hartid]);
}