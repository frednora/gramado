# garden - Entries

The entry point for the kernel. 

```
+ Startup routine for the kernel.
+ ISR/IRQ routines for the handlers.
+ Network infrastructure

```

## Folders

```
ap/      - AP processors
bsp/     - BSP processors
gramnet/ - Top level protocols (API)
net/     - Low level network infrastructure.
           (firewall, protocols and sockets)
```

## Purpose

```
+ Entrypoint during the BSP/AP startup.
+ Entrypoint during traps and syscalls.
+ The main routine in C language for the BSP processor.
+ The main routine in C language for the AP processor.
```
