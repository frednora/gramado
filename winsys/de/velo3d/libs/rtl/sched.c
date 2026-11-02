
// sched.c

#include <sys/types.h>
#include <sched.h>
#include <rtl/gramado.h>

/*
sched_yield() causes the calling thread to relinquish the CPU.
The thread is moved to the end of the queue for its static
priority and a new thread gets to run.
*/
// renunciar
int sched_yield(void)
{
    rtl_yield();
    return 0;
}

int sched_setparam(pid_t pid, const struct sched_param* param)
{
    return (int) -1;
}

int sched_getparam(pid_t pid, struct sched_param *param)
{
    return (int) -1;
}



