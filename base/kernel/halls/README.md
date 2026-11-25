# halls - Halls

  The resources. The built-in subsystems.

```
  These folders represents the built-in subsistems in the kernel core 
  that possibly can be virtualized and controlled by cgroups or namespaces 
  forming the infra-structure for a container runtime.
```

## Folders

```
 dev/      - Device drivers (application-driven)
 hcidev/   - Device drivers (user-driven)
 platform/ - BUS/plataform support
```

## dev

``` 
Application interaction - (application-driven interrupts):
 * dev/exec/  - Execution. (threads)
 * dev/per/   - Peripherals. (devices)
 * dev/req    - Requests. (memory)
```

## hcidev

```
User interaction - (user-driven interrupts):
 * hcidev/chardev/ - (Keyboard, mouse, display)
```

## platform

acpi, pci, usb.