// libgui.c 
// These are display device routines.
// #todo
// The goal here is building a graphics library interface.
// Here we gonna call the routines in the device driver.
// It is an abstraction.

/*
#include "include/vk.h"       // # view input events
#include "include/lt8x8.h"
#include "include/rop.h"
*/


// rtl
#include <types.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <heap.h>
//#include <sys/socket.h>
#include <rtl/gramado.h>

#include "include/libgui.h"


static int __fPaintStarted = FALSE;

// List of canvases
unsigned long libgui_canvasList[CANVAS_COUNT_MAX];



static long __old_mouse_x=0;
static long __old_mouse_y=0;
static long __new_mouse_x=0;
static long __new_mouse_y=0;

//
// private
//

// The application/server is gonna need methods
// to get all these values.

static int libgui_current_mode=0;

// Backbuffer - Dark Zone
static unsigned long libgui_BACKBUFFER_VA=0;
// Frontbuffer (LFB) - Light Zone
static unsigned long libgui_FRONTBUFFER_VA=0;

// #test: 
// Addresses used by the frontbuffer
// struct libgui_frontbuffer_info_d  FrontbufferInfo;

// Saving
static unsigned long libgui_SavedX=0;
static unsigned long libgui_SavedY=0;
static unsigned long libgui_SavedBPP=0; 
// Helper
static unsigned long libgui_device_width=0;
static unsigned long libgui_device_height=0;
static unsigned long libgui_device_bpp=0;
static unsigned long libgui_device_pitch=0;

// Device context
static struct dccanvas_d *libgui_dc_backbuffer;
static struct dccanvas_d *libgui_dc_frontbuffer;



// ====================================================
// Char and font support

struct libgui_char_initialization_d  CharInitialization;

static int __initialize_char_and_font_support(void);

// ===================================================

// Calling kgws in the kernel.
// Using the kgws to refresh the rectangle.
static void 
__kgws_adapter_refresh_rectangle ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height );

static void 
__draw_rectangle_via_kgws ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height,
    unsigned int color,
    unsigned long rop_flags );


static void __draw_component_borders_dc(
    struct dccanvas_d *dc,
    struct ui_component_d *ui_c,
    unsigned int tl_2,  // tl 2 inner (light)
    unsigned int tl_1,  // tl 1 most inner (lighter)
    unsigned int br_2,  // br 2 inner (dark)
    unsigned int br_1,  // br 1 most inner (light) 
    unsigned int outer_color );

// ===================================================


int libgui_start_paint(void)
{
    if (__fPaintStarted == TRUE)
        return 0;
    __fPaintStarted = TRUE;

    return 0;
}

// #maybe: It performs the final bitblt if we implement the
// double buffer in client-side too. And the final flush in this case.
int libgui_end_paint(void)
{
    // ...

    __fPaintStarted = FALSE;

    return 0;
}

//
// Create a new device context for a given buffer.
//
// #ps: 
// We can use this structure to create a dc for 
// drawing inside the client area of an application window.
// In this case the parameters we got with the server 
// calling a function to get informations of a window.
//
// Parameters:
//   base   - pointer to buffer memory
//   width  - width in pixels
//   height - height in pixels
//   bpp    - bits per pixel (e.g. 32, 24, 16)
//
// Returns:
//   pointer to a new dccanvas_d, or NULL on failure.
struct dccanvas_d *libgui_create_dc(
	unsigned char *base,
    unsigned long width,
    unsigned long height,
    unsigned long bpp )
{
    if (!base || width == 0 || height == 0 || bpp == 0)
        return NULL;

	// dc - Device Context struture
    struct dccanvas_d *dc = malloc(sizeof(struct dccanvas_d));
    if (!dc) 
        return NULL;
    memset(dc, 0, sizeof(struct dccanvas_d));

    dc->data          = base;
    dc->device_width  = width;
    dc->device_height = height;
    dc->bpp           = bpp;                  // bits per pixel
    dc->pitch         = width * (bpp / 8);    // bytes per row
    //dc->next        = NULL;

    dc->used = TRUE;
    dc->magic = 1234;
    dc->initialized = TRUE;

    return (struct dccanvas_d *) dc;
}

// Get the pointer for the backbufer dc
struct dccanvas_d *libgui_get_backbuffer_dc(void)
{
    if ((void *) libgui_dc_backbuffer == NULL)
        return NULL;
    if (libgui_dc_backbuffer->magic != 1234)
        return NULL;
    if (libgui_dc_backbuffer->initialized != TRUE)
        return NULL;

    return (struct dccanvas_d *) libgui_dc_backbuffer;
}

// Get the pointer for the frontbufer dc
struct dccanvas_d *libgui_get_frontbuffer_dc(void)
{
    if ((void *) libgui_dc_frontbuffer == NULL)
        return NULL;
    if (libgui_dc_frontbuffer->magic != 1234)
        return NULL;
    if (libgui_dc_frontbuffer->initialized != TRUE)
        return NULL;

    return (struct dccanvas_d *) libgui_dc_frontbuffer;
}


/*
void 
libgd_put_pixel(
    unsigned long x_in_bytes, 
    unsigned long y, 
    unsigned long surface_pitch, 
    unsigned long surface_height,
    unsigned int color,   // 4bytes color. Only 32 bpp.
    void *surface_buffer );
void 
libgd_put_pixel(
    unsigned long x_in_bytes, 
    unsigned long y, 
    unsigned long surface_pitch, 
    unsigned long surface_height,
    unsigned int color,   // 4bytes color. Only 32 bpp.
    void *surface_buffer )
{
// Print a pixel in a given surface.
// 4bytes color. Only 32 bpp.

// x = offset in bytes.
// surface_pitch = How many bytes in the surface.
    if (x_in_bytes >= surface_pitch || y >= surface_height) 
        return;

// Surface
    unsigned int *buf = (unsigned int*) surface_buffer;

//
// Draw
//

    unsigned long line_offset = (unsigned long) (y * surface_pitch);
    unsigned long col_offset  = (unsigned long) x_in_bytes; //Offset in bytes.
    unsigned long address = (unsigned long) (buf + (line_offset + col_offset));

// Draw 4bytes pixel.
// 32bpp.

    //#todo
    //unsigned int *p = (unsigned int *) address; 
    //p[0] = (unsigned int) color;

// #todo
// Não precisa ser assim.
    *(unsigned int*)( (unsigned int*) address ) = (unsigned int) color;
}
*/



