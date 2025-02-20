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

#include "protocol.h"

#include "globals.h"

#include "colors.h"
#include "nc2.h"   //nelson cole 2 font. 8x8

// Graphics device library.
#include <libdisp.h>

#include "char.h"
#include "dtext.h"
#include "line.h"

#include "wt.h"
#include "menu.h"
#include "xxxinput.h"   //#test
#include "window.h"
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

#include "packet.h"


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
#include "dc.h"


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


//
// MAIN STRUCTURE
//

// This is the main data structure for the window server.

struct gws_d 
{
    int initialized;

    // The name of the window server.
    char name[64];
    char edition_name[64];

    char version_string[16];

    unsigned long version_major;
    unsigned long version_minor;

    // fd
    int socket;

    // flag: When to quit the window server.
    int quit;

// window server status
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
extern struct gws_d  *display_server;

#define STATUS_RUNNING    1   

//
// =============================================================
//



//
// == buffer ===============================================
//

// O buffer para  as mensagens recebidas via socket.
#define MSG_BUFFER_SIZE 512
char __buffer[MSG_BUFFER_SIZE];   

// Esses valores serão enviados como 
// resposta ao serviço atual.
// Eles são configurados pelo dialogo na hora da 
// prestação do serviço.
// No início desse array fica o header.
#define NEXTRESPONSE_BUFFER_SIZE  32
unsigned long next_response[32];

//
// prototypes =============================
//

// see: main.c
int main (int argc, char **argv);
void gramado_terminate(void);

// These functions belong to another module
// inside another project.
//void gramado_setup(void);
//void gramado_update(void);
void gramado_clear_surface(struct gws_window_d *clipping_window, unsigned int color);
void gramado_flush_surface(struct gws_window_d *clipping_window);



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

void gwssrv_debug_print (char *string);

int gwssrv_clone_and_execute ( char *name );
unsigned long gwssrv_get_system_metrics (int index);

void gwssrv_enter_critical_section (void);
void gwssrv_exit_critical_section (void);
void gwssrv_show_backbuffer (void);

char *gwssrv_get_version(void);

// init
int gwsInit(void);

unsigned long gws_get_device_width(void);
unsigned long gws_get_device_height(void);

// Função padrão para todos os servidores ???
// #todo: temos que criar isso.
int serverInit (void);

void gws_show_backbuffer(void);



void gwssrv_wait_message(void);
void gwssrv_yield(void);
void gwssrv_quit(void);

//
// End
//


