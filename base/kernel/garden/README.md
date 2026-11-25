# garden - Entries

The entry point for the kernel. 

```
It's a place considered 'outside'.

+ Startup routine for the kernel.
+ ISR/IRQ routines for the handlers.
+ Network infrastructure.

```

## Folders

```
entry/   - Entry point and startup routines for BSP and AP processors.
         - ISR/IRQ low level handlers.
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
