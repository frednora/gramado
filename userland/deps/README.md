# deps - Non-graphical ring 3 components.

Ring 3 stuff: Some non-graphical ring 3 components, just like init process and posix commands.

## Main folders

```
libs/ - Libraries used by the ring 3 projects in deps/.
init/ - The init process.
cmds/ - Posix-like commands.
```

## Extra folders

```
drivers/ - Ring 3 drivers.
servers/ - Ring 3 servers.
zres/    - Resources.
```

## Initialization

```
The kernel loads and pass the command to the init process. The init process has 
and embedded command interpreter and you can use it to load the display server. Finally the display server will load the taskbar application.
```

## Posix-like commands

You can create Posix-like commands using the library found in cmds/alpha/.

