#ifndef _SCHED_H_
#define _SCHED_H_

#include "process.h"

//length of a time slice, in number of ticks
#define TIME_SLICE_LEN 2
#define MAX_SEMAPHORE_NUM 10

void insert_to_ready_queue(process *proc);
void insert_to_blocked_queue(process *proc);
void remove_from_ready_queue(process *proc);
void remove_from_blocked_queue(process *proc);
void from_ready_to_blocked(process *proc);
void from_blocked_to_ready(process *proc);

int add_to_semaphores(int init_num);
int P_semaphore(int index);
int V_semaphore(int index);

void schedule();

#endif
