// rect.c
// Created by Fred Nora.

#include <kernel.h>


//
// == private functions: prototypes ====================
//

static int 
__drawrectangle0( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    unsigned long rop_flags,
    int back_or_front );

static void 
__refresh_rectangle0 ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height,
    unsigned long buffer_dest,
    unsigned long buffer_src );


/*
static void *__rectStrCopyMemory32 ( 
    unsigned long *dest, 
    unsigned long *src, 
    int count );
*/

// =======================================


/* 
 * __drawrectangle0: 
 *     Draw a rectangle on backbuffer or frontbuffer.
 */
// Service 9.
// #bugbug
// Agora precisamos considerar o limite de apenas 2mb
// de lfb mapeados e de apenas 2 mb de backbuffer mapeados.
// Pois nao queremos escrever em area nao mapeada.
// IN:
// 1=backbuffer
// 2=frontbuffer

static int 
__drawrectangle0( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    unsigned long rop_flags,
    int back_or_front )
{
// Return the number of changed pixels.

    int npixels=0;

    //debug_print("__drawrectangle0: r0 :)\n");

// Copy
    unsigned long X      = (x      & 0xFFFF);
    unsigned long Y      = (y      & 0xFFFF);
    unsigned long Width  = (width  & 0xFFFF); 
    unsigned long Height = (height & 0xFFFF);
    unsigned int Color   = color;

// Invalid argument
    if ( back_or_front != 1 && back_or_front != 2 ){
        panic("__drawrectangle0: back_or_front\n");
    }

// Loop
    unsigned long internal_height = (unsigned long) Height;
// #todo
// Get the clipping window/rectangle.
    struct rect_d  Rect;
    struct rect_d  ClippingRect;
// Flag
    int UseClipping = TRUE;
// dc: Clipping
// Clipping support.
    unsigned long deviceWidth  = (unsigned long) screenGetWidth();
    unsigned long deviceHeight = (unsigned long) screenGetHeight();
    if ( deviceWidth == 0 || deviceHeight == 0 )
    {
        debug_print ("__drawrectangle0: [PANIC] w h\n");
        //panic       ("__drawrectangle0: [PANIC] w h\n");
        //return 0;
        goto fail;
    }

//
// Clipping rectangle
//

// #todo
// It need to be a blobal thing.
// We need to handle the surfaces used by 
// this embedded window server and the loadable one.

    ClippingRect.left   = (unsigned long) 0;
    ClippingRect.top    = (unsigned long) 0;
    ClippingRect.width  = (unsigned long) (deviceWidth  & 0xFFFF);
    ClippingRect.height = (unsigned long) (deviceHeight & 0xFFFF);

    ClippingRect.right  = (ClippingRect.left + ClippingRect.width);
    ClippingRect.bottom = (ClippingRect.top + ClippingRect.height);


// #debug
// Provisório

    if ( ClippingRect.width > 800 ){
        panic("__drawrectangle0: width\n");
    }
    if ( ClippingRect.height > 600 ){
        panic("__drawrectangle0: height\n");
    }
    if ( ClippingRect.right > 800 ){
        panic("__drawrectangle0: right\n");
    }
    if ( ClippingRect.bottom > 600 ){
        panic("__drawrectangle0: bottom\n");
    }

//
// == Target rectangle ================
//

// Dimensions
    Rect.x = 0;
    Rect.y = 0;
    Rect.width  = (Width  & 0xFFFF);
    Rect.height = (Height & 0xFFFF);
// Margins
    Rect.left   = (X & 0xFFFF);
    Rect.top    = (Y & 0xFFFF);
    Rect.right  = (unsigned long) (Rect.left + Rect.width);
    Rect.bottom = (unsigned long) (Rect.top  + Rect.height); 
// bg color
    Rect.bg_color = (unsigned int) Color;

//
// Clipping
//

// Limits.
// #todo: 
// Repensar os limites para uma janela.
// Uma janela poderá ser maior que as dimensões de um dispositivo.
// mas não poderá ser maior que as dimensões do backbuffer.
// Ou seja: 
// O dedicated buffer de uma janela deve ser menor que o backbuffer.

    //if ( Rect.right  > gSavedX ){  Rect.right  = gSavedX;  }
    //if ( Rect.bottom > gSavedY ){  Rect.bottom = gSavedY;  }

    if ( Rect.left   < ClippingRect.left   ){ Rect.left   = ClippingRect.left;   }
    if ( Rect.top    < ClippingRect.top    ){ Rect.top    = ClippingRect.top;    }
    if ( Rect.right  > ClippingRect.right  ){ Rect.right  = ClippingRect.right;  }
    if ( Rect.bottom > ClippingRect.bottom ){ Rect.bottom = ClippingRect.bottom; }

// Draw
// Draw lines on backbuffer.

    //#provisório
    if ( internal_height > 600 ){
       panic("__drawrectangle0: internal_height\n");
    }

// Paint
// Paint lines.
// Incrementa a linha a ser pintada.
// See: line.c
// IN:
// 1=backbuffer
// 2=frontbuffer

    while (1)
    {
        // 1=backbuffer
        if (back_or_front == 1)
        {
            npixels += 
                backbuffer_draw_horizontal_line ( 
                    Rect.left, Y, Rect.right, Rect.bg_color, rop_flags );
        }

        // 2=frontbuffer
        if (back_or_front == 2)
        {
            npixels += 
                frontbuffer_draw_horizontal_line ( 
                    Rect.left, Y, Rect.right, Rect.bg_color, rop_flags );
        }

        // Next line.
        Y++;

        // #??
        // Porque podemos desejar escrever no backbuffer
        // um retângulo que ultrapasse a área do frontbuffer.
        if (UseClipping == TRUE)
        {
            if (Y > ClippingRect.bottom){
                break;
            }
        }

        // Decrementa o contador.
        internal_height--;
        if (internal_height == 0){
            break;
        }
    };

// Invalidate
    Rect.dirty = TRUE;

    return (int) npixels;
fail:
    return 0;
}

