# Building Gramado OS

This project is only the kernel for the system. You need to 
import the commands and gui applications from other repositories.

## Repositories

```
 * https://github.com/gramado/de/ - Desktop Environment
 * https://github.com/gramado/kernel/ - Kernel
```

## Build on Linux

```bash
$ make -C de/
$ make -C games/
$ make
$ ./run
```


