// ksignal.c
// The unix-like signal support.
// Created by Fred Nora.

// #todo:
// We're already have some support for callback,
// where you can call routines in ring3 from kernel
// and return back using an interrupt.

#include <kernel.h>    


// #todo
// Fix the parameters follogin the posix standard.
int 
sys_sigaction ( 
    int signum, 
    void *act,
    void *oldact )
{
    if (signum < 0)
        goto fail;

    panic ("sys_sigaction: Not implemented\n");
fail:
    return (int) -1;
}

// #todo
// Fix the parameters follogin the posix standard.
int sys_signal(int signum, void *handler)
{
    if (signum < 0)
        goto fail;

    panic ("sys_signal: Not implemented\n");

fail:
    return (int) -1;
}


