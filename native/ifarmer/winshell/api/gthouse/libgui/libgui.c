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


#define DEFAULT_FONT_WIDTH  8
#define DEFAULT_FONT_HEIGHT  8


struct libgui_char_initialization_d
{
    int initialized;

    int width;
    int height;
};
struct libgui_char_initialization_d  CharInitialization;

static int char_initialize(void);

// ===================================================


// Structure for a rectangle.
// A rectangle belongs to a window.
struct libgui_rect_d 
{
    int used;
    int magic;

    // struct libdisp_window_d *window;

    unsigned long left;
    unsigned long top;
    unsigned long width;
    unsigned long height;

    unsigned int bg_color;  // color
    int is_filled;          // filled or not
    unsigned long rop;      // raster operation

    //unsigned int flags;      // Reserved for future use

    int dirty;  // Validation
	int is_solid; // Is it a solid color rectangle?

    struct libgui_rect_d *next;
};

// The view:
// UI element / UI component.
struct libgui_view_d 
{
    int used;
	int magic;

// If we want to draw the component in an offscreen buffer, 
// we can use this field to store the buffer's address.
// Offscreen buffer for the component's content
	//char *offscreen_buffer;

// Type of the component (e.g., button, checkbox, text field)
// it also can be a container for other components, like a panel or a window.
// maybe called viewgroup or something like that.
	int type;

// Text label for the component (e.g., button text, checkbox label)
    char *label;

    unsigned int fg;  // foreground (text, icon, border)
    unsigned int bg;  // background (fill)
	unsigned long rop; // raster operation for drawing the component

// Geometry
// The values here are relative to the window's client area.
    unsigned long left;
    unsigned long top;
    unsigned long width;
    unsigned long height;

// State of the component (e.g., normal, hovered, pressed, disabled)
	int state;

//
// Input Pointer support (keyboard)
//

// The state of the input ponter.
// Used to blink the cursor.
    int ip_on;

    unsigned long ip_x;
    unsigned long ip_y;
    unsigned int ip_color;
    unsigned long width_in_chars;
    unsigned long height_in_chars;

    //unsigned long ip_type; //?? algum estilo especifico de cursor?
    //unsigned long ip_style;
    // ...

// para input do tipo teclado
    unsigned long ip_pixel_x;
    unsigned long ip_pixel_y;

// Navigation
    struct libgui_view_d *next;
};

struct libgui_node_d 
{
	int used;
	int magic;

	struct libgui_view_d *component;
	struct libgui_node_d *next;
};


//======================================
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

// ===================================================


// Create a new device context for a given buffer.
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

// Plot pixel into the raster.
// The origin is top/left of the viewport. (0,0).
int 
libgui_backbuffer_putpixel3 ( 
    unsigned int color, 
    int x, 
    int y,
    unsigned long rop )
{
// #todo: Return the number of changed pixels.

    int ret_value=0;
// The base address for the target backbuffer.
    unsigned long target_buffer = 
        libgui_BACKBUFFER_VA;

// Clipping
// #bugbug:
// And the other limits?
    if (x<0){
        return 0;
    }
    if (y<0){
        return 0;
    }

    //if (x > 200 && x < 600)
    //    rop = 1;

    //if (target_buffer == 0)
        //return 0;

// IN: color, x, y, rop, target buffer.
    ret_value = 
        (int) libgui_fb_backbuffer_putpixel( 
                  color, 
                  x, 
                  y, 
                  rop, 
                  target_buffer );

// #test: This routine has rop support.
// #bugbug: These two routines are using different types
// in the parameters.
    //return (int) libgui_putpixel0( 
    //                 color, x, y, rop, target_buffer );

    return (int) ret_value;
}

// ## putpixel: 
// backbuffer e lfb ##
// IN: cor, x, y
// Put pixel using the kernel service.
// Slow!

int 
libgui_backbuffer_putpixel2 ( 
    unsigned int color, 
    int x, 
    int y )
{
    if (x<0){ return -1; }
    if (y<0){ return -1; }

    // Service number 6
    return (int) sc80( 6, color, x, y );
}

/*
 * libgui_fb_backbuffer_putpixel:
 *     Put pixel in the device screen.
 */
// #??
// Usando o endereço virtual do backbuffer
// Será que está mapeado ???
// Está em ring 3 ??? ou ring 0???
// Pinta um pixel no backbuffer.
// O buffer tem o tamanho da janela do dispositivo.
// A origem está em top/left.
// #bugbug
// #todo
// Precismos considerar o limite do backbuffer.
// Então teremos um Offset máximo.
// #todo
// Check some flags, just like rasterizations.
// We will need a lot of parameters in this kind of function
// Including the address of the backbuffer.
// Clipping against the device limits
// #todo
// rop_flags   ... raster operations
// See the same routine in the kernel side.
// Plot pixel into the raster.
// The origin is top/left of the viewport. (0,0).
// #todo:
// rop operations 
// Copy the same already did before in other parts
// of the system.

// Colors:
// b,   g,  r,  a = Color from parameter.
// b2, g2, r2, a2 = Color from backbuffer.
// b3, g3, r3, a3 = Color to be stored.

