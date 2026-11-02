# kernel

The kernel is devided into two main layers: The kernel layer and the executive layer.

# The kernel layer

```
The kernel is divided into two main folders:

+ garden/entry/      - The entry environment
  Here we have the entrypoints and the main routines.
  The interrupt handlers are found here.

+ halls/dev/exec/ke/ - The execution enviroment
  Here we have the execution environment.
  The dispatcher and the scheduler are found here.
  Inter-Process Communication is found here.

```

# The executive layer


```
Higher level abstractions that are exposed to the user via syscall.
These abstractions are organized via prefixes. The main prefixes are:
sys_xxxx() and go_xxxx() used during the syscalls. But the other prefixes 
are also part of these abstractions.

```


