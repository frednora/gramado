# kernel - The kernel image

This directory contains the code for the base kernel. It builds the image KERNEL.BIN.

## Gramado Kernel Hierarchy

```
From lowest to highest:
 + (0) gthouse/arch/
 + (1) halls/exec/ke/hal/
 + (2) halls/exec/intake/
 + (3) gthouse/kwrap/
 + (4) halls/ 
 + (5) gthouse/wink/
 + (6) gthouse/libk/
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
