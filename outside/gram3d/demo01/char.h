
#ifndef __CHAR_H
#define __CHAR_H  1


// Draw char support
extern int gcharWidth;
extern int gcharHeight;


void
DrawBorder( 
    unsigned long l, 
    unsigned long r, 
    unsigned long t, 
    unsigned long b,
    int solid );

void charSetCharWidth ( int width );
void charSetCharHeight ( int height );
int charGetCharWidth (void);
int charGetCharHeight (void);

void 
charBackbufferCharBlt ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long color, 
    unsigned long c );

/*
 * drawchar_transparent:
 *     Desenha um caractere sem alterar o pano de fundo.
 */

void 
grBackbufferDrawCharTransparent ( 
    unsigned long x, 
    unsigned long y, 
    unsigned int color, 
    unsigned long c );

/*
 * draw_char:
 *     Constrói um caractere 8x8 (configurável) no buffer.
 *     Desenha um caractere e pinta o pano de fundo.
 */ 

void 
grBackbufferDrawChar ( 
    unsigned long x, 
    unsigned long y,  
    unsigned long c,
    unsigned int fgcolor,
    unsigned int bgcolor );

int gwssrv_init_char(void);

#endif


//
// End.
//







  


