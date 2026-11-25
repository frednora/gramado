// runtime.h
// Runtime environment for processes.
// This is gonna be used for paravirtualization
// where a runtime defines a partition for a given kernel.
// It operates as a group of processes that belongs to a given
// partition.
// #ps: Only the te_d structure will have a pointer for the runtime.

#ifndef __VIRT_RUNTIME_H
#define __VIRT_RUNTIME_H    1

/*
 * Runtime hierarchy
 * -----------------
 *
 * A runtime is the highest-level execution domain managed by
 * the system. It contains one or more Thread Environments (TEs).
 *
 * A TE owns an address space and the resources associated with it.
 * Threads execute within a TE.
 *
 * The scheduler operates only on thread objects and is unaware
 * of runtimes and TEs.
 *
 * Relationship:
 *
 *      runtime_d
 *          |
 *          +-- te_d
 *          |      |
 *          |      +-- thread_d
 *          |      +-- thread_d
 *          |
 *          +-- te_d
 *                 |
 *                 +-- thread_d
 *
 * Concepts:
 *
 *      runtime_d
 *          Execution domain / runtime container.
 *
 *      te_d
 *          Thread Environment.
 *          Owns the address space and execution resources.
 *
 *      thread_d
 *          Execution context scheduled by the dispatcher.
 *
 * Future:
 *
 *      Hardware virtualization may introduce an additional
 *      layer (hv_partition_d) above runtime_d.
 *
 *      hv_partition_d
 *              |
 *              +-- runtime_d
 *                      |
 *                      +-- te_d
 *                              |
 *                              +-- thread_d
 *
 * This allows the runtime model to remain unchanged whether
 * the system is running on bare metal, using software
 * partitioning, or using hardware-assisted virtualization.
 */

struct runtime_d
{
    int used;
    int magic;

    int runtime_id;
    int initialized;

    int state;
    unsigned long flags;

    // ...
};
extern struct runtime_d *primary_runtime;


// ============================


struct runtime_d *runtimeObject(void);

void runtime_set_primary_runtime(struct runtime_d *runtime);
struct runtime_d *runtime_get_primary_runtime(void);

#endif

