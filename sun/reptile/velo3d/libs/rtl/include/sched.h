
// sched.h

#ifndef __SCHED_H
#define __SCHED_H    1

#include <sys/types.h>

struct sched_param
{
    //#todo
    int stuff;
};


int sched_yield(void);
int sched_setparam(pid_t pid, const struct sched_param* param);
int sched_getparam(pid_t pid, struct sched_param *param);

#endif    