int 
libgui_fb_backbuffer_putpixel ( 
    unsigned int color, 
    int x, 
    int y,
    unsigned long rop,
    unsigned long buffer_va )  
{
// #todo: Return the number of changed pixels.
// #bugbug: We don't have rop in this routine.

// #bugbug
// The lib needs to be already initialized.

    unsigned char *where = (unsigned char *) libgui_BACKBUFFER_VA;
    //unsigned char *where = (unsigned char *) buffer_va;

// Device context
    unsigned long deviceLeft   = 0;
    unsigned long deviceTop    = 0;
    unsigned long deviceWidth  = (libgui_device_width  & 0xFFFF );
    unsigned long deviceHeight = (libgui_device_height & 0xFFFF );
    // #todo
    // Precismos considerar o limite do backbuffer.
    // Então teremos um Offset máximo.
    unsigned long tmpOffset=0;
    unsigned long MaxOffset=0;
    int Offset=0;
// #todo
// raster operation. rasterization.
    // unsigned long rop;

// 2MB limit
// Our buffer size.
// 2mb is the limit for 64bit full pagetable.
// #bubug: Não fazer multilicações
//MaxOffset = (int) (1024*10124*4);
//MaxOffset = (int) 0x00400000;
    MaxOffset = (int) 0x00200000;  // 2MB

    char b, g, r, a;
    b = (color & 0xFF);
    g = (color & 0xFF00)   >> 8;
    r = (color & 0xFF0000) >> 16;
    a = (color >> 24) & 0xFF;

    int Operation = (int) (rop & 0xFF);

    // 3 = 24 bpp
    int bytes_count=0;

// Clipping
// Clipping against the device limits
    if (x<0){ goto fail; }
    if (y<0){ goto fail; }
    if ( x >= deviceWidth ) { goto fail; }
    if ( y >= deviceHeight ){ goto fail; }
// Purify
    x = (x & 0xFFFF);
    y = (y & 0xFFFF);

// bpp
// #danger
// Esse valor foi herdado do bootloader.

    switch (libgui_SavedBPP){
        case 32:  bytes_count = 4;  break;
        case 24:  bytes_count = 3;  break;
        //case 16:  bytes_count = 2;  break;
        //case 8:   bytes_count = 1;  break;
        default:
            printf("libgui_fb_backbuffer_putpixel: [ERROR] libgui_SavedBPP\n");
            goto fail;
            break;
    };

// #importante
// Pegamos a largura do dispositivo.
    //width = (int) libgui_SavedX; 

// unsigned long
// Nao pode ser maior que 2MB.
// Que eh o tamanho do buffer que temos ate agora.
    unsigned long pitch=0; 

    // #todo: Return the number of changed pixels. '0'
    if (bytes_count != 3 && bytes_count != 4)
        goto fail;

    if (bytes_count == 3){
        pitch = (unsigned long) (deviceWidth*bytes_count);
        tmpOffset = (unsigned long) ( (pitch*y) + (x*bytes_count) );
    }
    if (bytes_count == 4){
        pitch = (unsigned long) (deviceWidth<<2);
        tmpOffset = (unsigned long) ( (pitch*y) + (x<<2) );
    }

// #todo
// Debug
    if (tmpOffset >= MaxOffset)
    {
        printf ("libgui_fb_backbuffer_putpixel: MaxOffset\n");
        //printf ("tmpOffset=%x\n",tmpOffset);
        //printf ("x=%d\n",x);
        //printf ("y=%d\n",y);
        //printf ("width=%d\n",width);
        exit(1);
        goto fail;
    }

// int. menor que 2MB
    Offset = (int) tmpOffset;

// #bugbug
// #todo
// Para não termos problemas com o offset, temos que checar
// os limites de x e y.

//
// Backbuffer limit
//

// #bugbug
// Escrever fora do backbuffer pode gerar PF.
// #todo
// The rop_flags will give us some informations.
// the lsb is the operation code.
// See the same routine in the kernel side.

//
// ==================================================
//

// ------------------------------------------
// A cor encontrada no buffer.
    unsigned char b2, g2, r2, a2;
// Get
    b2 = where[Offset];
    g2 = where[Offset +1];
    r2 = where[Offset +2];
    if ( libgui_SavedBPP == 32 ){ a2 = where[Offset +3]; };

// ------------------------------------------
// A cor transformada.
// A cor a ser gravada.
    unsigned char b3, g3, r3, a3;

// ------------
// 0 = Sem modificação
// A cor a ser registrada é a mesma enviada por argumento.
    if (Operation == 0){
        r3=r;  g3=g;  b3=b;  a3=a;
    }
// ------------
// 1 = or
    if (Operation == 1)
    {
        r3 = (r2 | r);
        g3 = (g2 | g);
        b3 = (b2 | b);
        a3 = a2;
    }
// ------------
// 2 = and
    if (Operation == 2)
    {
        r3 = (r2 & r);
        g3 = (g2 & g);
        b3 = (b2 & b);
        a3 = a2;
    }
// ------------
// 3 = xor
    if (Operation == 3)
    {
        r3 = (r2 ^ r);
        g3 = (g2 ^ g);
        b3 = (b2 ^ b);
        a3 = a2;
    }
// ------------
// 10 - red
    if (Operation == 10)
    {
        r3 = (r2 & 0xFE);
        g3 = g2;
        b3 = b2; 
        a3 = a2;
    }
// ------------
// 11 - green
    if (Operation == 11)
    {
        r3 = r2;
        g3 = (g2 & 0xFE);
        b3 = b2; 
        a3 = a2;
    }
// ------------
// 12 - blue
    if (Operation == 12)
    {
        r3 = r2;
        g3 = g2;
        b3 = (b2 & 0xFE); 
        a3 = a2;
    }
// ------------
// 20 - gray
    if (Operation == 20)
    {
        r3 = (r2 & 0x80);
        g3 = (g2 & 0x80);
        b3 = (b2 & 0x80);
        a3 = a2;
    }
// ------------
// 21 - gray
    if (Operation == 21)
    {
        r3 = (r2 & 0x00);
        g3 = (g2 & 0xFF);
        b3 = (b2 & 0xFF);
        a3 = a2;
    }

// luminosity
// Gray: luminosity = R*0.3 + G*0.59 + B *0.11

/*
 // #test
 // This is a test yet.
    unsigned char common_gray=0;
    if ( Operation == 22 )
    {
        r3 = ((r2 * 30 )/100);
        g3 = ((g2 * 59 )/100);
        b3 = ((b2 * 11 )/100);
        common_gray = (unsigned char) (r3+g3+b3);
        r3=(unsigned char)common_gray;
        g3=(unsigned char)common_gray;
        b3=(unsigned char)common_gray;
        a3 = a2;
    }
*/


//
// == Record ==============================
//

// BGR and A
    where[Offset]    = b3;
    where[Offset +1] = g3;
    where[Offset +2] = r3;
    if (libgui_SavedBPP == 32){ where[Offset +3] = a3; };

// Return the number of changed pixels. '1'.
    return (int) 1;

// Return the number of changed pixels. '0'.
fail:
    return 0;
}

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

