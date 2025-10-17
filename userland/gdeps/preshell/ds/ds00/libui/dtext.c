// File: dtext.c 
// Draw text routines.
// Created by Fred Nora.


#include "../gwsint.h"

// grDrawString:
// Draw a string on the screen. 
// No clipping
void 
grDrawString ( 
    unsigned long x,
    unsigned long y,
    unsigned int color,
    unsigned char *string )
{
// Absolute positions.

    int Index=0;
    //#bugbug:  Determinando
    int cWidth = 8;   
    //int cWidth = get_char_width ();
    //int cHeight = get_char_height ();

    //int is_transparent = transparent;  // from parameters.
    //int is_transparent = TRUE;

    //#todo
    //unsigned int __fg_color = fg_color;
    //unsigned int __bg_color = bg_color;
    unsigned int __fg_color = color;
    //unsigned int __bg_color = color;

    if ((void *) string == NULL)
        return;
    if (*string == 0)
        return;

// #fail
// Char width
    //if ( cWidth == 0 || cHeight == 0 )
    if (cWidth == 0){
        //panic ("grDrawString: cWidth");
        //#debug
        //gde_message_box (3, "xxx","dtext-draw_string: cWidth");
        printf ("grDrawString: cWidth\n");
        while(1){
        };
    }

    //int size = sizeof(string);  
    //for ( Index=0; Index<size; Index++ )

// Draw
// see: char.c

    for ( Index=0; string[Index] != 0; Index++ )
    {
        // not transparent.
        /*
        if (is_transparent != TRUE)
        {
            grBackbufferDrawChar( 
                x, 
                y,
                string[Index],
                (unsigned int) __fg_color,     // fg color
                (unsigned int) __bg_color );   // bg color
        }
        */

        // transparent
        //if (is_transparent == TRUE)
        //{
            //grBackbufferDrawCharTransparent ( 
            //    x, y, 
            //    (unsigned int) __fg_color, 
            //    string[Index] );
            grBackbufferDrawCharTransparent2 ( 
                x, y, 
                (unsigned int) __fg_color, 
                string[Index],
                FontInitialization.address );
         //}

        // gradient
        //if( string_flags & ? ){
        //grBackbufferDrawCharTransparent ( 
            //x, y, 
            //(unsigned int) interpolate_color(COLOR_BLACK, __fg_color, x), 
            //string[Index] );
        //}

        x += cWidth;
    };
}

// dtextDrawText:
// Draw text on a window.
// Como opção, se nenhuma janela for especificada, 
// então pintaremos na janela principal.
void 
dtextDrawText ( 
    struct gws_window_d *window,
    unsigned long x,
    unsigned long y,
    unsigned int color,
    char *string )
{ 

// Window validation
    if ((void *) window == NULL)
        return;
    if (window->magic != 1234)
        return;

// String validation
    if ((void *) string == NULL)
        return;
    if (*string == 0)
        return;

// Draw
// Absolute
    grDrawString ( 
        (window->absolute_x +x), 
        (window->absolute_y +y), 
        (unsigned int) color, 
         string );
}

void 
dtextDrawText2 ( 
    struct gws_window_d *window,
    unsigned long x,
    unsigned long y,
    unsigned int color,
    char *string,
    int flush )
{ 

    unsigned long CharWidth=8;
    unsigned long CharHeight=8;

    if (FontInitialization.initialized == TRUE)
    {
        CharWidth = FontInitialization.width;
        CharHeight = FontInitialization.height;
    }

// Window validation
    if ((void *) window == NULL)
        return;
    if (window->magic != 1234)
        return;

// String validation
    if ((void *) string == NULL)
        return;
    if (*string == 0)
        return;

// Draw
// Absolute
    grDrawString ( 
        (window->absolute_x +x), 
        (window->absolute_y +y), 
        (unsigned int) color, 
         string );

// Flush
// Absolute
    if (flush){
        gws_refresh_rectangle ( 
            (window->absolute_x +x), 
            (window->absolute_y +y), 
            CharWidth, 
            CharHeight );
    }
}

