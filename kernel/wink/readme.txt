
Main components:

gre  - Graphics output.
user - User input.

gramk.c will be wrapper for everything here.

------------------------------------
The goal is creating two interfaces, one for output and another one for input.

+ gdi: 
  Will be the interface with the output devices and the graphics engine (gre).
  gdi/gre.
+ user: 
  Will be the interface with the input devices, like keyboard and mouse.

------------------------------------
Dependencies:
This module has a strong dependency of some components:
+ dev/chardev/console
+ dev/chardev/display
+ dev/chardev/i8042
+ dev/chardev/tty