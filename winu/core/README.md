# core

The core components for the windowing system.

```
 + ds/   - 2D display servers.
 + ds3d/ - 3D display server.
           It has embedded 3D demo.
```

## This is a screenshot of GramadoDE desktop environment running on top of the kernel.
![Screenshot](https://raw.githubusercontent.com/polard8/screenshots/main/gramado-8.png)

## This is a screenshot of 3D demo running on top of the kernel.
![Screenshot](https://raw.githubusercontent.com/polard8/screenshots/main/gramado-3.png)

```
  The main goal of this project is to create a set of programs 
the forms the basic graphical interface for the user. Just like,
the Taskbar and the Virtual terminal.
  The core component of this project is the display server, found in the
folder ds/ds00/.
```

> * ds     - Display servers

```
ds/ folder is the place for the display servers. The default display server is ds00.
```

## ds3d - A place for 3D display server with demos.

Here you can find some 3D demos that run in the same place of the display server.
So, instead of initializing the display server you can initialize a demo.
