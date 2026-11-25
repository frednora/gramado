# wink

Ring 0 support for the windowing system.


Main components:

The User Interaction Interface.
The wink/ module is all about the user interaction. 
Mainly graphical and input operations.

```
----------------------------------------
gdi/  - Graphical Device Interface. Mainly output.

It basically how the UI is drawn.
It normally interacts with the display device driver;

----------------------------------------
user/ - User interface.
      - Event Interface. For i/o events.
    
User interface and user interaction.
It normally interacts with the input device device drivers.
```

wink.c will be wrapper for everything here.

------------------------------------
The goal is creating two interfaces, one for output and another one for input.


+ gdi: 
  Will be the interface with the output devices and the graphics engine (gre).
  gdi/gre.

+ user/evi: 
  Will be the interface with the input devices, like keyboard and mouse.

