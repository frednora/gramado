// gramk.c
// Wrappers for the gramk/ kernel components.
// This is a safer way to call this services.
// Created by Fred Nora.

#include <kernel.h>

static void __x_panic_show_message(const char *final_string, unsigned long flags);


//
// =============================================
//

// see: bldisp.c
void refresh_screen(void)
{
    bldisp_flush(0);
}

void gramk_refresh_screen(void)
{
    refresh_screen();
}

void gramk_putchar_in_fgconsole(unsigned long _char)
{
    int c = (int) (_char & 0xFF);

    //if (fg_console < 0)
        //return;

// Draw and refresh.
    console_outbyte2 ( c, fg_console );
}

// __x_panic_show_message:
// Draw a small red rectangle in the top/left
// and print a message into it.
static void __x_panic_show_message(const char *final_string, unsigned long flags)
{
    unsigned long rectLeft=0;
    unsigned long rectTop=0;
    unsigned long rectWidth = gSavedX;  // (Device width) see: kmain.c
    unsigned long rectHeight = (8*3); // Char height * 3  //28;
    unsigned int RectColor = COLOR_RED;

    unsigned long orLeft=rectLeft;
    unsigned long orTop=rectHeight;
    unsigned long orWidth = rectWidth;
    //unsigned long orHeight=1;
    unsigned int OrnamentColor = COLOR_YELLOW;

    unsigned long s1_left=8;
    unsigned long s1_top=8;
    unsigned int s1_color = COLOR_YELLOW;

    unsigned long s2_left=80;
    unsigned long s2_top=8;
    unsigned int s2_color = COLOR_WHITE;

// #todo
// Check the string size?

//---------------
// Box

    backbuffer_draw_rectangle ( 
        rectLeft, rectTop, rectWidth, rectHeight, 
        RectColor, 
        0 );

    backbuffer_draw_horizontal_line( 
        orLeft, orTop, orWidth,
        OrnamentColor, 
        0 );

//---------------
// Text

// string 1
    draw_string(s1_left,s1_top,s1_color,"PANIC: ");
// string2
    if ((void*) final_string != NULL){
        draw_string(s2_left,s2_top,s2_color,final_string);
    }

    refresh_screen();
}

void x_panic(const char *final_string)
{

// Print the string
    __x_panic_show_message(final_string,0);

// A soft place to fall.
    while (1){
        asm ("cli");  
        asm ("hlt"); 
    };
}

void gramk_panic(const char *final_string)
{

// Print the string
    __x_panic_show_message(final_string,1);

// A soft place to fall.
    while (1){
        asm ("cli");  
        asm ("hlt"); 
    };
}

