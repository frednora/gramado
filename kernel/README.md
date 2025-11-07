# kernel - The kernel image

This directory contains the code for the base kernel. It builds the image KERNEL.BIN.

## Gramado Kernel Hierarchy

```
From lowest to highest:
 + (0) core/arch/
 + (1) core/ke/hal/
 + (2) core/ke/intake/
 + (3) core/ke/main/
 + (4) res/ 
 + (5) winu/
 + (6) libk/
```

## Kernel initialization

The kernel initialization is handled by the function I_kmain(), which follows these steps:

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
core/:
  The core of the kernel; the primary processing unit.
  core/deps   - Ring 0 modules, POSIX commands.
  core/gdeps  - Client-side GUI applications.
  core/arch   - Entry point and initialization.
  core/ke     - Task manager.
  core/kmain  - Main initialization routine and wrappers.

include/:
  Main header files for the kernel.

libk/:
  The kernel library.

res/:
  Kernel resources for containers.

wink/:
  Interface for the graphics device and the user interaction manager.
```

## About the core/ Folder

```
This folder primarily contains CPU and process management code.
```

## About the res/ folder:

```
The res/ directory is based on the idea of managing kernel resources used by process groups. All such resources are organized within this folder.

The core/ folder is responsible for controlling access to the resources in res/. The memory management component (mm/) is also considered a resource, so it belongs under res/ rather than as a component of core/.
```
      
