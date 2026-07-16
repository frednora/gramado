# Gramado OS build instructions

## Fred's host machine

```
    Host machine: Ubuntu 20.04.5 LTS
    Linux 5.4.0-146-generic x86_64
    gcc (Ubuntu) 9.4.0 
    GNU ld (GNU Binutils for Ubuntu) 2.34
    NASM version 2.14.02
```
```
    Host machine: Ubuntu 22.04.2 LTS
    Linux 5.15.0-75-generic x86_64
    gcc (Ubuntu) 11.3.0 
    GNU ld (GNU Binutils for Ubuntu) 2.38
    NASM version 2.15.05
```
```
    Host machine: Ubuntu 22.04.2 LTS
    Linux 5.15.0-79-generic x86_64
    gcc (Ubuntu) 11.4.0 
    GNU ld (GNU Binutils for Ubuntu) 2.38
    NASM version 2.15.05
```
```
    Host machine: Ubuntu 22.04.2 LTS
    Linux 5.15.0-84-generic x86_64
    gcc (Ubuntu) 11.4.0 
    GNU ld (GNU Binutils for Ubuntu) 2.38
    NASM version 2.15.05
```


## Linux dependencies

**Debian / Ubuntu**
```console
sudo apt install build-essential nasm make qemu-system-x86-64 qemu-system-gui qemu-utils 
```

## Build on Linux

```console
$ make
$ ./run
```

The command 'make' will compile the Gramado OS and install the files 
into the 'base/' directory. 

After that, You can run the disk image using the command './run' or 
the command './runnokvm'. 
You also can run the disk image using the Virtualbox.

To clean the mess you can use the command 'make clean-all'.