// Service 9: 
// Draw a rectangle into the backbuffer.
// Usado pelo window server para pintar retângulos.
// Called by sci0() in sci.c.
int 
backbuffer_draw_rectangle( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    unsigned long rop_flags )
{
// Return the number of changed pixels.
// 1=backbuffer
// 2=frontbuffer
    return (int) __drawrectangle0(
        x, y, width, height,
        color, rop_flags, 1 );
}

int 
frontbuffer_draw_rectangle( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    unsigned long rop_flags )
{
// Return the number of changed pixels.
// 1=backbuffer
// 2=frontbuffer
    return (int) __drawrectangle0(
        x, y, width, height,
        color, rop_flags, 2 );
}

/*
 * refresh_rectangle:
 * Copia um retângulo do backbuffer para o frontbuffer.
 * Histórico:
 *     2017 - Criado por Frederico Lamberti Pissarra.
 *     2018 - Fred Nora.
 */
// #importante
// É bem mais rápido com múltiplos de 4.
//
// #bugbug
// No momento precisamos considerar o limite de apenas 2MB de LFB mapeados e 
// de apenas 2MB de backbuffer mapeados.
// Pois nao queremos escrever em área não mapeada.
//

static void 
__refresh_rectangle0 ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height,
    unsigned long buffer_dest,
    unsigned long buffer_src )
{
// Copy a rectangle.
// Given the addresses of the buffers.

    //#debug
    // debug_print("__refresh_rectangle0: (Ring 0)\n");

// #todo
// Checar a validade desses endereços.
    //if (buffer_dest == 0){ return; }
    //if (buffer_src == 0) { return; }

    void *dest       = (void *)      buffer_dest;
    const void *src  = (const void*) buffer_src;

// Loop
    register unsigned int i=0;
    register unsigned int lines=0;
    unsigned int hl_width = 0;  // horizontal line width. 
    register int count=0; 

// Screen pitch.
// screen line size in pixels * bytes per pixel.
    unsigned int screen_pitch=0;
// Rectangle pitch
// rectangle line size in pixels * bytes per pixel.
    unsigned int rectangle_pitch=0;
    unsigned int offset=0;
// = 3; 24bpp
// = 4; 32bpp
    int bytes_count=0;
    int FirstLine = (int) (y & 0xFFFF);
    int UseClipping = TRUE;
    //int UseVSync = FALSE;

//==========
// dc
    unsigned long deviceWidth  = (unsigned long) screenGetWidth();
    unsigned long deviceHeight = (unsigned long) screenGetHeight();
    if ( deviceWidth == 0 || deviceHeight == 0 )
    {
        debug_print ("refresh_rectangle: w h\n");
        panic       ("refresh_rectangle: w h\n");
    }

// Internal

// Get function parameters.
    unsigned long X = (unsigned long) (x & 0xFFFF);
    unsigned long Y = (unsigned long) (y & 0xFFFF);
    hl_width = (unsigned int) (width  & 0xFFFF);
    lines    = (unsigned int) (height & 0xFFFF);

// How many bytes per pixel?
    switch (gSavedBPP){
        case 32:
            // (32/8)
            bytes_count = 4;
            break;
        case 24:
            // (24/8)
            bytes_count = 3;
            break;
        // ... #todo
        default:
            panic("refresh_rectangle: gSavedBPP\n");
            break;
    };

//
// Pitch
//

// Screen pitch.
// Screen line size in pixels plus bytes per pixel.
    screen_pitch = (unsigned int) (bytes_count * deviceWidth);

// Rectangle pitch.
// rectangle line size in pixels * bytes per pixel.
// (line_size * bytes_count) é o número de bytes por linha. 
    rectangle_pitch = (unsigned int) (bytes_count * hl_width);

    offset = (unsigned int) ( (Y*screen_pitch) + (X*bytes_count) );

    dest = (void *)       (dest + offset);
    src  = (const void *) (src  + offset);

// #bugbug
// Isso pode nos dar problemas.
// ?? Isso ainda é necessário nos dias de hoje ??

    //if ( UseVSync == TRUE){
        //vsync();
    //}

// ================================
// Se for divisível por 8.
// Copy lines
// See:'strength reduction'
// Clipping?
// Não copiamos a parte que está fora da janela do dispositivo.
// memcpy64: 8 bytes per time.

    if ( (rectangle_pitch % 8) == 0 )
    {
        count = (rectangle_pitch>>3);
        for ( i=0; i < lines; i++ )
        {
            if (UseClipping == TRUE)
            {
                // #todo: Não podemos escremer na linha 600, por exemple.
                //if ( (FirstLine + i) >= deviceHeight )
                if ( (FirstLine + i) > deviceHeight )
                { 
                    break;
                }
            }
            memcpy64 ( (void *) dest, (const void *) src, count );
            dest += screen_pitch;
            src  += screen_pitch;
        };
        return;
    }

// ================================
// Se for divisível por 4.
// Esse não será usado se for divisóvel por 8.
// Mas será chamado se for menor que 8, apenas 4.
// Copy lines
// See:'strength reduction'
// Clipping?
// Não copiamos a parte que está fora da janela do dispositivo.
// memcpy32: 4 bytes per time.

    if ( (rectangle_pitch % 4) == 0 )
    {
        count = (rectangle_pitch>>2);
        for ( i=0; i < lines; i++ )
        {
            if (UseClipping == TRUE)
            {
                // #todo: Não podemos escrever na linha 600, por exemplo.
                // if ( (FirstLine + i) >= deviceHeight )
                if ( (FirstLine + i) > deviceHeight )
                { 
                    break; 
                }
            }
            memcpy32 ( (void *) dest, (const void *) src, count );
            dest += screen_pitch;
            src  += screen_pitch;
        };
        return;
    }

// ================================
// Se não for divisível por 4. (slow)
// Copy lines
// Clipping?
// Não copiamos a parte que está fora da janela do dispositivo.
// memcpy: 1 byte per time.

    if ( (rectangle_pitch % 4) != 0 )
    {
        for ( i=0; i < lines; i++ )
        {
            if (UseClipping == TRUE)
            {
                // #todo: Não podemos escrever na linha 600, por exemplo.
                // if ( (FirstLine + i) >= deviceHeight )
                if ( (FirstLine + i) > deviceHeight )
                { 
                    break; 
                }
            }
            memcpy ( (void *) dest, (const void *) src, rectangle_pitch );
            dest += screen_pitch; 
            src  += screen_pitch; 
        };
        return;
    }
}

