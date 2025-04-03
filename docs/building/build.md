# Building Gramado OS

This project is only the kernel for the system. You need to 
import the commands and gui applications from other repositories.

## Repositories

Order: (Boot; Kernel; Userland; Data and tools)

```
distros/ - Building full distributions into this directory.
docs/    - The documentation.
os/      - Client-side GUI applications.
windows/ - The display server.
zbase/   - Init process, ring 3 drivers and ring 3 servers.
zcore/   - Boot, kernel and ring 0 modules.
```

## Build on Linux

```bash
$ make
$ ./run
```


