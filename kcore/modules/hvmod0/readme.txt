
    Project: GRAMKRNL.BIN

    The goal here is that in the future,
    this is gonna be the kernel for the os in the
    root partition of a virtualized environment.
    But for now, this is only a loadable ring0 module 
    for the hypervisor.

----------------------------------------
old comment:

    This is a loadable module for the kernel process. 
    It was compiled in a static address and was loaded by the kernel itself.
    The kernel is gonna dynamic link some symbols and call some functions in this program.

    This module cannot use the rtl libc, it is because we don't wanna use syscalls in ring0. 
    So, this way, the module needs to link to the kernel.

    #todo
    Describe here how the initialization works, how the kernel can call the services here and 
    how this program can call the kernel services.
