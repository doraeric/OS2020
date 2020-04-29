#include <linux/ktime.h>
#include <linux/kernel.h>
void sys_my_getnstimeofday(struct timespec *ts) {
	getnstimeofday(ts);
}
void sys_my_printk(char * msg) {
	printk(msg);
}
