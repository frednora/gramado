// runtime.h
// Runtime environment for processes.
// This is gonna be used for paravirtualization
// where a runtime defines a partition for a given kernel.
// It operates as a group of processes that belongs to a given
// partition.
// #ps: Only the te_d structure will have a pointer for the runtime.

#ifndef __VIRT_RUNTIME_H
#define __VIRT_RUNTIME_H    1

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
struct runtime_d *runtime_get_primary_runtime(void);

#endif

