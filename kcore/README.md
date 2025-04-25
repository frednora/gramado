# kcore - Core ring 0 components.

Ring 0 stuff: Boot, kernel, init process, and loadable ring 0 kernel modules.

## Folders:

```
+ boot/    - Boot loader.
+ kernel/  - 64bit kernel.
+ modules/ - Loadable ring 0 kernel modules.
```

## The initialization

```
The Bootloader loads the kernel, and the kernel loads the init process.
You can find the init process in usys/init/.
```
