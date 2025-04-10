# zbase - Non-graphical ring 3 components.

Some non-graphical ring 3 components, just like init process and posix commands.

## Folders:

```
+ udrivers/ - Loadable ring 3 drivers.
+ uservers/ - Loadable ring 3 servers.
+ usys/     - Ring 3 Init process and Posix-like commands. 
```

## Initialization

```
The kernel loads and pass the command to the init process. The init process has 
and embedded command interpreter and you can use it to load the display server. Finally the display server will load the taskbar application.
```

## Posix-like commands

You can create Posix-like commands using the library found in usys/libs/ or usys/commands/libs/.

