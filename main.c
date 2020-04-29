#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prog_sched.h"

int main() {
    char buf[56] = {0};
    POLICY policy;
    // fgets(buf, 6, stdin);
    scanf("%s", buf);
    if (strncmp(buf, "FIFO", 4) == 0) {
        policy = FIFO;
    } else if (strncmp(buf, "RR", 2) == 0) {
        policy = RR;
    } else if (strncmp(buf, "SJF", 3) == 0) {
        policy = SJF;
    } else if (strncmp(buf, "PSJF", 4) == 0) {
        policy = PSJF;
    } else {
        puts("Invalid policy");
        exit(-1);
    }
    // fgets(buf, 11, stdin);
    // int n_proc = atoi(buf);
    int n_proc;
    scanf("%d", &n_proc);
    Process * proc = (Process *)malloc(n_proc * sizeof(Process));
    for (int ni=0; ni < n_proc; ++ni) {
        scanf("%s%d%d", buf, &proc[ni].ready_time, &proc[ni].exec_time);
        buf[31] = 0;
        strncpy(proc[ni].name, buf, 32);
        // fgets(buf, 56, stdin);
        // char * name = strtok(buf, " ");
        // strncpy(proc[ni].name, name, 31);
        // proc[ni].ready_time = atoi(strtok(NULL, " "));
        // proc[ni].exec_time = atoi(strtok(NULL, " "));
    }
    do_schedule(policy, proc, n_proc);
    return 0;
}