// Write pixel inside a canvas.
// IN: dc, x, y, rop
int
libgui_putpixel0 ( 
    struct dccanvas_d *dc,
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags )
{
// #todo: Return the number of changed pixels.

// Validate context
    if (!dc || dc->initialized != TRUE || !dc->data)
        return -1;

// Local copies from dc
    unsigned char *dc_where = dc->data;
    unsigned long dc_width  = dc->device_width;
    unsigned long dc_height = dc->device_height;
    unsigned long dc_bpp    = dc->bpp;    // bits per pixel.
    unsigned long dc_pitch  = dc->pitch;  // bytes per row

// The address where we're gonna put the data into.
// #todo: It needs to be a valid ring3 address.
    //unsigned char *where = (unsigned char *) buffer_va;

// The pixel color.
    unsigned int Color = (unsigned int) (_color & 0xFFFFFFFF);
// The four elements of a color.
    char b=0;
    char g=0;
    char r=0;
    char a=0;
// The first byte:
// #todo: Create defines for these operations.
// 0 ~ FF
    int Operation = (int) (_rop_flags & 0xFF);

// 3 = 24 bpp
// 2 = 16 bpp
// ...
    int bytes_count=0;

    // bits per pixel
    //int bpp   = (int) libgui_SavedBPP;  // get from globals.
    int bpp   = (int) dc_bpp;            // get from dc

    //int width = (int) (libgui_SavedX & 0xFFFF);  // device width
    int width = (int) (dc_width & 0xFFFF);  // device width from dc

// Positions
    int offset=0;   // the offset of the pixel into the buffer.

    int x = (int) (_x & 0xFFFF);
    int y = (int) (_y & 0xFFFF);

    //if (x < 0 || y < 0) return -1;
    //if (x >= (int)dc_width || y >= (int)dc_height) return -1;


// Buffer address validation.
// The address where we're gonna put the data into.
// #todo: It needs to be a valid ring3 address.
/*
    if (buffer_va == 0){
        //panic("libgui_putpixel0: buffer_va\n");
        //server_debug_print("libgui_putpixel0: buffer_va\n");
        return 0;  // 0 changed pixels.
    }
*/

    // #test
    if ((void *) dc_where == NULL)
        return -1;

// Split: bgra
    b = (Color & 0xFF);
    g = (Color & 0xFF00) >> 8;
    r = (Color & 0xFF0000) >> 16;
    a = (Color >> 24) & 0xFF;

// bpp
// #danger
// This is a global variable.
// Esse valor foi herdado do bootloader.

    // Bits per pixel ... lets convert ir in bytes per pixel.
    switch (bpp){
    case 32:  bytes_count=4;  break;
    case 24:  bytes_count=3;  break;
    //#testando
    //case 16:
    //    bytes_count = 2;
    //    break;
    //case 8:
    //    bytes_count = 1;
    //    break;
    default:
        //server_debug_print("libgui_putpixel0: bpp\n");
        printf ("libgui_putpixel0: bpp\n");
        exit(1);
        while(1){}
        break;
    };

    // Device width
    width = (int) (width & 0xFFFF);

//
// Offset
//

// 32bpp
    if (bytes_count==4){
        offset = (int) ( ((width<<2)*y) + (x<<2) );
    }
// 24bpp
    if (bytes_count==3){
        offset = (int) ( (bytes_count*width*y) + (bytes_count*x) );
    }
// 16bpp
    //if (bytes_count==2){
    //    offset = (int) ( ((width<<1)*y) + (x<<1) );
    //}

//
// == Modify ==============================
//

// ROP (Raster Operations)

// ------------------------------------------
// A cor encontrada no buffer.
    unsigned char b2, g2, r2, a2;
// get
    b2 = dc_where[offset];
    g2 = dc_where[offset +1];
    r2 = dc_where[offset +2];
    if (bpp == 32){ a2 = dc_where[offset +3]; };
// ------------------------------------------
// A cor transformada.
// A cor a ser gravada.
    unsigned char b3, g3, r3, a3;
// ------------
// 0 = Sem modificação
// A cor a ser registrada é a mesma enviada por argumento.
// ROP_COPY?
    if (Operation == ROP_COPY){
        r3=r;  g3=g;  b3=b;  a3=a;
    }
// ------------
// 1 = or
    if (Operation == ROP_OR)
    {
        r3 = (r2 | r);
        g3 = (g2 | g);
        b3 = (b2 | b);
        a3 = a2;
    }
// ------------
// 2 = and
    if (Operation == ROP_AND)
    {
        r3 = (r2 & r);
        g3 = (g2 & g);
        b3 = (b2 & b);
        a3 = a2;
    }
// ------------
// 3 = xor
    if (Operation == ROP_XOR)
    {
        r3 = (r2 ^ r);
        g3 = (g2 ^ g);
        b3 = (b2 ^ b);
        a3 = a2;
    }
// -------------------------
// 4 - nand? #text
    if (Operation == ROP_NAND)
    {
        r3 = (unsigned char) ~(r2 & r);
        g3 = (unsigned char) ~(g2 & g);
        b3 = (unsigned char) ~(b2 & b);
        a3 = a2;
    }

// ------------
// 10 - red
    if (Operation == ROP_LESS_RED)
    {
        r3 = (r2 & 0xFE);
        g3 = g2;
        b3 = b2; 
        a3 = a2;
    }
// ------------
// 11 - green
    if (Operation == ROP_LESS_GREEN)
    {
        r3 = r2;
        g3 = (g2 & 0xFE);
        b3 = b2; 
        a3 = a2;
    }
// ------------
// 12 - blue
    if (Operation == ROP_LESS_BLUE)
    {
        r3 = r2;
        g3 = g2;
        b3 = (b2 & 0xFE); 
        a3 = a2;
    }

// ------------
// 20 - gray
    if (Operation == ROP_GRAY_MASK)
    {
        r3 = (r2 & 0x80);
        g3 = (g2 & 0x80);
        b3 = (b2 & 0x80);
        a3 = a2;
    }
// -------------------------
// 21 - no red 
    if (Operation == ROP_REMOVE_RED)
    {
        r3 = (r2 & 0x00);
        g3 = (g2 & 0xFF);
        b3 = (b2 & 0xFF);
        a3 = a2;
    }

// luminosity
// Gray: luminosity = R*0.3 + G*0.59 + B *0.11
    unsigned char common_gray=0;
    if (Operation == ROP_GRAY_LUMA)
    {
        common_gray = 
            (unsigned char)(((r2 * 30) + (g2 * 59) + (b2 * 11)) / 100);

        r3 = common_gray;
        g3 = common_gray;
        b3 = common_gray;
        a3 = a2;

        // #todo
        //common_gray = (unsigned char)((r2 * 77 + g2 * 150 + b2 * 29) >> 8);
        //r3 = g3 = b3 = common_gray;
        //a3 = a2;
    }

// ------------
// 30 - alpha blend (SRC over DST)

    unsigned char myA=0;
    unsigned char invA=0;
    if (Operation == ROP_ALPHA)
    {
        myA  = a;                        // Source alpha (0–255)
        invA = (unsigned char)(255 - myA);

        // Blend each channel: out = (src * A + dst * (255 - A)) / 255
        r3 = (unsigned char)((r * myA + r2 * invA) / 255);
        g3 = (unsigned char)((g * myA + g2 * invA) / 255);
        b3 = (unsigned char)((b * myA + b2 * invA) / 255);

        // Alpha channel policy:
        // Option 1: keep destination alpha
        a3 = a2;

        // Option 2: compute blended alpha
        // a3 = (unsigned char)(myA + ((a2 * invA) / 255));
    }

// ------------
// 31 - Invert (invert destination color channels)
    if (Operation == ROP_INVERT)
    {
        // Each channel becomes 255 - dstChannel
        r3 = (unsigned char)(255 - r2);
        g3 = (unsigned char)(255 - g2);
        b3 = (unsigned char)(255 - b2);

        // Alpha channel policy: keep destination alpha
        a3 = a2;
    }

// ------------
// 32 - Additive blend
// Combine source and destination color channels by simple addition.
// Each channel result is clamped to 255 to avoid overflow.
// This produces a "lighten" or "glow" effect, often used for
// additive blending in graphics pipelines.
// Alpha channel is not blended here; we preserve the destination alpha.
    int __rt = 0;
    int __gt = 0;
    int __bt = 0;

    if (Operation == ROP_ADD)
    {
        __rt = r2 + r;   // add red channels
        __gt = g2 + g;   // add green channels
        __bt = b2 + b;   // add blue channels

        // Clamp each channel to the valid 0–255 range
        r3 = (unsigned char)(__rt > 255 ? 255 : __rt);
        g3 = (unsigned char)(__gt > 255 ? 255 : __gt);
        b3 = (unsigned char)(__bt > 255 ? 255 : __bt);

        // Alpha channel policy: keep destination alpha unchanged
        a3 = a2;
    }

// ------------
// 33 - Multiply (darken/tint effect)
// Each channel is multiplied by the source channel and scaled back to 0–255.
// Formula: out = (src * dst) / 255
// This produces a darker result unless one of the channels is 255.
// Alpha channel is preserved from the destination.
    if (Operation == ROP_MULTIPLY)
    {
        r3 = (unsigned char)((r2 * r) / 255);
        g3 = (unsigned char)((g2 * g) / 255);
        b3 = (unsigned char)((b2 * b) / 255);

        // Alpha channel policy: keep destination alpha unchanged
        a3 = a2;
    }

// 34
// This is useful for skipping writes in a controlled way.
    if (Operation == ROP_KEEP_DST)
    {
        r3 = r2; g3 = g2; b3 = b2; a3 = a2;
    }

// 35
// Take the source color but halve its intensity:
    if (Operation == ROP_HALF_SRC)
    {
        r3 = r >> 1;
        g3 = g >> 1;
        b3 = b >> 1;
        a3 = a;
    }

// 36
// 36 - Half destination (dim dst channels by 50%)
    if (Operation == ROP_HALF_DST)
    {
        r3 = r2 >> 1;
        g3 = g2 >> 1;
        b3 = b2 >> 1;
        a3 = a2;
    }

// 37
// Quick way to get a “lighten” effect:
    if (Operation == ROP_MAX)
    {
        r3 = (r > r2 ? r : r2);
        g3 = (g > g2 ? g : g2);
        b3 = (b > b2 ? b : b2);
        a3 = a2;
    }

// 38
// Opposite of max, gives a “darken” effect:
// ------------
// 38 - Min (darken effect)
// Each channel takes the minimum of src and dst.
// Produces a darker result by keeping the lower intensity.
// Alpha channel preserved from destination.
    if (Operation == ROP_MIN)
    {
        r3 = (r < r2 ? r : r2);
        g3 = (g < g2 ? g : g2);
        b3 = (b < b2 ? b : b2);
        a3 = a2;
   }

// ------------
// 39 - Negate (bitwise NOT of destination channels)
    if (Operation == ROP_NEGATE)
    {
        r3 = (unsigned char)(~r2);
        g3 = (unsigned char)(~g2);
        b3 = (unsigned char)(~b2);

        // Alpha channel policy: keep destination alpha unchanged
        a3 = a2;
    }

// ------------
// 40 - Average (src + dst) / 2
// Each channel is averaged between source and destination.
// Produces a mid‑tone blend of the two colors.
// Alpha channel preserved from destination.
    if (Operation == ROP_AVERAGE)
    {
        r3 = (unsigned char)((r + r2) >> 1);
        g3 = (unsigned char)((g + g2) >> 1);
        b3 = (unsigned char)((b + b2) >> 1);
        a3 = a2;
    }

// ------------
// 41 - Difference (abs(src - dst))
// Each channel is the absolute difference between source and destination.
// Produces a high‑contrast effect where colors differ.
// Alpha channel preserved from destination.
    if (Operation == ROP_DIFF)
    {
        r3 = (unsigned char)( (r > r2) ? (r - r2) : (r2 - r) );
        g3 = (unsigned char)( (g > g2) ? (g - g2) : (g2 - g) );
        b3 = (unsigned char)( (b > b2) ? (b - b2) : (b2 - b) );
        a3 = a2;
    }

// ------------
// 42 - Brighten (dst + 64, clamped)
// Adds a fixed value to each destination channel.
// Produces a lighter/brighter effect.
// Alpha channel preserved from destination.
    int addVal = 64; // tweak as needed
    int my_rt = 0;
    int my_gt = 0;
    int my_bt = 0;
    if (Operation == ROP_BRIGHTEN)
    {
        addVal = 64; // tweak as needed
        my_rt = r2 + addVal;
        my_gt = g2 + addVal;
        my_bt = b2 + addVal;

        r3 = (unsigned char)(my_rt > 255 ? 255 : my_rt);
        g3 = (unsigned char)(my_gt > 255 ? 255 : my_gt);
        b3 = (unsigned char)(my_bt > 255 ? 255 : my_bt);
        a3 = a2;
    }


//
// == Register =====================
// 

// BGR and A
    dc_where[offset]    = b3;
    dc_where[offset +1] = g3;
    dc_where[offset +2] = r3;
    if (bpp == 32){ dc_where[offset +3] = a3; };

// Return the number of changed pixels
    return (int) 1;
}



