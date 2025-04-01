// bg.c
// Created by Fred Nora.

#include <kernel.h>

// see: bg.h
struct bg_info_d  background_info;

// -------------------------------------

static void __displayPaintBackground(unsigned int color, int show);

// -------------------------------------

// #bugbug
// Essa rotina é sensível, pois agora usamos 64bit.
// Called by kmain()
// Where? backbuffer?
// Draw a rectangle into the backbuffer.
static void __displayPaintBackground(unsigned int color, int show)
{
    unsigned long deviceWidth  = (unsigned long) screenGetWidth();
    unsigned long deviceHeight = (unsigned long) screenGetHeight();
    if ( deviceWidth == 0 || deviceHeight == 0 )
    {
        debug_print ("backgroundDraw: [PANIC] w h\n");
        //panic       ("drawDataRectangle: [PANIC] w h\n");
        return;
    }

    backbuffer_draw_rectangle( 
        0, 0, deviceWidth, deviceHeight, 
        color,
        0 );   //rop_flags

// Cursor.
// #bugbug: This routine is not working at this time.
    set_up_cursor(0,0);

    if (show == TRUE)
        refresh_screen();


// Save
    background_info.bg_color = color;

// Se a estrutura ja foi inicializa 
// anteriormente, entao podemos incrementar o 
// contador de pintura.
    if (background_info.initialized == TRUE){
        background_info.paint_cnt++;
    }

//#debug
    //debug_print ("backgroundDraw: done\n");
}

int displayInitializeBackground(unsigned int color,int show)
{
    __displayPaintBackground((unsigned int) color,show);
    //set_up_cursor(0,0);
    
    background_info.initialized = TRUE;
    return 0;
}


