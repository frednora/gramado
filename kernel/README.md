# kernel - The kernel image

This directory contains the code for the base kernel. It builds the image KERNEL.BIN.

# Folders

```
kernel/
 ├── garden/      #
 │    ├── arch/   # Architecture-specific boot & interrupt stubs
 ├── gthouse/     # Gatehouse: entry point, stubs, init, wrappers, libk, UI
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
// Levels:
// + [1]   earlyinit()
// + [2:0] mmInitialize(0)
// + [2:1] mmInitialize(1)
// + [3:0] keInitialize(0)
// + [3:1] keInitialize(1)
// + [3:2] keInitialize(2)
// + [4]   archinit()
// + [5]   deviceinit()
// + [6]   lateinit()
// ==================================
```


## Folders

```
gthouse/
  gthouse/kwrap/ - Main initialization routine and wrappers.
  gthouse/libk/  - The kernel library.
  gthouse/wink/  - Interface for the graphics device and the user interaction manager.

halls/ - Kernel resources for containers.
  The core of the kernel; the primary processing unit.
  exec/arch   - Entry point and initialization.
  exec/ke     - Task manager.

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
