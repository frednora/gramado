// text.c
// Created by Fred Nora.

#include <kernel.h>

// draw_string:
// Draw a string on the screen. 
void 
draw_string ( 
    unsigned long x,
    unsigned long y,
    unsigned int color,
    const char *string )
{
    register int Index=0;

   //int cWidth = 8;
    int cWidth = get_char_width ();
    //int cHeight = get_char_height ();

    //if ( cWidth == 0 || cHeight == 0 )
    if ( cWidth == 0 ){
        debug_print("dtext-draw_string: cWidth\n");
        //panic ("dtext-draw_string: cWidth\n");
        //panic ("dtext-draw_string: cWidth\n");
        return;
    }

    /*
    if ( (void *) string == NULL ){
        debug_print("dtext-draw_string: string\n");
        //panic ("dtext-draw_string: string\n");
        //panic ("dtext-draw_string: string\n");
        return;
    }
    if ( *string == 0  ){
        debug_print("dtext-draw_string: *string\n");
        //panic ("dtext-draw_string: *string\n");
        //panic ("dtext-draw_string: *string\n");
        return;
    }
    */

    // #bugbug
    // There is something not good with this routine.
    
    for ( Index=0; string[Index] != 0; Index++ )
    {
        d_drawchar_transparent ( x, y, color, string[Index] );

        x += cWidth;
    };
}