// #test
// Draw a pixel inside a canvas, given it's device context.
// #todo: No Clipping support yet
void 
libgui_draw_horizontal_line_dc ( 
    struct dccanvas_d *dc,
    unsigned long x1,
    unsigned long y, 
    unsigned long x2, 
    unsigned int color,
    unsigned long rop_flags )
{
// #ps: Not tested yet

    if ((void*)dc == NULL)
        return;

// #todo
// Maybe we need checking some limits here.
    if (x1 > x2){
        return;
    }
// IN: color, x, y, rop flags
    while (x1 < x2)
    {
        //backbuffer_putpixel( color, x1, y, rop_flags ); 

        // #test:
        // Draw a pixel inside a canvas, given it's device context.
        // see: libdisp.c
        // IN: dc, color, x, y, rop
        /*
		putpixel0 (
                dc,
                color, 
                x1, 
                y, 
                rop_flags 
		*/

        // #test: New method with dc.
        // IN: dc, color, x, y, rop
        libgui_putpixel0(
                dc,
                color,  //*work_char & bit_mask ? fgcolor: bgcolor, 
                x1, 
                y, 
                rop_flags
        );

        x1++;
    };
}

static int __initialize_char_and_font_support(void)
{
    CharInitialization.initialized = FALSE;

// Char width and height.
    CharInitialization.width = LIBGUI_DEFAULT_FONT_WIDTH;
    CharInitialization.height = LIBGUI_DEFAULT_FONT_HEIGHT;

    FontInitialization.initialized = FALSE;
    if (FontInitialization.initialized != TRUE)
    {
        FontInitialization.width = LIBGUI_DEFAULT_FONT_WIDTH;
        FontInitialization.height = LIBGUI_DEFAULT_FONT_HEIGHT;
        FontInitialization.address = (unsigned long) &font_lin8x8[0];
        FontInitialization.index_for_current_font = 0;
        FontInitialization.initialized = TRUE;
    }

    CharInitialization.initialized = TRUE;
    return 0;
}

