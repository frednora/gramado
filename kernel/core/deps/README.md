# deps - Dependencies 

Source code for non-kernel stuff. Actually the kernel only depends on the Init process, the rest is OS dependencies.

## Dependencies in kernel/core/deps/

```
# #test
# Putting the dependencies inside the kernel source tree.
# The OS has two major components:
# The 'kernel image' and the 'dependencies'
# The dependencies are: boot loader, modules, and apps.
# All the dependencies are in kernel/core/deps/ folder,
# It's because of the close interaction 
# with the other subfolders in kernel/core/.
# Where is the shell?, Where is the utilities? (by Unix guys) 
```

## Dependencies

```
Core os components:

----------------------
>> Ring 0
kcore/:
 - Kernelmode core componets
 - Boot, ring 0 modules

----------------------
>> Ring 3
ucore/
 - Usermode core componets
   Init process, ring 3 drivers and ring 3 servers.

```

## Where is the boot loader?

kernel/core/deps/kcore/boot/

## Where is the kernel?

kernel/

## Where are the ring 0 kernel modules?

kernel/core/deps/kcore/modules/

## Where is the Init Process?

kernel/core/deps/ucore/init/

## Where is the ring 3 device drivers?

kernel/core/deps/ucore/drivers/

## Where is the ring 3 servers?

kernel/core/deps/ucore/servers/

## Where is the display server?

kernel/core/gdeps/preshell/ds/ds00/

## Where are the POSIX-like command programs?

kernel/core/gdeps/shell/shell00/

## Where are the client-side GUI applications?

kernel/core/gdeps/shell/shell01/

