// gws.h

#ifndef __GWS_H
#define __GWS_H    1


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

// ====================================================================

void invalidate(void);
void validate(void);

int isdirty(void);

void invalidate_background(void);
void validate_background(void);

int is_background_dirty(void);

void gws_show_backbuffer(void);

// Initialization
int gwsInit(void);
int serverInit (void);

#endif   