// #todo
// Draw char into a given device context
void 
libgui_drawchar_dc (
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

// Get the font pointer.
// #todo:
// usar variavel g8x8fontAddress.	 
// + Criar e usar uma estrutura para fonte.
// + Usar o ponteiro para a fonte atual que foi carregada.
// + Criar um switch para o tamanho da fonte.
//   isso deveria estar na inicialização do módulo char.
 
    if ( FontInitialization.address == 0 ||  
         FontInitialization.width <= 0 || 
         FontInitialization.height <= 0 )
    {
        printf ("libgui_drawchar_dc: initialization fail\n");
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

            // #test: New method with dc.
            // IN: dc, color, x, y, rop
            libgui_putpixel0(
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


// Draw a string into a device context (dc)
// starting at (x, y), using fg/bg colors and rop.
void 
libgui_drawstring_dc (
    struct dccanvas_d *dc,
    unsigned long x, 
	unsigned long y,
    unsigned int fg_color,
    unsigned int bg_color,
    unsigned long rop,
    const char *string )
{
    int i=0;

    if (!dc || !string)
        return;

    unsigned long cursor_x = x;
    unsigned long cursor_y = y;

    for (i=0; string[i] != '\0'; i++) 
	{
        char c = string[i];

        // Draw one character
        libgui_drawchar_dc(
            dc,
            cursor_x,
            cursor_y,
            c,
            fg_color,
            bg_color,
            rop
        );

        // Advance cursor horizontally by font width (8px for 8x8 font)
        cursor_x += 8;
    }
}


// ======================================
// Copy a rectangle
// #todo
// IN:
// w, h, 
// dst l, dst t, dst address, 
// src l, src t, src address.
void 
libgui_refresh_rectangle1 ( 
    unsigned long width,    // common
    unsigned long height,   // common
    unsigned long dst_x,        // dst stuff
    unsigned long dst_y,        // dst stuff
    unsigned long buffer_dest,  // dst stuff
    unsigned long src_x,        // src stuff
    unsigned long src_y,        // src stuff
    unsigned long buffer_src )  // src stuff
{

    //debug_print("refresh_rectangle: r0 :)\n");

    //void *dest       = (void *)      FRONTBUFFER_ADDRESS;
    //const void *src  = (const void*) BACKBUFFER_ADDRESS;
    void *dest       = (void *)      buffer_dest;
    const void *src  = (const void*) buffer_src;

// loop
    register unsigned int i=0;
    register unsigned int lines=0;
    unsigned int line_size=0; 
    register int count=0; 

// Screen pitch.
// screen line size in pixels * bytes per pixel.
    unsigned int screen_pitch=0;  
// Rectangle pitch
// rectangle line size in pixels * bytes per pixel.
    unsigned int rectangle_pitch=0;  

    unsigned int src_offset=0;
    unsigned int dst_offset=0;

// = 3; 24bpp
    int bytes_count=0;

// First line for both
    int dstFirstLine = (int) (dst_y & 0xFFFF);
    int srcFirstLine = (int) (src_y & 0xFFFF);

    //int UseVSync = FALSE;
    int UseClipping = TRUE;

//==========
// dc
    //unsigned long deviceWidth  = (unsigned long) screenGetWidth();
    //unsigned long deviceHeight = (unsigned long) screenGetHeight();

    // Device info
    // #todo:
    // If it is doing a syscall we need to change it. (slow)
    //unsigned long deviceWidth  = (unsigned long) gws_get_device_width();
    //unsigned long deviceHeight = (unsigned long) gws_get_device_height();
    unsigned long deviceWidth  = (unsigned long) libgui_SavedX;
    unsigned long deviceHeight = (unsigned long) libgui_SavedY;

    if ( deviceWidth == 0 || deviceHeight == 0 )
    {
        debug_print ("refresh_rectangle: w h\n");
        //panic       ("refresh_rectangle: w h\n");
        return;
    }

//
// Internal
//

    unsigned long dstX = (unsigned long) (dst_x & 0xFFFF);
    unsigned long srcX = (unsigned long) (src_x & 0xFFFF);

    unsigned long dstY = (unsigned long) (dst_y & 0xFFFF);
    unsigned long srcY = (unsigned long) (src_y & 0xFFFF);

// both
    line_size = (unsigned int) (width  & 0xFFFF); 
    lines     = (unsigned int) (height & 0xFFFF);

    switch (libgui_SavedBPP){
    // (32/8)
    case 32:
        bytes_count = 4;
        break;
    // (24/8)
    case 24:
        bytes_count = 3;
        break;
    // ...
    default:
        //panic ("refresh_rectangle: SavedBPP\n");
        return;
        break;
    };

//
// Pitch
//

// Screen pitch.
// Screen line size in pixels plus bytes per pixel.
    screen_pitch = (unsigned int) (bytes_count * deviceWidth);

// both
// Rectangle pitch.
// rectangle line size in pixels * bytes per pixel.
//(line_size * bytes_count) é o número de bytes por linha. 
    rectangle_pitch = (unsigned int) (bytes_count * line_size);

// #atenção.
//offset = (unsigned int) BUFFER_PIXEL_OFFSET( x, y );

    dst_offset = (unsigned int) ( (dstY*screen_pitch) + (bytes_count*dstX) );
    src_offset = (unsigned int) ( (srcY*screen_pitch) + (bytes_count*srcX) );

    dest = (void *)       (dest + dst_offset); 
    src  = (const void *) (src  + src_offset); 

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

    if ((rectangle_pitch % 8) == 0)
    {
        count = (rectangle_pitch >> 3);
        for (i=0; i < lines; i++)
        {
            if (UseClipping == TRUE)
            {
                if ( (dstFirstLine + i) > deviceHeight )
                { 
                    break; 
                }
                if ( (srcFirstLine + i) > deviceHeight )
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

    if ((rectangle_pitch % 4) == 0)
    {
        count = (rectangle_pitch >> 2);
        for (i=0; i < lines; i++)
        {
            if (UseClipping == TRUE)
            {
                if ((dstFirstLine + i) > deviceHeight)
                { 
                    break; 
                }
                if ((srcFirstLine + i) > deviceHeight)
                { 
                    break; 
                }
            }
            memcpy32 ( (void *) dest, (const void *) src, count );
            //__rect_memcpy32 ( (void *) dest, (const void *) src, count );
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

    if ((rectangle_pitch % 4) != 0)
    {
        for (i=0; i < lines; i++){
            if (UseClipping == TRUE)
            {
                if ((dstFirstLine + i) > deviceHeight)
                { 
                    break; 
                }
                if ((srcFirstLine + i) > deviceHeight)
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

// Blit from one canvas into another.
// src_canvas: source canvas (offscreen, window, etc.)
// dst_canvas: destination canvas (backbuffer, frontbuffer, etc.)
void 
libgui_blit_canvas_to_canvas_imp(
    struct canvas_information_d *src_canvas,
    struct canvas_information_d *dst_canvas,
    int dst_x, int dst_y,
    int width, int height )
{

// Validation
    if ((void*) src_canvas == NULL || (void*) dst_canvas == NULL)
        return;
    if (src_canvas->used != TRUE || dst_canvas->used != TRUE)
        return;
    if (src_canvas->magic != 1234 || dst_canvas->magic != 1234)
        return;
    if (src_canvas->initialized != TRUE || dst_canvas->initialized != TRUE)
        return;
    if (src_canvas->base == NULL || dst_canvas->base == NULL)
        return;

    // Copy rectangle from source canvas (always top-left for now)
    libgui_refresh_rectangle1(
        width, height,
        dst_x, dst_y, (unsigned long) dst_canvas->base,   // destination
        0, 0, (unsigned long) src_canvas->base            // source
    );
}

// Given the indexes
void 
libgui_blit_canvas_to_canvas(
    int id_src_canvas,
    int id_dst_canvas,
    int dst_x, int dst_y,
    int width, int height )
{
    struct canvas_information_d *src;
    struct canvas_information_d *dst;

// We have few canvases for now
    if (id_src_canvas < 0 || id_src_canvas >= CANVAS_COUNT_MAX)
        return;
    if (id_dst_canvas < 0 || id_dst_canvas >= CANVAS_COUNT_MAX)
        return;
    // #todo: More filters.

    src = (struct canvas_information_d *) libgui_canvasList[id_src_canvas];
    if ((void *) src == NULL)
        return;
    if (src->magic != 1234)
        return;

    dst = (struct canvas_information_d *) libgui_canvasList[id_dst_canvas];
    if ((void *) dst == NULL)
        return;
    if (dst->magic != 1234)
        return;

    // Worker
    libgui_blit_canvas_to_canvas_imp(
        (struct canvas_information_d *) src,
        (struct canvas_information_d *) dst,
        dst_x, dst_y, width, height 
    );
}

// Initialize a list of pointer for canvas information structures
int lingui_initialize_canvas_list(void)
{
    int i;

    for (i=0; i<CANVAS_COUNT_MAX; i++){
        libgui_canvasList[i] = 0;
    }
    
    return 0;
}




//======================================
// Calling kgws in the kernel.
// Using the kgws to refresh the rectangle.
static void 
__kgws_adapter_refresh_rectangle ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height )
{
    static unsigned long buffer[5];

    buffer[0] = (unsigned long) x;
    buffer[1] = (unsigned long) y;
    buffer[2] = (unsigned long) (width  & 0xFFFF);
    buffer[3] = (unsigned long) (height & 0xFFFF);
    buffer[4] = 0; 

    sc80(10, (unsigned long) buffer, 0, 0);
}


//======================================
// Local worker.
// Calling kgws in the kernel.
// Using the kgws to draw the rectangle.
// #todo
// At this moment, no structure ware invalidated.
// So, the caller needs to specify a rect structure,
// this way we can invalidated it.
// IN: l, t, w, h, bg color, rop flags.
static void 
__draw_rectangle_via_kgws ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height,
    unsigned int color,
    unsigned long rop_flags )
{
    static unsigned long Buffer[6];

// Set parameters.
    Buffer[0] = (unsigned long) x;
    Buffer[1] = (unsigned long) y;
    Buffer[2] = (unsigned long) (width  & 0xFFFF);
    Buffer[3] = (unsigned long) (height & 0xFFFF);
    Buffer[4] = (unsigned long) (color & 0xFFFFFFFF);
    Buffer[5] = (unsigned long) rop_flags;

// Syscall 0x80, service 9.
// Refresh rectangle using the kernel services.

    sc80(9, (unsigned long) Buffer, 0, 0);
}




// Fill a rectangle, clipped against the dc. ROP_COPY only.
// Much cheaper than lingui_draw_rectangle0_dc(), which goes
// through the ROP chain for every single pixel.
// Valid only for 24bpp and 32bpp
void 
libgui_simple_fill_rect(
    struct dccanvas_d *dc,
    long x, long y, long w, long h,
    unsigned int color )
{
    long x2, y2, yy, xx;
    long dw, dh;
    unsigned char *row;
    unsigned int c = (color & 0x00FFFFFF);
    unsigned char b = (unsigned char)( c        & 0xFF);
    unsigned char g = (unsigned char)((c >> 8)  & 0xFF);
    unsigned char r = (unsigned char)((c >> 16) & 0xFF);

    if ((void*) dc == NULL || (void*) dc->data == NULL)
        return;
    if (w <= 0 || h <= 0)
        return;

    dw = (long) dc->device_width;
    dh = (long) dc->device_height;

    x2 = x + w;
    y2 = y + h;
    if (x  < 0)  x  = 0;
    if (y  < 0)  y  = 0;
    if (x2 > dw) x2 = dw;
    if (y2 > dh) y2 = dh;
    if (x >= x2 || y >= y2)
        return;

    for (yy = y; yy < y2; yy++)
    {
        row = (unsigned char *) dc->data + ((unsigned long) yy * dc->pitch);

        if (dc->bpp == 32)
        {
            unsigned int *p = (unsigned int *) (row + ((unsigned long) x << 2));
            for (xx = x; xx < x2; xx++)
                *p++ = c;
        }
        else if (dc->bpp == 24)
        {
            unsigned char *p = row + ((unsigned long) x * 3);
            for (xx = x; xx < x2; xx++)
            {
                p[0] = b;
                p[1] = g;
                p[2] = r;
                p += 3;
            }
        }

        // #bugbug
        // No more options here.
    }
}

// Drawing a rectangle inside a given canvas,
// given its device context.
void
lingui_draw_rectangle0_dc(
    struct dccanvas_d *dc,
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height,
    unsigned int color,
    unsigned long rop )
{
// #ps: Not tested yet

    int i=0;
    int NumberOfLines=0;

    if ((void*) dc == NULL){
        printf("dc_draw_rectangle0: dc\n");
        goto fail;
    }

    unsigned long DeviceWidth = dc->device_width;
    unsigned long DeviceHeight = dc->device_height;
    // ...

//
// Clipping
//

    // Starting out of limits
    if (left >= DeviceWidth){
        printf("dc_draw_rectangle0: left\n");
        return;
    }
    if (top >= DeviceHeight){
        printf("dc_draw_rectangle0: top\n");
        return;
    }

    // Available space
    unsigned long w_space = (DeviceWidth - left);
    unsigned long h_space = (DeviceHeight - top);
    // Final values
    unsigned long final_width = width;
    unsigned long final_height = height;

    if (width > w_space)
        final_width = w_space;
    if (height > h_space)
        final_height = h_space;

    if (final_width == 0){
        printf("dc_draw_rectangle0: final_width\n");
        return;
    }
    if (final_height == 0){
        printf("dc_draw_rectangle0: final_height\n");
        return;
    }

    unsigned long __right  = (unsigned long) (left + final_width);
    //unsigned long __bottom = (unsigned long) (top  + final_height); 

    if (__right > DeviceWidth)
        __right = DeviceWidth;
    //if (__bottom > DeviceHeight)
        //__bottom = DeviceHeight;

    NumberOfLines = final_height;

//
// Drawing multiple lines inside the canvas
//

    for (i=0; i<NumberOfLines; i++)
    {
        libgui_draw_horizontal_line_dc ( 
            dc,
            left,      // x1
            (top +i),  // y 
            __right,   // x2 
            color,
            rop 
        );
    };

fail:
    return;
}



void
libgui_DrawCharBlockStyle_dc(
    struct dccanvas_d *dc,
    unsigned long x,          // top-left in screen space
    unsigned long y,
    unsigned int fgcolor,
    int ch,         // character code
    int scale )     // 1 = classic 8×8, 2 = 16×16 blocks, etc.
{
    if ((void*) dc == NULL)
        return;

    if (scale < 1){
	    scale = 1;
	}

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

                lingui_draw_rectangle0_dc ( 
                    dc, 
                    bx, 
                    by,
                    scale, scale,
                    fgcolor,
                    0         // ROP
                );
            }
        };
    };
}

void 
libgui_drawstringblock_dc(
    struct dccanvas_d *dc,
    unsigned long x,
    unsigned long y,
    unsigned int color,
    const char *str,
    int scale )
{
    int advance = FontInitialization.width * scale;
    unsigned long cx = x;

    if ((void*) dc == NULL)
        return;

    while (*str)
    {
        if (*str == ' ') {
            cx += advance;          // or advance / 2 for tighter word spacing
        } else {

            libgui_DrawCharBlockStyle_dc(
                dc, cx, y, color, *str, scale );

			cx += advance;
        }
        str++;
    };
}

void libgui_set_mouse_pointer(unsigned long x, unsigned long y)
{
    __new_mouse_x = x;
    __new_mouse_y = y;
}

// Visual effect on button pressed
// #todo: Maybe the dc can be a field of ui component structure
void libgui_on_button_pressed(struct ui_component_d *uic, struct dccanvas_d *dc)
{
    if ((void*) uic == NULL)
        return;
    if (uic->magic != 1234)
        return;
    if ((void*) dc == NULL)
        return;
    if (dc->magic != 1234)
        return;

    libgui_set_ui_component_status(uic, BS_PRESSED);
    libgui_redraw_ui_component(uic, dc);
}

// Visual effect on button pressed
// #todo: Maybe the dc can be a field of ui component structure
void libgui_on_button_released(struct ui_component_d *uic, struct dccanvas_d *dc)
{
    if ((void*) uic == NULL)
        return;
    if (uic->magic != 1234)
        return;
    if ((void*) dc == NULL)
        return;
    if (dc->magic != 1234)
        return;

    libgui_set_ui_component_status(uic, BS_RELEASED);
    libgui_redraw_ui_component(uic, dc);
}

// Draw button given the dc and the ui component
static void __draw_component_borders_dc(
    struct dccanvas_d *dc,
    struct ui_component_d *ui_c,
    unsigned int tl_2,  // tl 2 inner (light)
    unsigned int tl_1,  // tl 1 most inner (lighter)
    unsigned int br_2,  // br 2 inner (dark)
    unsigned int br_1,  // br 1 most inner (light) 
    unsigned int outer_color )
{
// #test
// Size in pixels de apenas 1/3 de todo o size.
    unsigned long BorderSize = 1;
// Isso deve ser o total.
    //window->border_size = ?

    //debug_print("__draw_button_borders:\n");

// The dc
    if ((void*) dc == NULL){
        return;
    }
    if (dc->magic != 1234){
        return;
    }

// The ui component
    if ((void*) ui_c == NULL){
        return;
    }
    if (ui_c->magic != 1234){
        return;
    }

// Order:
// top/left ... right/bottom.

//  ____
// |
//
// board1, borda de cima e esquerda.
// Cores, de fora pra dentro:
// outer_color, color1, color1.

// -------------------------------
// :: Top
// top, top+1, top+2

    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left +1,
        ui_c->top,
        ui_c->width -2,
        BorderSize,   // height
        outer_color,  // color
        0             // rop 
    );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left +1,
        ui_c->top  +1,
        ui_c->width -2,
        BorderSize,  // height
        tl_2,        // color
        0            // rop 
    );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left +1+1,
        ui_c->top  +1+1,
        ui_c->width -4,
        BorderSize,  // height
        tl_1,        // color
        0            // rop 
    );

// -------------------------------
// :: Left
// left, left+1, left+2

    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left,
        ui_c->top  +1,
        BorderSize,       // width 
        ui_c->height -2,  // height
        outer_color,      // color
        0                 // rop 
    );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left +1,
        ui_c->top  +1,
        BorderSize,       // width 
        ui_c->height -2,  // height
        tl_2,             // color
        0                 // rop 
    );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left +1+1,
        ui_c->top  +1+1,
        BorderSize,       // width 
        ui_c->height -4,  // height
        tl_1,             // color
        0                 // rop 
    );


//  
//  ____|
//
// board2, borda direita e baixo.
// Cores, de fora pra dentro:
// outer_color, color2, color2_light.

// -------------------------------
// :: Right
// right-3, right-2, right-1

    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left + ui_c->width -1,
        ui_c->top +1,
        BorderSize,       // width
        ui_c->height -2,  // height
        outer_color,      // color
        0                 // rop 
    );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left + ui_c->width -2,
        ui_c->top +1,
        BorderSize,       // width
        ui_c->height -2,  // height
        br_2,             // color
        0                 // rop 
    );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left + ui_c->width -3,
        ui_c->top +1+1,
        BorderSize,       // width
        ui_c->height -4,  // height
        br_1,             // color
        0                 // rop 
    );