void 
libgui_backbuffer_putpixel ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags )
{
    if (!libgui_dc_backbuffer)
        return;

    //unsigned long buffer = (unsigned long) libgui_BACKBUFFER_VA;

// Putpixel at the given buffer address
    //libgui_putpixel0( _color, _x, _y, _rop_flags, buffer );

    // #test: New worker with dc
    libgui_putpixel0(libgui_dc_backbuffer, _color, _x, _y, _rop_flags);
}

void 
libgui_frontbuffer_putpixel ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags )
{
    if (!libgui_dc_frontbuffer)
        return;

    //unsigned long buffer = (unsigned long) libgui_FRONTBUFFER_VA;
// Putpixel at the given buffer address
    //libgui_putpixel0( _color, _x, _y, _rop_flags, buffer );

    // #test: New worker with dc
    libgui_putpixel0(libgui_dc_frontbuffer, _color, _x, _y, _rop_flags);
}

// IN: 
// back_or_front: 1=back | 2=front
int 
libgui_putpixel ( 
    unsigned int color, 
    int x, 
    int y,
    unsigned long rop,
    int back_or_front )
{
    if (back_or_front == 1){
        libgui_backbuffer_putpixel(color, x, y, rop);
        return 0;
    }
    if (back_or_front == 2){
        libgui_frontbuffer_putpixel(color, x, y, rop);
        return 0;
    }
    return (int) -1;
}

// Get the color value given the position
unsigned int libgui_backbuffer_getpixelcolor(int x, int y)
{
    unsigned char *where = (unsigned char *) libgui_BACKBUFFER_VA;
// 3 = 24 bpp
    int bytes_count=0;

// #bugbug
// Essa funçao eta errada,
// precisamos passar o ponteiro para o retorno via parametro
// e o retorno da funçao deve ser int, pra indicar sucesso ou nao.
    if (x<0){ return 0; }
    if (y<0){ return 0; }

// bpp
// #danger
// Esse valor foi herdado do bootloader.
    switch (libgui_SavedBPP){
    case 32:  bytes_count = 4;  break;
    case 24:  bytes_count = 3;  break;
    //case 16:  bytes_count = 2;  break;
    //case 8:   bytes_count = 1;  break;
    default:
        printf("libgui_backbuffer_getpixelcolor: [ERROR] libgui_SavedBPP\n");
        //panic ("libgui_backbuffer_getpixelcolor: libgui_SavedBPP");
        break;
    };

// #importante
// Pegamos a largura do dispositivo.
    int width = (int) libgui_SavedX;
// Offset
    int offset = (int) ( (bytes_count*width*y) + (bytes_count*x) );
// bgra
    char b, g, r, a;
// Get bytes.
    b = where[offset];
    g = where[offset +1];
    r = where[offset +2];
    if ( libgui_SavedBPP == 32 ){
        a = where[offset +3];
    };
// The buffer.
    unsigned int ColorBuffer=0;
    unsigned char *c = (unsigned char *) &ColorBuffer;

// Paint.
// Set bytes of ColorBuffer.
    c[0] = b;  c[1] = g;  c[2] = r;  c[3] = a;

// Return the color value
    return (unsigned int) ColorBuffer;
}

// libgui_backbuffer_draw_horizontal_line:
// Draw a horizontal line on backbuffer. 
void 
libgui_backbuffer_draw_horizontal_line ( 
    unsigned long x1,
    unsigned long y, 
    unsigned long x2, 
    unsigned int color,
    unsigned long rop_flags )
{
// #todo
// Maybe we need checking some limits here.
    if (x1 > x2){
        return;
    }
// IN: color, x, y, rop flags
    while (x1 < x2){
        libgui_backbuffer_putpixel( color, x1, y, rop_flags ); 
        x1++;
    };
}

