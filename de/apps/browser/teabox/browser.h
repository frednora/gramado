// browser.h
// Created by Fred Nora.

#ifndef __BROWSER_H
#define __BROWSER_H    1


// rtl 
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <sys/socket.h>
#include <base/packet.h>
// The client-side library.
#include <gws.h>

//#include "your/assets/..."

#include "globals.h"

#include "00ui/ui.h"
#include "01net/net.h"
#include "02engine/engine.h"

//
// It includes all the demo stuff.
// #include "02engine/box/src/gram3d.h"
//

// rtl
#include <limits.h>
#include <sys/types.h>
#include <sys/cdefs.h>
#include <stddef.h>
#include <rtl/gramado.h>
#include <math.h>

// #internal representation of the protocol indexes.
#include "02engine/box/src/protint.h"
#include "02engine/box/src/engglob.h"
#include "02engine/box/src/colors.h"
#include "02engine/box/src/nc2.h"   //nelson cole 2 font. 8x8
// Graphics device library.
#include <libdisp.h>

#include "02engine/box/src/char.h"
#include "02engine/box/src/dtext.h"
#include "02engine/box/src/line.h"

#include "02engine/box/src/window/wt.h"
#include "02engine/box/src/menu.h"
#include "02engine/box/src/xxxinput.h"   //#test
#include "02engine/box/src/window/window.h"
#include "02engine/box/src/bmp.h"

// osdep/
// X11 "Host:Display.Screen" pattern.
// h:0.0
#include "02engine/box/src/osdep/screen.h"   // X11-like
#include "02engine/box/src/osdep/display.h"  // X11-like
#include "02engine/box/src/osdep/host.h"     // X11-like h:0.0

// Teremos isso aqui no window server,
// kgws é outro ws par ao ambiente de setup e tera 
// seu próprio gerenciamento.
#include "02engine/box/src/security.h"
#include "02engine/box/src/gui.h"
#include "02engine/box/src/comp.h"
#include "02engine/box/src/event.h"    // view inputs

#include <grprim0.h>   // Common for eng/ and gramland/.
#include <libgr.h>     // Common for eng/ and gramland/.
#include <grprim3d.h>  // Only for eng/
#include <libgr3d.h>   // Only for eng/
#include "02engine/box/src/grprim.h"
#include "02engine/box/src/camera.h"
#include "02engine/box/src/world.h"


// Projection support using Float values.
#include "02engine/box/src/projf.h"
// Projection support using Integer values.
#include "02engine/box/src/projint.h"

#include "02engine/box/src/sprite.h"
#include "02engine/box/src/surface.h"
#include "02engine/box/src/models.h"

#include "02engine/box/src/demos.h"


#include "02engine/box/src/osdep/packet.h"

// #osdep
// #todo:
// We can put this thing in the library. 
// (libgws) or (libcon)
#include "02engine/box/src/osdep/connect.h"


#include "02engine/box/src/font.h"


// Client structure.
// O proprio servidor poderia ser o cliente 0??
#include "02engine/box/src/osdep/client.h"
// # model. business logic
#include "02engine/box/src/services.h"


// Device Context.
// This is the structure that is gonna be used by the
// drawing routines.
// 'dc->something'
// It needs to be the last one.
#include "02engine/box/src/osdep/dc.h"


#include "02engine/box/src/gram3dint.h"

//#important
// Do not include gram3d.h. It's redundant.

#endif   


