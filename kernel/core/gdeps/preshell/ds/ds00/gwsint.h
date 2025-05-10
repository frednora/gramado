// gwsint.h 
// Internal definitions for the Display Server.
// Created by Fred Nora.

// Version?
// See protocol version in protocol.h.


extern int os_mode;      // GRAMADO_P1 ...
extern int server_mode;  // DEMO ...

// rtl
#include <types.h>
#include <sys/types.h>
#include <sys/cdefs.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <rtl/gramado.h>

// Gramado Window System
#include "ui/gws.h"
#include "libos/gramado/callback.h"

// Configuration and debug support.
#include "config/config.h"
#include "protocol.h"
#include "async.h"
#include "z8server/globals.h"
#include "ui/colors.h"
#include "ui/themes/humility.h"
#include "ui/themes/honey.h"

// #imported
// Display device library.
#include <libdisp.h>

#include "libui/line.h"

#include "ui/wt.h"
#include "ui/menu.h"
//#include "zres/grinput.h"   //#test
#include "ui/metrics.h"

// #test: Create a room for rect support.
#include "libui/rect.h"

#include "ui/window.h"

// Draw inside the windows.
#include "libui/char.h"
#include "libui/dtext.h"


#include "ui/bar.h"    // Notification bar. (yellow)
#include "ui/wa.h"     // Working Area
#include "ui/swamp.h"  // Swamp

#include "ui/wm.h"
#include "ui/wminput.h"


#include "libui/bitblt.h"

#include "ui/vdesktop.h"
#include "ui/painter.h"
#include "ui/bmp.h"

// h:0.0
#include "libos/gramado/screen.h"   // X-like
#include "libos/gramado/display.h"  // X-like
#include "libos/gramado/host.h"     // X-like h:0.0

#include "libos/gramado/surface.h"
#include "libos/gramado/gramado.h"


#include "ui/gui.h"

// Compositor
#include "uicomp/comp.h"

// view inputs
#include "ui/event.h"

// #imported

#include <grprim0.h>
#include <libgr.h>

#include "libui/grprim.h"
#include "libui/camera.h"
#include "libui/proj.h"

#include "ui/sprite.h"
#include "ui/demos.h"

#include "libos/gramado/packet.h"
#include "libos/gramado/connect.h"

#include "ui/font.h"

// Client structure.
// O proprio servidor poderia ser o cliente 0??
#include "z8server/client.h"

#include "z8server/shutdown.h"

// Device Context.
// This is the structure that is gonna be used by the
// drawing routines.
// 'dc->something'
// It needs to be the last one.
#include "libos/gramado/dc.h"

// The services called by the dispacher.
#include "service.h"


struct gws_graphics_d
{
    int used;
    int magic;
    struct gui_d  *gui;
    // ...
};
extern struct gws_graphics_d *Currentgraphics;


struct engine_d
{

// flag: When to quit the engine.
// We can quit the engine and reinitialize it again.
    int quit;
// Engine status
    int status;
// graphics support.
    struct gws_graphics_d *graphics;
    // ...
};
extern struct engine_d  Engine;


// MAIN STRUCTURE
// This is the main data structure for the display server.

struct display_server_d 
{
    int initialized;

// The name of the display server.
    char name[64];
    char edition_name[64];

    char version_string[16];

    unsigned long version_major;
    unsigned long version_minor;

// fd
    int socket;

// flag: When to quit the display server.
    int quit;

// display server status
    int status;

// sinaliza que registramos o servidor no sistema.
    int registration_status;
    int graphics_initialization_status;
    // ...

// Se devemos ou não lançar o primeiro cliente.
    int launch_first_client;

// graphics engine 
    struct engine_d *engine;
    
    // os info.
    // input support
    // ...
};

//see: main.c
extern struct display_server_d  *display_server;

#define STATUS_RUNNING    1

//
// == Prototypes =============================
//

//
// Function in main.c
//

int server_is_accepting_input(void);
void server_set_input_status(int is_accepting);
void server_debug_print (char *string);
unsigned long server_get_system_metrics (int index);
void server_enter_critical_section (void);
void server_exit_critical_section (void);
void server_quit(void);
char *gwssrv_get_version(void);

// --------------------------

void invalidate(void);
void validate(void);
int isdirty(void);

void invalidate_background(void);
void validate_background(void);
int is_background_dirty(void);

void gwssrv_set_keyboard_focus(int window);

void xxxThread (void);
void ____test_threads (void);

void *gwssrv_create_thread ( 
    unsigned long init_eip, 
    unsigned long init_stack, 
    char *name );

void gwssrv_start_thread (void *thread);

// Drain input
int service_drain_input (void);

unsigned long gws_get_device_width(void);
unsigned long gws_get_device_height(void);

void gws_show_backbuffer(void);
void gwssrv_broadcast_close(void);

void ServerShutdown(int server_fd);

//
// End
//