void 
libgui_frontbuffer_draw_horizontal_line ( 
    unsigned long x1,
    unsigned long y, 
    unsigned long x2, 
    unsigned int color,
    unsigned long rop_flags )
{
// #todo
// Maybe we need checking some limits here.
    if (x1 > x2){
        return;
    }
// IN: color, x, y, rop flags.
    while (x1 < x2){
        libgui_frontbuffer_putpixel( color, x1, y, rop_flags );
        x1++;
    };
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


static int char_initialize(void)
{
    CharInitialization.initialized = FALSE;

// Char width and height.
    CharInitialization.width = DEFAULT_FONT_WIDTH;
    CharInitialization.height = DEFAULT_FONT_HEIGHT;

    FontInitialization.initialized = FALSE;
    if (FontInitialization.initialized != TRUE)
    {
        FontInitialization.width = DEFAULT_FONT_WIDTH;
        FontInitialization.height = DEFAULT_FONT_HEIGHT;
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

void 
libgui_drawchar (
    unsigned long x, 
    unsigned long y,  
    unsigned long c,
    unsigned int fgcolor,
    unsigned int bgcolor,
    unsigned long rop )
{
    if (!libgui_dc_backbuffer)
        return;

    libgui_drawchar_dc(
		libgui_dc_backbuffer, x, y, c, fgcolor, bgcolor, rop );
}

void 
libgui_drawstring(
    unsigned long x, 
    unsigned long y, 
    const char *s, 
    unsigned int fg,  // color
    unsigned int bg,  // color
    unsigned long rop )
{
    int CharWidth = 8;  // #todo

    if (FontInitialization.initialized == TRUE)
        CharWidth = FontInitialization.width;
    if ((void*)s == NULL)
	    return;

    while (*s) {
        libgui_drawchar(x, y, *s, fg, bg, rop);
        x += CharWidth;  // Advance by font width
        s++;
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

    //gramado_system_call ( 10, (unsigned long) buffer, 0, 0 );
    sc80 ( 10, (unsigned long) buffer, 0, 0 );
}

void 
libgui_refresh_rectangle_via_kernel(
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height )
{
    __kgws_adapter_refresh_rectangle(x, y, width, height);
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
    //gramado_system_call ( 9, (unsigned long) Buffer, 0, 0 );
    sc80 ( 9, (unsigned long) Buffer, 0, 0 );
}


/*
 * libgui_backbuffer_draw_rectangle0: (API)
 *     Draw a rectangle on backbuffer. 
 */
// #todo
// At this moment, no structure ware invalidated.
// So, the caller needs to specify a rect structure,
// this way we can invalidated it.
// use_kgws?
// TRUE = use kgws.
// FALSE = do not use kgws. #bugbug

void 
libgui_backbuffer_draw_rectangle0 ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    int fill,
    unsigned long rop_flags,
    int use_kgws )
{

//
// flag
//

// The rectangle can be painted by the kgws inside the base kernel.
// #todo
// Let's include this flag into the function's parameters.
// #bugbug
// The ws routine is not working everytime we call it.

    // #important: Flag.
    // Draw rectangle using the kernel painter.
    int fDrawRectangleUsingKGWS = (int) use_kgws;
    //int fDrawRectangleUsingKGWS = FALSE;  // #test
    //int fDrawRectangleUsingKGWS = TRUE;   // #test

    struct libgui_rect_d rect;

    // debug_print("libgui_backbuffer_draw_rectangle0: :(\n");

// device:

    //unsigned long device_w = (unsigned long) gws_get_device_width();
    //unsigned long device_h = (unsigned long) gws_get_device_height();

    unsigned long device_w  = (unsigned long) rtl_get_system_metrics(1);
    unsigned long device_h = (unsigned long) rtl_get_system_metrics(2);
    //unsigned long __device_bpp    = (unsigned long) rtl_get_system_metrics(9);

    device_w = (unsigned long) (device_w & 0xFFFF);
    device_h = (unsigned long) (device_h & 0xFFFF);

// #hack provisory
// Resolution limits


/*
    if (device_w > 800){
        debug_print("libgui_backbuffer_draw_rectangle0: [FAIL] device_w\n");
        //return; 
    }
*/

/*
    // #hack provisory
    // We dont wanna mess up the memory beyond the buffer
    if (device_h > 600)
	{
        height = 600;
        debug_print("libgui_backbuffer_draw_rectangle0: [FAIL] device_h\n");
        //return; 
    }
*/

    if (device_h > 768)
	{
		return;
	}

// Set values
    rect.left   = (unsigned long) (x      & 0xFFFF);
    rect.top    = (unsigned long) (y      & 0xFFFF);
    rect.width  = (unsigned long) (width  & 0xFFFF);
    rect.height = (unsigned long) (height & 0xFFFF);
// Margins
    //rect.right  = (unsigned long) (rect.left + rect.width);
    //rect.bottom = (unsigned long) (rect.top  + rect.height); 
    rect.bg_color = (unsigned int)(color & 0xFFFFFF);

//
// Checks
//

// #bugbug
// O início não pode ser depois do fim.

    unsigned long __right  = (unsigned long) (rect.left + rect.width);
    unsigned long __bottom = (unsigned long) (rect.top  + rect.height); 


    if (rect.left > __right)
    {
        debug_print("libgui_backbuffer_draw_rectangle0: [FAIL] left > __right\n");
        //#debug
        printf ("libgui_backbuffer_draw_rectangle0: l:%d r:%d\n",
            rect.left, __right );
        exit(0);
        return; 
    }
    if (rect.top > __bottom)
    { 
        debug_print("libgui_backbuffer_draw_rectangle0: [FAIL] top  > __bottom\n");
        //#debug
        printf ("libgui_backbuffer_draw_rectangle0: t:%d b:%d\n",
            rect.top, __bottom);
        exit(0);
        return; 
    }

// Clip

// Se a largura for maior que largura do dispositivo.
    if (rect.width > device_w){
        rect.width = (unsigned long) device_w;
        //debug_print("libgui_backbuffer_draw_rectangle0: [FAIL] rect.width > device_w\n");
        //return;
    }
// Se a altura for maior que altura do dispositivo.
    if (rect.height > device_h){
        rect.height = (unsigned long) device_h;
        //debug_print("libgui_backbuffer_draw_rectangle0: [FAIL] rect.height > device_h\n");
        //return;
    }

// Limits
// Se for maior que o espaço que sobra, 
// então será igual ao espaço que sobra.

// Empty
    if (fill == TRUE){
        rect.is_filled = FALSE;
    } else if (fill == FALSE){
        rect.is_filled = TRUE;
    };

/*
// #todo
// Desenhar as bordas com linhas
// ou com retangulos

    if (fill==0)
    {
            //  ____
            // |
            //
            
            //board1, borda de cima e esquerda.
            rectBackbufferDrawRectangle ( 
                window->left, window->top,
                window->width, 1, 
                color, 1 );
            rectBackbufferDrawRectangle ( 
                window->left, window->top, 
                1, window->height,
                color, 1 );

            //  
            //  ____|
            //

            //board2, borda direita e baixo.
            rectBackbufferDrawRectangle ( 
                 ((window->left) + (window->width) -1), window->top, 
                 1, window->height, 
                 color, 1 );
            rectBackbufferDrawRectangle ( 
                 window->left, ( (window->top) + (window->height) -1 ),  
                 window->width, 1, 
                 color, 1 );
          
        return;
    }
*/

// Draw:
// Drawing in the kernel using kgws.
// Draw lines on backbuffer.
// Invalidate the rectangle.

    if (fDrawRectangleUsingKGWS == TRUE)
    {
        // debug_print("libgui_backbuffer_draw_rectangle0: Using R0");
        // IN: l,t,w,h,bg color, rop flags.
        __draw_rectangle_via_kgws (
            rect.left, rect.top, rect.width, rect.height,
            rect.bg_color, rop_flags );
        rect.dirty = TRUE;
        return;
    }

//===============================================================
// Draw:
// Draw the rectangle 
// using the routine here in the display server.

/*
// Clip
    if ( rect.width > device_w )
        rect.width = (unsigned long) device_w;
    if ( rect.height > device_h )
        rect.height = (unsigned long) device_h;
*/

/*
// Fail
    if ( rect.left > rect.width  ){ 
        debug_print("libgui_backbuffer_draw_rectangle0: [FAIL] rect.left > rect.width\n");
        return; 
    }
    if ( rect.top  > rect.height ){ 
        debug_print("libgui_backbuffer_draw_rectangle0: [FAIL] rect.top  > rect.height\n");
        return; 
    }
*/

    //#debug
    //printf ("w=%d h=%d l=%d t=%d \n",
        //rect.width, rect.height, rect.left, rect.top );
    //exit(1);
    //asm ("int $3");

// ===============================
// Draw lines on backbuffer.
// It's using the ws routine.
    register unsigned long number_of_lines=0;
    number_of_lines = (unsigned long) rect.height;

// #todo
// Test this one for painting using the ring 3 ws.
// libgui_backbuffer_draw_horizontal_line(...)
    while (number_of_lines--)
    {
        // last line?
        if (rect.top >= __bottom)
        {
            break;
        }
        // End of the device screen?
        if (rect.top >= device_h){
            break;
        }

        // Draw horizontal line
        // see: line.c
        //grBackbufferDrawHorizontalLine ( 
            //rect.left, rect.top, __right, 
            //(unsigned int) rect.bg_color );

		libgui_backbuffer_draw_horizontal_line ( 
			rect.left, rect.top, __right, 
			(unsigned int) rect.bg_color, rop_flags );

        // Next line
        rect.top++;
    };

    rect.dirty = TRUE;  // Invalidate
    return;  // Done
}

/*
 * libgui_frontbuffer_draw_rectangle0: (API)
 *     Draw a rectangle on backbuffer. 
 */
// #todo
// At this moment, no structure ware invalidated.
// So, the caller needs to specify a rect structure,
// this way we can invalidated it.

void 
libgui_frontbuffer_draw_rectangle0 ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    int fill,
    unsigned long rop_flags )
{

//
// flag
//

// The rectangle can be painted by the kgws inside the base kernel.
// #todo
// Let's include this flag into the function's parameters.
// #bugbug
// The ws routine is not working everytime we call it.

    // #important: Flag.
    // Draw rectangle using the kernel painter.
    //int fDrawRectangleUsingKGWS = (int) use_kgws;
    int fDrawRectangleUsingKGWS = FALSE;  // #test
    //int fDrawRectangleUsingKGWS = TRUE;   // #test

    struct libgui_rect_d rect;

    // debug_print("libgui_backbuffer_draw_rectangle0: :(\n");

// device:

    //unsigned long device_w = (unsigned long) gws_get_device_width();
    //unsigned long device_h = (unsigned long) gws_get_device_height();

    unsigned long device_w  = (unsigned long) rtl_get_system_metrics(1);
    unsigned long device_h = (unsigned long) rtl_get_system_metrics(2);
    //unsigned long __device_bpp    = (unsigned long) rtl_get_system_metrics(9);

    device_w = (unsigned long) (device_w & 0xFFFF);
    device_h = (unsigned long) (device_h & 0xFFFF);
// #provisório
// limites do dispositivo
    //if (device_w > 800)
	if (device_w > 1024)
	{
        //debug_print("libgui_backbuffer_draw_rectangle0: [FAIL] device_w\n");
        return; 
    }
    //if (device_h > 600)
    if (device_h > 768)
	{
        //debug_print("libgui_backbuffer_draw_rectangle0: [FAIL] device_h\n");
        return; 
    }


// Set values
    rect.left   = (unsigned long) (x      & 0xFFFF);
    rect.top    = (unsigned long) (y      & 0xFFFF);
    rect.width  = (unsigned long) (width  & 0xFFFF);
    rect.height = (unsigned long) (height & 0xFFFF);
// Margins
    //rect.right  = (unsigned long) (rect.left + rect.width);
    //rect.bottom = (unsigned long) (rect.top  + rect.height); 
    rect.bg_color = (unsigned int)(color & 0xFFFFFF);

//
// Checks
//

// #bugbug
// O início não pode ser depois do fim.

    unsigned long __right  = (unsigned long) (rect.left + rect.width);
    unsigned long __bottom = (unsigned long) (rect.top  + rect.height); 


    if (rect.left > __right)
    {
        debug_print("libgui_backbuffer_draw_rectangle0: [FAIL] left > __right\n");
        //#debug
        printf ("libgui_backbuffer_draw_rectangle0: l:%d r:%d\n",
            rect.left, __right );
        exit(0);
        return; 
    }
    if (rect.top > __bottom)
    { 
        debug_print("libgui_backbuffer_draw_rectangle0: [FAIL] top  > __bottom\n");
        //#debug
        printf ("libgui_backbuffer_draw_rectangle0: t:%d b:%d\n",
            rect.top, __bottom);
        exit(0);
        return; 
    }

// Clip

// Se a largura for maior que largura do dispositivo.
    if (rect.width > device_w){
        rect.width = (unsigned long) device_w;
        //debug_print("libgui_backbuffer_draw_rectangle0: [FAIL] rect.width > device_w\n");
        //return;
    }
// Se a altura for maior que altura do dispositivo.
    if (rect.height > device_h){
        rect.height = (unsigned long) device_h;
        //debug_print("libgui_backbuffer_draw_rectangle0: [FAIL] rect.height > device_h\n");
        //return;
    }

// Limits
// Se for maior que o espaço que sobra, 
// então será igual ao espaço que sobra.

// Empty
    if (fill == TRUE){
        rect.is_filled = FALSE;
    } else if (fill == FALSE){
        rect.is_filled = TRUE;
    };

/*
// #todo
// Desenhar as bordas com linhas
// ou com retangulos

    if (fill==0)
    {
            //  ____
            // |
            //
            
            //board1, borda de cima e esquerda.
            rectBackbufferDrawRectangle ( 
                window->left, window->top,
                window->width, 1, 
                color, 1 );
            rectBackbufferDrawRectangle ( 
                window->left, window->top, 
                1, window->height,
                color, 1 );

            //  
            //  ____|
            //

            //board2, borda direita e baixo.
            rectBackbufferDrawRectangle ( 
                 ((window->left) + (window->width) -1), window->top, 
                 1, window->height, 
                 color, 1 );
            rectBackbufferDrawRectangle ( 
                 window->left, ( (window->top) + (window->height) -1 ),  
                 window->width, 1, 
                 color, 1 );
          
        return;
    }
*/

// Draw:
// Drawing in the kernel using kgws.
// Draw lines on backbuffer.
// Invalidate the rectangle.

    if (fDrawRectangleUsingKGWS == TRUE)
    {
        // debug_print("libgui_backbuffer_draw_rectangle0: Using R0");
        // IN: l,t,w,h,bg color, rop flags.
        __draw_rectangle_via_kgws (
            rect.left, rect.top, rect.width, rect.height,
            rect.bg_color, rop_flags );
        rect.dirty = TRUE;
        return;
    }

//===============================================================
// Draw:
// Draw the rectangle 
// using the routine here in the display server.

/*
// Clip
    if ( rect.width > device_w )
        rect.width = (unsigned long) device_w;
    if ( rect.height > device_h )
        rect.height = (unsigned long) device_h;
*/

/*
// Fail
    if ( rect.left > rect.width  ){ 
        debug_print("libgui_backbuffer_draw_rectangle0: [FAIL] rect.left > rect.width\n");
        return; 
    }
    if ( rect.top  > rect.height ){ 
        debug_print("libgui_backbuffer_draw_rectangle0: [FAIL] rect.top  > rect.height\n");
        return; 
    }
*/

    //#debug
    //printf ("w=%d h=%d l=%d t=%d \n",
        //rect.width, rect.height, rect.left, rect.top );
    //exit(1);
    //asm ("int $3");

// ===============================
// Draw lines on backbuffer.
// It's using the ws routine.
    register unsigned long number_of_lines=0;
    number_of_lines = (unsigned long) rect.height;

// #todo
// Test this one for painting using the ring 3 ws.
// libgui_backbuffer_draw_horizontal_line(...)
    while (number_of_lines--)
    {
        // last line?
        if (rect.top >= __bottom)
        {
            break;
        }
        // End of the device screen?
        if (rect.top >= device_h){
            break;
        }

        // Draw horizontal line
        // see: line.c
        //grBackbufferDrawHorizontalLine ( 
            //rect.left, rect.top, __right, 
            //(unsigned int) rect.bg_color );

		/*
		libgui_backbuffer_draw_horizontal_line ( 
			rect.left, rect.top, __right, 
			(unsigned int) rect.bg_color, rop_flags );
		*/
		libgui_frontbuffer_draw_horizontal_line ( 
			rect.left, rect.top, __right, 
			(unsigned int) rect.bg_color, rop_flags );

        // Next line
        rect.top++;
    };

    rect.dirty = TRUE;  // Invalidate
    return;  // Done
}

// #test
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
libgui_BackbufferDrawCharBlockStyle(
    unsigned long x,          // top-left in screen space
    unsigned long y,
    unsigned int fgcolor,
    int ch,         // character code
    int scale )     // 1 = classic 8×8, 2 = 16×16 blocks, etc.
{

    // #test: NOT TESTED YET

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

				/*
                rectBackbufferDrawRectangle(
                    bx, by,
                    scale, scale,           // block size
                    fgcolor,
                    TRUE,                   // filled = TRUE
                    0                       // rop normal
                );
				*/

                libgui_backbuffer_draw_rectangle0 ( 
					bx, by,
					scale, scale,           // block size
					fgcolor,
					TRUE,                   // filled = TRUE
					0,                      // rop normal
					FALSE                   // use kgws = FALSE
				 );

            }
        }
    }
}

