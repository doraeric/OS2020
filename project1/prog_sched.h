#ifndef _PROG_SCHED_H
#define _PROG_SCHED_H
#include <sys/types.h>

#define TIME_UNIT() { volatile unsigned long i; for(i=0;i<1000000UL;i++); }
#define PARENT_CORE 0
#define CHILD_CORE  1
#define getnstimeofday 333
#define printk 334

typedef enum POLICY {FIFO, RR, SJF, PSJF} POLICY;

typedef struct Process {
    char name[32];
    unsigned int ready_time, exec_time;
    pid_t pid;
} Process;

void do_schedule(POLICY policy, Process * procs, int n_proc);

pid_t prog_fork(unsigned int exec_time, char * name);
int select_next(Process * procs, POLICY policy);
void set_idle(pid_t pid);
void set_run(pid_t pid);

#endif // _PROG_SCHED_H
