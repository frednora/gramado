// globals.c
// Created by Fred Nora.

#include "gram3d.h"

int use_vsync=FALSE;

struct gws_color_scheme_d* GWSCurrentColorScheme;
struct gws_color_scheme_d* GWSHumilityColorScheme; // Simples.
struct gws_color_scheme_d* GWSPrideColorScheme;    // Colorido.


int refresh_device_screen_flag=0;
int refresh_valid_screen_flag=0;

int current_mode=0;

int current_client=0;

int gUseLeftHand=TRUE;
int gUseCallback=TRUE;

unsigned long starting_tick=0;

// unsigned long gws_pallete_16[16];


struct gws_client_d  *serverClient;
struct gws_client_d  *currentClient;
// ...
// list
struct gws_client_d  *first_client;

// see: client.h
unsigned long connections[CLIENT_COUNT_MAX];

// =========================
// h:d.s
char *host_name;
char *display_number;
char *screen_number;

// The device screen.
// The refresh is called by refresh_device_screen();
// It is called explicit by the app.
// see: screen.h
struct gws_screen_d  *DeviceScreen;
unsigned long screens[MAX_SCREENS];

// See: display.h
struct gws_display_d  *CurrentDisplay;

struct gws_host_d  *CurrentHost;


// #ordem
// hardware, software
unsigned long  ____BACKBUFFER_VA=0;
unsigned long  ____FRONTBUFFER_VA=0;
// Saving
unsigned long SavedBootBlock=0;
unsigned long SavedLFB=0;   // #bugbug? VA ?
unsigned long SavedX=0;
unsigned long SavedY=0;
unsigned long SavedBPP=0; 
// helper.
unsigned long __device_width=0;
unsigned long __device_height=0;
unsigned long __device_bpp=0;

// Device context
struct dc_d  *gr_dc;  //default dc

struct gui_d  *gui;

