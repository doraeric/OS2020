#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include "prog_sched.h"
#include "queue.h"

int running_idx = -1;
int coming_idx = 0;
unsigned int time_cnt = 0;
unsigned int time_rr_cnt = 0;
int finished = 0;
Queue * queue = NULL;
int psjf_flag = 0;

int compar(const void * a, const void * b) {
    return ((Process *)a)->ready_time - ((Process *)b)->ready_time;
}
void prog_setaffinity(pid_t pid, unsigned int core) {
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(core, &set);
    if (sched_setaffinity(pid, sizeof(set), &set) == -1) {
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }
}
void do_schedule(POLICY policy, Process * procs, int n_proc) {
    qsort(procs, n_proc, sizeof(Process), compar);
    prog_setaffinity(0, PARENT_CORE);
    if (policy == RR) {
        queue = new_Queue();
    }
    // for (int i = 0; i < n_proc; ++i) {
    //     printf("%s %u %u\n", procs[i].name, procs[i].ready_time, procs[i].exec_time);
    // }
    while(1) {
        // 1. finish currently running process if should
        // printf("running_idx: %d\n", running_idx);
        if (running_idx != -1 && procs[running_idx].exec_time == 0) {
            int wstatus;
            // waitpid(procs[running_idx].pid, &wstatus, WNOHANG);
            waitpid(procs[running_idx].pid, &wstatus, 0);
            // printf("finish: %d\n", procs[running_idx].pid);
            ++finished;
        }
        // printf("[%u] %d/%d\n", time_cnt, finished, n_proc);
        // 2. fork the process if new one comes
        for (;coming_idx < n_proc; ++coming_idx) {
            if (procs[coming_idx].ready_time <= time_cnt) {
                procs[coming_idx].pid = prog_fork(procs[coming_idx].exec_time, procs[coming_idx].name);
                set_idle(procs[coming_idx].pid);
                if (queue) Queue_push(queue, coming_idx);
                psjf_flag = 1;
            } else { break; }
        }
        // 3. select one to run
        if (finished < n_proc) {
            int next = select_next(procs, policy);
            // if (next != -1) printf("next is %d, exec_time: %u\n", next, procs[next].exec_time);
            if (running_idx != next) {
                if(next != -1) set_run(procs[next].pid);
                if(running_idx != -1 && procs[running_idx].exec_time > 0)
                    set_idle(procs[running_idx].pid);
                running_idx = next;
            }
        } else {
            break;
        }
        // printf("check head %p %d\n", queue->head, Queue_empty(queue));
        // if (!Queue_empty(queue)) {
        //     printf("queue %d\n", queue->head->data);
        // }
        TIME_UNIT();
        ++time_cnt;
        --procs[running_idx].exec_time;
    }
    if (queue) {
        delete_Queue(queue);
        queue = NULL;
    }
}

int select_next(Process * procs, POLICY policy) {
    if (policy == FIFO) {
        for (int i = running_idx; i < coming_idx; ++i) {
            if (i == -1) continue;
            if (procs[i].exec_time > 0) return i;
        }
        return -1;
    } else if (policy == RR) {
        // keep original
        int ret = -1;
        if (running_idx != -1 && procs[running_idx].exec_time > 0 && time_rr_cnt % 500 != 0) {
            ret = running_idx;
        } else {
            if(running_idx != -1 && procs[running_idx].exec_time > 0)
                Queue_push(queue, running_idx);
            if (Queue_empty(queue))
                ret = -1;
            else
                ret = Queue_pop(queue);
            time_rr_cnt = 0;
        }
        time_rr_cnt = (time_rr_cnt +1) % 500;
        return ret;
    } else if (policy == SJF) {
        if(running_idx != -1 && procs[running_idx].exec_time > 0)
            return running_idx;
        unsigned int min = 0;
        int idx = -1;
        for (int i = 0; i < coming_idx; ++i) {
            if (idx == -1 && procs[i].exec_time > 0) {
                min = procs[i].exec_time;
                idx = i;
            } else if (procs[i].exec_time > 0 && procs[i].exec_time < min) {
                min = procs[i].exec_time;
                idx = i;
            }
        }
        return idx;
    } else if (policy == PSJF) {
        if (!psjf_flag && running_idx != -1 && procs[running_idx].exec_time > 0)
            return running_idx;
        unsigned int min = 0;
        int idx = -1;
        for (int i = 0; i < coming_idx; ++i) {
            if (idx == -1 && procs[i].exec_time > 0) {
                min = procs[i].exec_time;
                idx = i;
                // printf("1 %d %u\n", idx, min);
            } else if (procs[i].exec_time > 0 && procs[i].exec_time < min) {
                min = procs[i].exec_time;
                idx = i;
                // printf("2 %d %u\n", idx, min);
            }
        }
        psjf_flag = 0;
        return idx;
    }
    return -1;
}

pid_t prog_fork(unsigned int exec_time, char * name) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork()");
    } else if (pid == 0) { // the child segment
        char dmesg_buf[128] = {0};
        struct timespec start, stop;
        pid_t child_pid = getpid();
        printf("%s %d\n", name, child_pid);
        prog_setaffinity(0, CHILD_CORE);
        syscall(getnstimeofday, &start);
        for (unsigned int i=0; i < exec_time; ++i) {
            TIME_UNIT();
        }
        syscall(getnstimeofday, &stop);
        sprintf(dmesg_buf, "[Project1] %d %lu.%lu %lu.%lu\n",
                child_pid, start.tv_sec, start.tv_nsec, stop.tv_sec, stop.tv_nsec);
        syscall(printk, dmesg_buf);
        exit(0);
    }
    return pid;
}

void set_idle(pid_t pid) {
    const struct sched_param param = {0};
    // printf("idle %d\n", pid);
    int ret = sched_setscheduler(pid, SCHED_IDLE, &param);
    if (ret < 0) {
		perror("sched_setscheduler");
		return;
	}
}
void set_run(pid_t pid) {
    const struct sched_param param = {0};
    // printf("run %d\n", pid);
    int ret = sched_setscheduler(pid, SCHED_OTHER, &param);
    if (ret < 0) {
		perror("sched_setscheduler");
		return;
	}
}
