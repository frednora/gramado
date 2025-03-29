// line.c
// Created by Fred Nora.

#include <kernel.h>


//Herdadas do Boot Loader.
// De onde vem isso ?? head.s
// @todo: Devemos chamar o módulo hal para obtermos esses valores.
//depois salvamos em variáveis internas usadas pela gui.
//extern unsigned long SavedBootBlock;
//extern unsigned long SavedLFB;
extern unsigned long SavedX;
extern unsigned long SavedY;
extern unsigned long SavedBPP; 

/*
 * backbuffer_draw_horizontal_line:
 *     Draw a horizontal line on backbuffer. 
 */
int 
backbuffer_draw_horizontal_line ( 
    unsigned long x1,
    unsigned long y, 
    unsigned long x2, 
    unsigned int color,
    unsigned long rop_flags )
{
// Return the number of changed pixels.

// #todo
// Maybe we need checking some limits here.

    if (x1 > x2){
        return 0;
    }

    int n = (int) (x2-x1);
    int n2=0;

// IN: color, x, y, rop flags.
    while (x1 < x2)
    {
        n2 += backbuffer_putpixel ( color, x1, y, rop_flags ); 
        x1++;
    };
    
    if (n2 < n)
        n = n2;

    return (int) n;
}

int 
frontbuffer_draw_horizontal_line ( 
    unsigned long x1,
    unsigned long y, 
    unsigned long x2, 
    unsigned int color,
    unsigned long rop_flags )
{
// Return the number of changed pixels.

// #todo
// Maybe we need checking some limits here.

    if (x1 > x2){
        return 0;
    }

    int n = (int) (x2-x1);
    int n2=0;

// IN: color, x, y, rop flags.
    while (x1 < x2)
    {
        n2 += frontbuffer_putpixel ( color, x1, y, rop_flags ); 
        x1++;
    };

    if (n2 < n)
        n = n2;

    return (int) n;
}