void 
libgui_drawstringblock(
    unsigned long x,
    unsigned long y,
    unsigned int color,
    const char *str,
    int scale )
{
    int advance = FontInitialization.width * scale;
    unsigned long cx = x;

    while (*str)
    {
        if (*str == ' ') {
            cx += advance;          // or advance / 2 for tighter word spacing
        } else {
            //grDrawCharBlockStyleInsideWindow(wid, cx, y, color, *str, scale, 0);
            libgui_BackbufferDrawCharBlockStyle(cx, y, color, *str, scale);
			cx += advance;
        }
        str++;
    }
}

void libgui_set_mouse_pointer(unsigned long x, unsigned long y)
{
    __new_mouse_x = x;
    __new_mouse_y = y;
}

// Draw button given the dc and the ui component
void 
__draw_button_borders_dc(
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
    //rectBackbufferDrawRectangle (   // outer
    //    w->absolute_x+1, 
    //    w->absolute_y, 
    //    w->width-2,
    //    BorderSize, 
    //    outer_color, TRUE,0 );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left +1,
        ui_c->top,
        ui_c->width -2,
        BorderSize,   // height
        outer_color,  // color
        0             // rop 
    );

    //rectBackbufferDrawRectangle (   // tl 2   inner
        //w->absolute_x+1, 
        //w->absolute_y+1, 
        //w->width-2, 
        //BorderSize, 
        //tl_2, TRUE,0 );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left +1,
        ui_c->top  +1,
        ui_c->width -2,
        BorderSize,   // height
        tl_2,  // color
        0             // rop 
    );
    //rectBackbufferDrawRectangle (   // tl 1  most inner
        //w->absolute_x+1+1, 
        //w->absolute_y+1+1,
        //w->width-4, 
        //BorderSize, 
        //tl_1, TRUE,0 );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left +1+1,
        ui_c->top  +1+1,
        ui_c->width -4,
        BorderSize,   // height
        tl_1,  // color
        0             // rop 
    );

