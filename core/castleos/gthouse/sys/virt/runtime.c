// runtime.c
// Runtime environment for processes.
// This is gonna be used for paravirtualization
// where a runtime defines a partition for a given kernel.
// It operates as a group of processes that belongs to a given
// partition.
// #ps: Only the te_d structure will have a pointer for the runtime.


#include <kernel.h>

struct runtime_d *primary_runtime;

// =========================================================

// Create runtime object
struct runtime_d *runtimeObject(void)
{
    //#todo

    return NULL;
};

void runtime_set_primary_runtime(struct runtime_d *runtime)
{
    if ((void*) runtime == NULL)
        return;
    if (runtime->magic != 1234)
        return;

    primary_runtime = runtime;
}

// Get primary runtime
struct runtime_d *runtime_get_primary_runtime(void)
{
    return (struct runtime_d *) primary_runtime;
};

