# halls - Halls

  The resources. The built-in subsystems.

```
  These folders represents the built-in subsistems in the kernel core 
  that possibly can be virtualized and controlled by cgroups or namespaces 
  forming the infra-structure for a container runtime.
```

## Folders


``` 
Application interaction - (application-driven interrupts):
 * dev/exec/  - Execution. (threads)
 * dev/per/   - Peripherals. (devices)
 * dev/req    - Requests. (memory)
```

```
User interaction - (user-driven interrupts):
 * dev/chardev/ - (Keyboard, mouse, display)
```

