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
// #ps: Here we are dealing with the sigaction_d struture
//  in the parameters.
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

// #ps
// While signal() in ring 3 has only two parameter, the libc 
// expands it to 3 parameters.
unsigned long 
sys_signal(
    int signum, 
    unsigned long handler, 
    unsigned long restorer )
{
    struct sigaction_d *sa;
    int Slot;
    struct thread_d *current_thread;
    tid_t CurrentTID;
    int core_id = 0;  // #provisory: Core ID

    if (signum < 1 || signum >= NSIG)
        goto fail;

// These two cannot be caught or ignored
    if (signum == SIGKILL || signum == SIGSTOP)
        goto fail;

    Slot = (signum -1);

// Get current thread
// #ps: core 0
    CurrentTID = (tid_t) GetCurrentTID(core_id);
    current_thread = (struct thread_d *) GetThreadByTID(CurrentTID);
    if ((void*)current_thread == NULL)
        goto fail;
    if (current_thread->magic != 1234)
        goto fail;

// #ps: Signal numbers start at 1, while C arrays start at 0.

    unsigned long old_handler = 
        current_thread->sigaction[Slot].sa_handler;

    current_thread->sigaction[Slot].sa_handler = handler;
    current_thread->sigaction[Slot].sa_restorer = restorer;

// Classic signal() behaviour
    current_thread->sigaction[Slot].sa_flags = SA_ONESHOT | SA_NOMASK;
    current_thread->sigaction[Slot].sa_mask  = 0;

// Clear the extra fields you have
    current_thread->sigaction[Slot].sa_msgcode = 0;
    current_thread->sigaction[Slot].sa_long1   = 0;
    current_thread->sigaction[Slot].sa_long2   = 0;

    // #debug
    printk("sys_signal: done\n");

    return (unsigned long) old_handler;

fail:
    return (unsigned long) -1;
}