// -------------------------------
// :: Left
// left, left+1, left+2
    //rectBackbufferDrawRectangle (    // outer
        //w->absolute_x, 
        //w->absolute_y+1, 
        //BorderSize, 
        //w->height-2,
        //outer_color, TRUE,0 );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left,
        ui_c->top  +1,
        BorderSize,       // width 
        ui_c->height -2,  // height
        outer_color,      // color
        0                 // rop 
    );
    //rectBackbufferDrawRectangle (    // tl 2   inner
        //w->absolute_x+1, 
        //w->absolute_y+1, 
        //BorderSize, 
        //w->height-2,
        //tl_2, TRUE,0 );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left +1,
        ui_c->top  +1,
        BorderSize,       // width 
        ui_c->height -2,  // height
        tl_2,             // color
        0                 // rop 
    );
    //rectBackbufferDrawRectangle (    // tl 1  most inner
        //w->absolute_x+1+1, 
        //w->absolute_y+1+1, 
        //BorderSize, 
        //w->height-4,
        //tl_1, TRUE,0 );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left +1+1,
        ui_c->top  +1+1,
        BorderSize,       // width 
        ui_c->height -4,  // height
        tl_1,      // color
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
    //rectBackbufferDrawRectangle (           // outer
        //((w->absolute_x) + (w->width) -1), 
        //w->absolute_y+1, 
        //BorderSize, 
        //w->height-2, 
        //outer_color, TRUE, 0 );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left + ui_c->width -1,
        ui_c->top +1,
        BorderSize,       // width
        ui_c->height -2,  // height
        outer_color,      // color
        0                 // rop 
    );
    //rectBackbufferDrawRectangle (              // br 2 inner
        //((w->absolute_x) + (w->width) -2), 
        //w->absolute_y+1, 
        //BorderSize, 
        //w->height-2, 
        //br_2, TRUE, 0 );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left + ui_c->width -2,
        ui_c->top +1,
        BorderSize,       // width
        ui_c->height -2,  // height
        br_2,      // color
        0                 // rop 
    );
    //rectBackbufferDrawRectangle (               // br 1 most inner
        //((w->absolute_x) + (w->width) -3), 
        //w->absolute_y+1+1, 
        //BorderSize, 
        //w->height-4, 
        //br_1, TRUE, 0 );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left + ui_c->width -3,
        ui_c->top +1+1,
        BorderSize,       // width
        ui_c->height -4,  // height
        br_1,      // color
        0                 // rop 
    );