// -------------------------------
// :: Bottom
// bottom-1, bottom-2, bottom-3

    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left +1,
        ui_c->top + ui_c->height -1,
        ui_c->width -2,  // width
        BorderSize,      // height
        outer_color,     // color
        0                // rop 
    );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left +1,
        ui_c->top + ui_c->height -2,
        ui_c->width -2,  // width
        BorderSize,      // height
        br_2,            // color
        0                // rop 
    );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left +1+1,
        ui_c->top + ui_c->height -3,
        ui_c->width -4,  // width
        BorderSize,      // height
        br_1,            // color
        0                // rop 
    );
}

// #todo:
// We need to handle the ID of the component.
// Maybe listing them in a linked list or array.

struct ui_component_d *libgui_create_ui_component(
    struct dccanvas_d *dc,
    int type,
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height,
    const char *label,
    unsigned long flags )
{
    struct ui_component_d *uic;
    unsigned long text_x=0;
    unsigned long text_y=0;
    unsigned long LongSize=0;
    unsigned long scale = 1;

    unsigned int  bg_color = 0x00E0E0E0;
    int has_borders = FALSE;   // button-like components
    unsigned int  text_color = 0x00101010;
    int is_editbox  = FALSE;   // sunken border, white background


    if ((void*) dc == NULL)
	    return NULL;
	if (dc->magic != 1234)
	    return NULL;
	if (type < 0){
	    return NULL;
    }

    uic = (struct ui_component_d *) malloc(sizeof(struct ui_component_d));
    if ((void*) uic == NULL)
	    return NULL;
    memset(uic, 0, sizeof(struct ui_component_d));

    uic->used = TRUE;
	uic->magic = 1234;
    uic->id = -1;  // #todo: We need to handle the ID of the component

    uic->left = left;
    uic->top = top;
    uic->width = width;
    uic->height = height;

    uic->status = 0;  // default status

// ================================

    if (label != NULL) {
        strncpy(uic->label, label, sizeof(uic->label)-1);
        uic->label[sizeof(uic->label)-1] = '\0';  // ensure null-termination
        uic->label_size = strlen(uic->label);
    } else {
        uic->label[0] = '\0';
        uic->label_size = 0;
    }

// ================================

    uic->type = type;
    uic->flags = (unsigned long) flags;

// ================================
// Per-type settings (the only things that differ)

    switch (type){
    case UI_COMPONENT_BUTTON:
        scale = 1;
        bg_color = LIBGUI_COLOR_BUTTON_FACE;  //0x00C0C0C0;
        has_borders = TRUE;
        text_x = left + 8;
        text_y = top  + 8;
        break;
    case UI_COMPONENT_ICON:
        scale = 1;
        bg_color = LIBGUI_COLOR_ICON_FACE;  //0x00C0C0C0;
        has_borders = TRUE;
        text_x = left + 8;
        text_y = top  + 8;
        break;
    case UI_COMPONENT_LABEL:
        scale = 1;
        bg_color = LIBGUI_COLOR_PANEL_FACE;  //0x00E0E0E0;
        text_x = left + 8;
        text_y = top  + 8;
        break;
    case UI_COMPONENT_HEADER:
        scale = 2;
        bg_color = LIBGUI_COLOR_PANEL_FACE;  //0x00E0E0E0;
        text_x = left + 8;
        text_y = top  + 8;
        break;
    case UI_COMPONENT_FOOTER:
        scale = 1;
        bg_color = LIBGUI_COLOR_PANEL_FACE;  //0x00E0E0E0;
        text_x = left + 8;
        text_y = top  + 8;
        break;
    case UI_COMPONENT_EDITBOX:
        scale = 1;
        bg_color = LIBGUI_COLOR_EDITBOX_BG;      // 0x00FFFFFF
        text_color = LIBGUI_COLOR_EDITBOX_TEXT;  // 0x00101010
        is_editbox = TRUE;
        text_x = left + 4;                       // inner padding
        text_y = top  + 4;
        break;
    default:
        // Unknown type: keep the old behavior (nothing is drawn).
        return (struct ui_component_d *) uic;
    };

// ====================================

    // Center horizontally based on the component width
    if ((uic->flags & 0x0001) && is_editbox == FALSE)
    {
        LongSize = (unsigned long) (uic->label_size * 8 * scale);  // 8px per char
        if (width > LongSize)
            text_x = left + ((width - LongSize) / 2);
    }
    // Center vertically based on the component height
    // (the editbox is always centered vertically)
    // #test: Using (8x16) for now, because we still do not have the char dimensions.
    if ((uic->flags & 0x0002) || is_editbox == TRUE)
    {
        // LongSize = (unsigned long) (1 * 16 * scale);  // 16px per char
        // LongSize = (unsigned long) (1 * 8 * scale);  // 8px per char
        LongSize = (unsigned long) (1 * FontInitialization.height * scale);
        if (height > LongSize)
            text_y = top + ((height - LongSize) / 2);
    }

// ================================
// Background (color depends on the type)

    lingui_draw_rectangle0_dc(
        dc,
        left, top, width, height,
        bg_color,
        0   // rop
    );

// =======================================

// Borders (only for button-like components)

    if (has_borders == TRUE)
    {
        // #todo: Hardcoded for now. Maybe the function can receive a parameter for the status.
        uic->status = BS_DEFAULT;  // default status for buttons
        __draw_component_borders_dc(
            dc,
            uic,
            0x00E0E0E0,  // tl 2 inner (light)
            0x00FAFAFA,  // tl 1 most inner (lighter)
            0x00505050,  // br 2 inner (dark)
            0x00989898,  // br 1 most inner (light)
            0x00101010   // outer
        );
    }

// Sunken (editbox): same routine, sunken colors.
    if (is_editbox == TRUE)
    {
        uic->status = BS_DEFAULT;
        __draw_component_borders_dc(
            dc,
            uic,
            LIBGUI_COLOR_BEVEL_SUNKEN_TL_2,
            LIBGUI_COLOR_BEVEL_SUNKEN_TL_1,
            LIBGUI_COLOR_BEVEL_SUNKEN_BR_2,
            LIBGUI_COLOR_BEVEL_SUNKEN_BR_1,
            0x00101010   // outer
        );
    }


// =====================================

// Label (drawn once, on top of everything)

    if (uic->label_size > 0)
    {
        const char *str = uic->label;

        // Editbox: if the text doesn't fit, show the tail.
        if (is_editbox == TRUE)
        {
            unsigned long char_w = FontInitialization.width * scale;
            unsigned long inner_w = (width > 8) ? (width - 8) : 0;  // 4px padding each side
            unsigned long max_chars = (char_w > 0) ? (inner_w / char_w) : 0;

            if (max_chars == 0)
                return (struct ui_component_d *) uic;

            if (uic->label_size > max_chars)
                str = uic->label + (uic->label_size - max_chars);
        }

        libgui_drawstringblock_dc(
            dc, text_x, text_y, text_color, uic->label, scale );
    }

    return (struct ui_component_d *) uic;
}


