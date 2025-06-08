
// gram3d.h 
// Created by Fred Nora.

// ...
extern int current_mode;

#define STATUS_RUNNING    1   


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

#include "lib00/char.h"
#include "lib00/dtext.h"

#include "libui/line.h"
#include "libui/wt.h"
#include "libui/menu.h"
#include "libui/rect.h"
#include "libui/window.h"
#include "libui/bmp.h"


#include "xxxinput.h"   //#test


// h:0.0
#include "screen.h"   // X-like
#include "display.h"  // X-like
#include "host.h"     // X-like h:0.0

// Teremos isso aqui no window server,
// kgws é outro ws par ao ambiente de setup e tera 
// seu próprio gerenciamento.
#include "security.h"

#include "ui/gui.h"
#include "ui/comp.h"

#include "event.h"    // view inputs

// Imported
#include <grprim0.h>   // Common for eng/ and gramland/.
#include <libgr.h>     // Common for eng/ and gramland/.
#include <grprim3d.h>  // Only for eng/
#include <libgr3d.h>   // Only for eng/

#include "lib00/grprim.h"
#include "lib00/camera.h"
#include "lib00/world.h"
#include "lib00/proj.h"
#include "lib00/sprite.h"
#include "lib00/surface.h"

// Common for all the demos
#include "demos/demos.h"
#include "demos/models.h"
#include "demos/scan00.h"

// Demos
#include "demos/flycube.h"
#include "demos/cat00.h"
#include "demos/polygon.h"
#include "demos/tri00.h"
#include "demos/lin00.h"
#include "demos/curve00.h"


#include "packet.h"


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


// # model. business logic
// #todo:
// We can put this thing in the library. (libgws)
// or (libcon)
#include "connect.h"

#include "font.h"

// Client structure.
// O proprio servidor poderia ser o cliente 0??
#include "client.h"
// # model. business logic
#include "services.h"

// Device Context.
// This is the structure that is gonna be used by the
// drawing routines.
// 'dc->something'
// It needs to be the last one.
#include "dc.h"

#include "ui/wm.h"


#include "gws.h"


//
// prototypes =============================
//

// see: main.c
int main (int argc, char **argv);
void gramado_terminate(void);


void xxxThread (void);
void ____test_threads (void);

void *gwssrv_create_thread ( 
    unsigned long init_eip, 
    unsigned long init_stack, 
    char *name );

void gwssrv_start_thread (void *thread);

void gwssrv_set_keyboard_focus(int window);
int service_drain_input (void);

void gwssrv_debug_print (char *string);

int gwssrv_clone_and_execute ( char *name );
unsigned long gwssrv_get_system_metrics (int index);

void gwssrv_enter_critical_section (void);
void gwssrv_exit_critical_section (void);
void gwssrv_show_backbuffer (void);

char *gwssrv_get_version(void);

unsigned long gws_get_device_width(void);
unsigned long gws_get_device_height(void);

void gwssrv_wait_message(void);
void gwssrv_yield(void);
void gwssrv_quit(void);



//
// End
//


