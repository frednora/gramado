# gramk 


```
Main components:
#todo: Maybe we can provide two 'devices' that can be open
in order to the ring 3 applications make configurations 
on these two subsystems.
ex: /dev/ev/
ex: /dev/gd/

evi  - [Input support]  Event Interface,.
gdi  - [Output support] Graphics device interface.
user - User configuration.

gramk.c will be wrapper for everything here.

------------------------------------
The goal is creating two interfaces, one for output and another one for input.

+ evi: 
  Will be the interface with the input devices, like keyboard and mouse.

+ gdi: 
  Will be the interface with the output devices and the graphics engine (gre).
  gdi/gre.

```