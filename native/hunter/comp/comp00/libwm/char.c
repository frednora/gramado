// char.c 
// view: Char support.
// 2019 - Created by Fred Nora.

#include "../ds.h"

struct char_initialization_d  CharInitialization;

// ================================================

// Constrói um caractere transparente 8x8 no buffer.
void 
charBackbufferCharBlt ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long color, 
    unsigned long c )
{
    grBackbufferDrawCharTransparent ( x, y, color, c );
}

void charSetCharWidth (int width)
{
    FontInitialization.width = (int) width;
}

void charSetCharHeight (int height)
{
    FontInitialization.height = (int) height;
}

int charGetCharWidth (void)
{
    return (int) FontInitialization.width;
}

int charGetCharHeight (void)
{
    return (int) FontInitialization.height;
}

// Draw a char but don't change the background color
// Transparent character blitting
void 
grBackbufferDrawCharTransparent ( 
    unsigned long x, 
    unsigned long y, 
    unsigned int color, 
    int ch )
{
    int Char = (int) (ch & 0xFF);
    //int CharWidth;
    //int CharHeight;

    unsigned char *work_char_p;
    unsigned char bit_mask = 0x80;
//loop
    register int y2=0;
    register int x2=0;

/*
 Get the font pointer.
 #todo:
 usar variavel g8x8fontAddress.
 + Criar e usar uma estrutura para fonte.
 + Usar o ponteiro para a fonte atual que foi carregada.
 + Criar um switch para o tamanho da fonte.
   isso deveria estar na inicialização do módulo char.
 */

    if ( FontInitialization.address == 0 || 
         FontInitialization.width <= 0 || 
         FontInitialization.height <= 0 )
    {
        printf ("grBackbufferDrawCharTransparent: Initialization fail\n");
        while (1){
        };
    }

// #todo: 
// Criar essas variáveis e definições.

// O caractere sendo trabalhado.

    //int ascii = (int) (c & 0xFF);
    //if(ascii == 'M'){
    //    printf("M: %d\n",ascii);
    //}

    work_char_p = 
        (void *) FontInitialization.address + (Char * FontInitialization.height);

// Draw char
// Put pixel using the ring3 routine.
// See: libgd.c

    for ( y2=0; y2 < FontInitialization.height; y2++ )
    {
        bit_mask = 0x80;

        for ( x2=0; x2 < FontInitialization.width; x2++ )
        {
            if ( ( *work_char_p & bit_mask ) )
            {
                // IN: color, x, y,rop
                libdisp_backbuffer_putpixel ( 
                    (unsigned int) color, 
                    (x + x2), 
                    y,
                    (unsigned long) 0 );  
            }

            // Rotate bitmask.
            bit_mask = (bit_mask >> 1);
        };

        // Próxima linha das 8 linhas do caractere.
        y++; 
        work_char_p++; 
    };
}

// Given a pointer for the font base address
void 
grBackbufferDrawCharTransparent2 ( 
    unsigned long x, 
    unsigned long y, 
    unsigned int color, 
    int ch,
    char *stock_address )
{
    int Char = (int) (ch & 0xFF);
    //int CharWidth;
    //int CharHeight;
    char *work_char_p;
    unsigned char bit_mask = 0x80;
// Where the font is.
    char *base_address;
    base_address = stock_address;
//loop
    register int y2=0;
    register int x2=0;

// Invalid base address
    if ((void *) base_address == NULL)
    {
        //#debug
        //printf("grBackbufferDrawCharTransparent2: base_address\n");
        goto fail;
    }

// Invalid char dimensions.
    if ( FontInitialization.width <= 0 || FontInitialization.height <= 0 )
    {
        //#debug
        //printf("grBackbufferDrawCharTransparent2: base_address\n");
        goto fail;
    }

// The pointer for the working char.
    work_char_p = 
        (void *) base_address + (Char * FontInitialization.height);

// Draw char
// Put pixel using the ring3 routine.
// See: libgd.c

    for ( y2=0; y2 < FontInitialization.height; y2++ )
    {
        bit_mask = 0x80;

        for ( x2=0; x2 < FontInitialization.width; x2++ )
        {
            if ( *work_char_p & bit_mask )
            {
                // IN: color, x, y, rop
                libdisp_backbuffer_putpixel ( 
                    (unsigned int) color, 
                    (x + x2), 
                    y,
                    (unsigned long) 0 );  
            }

            // Rotate bitmask.
            bit_mask = (bit_mask >> 1);
        };

        // Próxima linha das 8 linhas do caractere.
        y++; 
        work_char_p++; 
    };

//done:
    return;
fail:
    printf("grBackbufferDrawCharTransparent2: Fail\n");
    while (1){
    };
}