// Service 10.
// ??
// A ideia aqui é efetuar o refresh de um retângulo 
// que esteja em um dado buffer.
// #todo:
// Refresh de retângulos é tudo o que precisamos 
// para termos um compositor dentro do kernel.
// Assim como o display driver pode ficar dentro do kernel,
// o compositor tambem pode ficar dentro do kernel.
// Podemos fazer o compositor dentro do window server em ring3
// se tivermos um timer que interrompa o window server.

void 
refresh_rectangle ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height )
{

// == FLASH ========
// #todo: Create a global variable for this.
// #debug
// Used for optimization tests.

   //#debug
    //int RefreshFlash=TRUE;
    int RefreshFlash=FALSE;
    long i=0;
    long count=2000000;

    if (RefreshFlash==TRUE)
    {
        // Draw directly into the lfb.
        frontbuffer_draw_rectangle( 
            x, y, width, height, 
            COLOR_YELLOW, 0 );
         // Wait ...
         for (i=0; i<count; i++){};
    }
// =====================

    __refresh_rectangle0(
        x, y, width, height,
        FRONTBUFFER_ADDRESS,   // dest
        BACKBUFFER_ADDRESS );  // src
}

/*
 * scroll_screen_rect:
 *     Scroll a rectangle. ?
 */
// Helper function to scroll routine.
// Called by console_scroll() in tty/console.c
// Only for full screen console.
// See flag.
void scroll_screen_rect (void)
{
    register unsigned int i=0;
    unsigned int line_size=0;  // w 
    unsigned int lines=0;      // h
    unsigned int offset=0;

// Device info
    unsigned long deviceWidth  = (unsigned long) screenGetWidth();
    unsigned long deviceHeight = (unsigned long) screenGetHeight();
    //int cWidth = get_char_width();
    int cHeight = get_char_height();
// = 3; 
//24bpp
    int bytes_count = 0;
    int count = 0; 

    // #debug
    //debug_print ("scroll_screen_rect:\n");

    if ( deviceWidth == 0 || deviceHeight == 0 ){
        debug_print ("scroll_screen_rect: [PANIC] w h\n");
        panic       ("scroll_screen_rect: [PANIC] w h\n");
    }

    // #debug
    //if(cHeight == 8)
        //debug_print("8\n");
    // #debug
    //if(cHeight == 16)
        //debug_print("16\n");

    line_size = (unsigned int) deviceWidth; 
    lines     = (unsigned int) deviceHeight;

    switch (gSavedBPP){
    case 32: bytes_count = 4; break;
    case 24: bytes_count = 3; break;
    // ...
    default:
        panic("scroll_screen_rect: gSavedBPP");
        break;
    };

// Pointers
// Destination and Source.
// Destination is the first line.
// Source is the second line. It has the height of a char.

    void *Dest = 
        (void *) BACKBUFFER_ADDRESS;
    
    unsigned long SrcOffset = 
        (unsigned long) (bytes_count * gSavedX * cHeight);
    
    const void *Src = 
        (const void *) (BACKBUFFER_ADDRESS + SrcOffset);

// Copy
// #importante
// É bem mais rápido com múltiplos de 4.
// Se for divisível por 4.
// Copia uma linha, quatro bytes de cada vez.  
        
    if ( ((line_size * bytes_count) % 4) == 0 )
    {
        // #todo: Create a variable for 'pitch' and use streigh reduction
        count = ((line_size * bytes_count) / 4); 
        // count = (internal_pitch>>2);  // #todo: Use this one.
        for ( i=0; i < lines; i++ ){
            memcpy32 (Dest,Src,count);
            Src  += (deviceWidth * bytes_count);
            Dest += (deviceWidth * bytes_count);
        };
        return;
    }

// Se não for divisível por 4.
// Copia a linha, um bytes por vez.
// #todo: Podemos suprimir esse if e deixarmos ssomente o for.
    if ( ((line_size * bytes_count) % 4) != 0 )
    {
        for ( i=0; i < lines; i++ )
        {
            memcpy ( 
                (void *) Dest, 
                (const void *) Src, 
                (line_size * bytes_count) );

            Src  += (deviceWidth * bytes_count);
            Dest += (deviceWidth * bytes_count);
        };
    }
}

// ??
// Copiando ...
// Destination is an Null pointer? 
// Source is an Null pointer? 
// Zero-sized copy? 
// Destination is Source?

/*
static void *__rectStrCopyMemory32 ( 
    unsigned long *dest, 
    unsigned long *src, 
    int count ) 
{
    register int i=0;

    // Yes
    if ( (dest == NULL)  || 
         (src == NULL)   || 
         (count == 0)    || 
         (src == dest) ) 
    {
        return dest;
    }

    // GCC should optimize this for us :)
    // I'm lying.
    for ( i=0; i < count; i++ ) 
    {
        dest[i] = src[i];
    };

    return dest;
}
*/

