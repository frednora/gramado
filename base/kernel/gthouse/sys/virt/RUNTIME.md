# Runtime Architecture

## Overview

The Runtime Architecture defines the execution hierarchy used by the operating system. 
It separates execution contexts (threads) from memory environments (TEs) and 
from higher-level execution domains (runtimes).

The design intentionally avoids coupling the scheduler to process-like objects. 
The scheduler manages only threads. Everything else is considered a resource container.

This separation simplifies the kernel design and provides a natural foundation 
for future partitioning and virtualization support.

The hierarchy is:

```text
runtime_d
    |
    +-- te_d
            |
            +-- thread_d
            +-- thread_d
```

Where:

- `runtime_d` represents an execution domain.
- `te_d` (Thread Environment, fka process) represents an address space and its resources.
- `thread_d` represents a schedulable execution context.

---

# Threads

A thread is the smallest schedulable entity in the system.

The scheduler knows only about thread objects.

A thread contains:

- CPU context
- register state
- stack information
- scheduling information
- execution state

Conceptually:

```text
thread_d
=
execution context
```

The scheduler does not schedule TEs or runtimes.

The scheduler performs operations such as:

```text
Create thread
Block thread
Wake thread
Dispatch thread
Destroy thread
```

without needing to understand which runtime or address space owns the thread.

---

# Thread Environments (TE)

A Thread Environment (TE) owns the resources required by threads.

A TE is primarily responsible for:

- address space management
- page tables
- image information
- heap information
- virtual memory resources

Conceptually:

```text
te_d
=
execution environment
```

Every thread belongs to exactly one TE.

Relationship:

```text
te_d
    |
    +-- thread_d
    +-- thread_d
    +-- thread_d
```

Multiple threads may execute inside the same TE and therefore share 
the same address space.

A TE does not need to be scheduled because only threads execute.

---

# Runtime

A Runtime is the highest-level execution domain currently defined by the system.

A Runtime groups one or more TEs.

Conceptually:

```text
runtime_d
=
runtime container
```

The Runtime is intended to be a logical boundary for:

- resource ownership
- partitioning
- future hypervisor integration
- runtime-level policies

Relationship:

```text
runtime_d
    |
    +-- te_d
            |
            +-- thread_d
```

A Runtime is not an address space.

A Runtime is not a schedulable object.

A Runtime is not a thread.

Instead, it acts as a container above TEs.

---

# Design Motivation

Traditional operating systems often use the term "process" to 
represent several different concepts:

- execution
- scheduling
- address space
- ownership
- security context

This can create confusion.

The Runtime Architecture separates those concepts:

```text
Thread
    = execution

TE
    = environment

Runtime
    = execution domain
```

This keeps responsibilities clear.

For example:

```text
Scheduler
    -> operates on threads

Memory Manager
    -> operates on TEs

Runtime Manager
    -> operates on runtimes
```

Each subsystem interacts only with the objects relevant to its responsibilities.

---

# Future Hypervisor Integration

The Runtime Architecture was designed so it can evolve toward virtualization 
without major changes.

A future hardware-assisted virtualization layer may introduce a new object:

```text
hv_partition_d
```

Hierarchy:

```text
hv_partition_d
       |
       +-- runtime_d
                |
                +-- te_d
                        |
                        +-- thread_d
```

In this model:

- `hv_partition_d` represents a hardware partition. (see: hv.h)
- `runtime_d` represents the operating system runtime.
- `te_d` represents an address space.
- `thread_d` represents execution.

The Runtime layer remains unchanged.

Only a new layer is introduced above it.

This allows the existing kernel architecture to work in:

- bare-metal mode
- software partitioning mode
- hardware-assisted virtualization mode

without redesigning the Runtime, TE, or Thread structures.

---

# Guiding Principle

The architecture follows a simple rule:

```text
Threads execute.
TEs provide environments.
Runtimes provide domains.
Partitions provide isolation.
```

By keeping these concepts independent, the system remains simple, modular, and 
adaptable to future virtualization and partitioning mechanisms.