// Draw a char and change the background color
// Opaque character blitting with foreground/background color
void 
grBackbufferDrawChar ( 
    unsigned long x, 
    unsigned long y,  
    unsigned long c,
    unsigned int fgcolor,
    unsigned int bgcolor )
{
    register int y2=0;
    register int x2=0;
    char *work_char; 
    unsigned char bit_mask = 0x80;

    unsigned long rop=0;

/*
 Get the font pointer.
 #todo:
 usar variavel g8x8fontAddress.	 
 + Criar e usar uma estrutura para fonte.
 + Usar o ponteiro para a fonte atual que foi carregada.
 + Criar um switch para o tamanho da fonte.
   isso deveria estar na inicialização do módulo char.
 */
 
    if ( FontInitialization.address == 0 ||  
         FontInitialization.width <= 0 || 
         FontInitialization.height <= 0 )
    {
        printf ("grBackbufferDrawChar: initialization fail\n");
        while(1){}
    }

// Tentando pintar um espaço em branco.
// Nas rotinas da biblioteca gráfica, quando encontram
//um espaço(32), nem manda para cá, apenas incrementam o cursor.

// O caractere sendo trabalhado.
// Offset da tabela de chars de altura 8 na ROM.

    work_char = 
        (void *) FontInitialization.address + (c * FontInitialization.height);

// Draw:
// Draw a char using a ring3 routine.
// #todo
// Some flag for modification here?
// Put pixel.
// A cor varia de acordo com a mascara de bit.

    for ( y2=0; y2 < FontInitialization.height; y2++ )
    {
        bit_mask = 0x80;

        for ( x2=0; x2 < FontInitialization.width; x2++ )
        {
            // IN: color, x, y, rop
            libdisp_backbuffer_putpixel ( 
                *work_char & bit_mask ? fgcolor: bgcolor, 
                (x + x2), 
                y,
                (unsigned long) rop );

            bit_mask = (bit_mask >> 1); 
        };

        // Próxima linha da (y) linhas do caractere.
        y++; 
        work_char++; 
    };
}

// Draw char into a given device context
void 
dc_drawchar (
    struct dccanvas_d *dc, 
    unsigned long x, 
    unsigned long y,  
    unsigned long c,
    unsigned int fgcolor,
    unsigned int bgcolor,
    unsigned long rop )
{
    register int y2=0;
    register int x2=0;
    char *work_char; 
    unsigned char bit_mask = 0x80;

    if ((void *)dc == NULL)
        return;
    if (dc->magic != 1234)
        return;

/*
 Get the font pointer.
 #todo:
 usar variavel g8x8fontAddress.	 
 + Criar e usar uma estrutura para fonte.
 + Usar o ponteiro para a fonte atual que foi carregada.
 + Criar um switch para o tamanho da fonte.
   isso deveria estar na inicialização do módulo char.
 */
 
    if ( FontInitialization.address == 0 ||  
         FontInitialization.width <= 0 || 
         FontInitialization.height <= 0 )
    {
        printf ("dc_drawchar: initialization fail\n");
        while(1){}
    }

// Tentando pintar um espaço em branco.
// Nas rotinas da biblioteca gráfica, quando encontram
//um espaço(32), nem manda para cá, apenas incrementam o cursor.

// O caractere sendo trabalhado.
// Offset da tabela de chars de altura 8 na ROM.

    work_char = 
        (void *) FontInitialization.address + (c * FontInitialization.height);

// Draw:
// Draw a char using a ring3 routine.
// #todo
// Some flag for modification here?
// Put pixel.
// A cor varia de acordo com a mascara de bit.

    for ( y2=0; y2 < FontInitialization.height; y2++ )
    {
        bit_mask = 0x80;

        for ( x2=0; x2 < FontInitialization.width; x2++ )
        {
            /*
            // IN: color, x, y, rop
            libdisp_backbuffer_putpixel ( 
                *work_char & bit_mask ? fgcolor: bgcolor, 
                (x + x2), 
                y,
                (unsigned long) 0 );
            */

            // #test: New method with dc.
            // IN: dc, color, x, y, rop
            putpixel0(
                dc,
                *work_char & bit_mask ? fgcolor: bgcolor, 
                (x + x2), 
                y, 
                rop );

            bit_mask = (bit_mask >> 1); 
        };

        // Próxima linha da (y) linhas do caractere.
        y++; 
        work_char++; 
    };
}

