# kernel - The kernel image

This directory contains the code for the base kernel. It builds the image KERNEL.BIN.

# Folders

```
kernel/
 ├── gthouse/     # Gatehouse: entry point, stubs, init, wrappers, libk, UI
 │    ├── arch/   # Architecture-specific boot & interrupt stubs
 │    ├── kwrap/  # Kernel wrappers (high-level abstractions)
 │    ├── libk/   # Kernel library (shared utilities)
 │    └── wink/   # User interface (input events, GDI)
 ├── halls/       # Castle halls: execution, requests, peripherals
 │    ├── exec/   # CPU Hall: threads, dispatcher, scheduler, exceptions
 │    ├── per/    # Peripheral Hall: external devices, drivers
 │    └── req/    # Memory Hall: syscalls, memory management (mm/)
 └── include/     # Castle library: headers, definitions, shared interfaces
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
