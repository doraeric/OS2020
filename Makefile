main: main.c prog_sched.c queue.c prog_sched.h queue.h
	gcc main.c prog_sched.c queue.c -o main -std=c11