// dc_drawchar_block_style
// Draw a single character inside a given device context (dc)
// using filled rectangles instead of per-pixel plotting.
// Each "on" bit in the font becomes a filled square block.
//
// Benefits:
// - Chunky/retro large-font appearance
// - Integer scaling (scale=1 → classic size, scale=2 → 2× bigger, etc.)
// - Fewer draw operations when scale > 1 (especially useful if putpixel0 is expensive)
//
// IN:
//     dc       - pointer to device context (struct dccanvas_d *)
//     x        - left position in DC coordinates
//     y        - top position in DC coordinates
//     c        - character code (ascii / byte)
//     fgcolor  - color for the "on" blocks
//     bgcolor  - color for the "off" blocks  (used only if opaque_mode == TRUE)
//     rop      - raster operation flag
//     scale    - block size multiplier (1 = normal 8×8, 2 = 16×16 blocks, ...)
//     opaque_mode - TRUE = fill background rectangles (solid char), FALSE = transparent
//
// Notes:
//   - Uses the current font from FontInitialization
//   - Clipping / bounds checking is delegated to putpixel0 / rect drawing in dc
//   - If scale == 1 and opaque_mode == TRUE, it behaves very similarly to original dc_drawchar
//
void
dc_drawchar_block_style(
    struct dccanvas_d  *dc,
    unsigned long   x,
    unsigned long   y,
    unsigned long   c,
    unsigned int    fgcolor,
    unsigned int    bgcolor,
    unsigned long   rop,
    int             scale,
    int             opaque_mode )
{

    // #test: NOT TESTED YET

    if (dc == NULL) return;
    if (dc->magic != 1234) return;

    if (scale < 1) scale = 1;

    // Font not ready
    if (FontInitialization.address == 0 ||
        FontInitialization.width  <= 0 ||
        FontInitialization.height <= 0)
    {
        return;
    }

    int Char = (int)(c & 0xFF);

    unsigned char *work_char_p =
        (unsigned char *)FontInitialization.address + (Char * FontInitialization.height);

    register int row, col;
    unsigned char bit_mask;

    unsigned long block_w = (unsigned long)scale;
    unsigned long block_h = (unsigned long)scale;

    for (row = 0; row < FontInitialization.height; row++)
    {
        bit_mask = 0x80;

        for (col = 0; col < FontInitialization.width; col++)
        {
            unsigned long bx = x + (col * scale);
            unsigned long by = y + (row * scale);

            if (*work_char_p & bit_mask)
            {
                // Foreground block
                putpixel0(dc, fgcolor, bx, by, rop);

                // If scale > 1 → fill the whole block with fgcolor
                if (scale > 1)
                {
                    // You can replace this with your own filled-rect-in-dc function
                    // if you later create one. For now we simulate with multiple putpixels
                    // or call an existing rect fill if available in your dc.
                    //
                    // Option A: naive (slow for large scale)
                    register int i, j;
                    for (j = 0; j < scale; j++)
                    {
                        for (i = 0; i < scale; i++)
                        {
                            putpixel0(dc, fgcolor, bx + i, by + j, rop);
                        }
                    }

                    // Option B: better – if you have a dc_fill_rect / dc_drawrectangle function:
                    // dc_drawrectangle(dc, bx, by, block_w, block_h, fgcolor, TRUE, rop);
                }
            }
            else if (opaque_mode)
            {
                // Background block (only when opaque/solid mode)
                putpixel0(dc, bgcolor, bx, by, rop);

                if (scale > 1)
                {
                    // Same as above – fill block with bgcolor
                    register int i, j;
                    for (j = 0; j < scale; j++)
                    {
                        for (i = 0; i < scale; i++)
                        {
                            putpixel0(dc, bgcolor, bx + i, by + j, rop);
                        }
                    }

                    // Again: better with dc_drawrectangle if you have it
                }
            }

            bit_mask >>= 1;
        }

        // Next font row
        work_char_p++;
    }
}

