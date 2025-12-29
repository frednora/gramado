# halls - Halls

  The resources. The built-in subsystems.

```
  These folders represents the built-in subsistems in the kernel core 
  that possibly can be virtualized and controlled by cgroups or namespaces 
  forming the infra-structure for a container runtime.
```

## Folders


``` 
 Application interaction
 * apphalls/exec/  - Execution. (threads)
 * apphalls/per/   - Peripherals. (devices)
 * apphalls/req    - Requests. (memory)
```

```
 User interaction
 * uhalls/chardev/ - (Keyboard, mouse, display)
```

