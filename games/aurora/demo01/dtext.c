/*
 * File: dtext.c 
 *     Draw text routines.
 */

#include "gram3d.h"


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
    register int Index=0;
//#bugbug:  Determinando
    int cWidth = 8;   
    //int cWidth = get_char_width();
    //int cHeight = get_char_height();

    //if ( cWidth == 0 || cHeight == 0 )
    if ( cWidth == 0 )
    {
        //panic ("grDrawString: cWidth");
        //#debug
        //gde_message_box (3, "xxx","dtext-draw_string: cWidth");
        printf ("grDrawString: cWidth\n");
        while(1){}
    }

    //int size = sizeof(string);  
    //for ( Index=0; Index<size; Index++ )

// Draw

    for ( Index=0; string[Index] != 0; Index++ )
    {
        grBackbufferDrawCharTransparent ( 
            x, y, 
            (unsigned int) color, string[Index] );

        // gradient
        //if( string_flags & ? ){
        //grBackbufferDrawCharTransparent ( 
            //x, y, 
            //(unsigned int) interpolate_color(COLOR_BLACK, color, x), 
            //string[Index] );
        //}

        x += cWidth;
    };
}


/* 
 * dtextDrawText:
 *     Draw text on a window.
 * #test: 
 *     Como opção, se nenhuma janela for especificada, 
 *     então pintaremos na janela principal.
 */

void 
dtextDrawText ( 
    struct gws_window_d *window,
    unsigned long x,
    unsigned long y,
    unsigned int color,
    unsigned char *string )
{ 
    struct gws_window_d * __w;
    __w = (struct gws_window_d *) gui->screen_window;

    if ( (void *) window == NULL ){

        if ( (void*)__w != NULL )
        {
            grDrawString ( 
                (__w->absolute_x +x), 
                (__w->absolute_y +y), 
                (unsigned int) color, 
                string );
            return;
        }
        gwssrv_debug_print("dtextDrawText: __w\n");
        return;

    }else{
        grDrawString ( 
            (window->absolute_x +x), 
            (window->absolute_y +y), 
            (unsigned int) color, 
            string );
    };
}


