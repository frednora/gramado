// gws.h 
// Gramado Window System (GWS)
// Master file for the client-side library. 
// The main goal for this routines is to send requests to the 
// display server, Gramland.
// 2020 -  Created by Fred Nora.

//
// == Definitions ===========================================
//

// Basic type definitions

#include "gwsdefs.h"  // Definitions and types

//
// == Base ===========================================
//

// Basic system components
// Connection support

#include "version.h"
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
// For asynch requests.
#include "async.h" 
// The lingws protocol.
#include "protocol.h"
#include "grambase.h"

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

