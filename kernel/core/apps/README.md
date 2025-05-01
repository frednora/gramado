# apps - Not the kernel 

```
Core os components:

kcore/:
 - Kernel Core componets
kernel/core/apps/:
 - Boot, ring 0 modules, posix commands.
 - Display server and client-side GUI applications.

kservices/
 - System services. 
   Init process, ring 3 drivers and ring 3 servers.

Desktop environment components:

preshell/ 
  Shell Pre-UI:
  The display server.
  3D demos. A game engine running on top of the kernel,
  that works like a display server.

shell/
  - Shell UI:
    Posix-like commands to run on virtual terminals.
    Client-side GUI applications.
    (DS00 SUBSYSTEM applications)
    (KERNEL CONSOLE SUBSYSTEM applications)
    (pubsh.bin runs on PUBTERM SUBSYSTEM)
    (shell.bin runs on TERMINAL SUBSYSTEM)

Distro and documentation:

distros/ - Building full distributions into this directory.
docs/    - The documentation.

```

