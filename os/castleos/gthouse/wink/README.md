# wink

Ring 0 support for the windowing system.

```
Main components:

evi  - [Input support]  Event Interface.
gdi  - [Output support] Graphics device interface.
user - User configuration.

wink.c will be wrapper for everything here.

------------------------------------
The goal is creating two interfaces, one for output and another one for input.

+ evi: 
  Will be the interface with the input devices, like keyboard and mouse.

+ gdi: 
  Will be the interface with the output devices and the graphics engine (gre).
  gdi/gre.

```