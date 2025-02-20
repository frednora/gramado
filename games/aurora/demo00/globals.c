// globals.c
// Created by Fred Nora.

#include "gram3d.h"

// =========================
// h:d.s
char *host_name;
char *display_number;
char *screen_number;

// struct gws_host_d  Hostinfo;

int current_mode=0;

int gUseLeftHand=TRUE;
int gUseCallback=TRUE;

unsigned long starting_tick=0;

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

unsigned long screens[MAX_SCREENS];

struct gui_d  *gui;

int use_vsync=FALSE;


// unsigned long gws_pallete_16[16];




