# k08

Some OS main components that are not the kernel image itself.

## The initialization

```
The Bootloader loads the kernel image, and the kernel loads the init process.

  * boot  - Boot loader.
  * deps  - kernel module, Init process and ring 3 posix-commands.
  * gdeps - Client-side GUI applications.
```
