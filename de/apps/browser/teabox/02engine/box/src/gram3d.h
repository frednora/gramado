// gram3d.h 
// Main header for the project.
// Created by Fred Nora.

// ...
extern int current_mode;

// rtl
#include <types.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/cdefs.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <rtl/gramado.h>

//#test
#include <math.h>

// Internal protocol
// Remember, the lib has the same protocol
// but it doesn't use the same names.
#include "protint.h"

#include "engglob.h"

#include "colors.h"
#include "nc2.h"   //nelson cole 2 font. 8x8

// Graphics device library.
#include <libdisp.h>

#include "char.h"
#include "dtext.h"
#include "line.h"

#include "window/wt.h"
#include "menu.h"
#include "xxxinput.h"   //#test
#include "window/window.h"
#include "bmp.h"

// osdep/
// X11 "Host:Display.Screen" pattern.
// h:0.0
#include "osdep/screen.h"   // X11-like
#include "osdep/display.h"  // X11-like
#include "osdep/host.h"     // X11-like h:0.0

// Teremos isso aqui no window server,
// kgws é outro ws par ao ambiente de setup e tera 
// seu próprio gerenciamento.
#include "security.h"
#include "gui.h"
#include "comp.h"
#include "event.h"    // view inputs

#include <grprim0.h>   // Common for eng/ and gramland/.
#include <libgr.h>     // Common for eng/ and gramland/.
#include <grprim3d.h>  // Only for eng/
#include <libgr3d.h>   // Only for eng/
#include "grprim.h"
#include "camera.h"
#include "world.h"

// Projection support using Float values.
#include "projf.h"
// Projection support using Integer values.
#include "projint.h"

#include "sprite.h"
#include "surface.h"
#include "models.h"

#include "demos.h"

#include "osdep/packet.h"

// #osdep
// #todo:
// We can put this thing in the library. 
// (libgws) or (libcon)
#include "osdep/connect.h"

#include "font.h"

// Client structure.
// O proprio servidor poderia ser o cliente 0??
#include "osdep/client.h"
// # model. business logic
#include "services.h"

// Device Context.
// This is the structure that is gonna be used by the
// drawing routines.
// 'dc->something'
// It needs to be the last one.
#include "osdep/dc.h"

#include "gram3dint.h"

//
// End
//


