# gramk 


```
Main components:

con  - Console interface.
       see: The console infrastructure is found in dev/.

gdi  - [Output support] Graphics device interface.
user - [Input support]  User interaction interface.

gramk.c will be wrapper for everything here.

------------------------------------
The goal is creating two interfaces, one for output and another one for input.

+ con:
   Console interface.
   see: The console infrastructure is found in dev/.

+ gdi: 
  Will be the interface with the output devices and the graphics engine (gre).
  gdi/gre.

+ user: 
  Will be the interface with the input devices, like keyboard and mouse.
```