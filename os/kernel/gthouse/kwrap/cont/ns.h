// ns.h
// namespaces for containers.
// Created by Fred Nora.

/*
Namespaces:
    Provide isolation by restricting what a process can see (network, PID, mounts).

namespace: wraps a global system resource in an abstraction that makes it appear 
to the processes within the namespace that they have their own isolated instance of the global resource.

namespaces = limits what you can see (and therefore use)
*/


#ifndef __CONT_NS_H
#define __CONT_NS_H    1

// namespaces in other words are the container per se.
struct ns_d
{
    int used;
    int magic;
    int id;
    int initialized;

// This is all about the pusepose of this isolated group of processes.
// Ex: 'system ns' is a container for processes that belongs to the system.
    int type;

    // ....

// This address for a list of thread environment that belongs to this ns.
    void *te_list_base_address;
    size_t list_size;

    // ....

    struct ns_d *next;
};

// The list of namespaces in this machine.
// Only 32 containers.
extern struct ns_d ns[32];

// ======================================

int ns_initialize(int phase);

#endif    

