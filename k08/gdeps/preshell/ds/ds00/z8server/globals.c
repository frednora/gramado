// globals.c
// 2022 - Created by Fred Nora.

#include "../gwsint.h"

// os mode = GRAMADO_P1 ...
int os_mode=0;  

// server mode = demo, server ...
int server_mode = 0;

// =========================
// h:d.s
char *host_name;
char *display_number;
char *screen_number;
// struct gws_host_d  Hostinfo;


//
// == buffer ===============================================
//


// This is the buffer used when we receive 
// requests via socket.
// see: globals.h
char __buffer[MSG_BUFFER_SIZE];

// Esses valores serão enviados como 
// resposta ao serviço atual.
// Eles são configurados pelo dialogo na hora da 
// prestação do serviço.
// No início desse array fica o header.
// see: globals.h
unsigned long next_response[32];

//----------------------------------------


// See: screen.h
int refresh_device_screen_flag=FALSE;
int refresh_valid_screen_flag=FALSE;

int gUseCallback=TRUE;

int gUseMouse=FALSE;

// #ordem
// hardware, software
unsigned long ____BACKBUFFER_VA=0;
unsigned long ____FRONTBUFFER_VA=0;
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

// See: globals.h
struct screen_information_d  screens[MAX_SCREENS];

struct gui_d  *gui;

int use_vsync=FALSE;

// unsigned long gws_pallete_16[16];

//
// Color scheme
//

/*
// Losethos colors.
public BGREntry vga_palette4_std[16] = {
    0x000000,0x0000AA,0x00AA00,0x00AAAA,
    0xAA0000,0xAA00AA,0xAA5500,0xAAAAAA,
    0x555555,0x5555FF,0x55FF55,0x55FFFF,
    0xFF5555,0xFF55FF,0xFFFF55,0xFFFFFF };


public BGREntry vga_palette4_gray[16] = {
    0x000000,0x111111,0x222222,0x333333,
    0x444444,0x555555,0x666666,0x777777,
    0x888888,0x999999,0xAAAAAA,0xBBBBBB,
    0xCCCCCC,0xDDDDDD,0xEEEEEE,0xFFFFFF };
*/

