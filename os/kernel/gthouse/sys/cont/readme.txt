

Namespaces:
    Provide isolation by restricting what a process can see (network, PID, mounts).

cgroups:
    Provide resource management by limiting what a process can use (CPU, memory, I/O).

================================================================


namespace: wraps a global system resource in an abstraction that makes it appear 
to the processes within the namespace that they have their own isolated instance of the global resource.

cgroup: Control Groups provide a mechanism for aggregating/partitioning sets of tasks, and 
all their future children, into hierarchical groups with specialized behaviour.


In short:
namespaces = limits what you can see (and therefore use)
Cgroups = limits how much you can use;