void
grBackbufferDrawCharBlockStyle(
    unsigned long x,          // top-left in screen space
    unsigned long y,
    unsigned int  fgcolor,
    int           ch,         // character code
    int           scale)      // 1 = classic 8×8, 2 = 16×16 blocks, etc.
{

    // #test: NOT TESTED YET

    if (scale < 1) scale = 1;

    if (!FontInitialization.initialized || FontInitialization.address == 0)
        return;

    int char_index = (int)(ch & 0xFF);
    unsigned char *glyph = 
        (unsigned char *)FontInitialization.address + (char_index * FontInitialization.height);

    int row;
    int col;

    for (row = 0; row < FontInitialization.height; row++)
    {
        unsigned char bits = glyph[row];

        for (col = 0; col < FontInitialization.width; col++)
        {
            if (bits & (0x80 >> col))
            {
                unsigned long bx = x + (col * scale);
                unsigned long by = y + (row * scale);

                rectBackbufferDrawRectangle(
                    bx, by,
                    scale, scale,           // block size
                    fgcolor,
                    TRUE,                   // filled = TRUE
                    0                       // rop normal
                );
            }
        }
    }
}

// grDrawCharBlockStyleInsideWindow
// Draws a single character using filled rectangular blocks
// inside the client area of a given window.
// The blocks give a chunky/retro/pixelated large-font appearance.
//
// IN:
//     wid      - window ID
//     rel_x    - x position relative to client area (0 = left edge of client)
//     rel_y    - y position relative to client area (0 = top edge of client)
//     fgcolor  - color of the "on" pixels/blocks
//     ch       - character code (0–255)
//     scale    - size multiplier of each font pixel (1,2,3,...)
//     rop      - raster operation flag passed to rectBackbufferDrawRectangleInsideWindow
//
// Notes:
//   - Does NOT draw background/transparent pixels (true transparency)
//   - Clipping is handled automatically by rectBackbufferDrawRectangleInsideWindow
//   - If scale == 1 it behaves roughly like the original pixel-style renderer
//   - Uses the current font from FontInitialization
//

void
grDrawCharBlockStyleInsideWindow(
    int           wid,
    unsigned long rel_x,
    unsigned long rel_y,
    unsigned int  fgcolor,
    int           ch,
    int           scale,
    unsigned long rop )
{

    // #test: NOT TESTED YET
    // this function will work only with application windows, not with the root window.

    if (scale < 1) scale = 1;

    // Basic validation
    if (!FontInitialization.initialized || FontInitialization.address == 0)
    {
        return;
    }

    int char_index = (int)(ch & 0xFF);
    unsigned char *glyph =
        (unsigned char *)FontInitialization.address + (char_index * FontInitialization.height);

    int row;
    int col;

    // For each row of the font glyph
    for (row = 0; row < FontInitialization.height; row++)
    {
        unsigned char bits = glyph[row];

        // For each column (bit) in the row
        for (col = 0; col < FontInitialization.width; col++)
        {
            // If this "pixel" in the font is on
            if (bits & (0x80 >> col))
            {
                // Calculate position in client-area coordinates
                unsigned long block_left   = rel_x + (col * scale);
                unsigned long block_top    = rel_y + (row * scale);
                unsigned long block_width  = (unsigned long)scale;
                unsigned long block_height = (unsigned long)scale;

                // Draw one filled block using the window-aware rectangle drawer
                rectBackbufferDrawRectangleInsideWindow(
                    wid,
                    block_left,
                    block_top,
                    block_width,
                    block_height,
                    fgcolor,
                    TRUE,          // always fill
                    rop
                );
            }
            // else: do nothing → leaves background as-is (transparency)
        }
    }
}



int char_initialize(void)
{
// Called by gwsInitGUI() in gws.c.

    CharInitialization.initialized = FALSE;

// Char width and height.
    CharInitialization.width = DEFAULT_FONT_WIDTH;
    CharInitialization.height = DEFAULT_FONT_HEIGHT;

    if (FontInitialization.initialized != TRUE)
    {
        FontInitialization.width = DEFAULT_FONT_WIDTH;
        FontInitialization.height = DEFAULT_FONT_HEIGHT;
    }

    CharInitialization.initialized = TRUE;
    return 0;
}


//
// End
//

