# Gramado OS

Gramado OS is a 64-bit graphical operating system. This project includes the bootloader, the core kernel, and the init process. Please note that Gramado OS kernel is in a pre-alpha state and is only suitable for use by developers.

![Screenshot](https://raw.githubusercontent.com/igapo/screenshots/main/kernel-01.png)

## Overview

You can install a desktop environment on top of the kernel. The default desktop environment can be found in the `xde/` folder.

### What is Gramado OS Kernel?

This project includes the bootloader, the core kernel, and the init process.

### What is Gramado Operating System?

Gramado OS is a hobby operating system, not a commercial operating system. It is small and has only a few features.

## Features

### Kernel Features

- **Hardware:**
  - CPU: Intel and AMD, 64-bit only.
  - Memory Management: Paging with 2MB chunks using 4KB pages.
  - Block Device: IDE (Primary master only).
  - Character Devices: PS/2 keyboard, PS/2 mouse (works fine only on QEMU), Serial port (COM) for debugging.
  - Network Device: e1000 Intel NIC (works on Oracle VirtualBox).

- **Software:**
  - Processes: Process and Thread structure.
  - Scheduler: Round robin (Threads only).
  - Syscalls: Using software interrupts (Traps).
  - IPC: Sockets and system messages using a queue in the thread structure.
  - TTY: 4 ring 0 kernel virtual consoles in fullscreen raw mode and general use TTYs and PTYs for ring 3 virtual terminals.
  - Filesystem: FAT16 file system for boot partition (no system partition yet).
  - POSIX libc: Ring 0 implementation of libc functions called by the ring 3 libraries.
  - Networking: Small support for sockets and a small ring 0 implementation of protocol stack (ETHERNET, ARP, IP, UDP, TCP, and DHCP).
  - Display: Bootloader display device (VESA).
  - User: Very small implementation of user structure.
  - APIs and one loadable ring 0 module, using static address.

### Userland Features

- Display Server.
- Unix-like commands running in the virtual console.
- Some clients connected to the display server via UNIX sockets.
- Ring 3 processes can access the I/O ports via syscall (for ring 3 drivers only).

## Source Code

You can find the source code on [GitHub](https://github.com/frednora/gramado), where you can clone the repository, contribute to the project, or download the code. Dependencies for the display server and client-side GUI applications can be found [here](https://github.com/frednora/gramado/xde).

## Directory Structure

- `boot/`: The bootloader.
- `kernel/`: The core kernel.
- `mods/`: Kernel modules.
- `udrivers/`: Ring 3 device drivers.
- `uservers/`: Ring 3 servers.
- `usys/`: Init process and commands.
- `xde/`: Desktop Environment.
- `xgames/`: 3D demos and games.
- `your/`: Your stuff.

## Building and Running on VM

```bash
$ make -C xde/
$ make -C xgames/
$ make
$ ./run
```

### Clean Up

```bash
$ make clean-all
```

### Build Environment

Compiled on Ubuntu operating system, LTS releases. Example configurations:

- Windows 10, WSL2 with Ubuntu
  - Linux kernel: 5.15.146.1-microsoft-standard-WSL2
  - GCC: 11.4.0
  - GNU ld: 2.38
  - NASM: 2.15.05

- Ubuntu 20.04.5 LTS
  - Linux: 5.4.0-146-generic x86_64
  - GCC: 9.4.0
  - GNU ld: 2.34
  - NASM: 2.14.02

- Ubuntu 22.04.2 LTS
  - Linux: 5.15.0-78-generic x86_64
  - GCC: 11.4.0
  - GNU ld: 2.38
  - NASM: 2.15.05

## Testing

### Virtual Machine

You can test the system on a virtual machine (QEMU, QEMU with KVM, and VirtualBox).

### Real Machine

Testing on real machines is encouraged to improve the system. Previous versions were tested on a Gigabyte machine with an Intel Core 2 Duo processor and a VIA chipset.

## Feedback

We need feedback from users and developers. Please leave comments on GitHub or send messages to Fred Nora.

## Author

- **Fred Nora** - [X Account](https://x.com/frednora), [Facebook](https://facebook.com/frednora)

## Contributors

- **Fred Nora** - [X Account](https://x.com/frednora), [Facebook](https://facebook.com/frednora)

## How to Build?

See the [Gramado OS build instructions](https://github.com/frednora/gramado/blob/main/your/docs/build/build.md).

## Documentation

Documentation and design notes can be found in the `docs/` folder. See the [docs](https://github.com/frednora/gramado/tree/main/your/docs). Contributions to improve documentation are welcome.

## License

Gramado OS is a Free and Open Source operating system. The source code uses the MIT license.

---
