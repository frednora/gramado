// libx.h 
// Gramado Window System.
// Master file for the client-side library. 
// The main goal for this routines is to send requests to the 
// display server, Gramland.
// 2020 -  Created by Fred Nora.

//
// == Base ===========================================
//

// Basic components.
// Sometimes OS dependent components.
// Connection support.

#include "version.h"
#include "gwsdefs.h"  // Definitions and types.
//h:d.s
#include "base/screen.h"
#include "base/display.h"
#include "base/host.h"
// Network ports used by the socket routines.
#include "base/ports.h"
// Part of the connection/communication support.
#include "base/packet.h"
// Read and write from socket.
#include "base/rw.h"
#include "base/connect.h"
// The lingws protocol.
#include "protocol.h"
#include "grambase.h"

//
// == Gr ===========================================
//

// Client-side 3D graphics support.
#include "gr/gr.h"

//
// == User ===========================================
//

// This is the part when we support
// the interaction with the user.

#include "user/vk.h"        // Virtual keys
#include "user/wm.h"        // Window messages
#include "user/wt.h"        // Window types
#include "user/colors.h"    // Colors
#include "user/window.h"    // Windows
#include "user/menu.h"      // Menu
#include "user/events.h"    // Events
#include "gramuser.h"

//
// End
//