/*
Raised button
top/left = light
bottom/right = dark

Pressed button
top/left = dark
bottom/right = light
*/


int 
libgui_redraw_ui_component(
	struct ui_component_d *uic,
    struct dccanvas_d *dc )
{
    unsigned long text_x=0;
    unsigned long text_y=0;
    unsigned long LongSize=0;
    unsigned long scale = 1;  // default

    unsigned int bg_color = 0x00E0E0E0;
    unsigned int text_color = 0x00101010;
    int has_borders = FALSE;   // raised, button-like components
    int is_editbox  = FALSE;   // sunken border, white background

    // Border colors (used when has_borders == TRUE or is_editbox == TRUE)
    unsigned int tl1_color = 0;  // tl 1 most inner
    unsigned int tl2_color = 0;  // tl 2 inner
    unsigned int br1_color = 0;  // br 1 most inner
    unsigned int br2_color = 0;  // br 2 inner



	// uic
    if ((void*) uic == NULL)
        return -1;
	if (uic->magic != 1234)
        return -1;

	// dc
    if ((void*) dc == NULL)
        return -1;
	if (dc->magic != 1234)
        return -1;

	if (uic->type < 0)
	    return -1;



// ================================
// Per-type settings (the only things that differ)

    switch (uic->type){
    case UI_COMPONENT_BUTTON:
        scale = 1;
        bg_color = LIBGUI_COLOR_BUTTON_FACE;  //0x00C0C0C0;
        has_borders = TRUE;
        text_x = uic->left + 8;
        text_y = uic->top  + 8;
        break;
    case UI_COMPONENT_ICON:
        scale = 1;
        bg_color = LIBGUI_COLOR_ICON_FACE; //0x00C0C0C0;
        has_borders = TRUE;
        text_x = uic->left + 8;
        text_y = uic->top  + 8;
        break;
    case UI_COMPONENT_LABEL:
        scale = 1;   // same as create (was 2 here before)
        bg_color = LIBGUI_COLOR_PANEL_FACE;  //0x00E0E0E0;
        text_x = uic->left + 8;
        text_y = uic->top  + 8;
        break;
    case UI_COMPONENT_HEADER:
        scale = 2;
        bg_color = LIBGUI_COLOR_PANEL_FACE; //0x00E0E0E0;
        text_x = uic->left + 8;
        text_y = uic->top  + 8;
        break;
    case UI_COMPONENT_FOOTER:
        scale = 1;
        bg_color = LIBGUI_COLOR_PANEL_FACE; //0x00E0E0E0;
        text_x = uic->left + 8;
        text_y = uic->top  + 8;
        break;
    case UI_COMPONENT_EDITBOX:
        scale = 1;
        bg_color = LIBGUI_COLOR_EDITBOX_BG;
        text_color = LIBGUI_COLOR_EDITBOX_TEXT;
        is_editbox = TRUE;
        text_x = uic->left + 4;   // inner padding
        text_y = uic->top  + 4;
        break;
    default:
        // Unknown type: nothing to draw.
        return -1;
    };


// ==================================================
// Border colors depending on the status (button-like only)

