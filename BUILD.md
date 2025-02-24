# Building Gramado OS

This project is only the kernel for the system. You need to 
import the commands and gui applications from other repositories.

## Repositories

Order: (Boot; Kernel; Userland; Data and tools)

```
  * boot/    - The bootloader.
  * kernel/  - The core kernel.
  * mods/    - Kernel modules.
  * udrivers - Ring3 device drivers.
  * uservers - Ring3 servers.
  * usys/    - Init process and commands.
  * xde/     - Desktop Environment.
  * xgames/  - 3D demos and games.
  * your/    - Your stuff.
```

## Build on Linux

```bash
$ make -C xde/
$ make -C xgames/
$ make
$ ./run
```


