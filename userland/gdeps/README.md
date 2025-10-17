# gdeps

Graphical dependencies for Gramado OS.

```
preshell - Display server and 3D demos.
shell    - Client-side GUI applications.
```

```
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
```