# core

  The core components of the kernel (The controllers) and applications.

## Folders

```
  * arch  - Kernel entry
  * ke    - Top level kernel functions.
            (Kernel Executive Layer)
            provides the mechanisms (scheduling, hardware abstraction).

  * kmain - Main functions. Wrappers.
            (Kernel Management Layer)
            defines the policies (resource limits, namespaces, virtualization).
```

## Pupose

```
[HOT STUFF]
The purpose here is putting together all the things related with interrups and syscalls.

Timer interrups: (The main thing)
arch/ has the handler for timer interrupt that will call the PIT and 
end up into ke/intake/, where ts.c (Task Switch routine) will call 
disp/ (Dispatcher) and sched/ (Scheduler).

Syscalls: (The second main thing)
arch/ has the handler for the syscalls and 
kmain/sci/ is the interface for all the syscalls. 
```

## Pupose 2

```
kmain/cont/ has the container stuff.
kmain/virt/ has the virtualization stuff.
```

## Pupose 3

```
kmain/mod/ has the interface for the kernel modules.
```

## Pupose 4

```
arch/ has the entry point to the kernel, that is gonna call the I_kmain 
for machine independent initialization.
```