// -------------------------------
// :: Bottom
// bottom-1, bottom-2, bottom-3
    //rectBackbufferDrawRectangle (        // outer
        //w->absolute_x+1, 
        //((w->absolute_y) + (w->height) -1),  
        //w->width-2, 
        //BorderSize, 
        //outer_color, TRUE, 0 );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left +1,
        ui_c->top + ui_c->height -1,
        ui_c->width -2,    // width
        BorderSize,       // height
        outer_color,      // color
        0                 // rop 
    );
    //rectBackbufferDrawRectangle (           // br 2 inner
        //w->absolute_x+1, 
        //((w->absolute_y) + (w->height) -2),  
        //w->width-2, 
        //BorderSize, 
        //br_2, TRUE, 0 );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left +1,
        ui_c->top + ui_c->height -2,
        ui_c->width -2,    // width
        BorderSize,       // height
        br_2,      // color
        0                 // rop 
    );
    //rectBackbufferDrawRectangle (            // br 1 most inner
        //w->absolute_x+1+1, 
        //((w->absolute_y) + (w->height) -3),  
        //w->width-4, 
        //BorderSize, 
        //br_1, TRUE, 0 );
    lingui_draw_rectangle0_dc(
        dc,   // Device Context
        ui_c->left +1+1,
        ui_c->top + ui_c->height -3,
        ui_c->width -4,    // width
        BorderSize,       // height
        br_1,      // color
        0                 // rop 
    );
}

