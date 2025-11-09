// libdisp.c 
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

#include "include/libdisp.h"

//
// private
//

// The application/server is gonna need methods
// to get all these values.

static int libgd_current_mode=0;

// Backbuffer - Dark Zone
static unsigned long libgd_BACKBUFFER_VA=0;
// Frontbuffer (LFB) - Light Zone
static unsigned long libgd_FRONTBUFFER_VA=0;

// #test: 
// Addresses used by the frontbuffer
// struct libgd_frontbuffer_info_d  FrontbufferInfo;

// Saving
static unsigned long libgd_SavedX=0;
static unsigned long libgd_SavedY=0;
static unsigned long libgd_SavedBPP=0; 
// Helper
static unsigned long libgd_device_width=0;
static unsigned long libgd_device_height=0;
static unsigned long libgd_device_bpp=0;

// ----------------------------------


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
libdisp_backbuffer_putpixel ( 
    unsigned int color, 
    int x, 
    int y,
    unsigned long rop )
{
// #todo: Return the number of changed pixels.

    int ret_value=0;
// The base address for the target backbuffer.
    unsigned long target_buffer = 
        libgd_BACKBUFFER_VA;

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
        (int) fb_BackBufferPutpixel( 
                  color, 
                  x, 
                  y, 
                  rop, 
                  target_buffer );

// #test: This routine has rop support.
// #bugbug: These two routines are using different types
// in the parameters.
    //return (int) putpixel0( 
    //                 color, x, y, rop, target_buffer );

    return (int) ret_value;
}

// ## putpixel: 
// backbuffer e lfb ##
// IN: cor, x, y
// Put pixel using the kernel service.
// Slow!

int 
grBackBufferPutpixel2 ( 
    unsigned int color, 
    int x, 
    int y )
{
    if (x<0){ return -1; }
    if (y<0){ return -1; }
// Service number 6.
    return (int) gramado_system_call ( 6, color, x, y );
}

/*
 * fb_BackBufferPutpixel:
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
fb_BackBufferPutpixel ( 
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

    unsigned char *where = (unsigned char *) libgd_BACKBUFFER_VA;
    //unsigned char *where = (unsigned char *) buffer_va;

// Device context
    unsigned long deviceLeft   = 0;
    unsigned long deviceTop    = 0;
    unsigned long deviceWidth  = (libgd_device_width  & 0xFFFF );
    unsigned long deviceHeight = (libgd_device_height & 0xFFFF );
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

    switch (libgd_SavedBPP){
        case 32:  bytes_count = 4;  break;
        case 24:  bytes_count = 3;  break;
        //case 16:  bytes_count = 2;  break;
        //case 8:   bytes_count = 1;  break;
        default:
            printf("fb_BackBufferPutpixel: [ERROR] libgd_SavedBPP\n");
            goto fail;
            break;
    };

// #importante
// Pegamos a largura do dispositivo.
    //width = (int) libgd_SavedX; 

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
        printf ("fb_BackBufferPutpixel: MaxOffset\n");
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
    if ( libgd_SavedBPP == 32 ){ a2 = where[Offset +3]; };

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
    if (libgd_SavedBPP == 32){ where[Offset +3] = a3; };

// Return the number of changed pixels. '1'.
    return (int) 1;
// Return the number of changed pixels. '0'.
fail:
    return 0;
}

/*
 * putpixel0:
 *   Write a pixel into a backbuffer using a raster operation (ROP).
 *   The server (kgws) can access the backbuffer but not the frontbuffer,
 *   so it must use the video driver dialog for display.
 *
 * Parameters:
 *   _color     - Source color (BGRA)
 *   _x, _y     - Pixel coordinates
 *   _rop_flags - ROP code (low byte)
 *   buffer_va  - Virtual address of the backbuffer
 *
 * Returns:
 *   Number of pixels changed (currently always 1, or 0 on error).
 */

/*
 * putpixel0:
 *     O servidor kgws pode acessar um buffer. Mas não tem acesso
 * ao frontbuffer. Para isso ele precisa usasr o diálogo do driver 
 * de vídeo.
 * IN: 
 *     color, x, y, rop_flags, buffer va.
 */
// #todo
// + Change the names of these parameters.
// + Create a parameter for the address of the buffer.

// Colors:
// b,   g,  r,  a = Color from parameter.
// b2, g2, r2, a2 = Color from backbuffer.
// b3, g3, r3, a3 = Color to be stored.
// Performance
// Direct memory writes are fine for now, but later you might want:
//  + Inline assembly for speed.
//  + SIMD operations for batch pixel effects.
//  + Double-buffering strategies (swap back/front).

