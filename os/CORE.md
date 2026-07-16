# core - Core components

This is the list of the core components for the system.


## Core System (P)

```
boot/    - The boot loader.
kernel/  - The base kernel. (hypervisor/supervisor)
modules/ - Kernel modules.
moon/    - The init process. (The first ring 3 process)

```

```
This creates a very clear startup flow:
    boot → kernel → modules → moon (init)

The moon is our starting point now. It is our init process. INIT.BIN.
```

## Native UI (S)

```
hunter/  - Compositor (display server)
ifarmer/ - Client-side GUI applications
```

## Nomad Pastors (E)

```
np/
```