struct ui_component_d *libgui_create_ui_component(
    struct dccanvas_d *dc,
    int type,
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height,
    const char *label )
{
    struct ui_component_d *uic;

    if ((void*) dc == NULL)
	    return NULL;
	if (dc->magic != 1234)
	    return NULL;

    uic = (struct ui_component_d *) malloc(sizeof(struct ui_component_d));
    if ((void*) uic == NULL)
	    return NULL;
    uic->used = TRUE;
	uic->magic = 1234;

    uic->left = left;
    uic->top = top;
    uic->width = width;
    uic->height = height;

// ================================

	if (type < 0)
	    return NULL;

    // Button
	if (type == 1)
	{
		uic->type = 1;

		// Draw button's background
        lingui_draw_rectangle0_dc(
            dc,
            left, top, width, height,
            0x00C0C0C0,  //color
            0   //rop 
        );

		// Draw button's borders
        __draw_button_borders_dc(
            dc,
            uic,
            0x00E0E0E0,  // tl 2 inner (light)
            0x00FAFAFA,  // tl 1 most inner (lighter)
            0x00505050,  // br 2 inner (dark)
            0x00989898,  // br 1 most inner (light) 
            0x00101010   // outer 
		);

		// Draw button's label
        libgui_drawstring_dc(
            dc,
            left +8,
            top  +8,
            0x00101010,   // white
            0x00C0C0C0,   // gray
            0,    // ROP 
            label 
        );

		// ...
	}

	// ...
    return (struct ui_component_d *) uic;
}

int 
libgui_redraw_ui_component(
	struct ui_component_d *uic,
    struct dccanvas_d *dc )
{
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

    // Button
	if (uic->type == 1)
	{

		// Draw button's background
        lingui_draw_rectangle0_dc(
            dc,
            uic->left, uic->top, uic->width, uic->height,
            0x00C0C0C0,  //color
            0   //rop 
        );

		// Draw button's borders
        __draw_button_borders_dc(
            dc,
            uic,
            0x00E0E0E0,  // tl 2 inner (light)
            0x00FAFAFA,  // tl 1 most inner (lighter)
            0x00505050,  // br 2 inner (dark)
            0x00989898,  // br 1 most inner (light) 
            0x00101010   // outer 
		);

		// Draw button's label
        libgui_drawstring_dc(
            dc,
            uic->left +8,
            uic->top  +8,
            0x00101010,   // white
            0x00C0C0C0,   // gray
            0,            // ROP 
            "#todo"       // label 
        );

		// ...
	}

	// ...

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
    if ( libgui_FRONTBUFFER_VA == 0 || libgui_BACKBUFFER_VA == 0 )
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
 
    char_initialize();

    return 0;

fail:
    return (int) -1;
}

//
// End
//