int
putpixel0 ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags,
    unsigned long buffer_va )
{
// #todo: Return the number of changed pixels.

// The address where we're gonna put the data into.
// #todo: It needs to be a valid ring3 address.
    unsigned char *where = (unsigned char *) buffer_va;
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
    int bpp   = (int) libgd_SavedBPP;           // get from globals.
    int width = (int) (libgd_SavedX & 0xFFFF);  // device width
// Positions
    int offset=0;   // the offset of the pixel into the buffer.

    int x = (int) (_x & 0xFFFF);
    int y = (int) (_y & 0xFFFF);

// Buffer address validation.
// The address where we're gonna put the data into.
// #todo: It needs to be a valid ring3 address.
    if (buffer_va == 0){
        //panic("putpixel0: buffer_va\n");
        //server_debug_print("putpixel0: buffer_va\n");
        return 0;  // 0 changed pixels.
    }

// Split: bgra
    b = (Color & 0xFF);
    g = (Color & 0xFF00) >> 8;
    r = (Color & 0xFF0000) >> 16;
    a = (Color >> 24) & 0xFF;

// bpp
// #danger
// This is a global variable.
// Esse valor foi herdado do bootloader.
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
        //server_debug_print("putpixel0: bpp\n");
        printf            ("putpixel0: bpp\n");
        exit(1);
        while(1){}
        break;
    };

// Device width.
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
    b2 = where[offset];
    g2 = where[offset +1];
    r2 = where[offset +2];
    if (bpp == 32){ a2 = where[offset +3]; };
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
    where[offset]    = b3;
    where[offset +1] = g3;
    where[offset +2] = r3;
    if (bpp == 32){ where[offset +3] = a3; };

// Return the number of changed pixels.
    return (int) 1;
}

void 
backbuffer_putpixel ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags )
{
    unsigned long buffer = (unsigned long) libgd_BACKBUFFER_VA;
// Putpixel at the given buffer address
    putpixel0( _color, _x, _y, _rop_flags, buffer );
}

void 
frontbuffer_putpixel ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags )
{
    unsigned long buffer = (unsigned long) libgd_FRONTBUFFER_VA;
// Putpixel at the given buffer address
    putpixel0( _color, _x, _y, _rop_flags, buffer );
}

// IN: 
// back_or_front: 1=back | 2=front
int 
libgd_putpixel ( 
    unsigned int color, 
    int x, 
    int y,
    unsigned long rop,
    int back_or_front )
{
    if (back_or_front ==1){
        backbuffer_putpixel(color,x,y,rop);
        return 0;
    }
    if (back_or_front == 2){
        frontbuffer_putpixel(color,x,y,rop);
        return 0;
    }
    return (int) -1;
}

//============

// Get the color value given the position
unsigned int grBackBufferGetPixelColor(int x, int y)
{
    unsigned char *where = (unsigned char *) libgd_BACKBUFFER_VA;
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
    switch (libgd_SavedBPP){
    case 32:  bytes_count = 4;  break;
    case 24:  bytes_count = 3;  break;
    //case 16:  bytes_count = 2;  break;
    //case 8:   bytes_count = 1;  break;
    default:
        printf("grBackBufferGetPixelColor: [ERROR] libgd_SavedBPP\n");
        //panic ("grBackBufferGetPixelColor: libgd_SavedBPP");
        break;
    };

// #importante
// Pegamos a largura do dispositivo.
    int width = (int) libgd_SavedX;
// Offset
    int offset = (int) ( (bytes_count*width*y) + (bytes_count*x) );
// bgra
    char b, g, r, a;
// Get bytes.
    b = where[offset];
    g = where[offset +1];
    r = where[offset +2];
    if ( libgd_SavedBPP == 32 ){
        a = where[offset +3];
    };
// The buffer.
    unsigned int ColorBuffer=0;
    unsigned char *c = (unsigned char *) &ColorBuffer;

// Paint.
// Set bytes of ColorBuffer.
    c[0] = b;  c[1] = g;  c[2] = r;  c[3] = a;

// Return the color value.
    return (unsigned int) ColorBuffer;
}

//
// #
// INITIALIZATION 
//

// Initialize the libgd library
int libgd_initialize(void)
{

// Get current mode
// Gramado mode
// get gramado mode.
// jail, p1, home, p2, castle ...
    libgd_current_mode = server_get_system_metrics(130);
    if (libgd_current_mode < 0){
        printf("libgd_initialize: [FAIL] libgd_current_mode\n");
        goto fail;
    }

// Get backbuffer and frontbuffer virtual addresses
    libgd_BACKBUFFER_VA  = (unsigned long) rtl_get_system_metrics(12);
    libgd_FRONTBUFFER_VA = (unsigned long) rtl_get_system_metrics(11);

// Screen
// Width, Height and Bits Per Pixel.
    libgd_device_width  = (unsigned long) server_get_system_metrics(1);
    libgd_device_height = (unsigned long) server_get_system_metrics(2);
    libgd_device_bpp    = (unsigned long) server_get_system_metrics(9);
// Saving
    libgd_SavedX   = (unsigned long) libgd_device_width;
    libgd_SavedY   = (unsigned long) libgd_device_height;
    libgd_SavedBPP = (unsigned long) libgd_device_bpp;


// Backbuffer and frontbuffer.
    if ( libgd_FRONTBUFFER_VA == 0 || libgd_BACKBUFFER_VA == 0 )
    {
        printf("libgd_initialize: Buffers\n");
        goto fail;
    }

// Width, Height and Bits Per Pixel.
    if ( libgd_device_width == 0 || 
         libgd_device_height == 0 || 
         libgd_device_bpp == 0 )
    {
        printf("libgd_initialize: w, h and bpp\n");
        goto fail;
    }

    return 0;
fail:
    return (int) -1;
}

//
// End
//

