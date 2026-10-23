// idle.c
// Created by Fred Nora

#include <kernel.h>

struct thread_d *get_idle_thread(int lapic_info_id)
{
    tid_t IdleTID;
    struct thread_d *t;

// Parameters:
    if (lapic_info_id < 0)
        goto fail;
    if (lapic_info_id >= NR_CPUS)
        goto fail;

    if (lapic_info[lapic_info_id].initialized != TRUE)
        goto fail;

    IdleTID = (tid_t) lapic_info[lapic_info_id].idle_tid;

    t = (struct thread_d *) get_thread_by_tid(IdleTID);

    return (struct thread_d *) t;

fail:
    return NULL;
}