    if (has_borders == TRUE)
    {
    
        // #test: Changing colors based on the button's status
        switch (uic->status){
        case BS_RELEASED:
            tl1_color = LIBGUI_COLOR_BEVEL_RAISED_TL_1;  //0x00FAFAFA;  // tl 1 most inner (lighter)
            tl2_color = LIBGUI_COLOR_BEVEL_RAISED_TL_2;  //0x00E0E0E0;  // tl 2 inner (light)
            br1_color = LIBGUI_COLOR_BEVEL_RAISED_BR_1;  //0x00989898;  // br 1 most inner (light) 
            br2_color = LIBGUI_COLOR_BEVEL_RAISED_BR_2;  //0x00505050;  // br 2 inner (dark)
            break;
        case BS_FOCUS:
            tl1_color = LIBGUI_COLOR_BEVEL_RAISED_TL_1;  //0x00FAFAFA;  // tl 1 most inner (lighter)
            tl2_color = LIBGUI_COLOR_BEVEL_RAISED_TL_2;  //0x00E0E0E0;  // tl 2 inner (light)
            br1_color = LIBGUI_COLOR_BEVEL_RAISED_BR_1;  //0x00989898;  // br 1 most inner (light) 
            br2_color = LIBGUI_COLOR_BEVEL_RAISED_BR_2;  //0x00505050;  // br 2 inner (dark)
            break;
        case BS_PRESSED:
            tl1_color = LIBGUI_COLOR_BEVEL_SUNKEN_TL_1;  //0x00505050;  // tl 1 most inner (dark)
            tl2_color = LIBGUI_COLOR_BEVEL_SUNKEN_TL_2;  //0x00989898;  // tl 2 inner (light)
            br1_color = LIBGUI_COLOR_BEVEL_SUNKEN_BR_1;  //0x00E0E0E0;  // br 1 most inner (light) 
            br2_color = LIBGUI_COLOR_BEVEL_SUNKEN_BR_2;  //0x00FAFAFA;  // br 2 inner (lighter)
            break;
        case BS_HOVER:
            tl1_color = LIBGUI_COLOR_BEVEL_RAISED_TL_1;  //0x00FAFAFA;  // tl 1 most inner (lighter)
            tl2_color = LIBGUI_COLOR_BEVEL_RAISED_TL_2;  //0x00E0E0E0;  // tl 2 inner (light)
            br1_color = LIBGUI_COLOR_BEVEL_RAISED_BR_1;  //0x00989898;  // br 1 most inner (light) 
            br2_color = LIBGUI_COLOR_BEVEL_RAISED_BR_2;  //0x00505050;  // br 2 inner (dark)
            break;
        case BS_DISABLED:
            tl1_color = 0x00FAFAFA;  // tl 1 most inner (lighter)
            tl2_color = 0x00E0E0E0;  // tl 2 inner (light)
            br1_color = 0x00989898;  // br 1 most inner (light) 
            br2_color = 0x00505050;  // br 2 inner (dark)
            break;
        case BS_PROGRESS:
            tl1_color = 0x00FAFAFA;  // tl 1 most inner (lighter)
            tl2_color = 0x00E0E0E0;  // tl 2 inner (light)
            br1_color = 0x00989898;  // br 1 most inner (light) 
            br2_color = 0x00505050;  // br 2 inner (dark)
            break;
        default:
            tl1_color = LIBGUI_COLOR_BEVEL_DEFAULT_TL_1;  //0x00FAFAFA;  // tl 1 most inner (lighter)
            tl2_color = LIBGUI_COLOR_BEVEL_DEFAULT_TL_2;  //0x00E0E0E0;  // tl 2 inner (light)
            br1_color = LIBGUI_COLOR_BEVEL_DEFAULT_BR_1;  //0x00989898;  // br 1 most inner (light) 
            br2_color = LIBGUI_COLOR_BEVEL_DEFAULT_BR_2;  //0x00505050;  // br 2 inner (dark)
            break;
        };
    }


// Editbox: always sunken, whatever the status
    if (is_editbox == TRUE)
    {
        tl1_color = LIBGUI_COLOR_BEVEL_SUNKEN_TL_1;
        tl2_color = LIBGUI_COLOR_BEVEL_SUNKEN_TL_2;
        br1_color = LIBGUI_COLOR_BEVEL_SUNKEN_BR_1;
        br2_color = LIBGUI_COLOR_BEVEL_SUNKEN_BR_2;
    }

// ====================================
// Label position (computed once for all types)

    // Center horizontally based on the component width
    // Center horizontally (not used by the editbox)
    if ((uic->flags & 0x0001) && is_editbox == FALSE )
    {
        LongSize = (unsigned long) (uic->label_size * 8 * scale);  // 8px per char
        if (uic->width > LongSize)
            text_x = uic->left + ((uic->width - LongSize) / 2);
    }
    // Center vertically based on the component height
    // Center vertically (the editbox is always centered)
    // #test: Using (8x16) for now, because we still do not have the char dimensions.
    if ((uic->flags & 0x0002) || is_editbox == TRUE)
    {
        //LongSize = (unsigned long) (1 * 16 * scale);  // 16px per char
        //LongSize = (unsigned long) (1 * 8 * scale);  // 8px per char
        LongSize = (unsigned long) (1 * FontInitialization.height * scale);
        if (uic->height > LongSize)
            text_y = uic->top + ((uic->height - LongSize) / 2);
    }

// ==================================================
// Background (color depends on the type)

