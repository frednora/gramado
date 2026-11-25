# kernel - The kernel image

This directory contains the code for the base kernel. It builds the image KERNEL.BIN.

# Folders

```
garden/  - entrypoint
gthouse/ - top level wrappers (Interfaces)
halls/   - hardware abstractions
```


```
#todo: We need to update this
kernel/
 ├── garden/      #
 │    ├── entry/  # Architecture-specific boot & interrupt stubs
 |    |           # entry point
 ├── gthouse/     # Gatehouse: stubs, init, wrappers, libk, UI
 │    ├── kwrap/  # Kernel wrappers (high-level abstractions)
 │    ├── libk/   # Kernel library (shared utilities)
 │    └── wink/   # User interface (input events, GDI)
 |
 ├── halls/       # Castle halls: execution, peripherals, requests
 |    ├── uih: chardev/#  
 │    ├── aih: exec/   # CPU Hall: threads, dispatcher, scheduler, exceptions
 │    ├── aih: per/    # Peripheral Hall: external devices, drivers
 │    └── aih: req/    # Memory Hall: syscalls, memory management (mm/)
 |
 └── include/     # Castle library: headers, definitions, shared interfaces
```

```
Garden → the outermost layer, handling the entry points (startup, syscalls, interrupts). It’s like the “front yard” where everything first arrives before being directed inward.

Gate House → the controlled entrance, wrapping and dispatching requests into the kernel. This is essentially your abstraction layer, ensuring external calls are translated into internal service invocations.

Halls → the inner chambers where the actual services live, such as device drivers and core implementations. This is where the real work happens once requests have passed through the gate.
```

```
In Halls:
+ program-driven events: (application interaction)
+ human-driven events: (user interaction)
```


## Where is the kernel?

```
(The kernel proper)

halls/dev/exec/ke/intake/:
+ Inter-process communication (IPC)
+ Dispatcher 
+ Scheduler
+ Low-level synchronization primitives

garden/entry/x86_64/ints/:
+ Interrupt and exception handling
```

```
(System service interface)

+ gthouse/sys/sci/sci.c
+ gthouse/sys/sys.c
```


## Kernel initialization

```
 See: kmain.c
 The first function for the BSP is I_main, but the real main routine 
 for all the processors is I_initialize_kernel().
 The kernel initialization is handled by the function I_initialize_kernel, 
 which follows these steps:
```

```
// ==================================
// Main functions during the initialization:
// + ... startup_64() in x86_64/head_64.asm
// + ... start_kernel() in kmain.c       (wrapper)
// + ... x86_64_start_kernel in kmain.c  (wrapper)
// + [x] I_kmain() in kmain.c            (real initialization)
//
// Main function called by I_kmain(): in kmain.c
// + [1] I_initialize_kernel()
// + [2] I_x64CreateKernelProcess()
// + [3] I_x64CreateInitialProcess()
// + [4] ke_x64ExecuteInitialProcess()
//
// Main functions called by I_initialize_kernel(): in kmain.c
// + [1:1]   earlyinit()
// + [1:2:0] mmInitialize(0)
// + [1:2:1] mmInitialize(1)
// + [1:3:0] keInitialize(0)
// + [1:3:1] keInitialize(1)
// + [1:3:2] keInitialize(2)
// + [1:4]   archinit()
// + [1:5]   deviceinit()
// + [1:6]   lateinit()
// ==================================
```


## Folders

```
garden/          - Entries
    entry/

gthouse/
    fs/
    gramnet/
    libk/  - The kernel library.
    net/
    sys/
    wink/  - Interface for the graphics device and the user interaction manager.
    ...

halls/ - Kernel resources for containers.
  The core of the kernel; the primary processing unit.
    chardev/
    dev/
    platform/

include/:
  Main header files for the kernel.
```

## About the halls/ folder:

```
The halls/ directory is based on the idea of managing kernel resources used by process groups. All such resources are organized within this folder.

```

## About the halls/exec/ Folder

```
This folder primarily contains CPU and process management code.
```