    lingui_draw_rectangle0_dc(
        dc,
        uic->left, uic->top, uic->width, uic->height,
        bg_color,
        0   // rop
    );


// ==================================================
// Borders (only for button-like components)
// Do not touch uic->status here. Redraw must keep the current status.

    if (has_borders == TRUE || is_editbox == TRUE)
    {
        __draw_component_borders_dc(
            dc,
            uic,
            tl2_color,   // tl 2 inner
            tl1_color,   // tl 1 most inner
            br2_color,   // br 2 inner
            br1_color,   // br 1 most inner
            0x00101010   // outer
        );
    }


    if (is_editbox == TRUE)
    {
        const char *str = uic->label;
        unsigned long char_w = FontInitialization.width * scale;
        unsigned long inner_w = (uic->width > 8) ? (uic->width - 8) : 0;
        unsigned long max_chars = (char_w > 0) ? (inner_w / char_w) : 0;
        unsigned long len = (unsigned long) uic->label_size;
        unsigned long shown = 0;
        int focused = (uic->status == BS_FOCUS) ? TRUE : FALSE;

        // Reserve one cell for the caret when focused
        if (focused == TRUE && max_chars > 0)
            max_chars--;

        if (max_chars > 0 && len > 0)
        {
            // Show the tail if the text doesn't fit
            shown = (len > max_chars) ? max_chars : len;
            str = uic->label + (len - shown);

            libgui_drawstringblock_dc(
                dc, text_x, text_y, text_color, str, scale );
        }

        // Caret: 1px wide line right after the visible text
        if (focused == TRUE)
        {
            lingui_draw_rectangle0_dc(
                dc,
                text_x + (shown * char_w),
                text_y,
                1,
                FontInitialization.height * scale,
                text_color,
                0   // rop
            );
        }

        return 0;
    }


// =====================================
// Label (drawn once, on top of everything)

    if (uic->label_size > 0)
    {
        libgui_drawstringblock_dc(
            dc, text_x, text_y, 0x00101010, uic->label, scale );
    }

    return 0;
}


int 
libgui_set_ui_component_position(
    struct ui_component_d *uic,
	unsigned long left,
	unsigned long top )
{
	// uic
    if ((void*) uic == NULL)
        return -1;
	if (uic->magic != 1234)
        return -1;

	uic->left = left;
	uic->top = top;
	return 0;
}

int 
libgui_set_ui_component_dimension(
    struct ui_component_d *uic,
	unsigned long width,
	unsigned long height )
{
	// uic
    if ((void*) uic == NULL)
        return -1;
	if (uic->magic != 1234)
        return -1;

	uic->width = width;
	uic->height = height;
	return 0;
}

int
libgui_set_ui_component_status(
    struct ui_component_d *uic,
    int status )
{
    if ((void*) uic == NULL)
        return -1;

    if (uic->magic != 1234)
        return -1;

    switch (status)
    {
        case BS_RELEASED:
        case BS_FOCUS:
        case BS_PRESSED:
        case BS_HOVER:
        case BS_DISABLED:
        case BS_PROGRESS:

            uic->status = status;
            return 0;

        default:
            return -1;
    };

    return -1;
}

int 
libgui_set_ui_component_flags(
    struct ui_component_d *uic,
	unsigned long flags )
{
	// uic
    if ((void*) uic == NULL)
        return -1;
	if (uic->magic != 1234)
        return -1;

	uic->flags = (unsigned long) flags;
	return 0;
}


//
// #
// INITIALIZATION 
//

// Initialize the libgd library
int libgui_initialize(void)
{

// Get current mode
// Gramado mode
// get gramado mode.
// jail, p1, home, p2, castle ...
    
    //libgui_current_mode = server_get_system_metrics(130);
    libgui_current_mode = rtl_get_system_metrics(130);
    if (libgui_current_mode < 0){
        printf("libgui_initialize: [FAIL] libgui_current_mode\n");
        goto fail;
    }

// Get backbuffer and frontbuffer virtual addresses
    libgui_BACKBUFFER_VA  = (unsigned long) rtl_get_system_metrics(12);
    libgui_FRONTBUFFER_VA = (unsigned long) rtl_get_system_metrics(11);

// Screen
// Width, Height and Bits Per Pixel.
    libgui_device_width  = (unsigned long) rtl_get_system_metrics(1);
    libgui_device_height = (unsigned long) rtl_get_system_metrics(2);
    libgui_device_bpp    = (unsigned long) rtl_get_system_metrics(9);
// Saving
    libgui_SavedX   = (unsigned long) libgui_device_width;
    libgui_SavedY   = (unsigned long) libgui_device_height;
    libgui_SavedBPP = (unsigned long) libgui_device_bpp;

// Pitch
// Get bytes per pixel then multiply by the width.

    libgui_device_pitch = 
        (unsigned long) ((libgui_device_bpp/8) * libgui_device_width);


// Backbuffer and frontbuffer.
    if ( libgui_FRONTBUFFER_VA == 0 || 
         libgui_BACKBUFFER_VA == 0 )
    {
        printf("libgui_initialize: Buffers\n");
        goto fail;
    }

// Width, Height and Bits Per Pixel.
    if ( libgui_device_width == 0 || 
         libgui_device_height == 0 || 
         libgui_device_bpp == 0 )
    {
        printf("libgui_initialize: w, h and bpp\n");
        goto fail;
    }

//
// Device context
//

// The drawing context. 
// Thats is not a structure for hardware device driver.

    // Backbuffer
    libgui_dc_backbuffer = (void *) malloc(sizeof(struct dccanvas_d));
    if ((void*)libgui_dc_backbuffer == NULL){
        printf("libgui_initialize: libgui_dc_backbuffer\n");
        goto fail; 
    }
    memset ( libgui_dc_backbuffer, 0, sizeof(struct dccanvas_d) );
    libgui_dc_backbuffer->device_width  = libgui_device_width;
    libgui_dc_backbuffer->device_height = libgui_device_height;
    // Bits per pixel
    libgui_dc_backbuffer->bpp = libgui_device_bpp; 
    libgui_dc_backbuffer->pitch = libgui_device_pitch;
    libgui_dc_backbuffer->data = (unsigned char*) libgui_BACKBUFFER_VA;
    //libgui_dc_backbuffer->next = NULL;
    libgui_dc_backbuffer->used = TRUE;
    libgui_dc_backbuffer->magic = 1234;
    libgui_dc_backbuffer->initialized = TRUE;

    // Frontbuffer
    libgui_dc_frontbuffer = malloc(sizeof(struct dccanvas_d));
    if ((void*)libgui_dc_frontbuffer == NULL){
        printf("libgui_initialize: libgui_dc_frontbuffer\n");
        goto fail; 
    }
    memset ( libgui_dc_frontbuffer, 0, sizeof(struct dccanvas_d) );

    libgui_dc_frontbuffer->device_width  = libgui_device_width;
    libgui_dc_frontbuffer->device_height = libgui_device_height;
    // bits per pixel.
    libgui_dc_frontbuffer->bpp = libgui_device_bpp;
    libgui_dc_frontbuffer->pitch = libgui_device_pitch;
    libgui_dc_frontbuffer->data = (unsigned char*) libgui_FRONTBUFFER_VA;
    //libgui_dc_frontbuffer->next = NULL;
    libgui_dc_frontbuffer->used = TRUE;
    libgui_dc_frontbuffer->magic = 1234;
    libgui_dc_frontbuffer->initialized = TRUE;
 

// Char and font initialization
    __initialize_char_and_font_support();


// Initialize a list of pointer for canvas information structures
    lingui_initialize_canvas_list();

	// ...

    return 0;  // Initialization OK

fail:
    return (int) -1;
}

//
// End
//

