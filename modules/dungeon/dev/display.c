// display.c
// Display support for ring0 lodable module.
// Created by Fred Nora.

#include <kernel.h>

//
#include "rop.h"

// See:
struct display_info_d  DisplayInfo;


//
// =================================================
//

/*
 * bldisp_putpixel0:
 *     Ok. 
 *     O servidor kgws pode acessar um buffer. Mas não tem acesso
 * ao frontbuffer. Para isso ele precisa usasr o diálogo do driver 
 * de vídeo.
 * IN: 
 *     color, x, y, rop_flags
 */
// #todo
// + Change the names of these parameters.
// + Create a parameter for the address of the buffer.
// Colors:
// b,   g,  r,  a = Color from parameter.
// b2, g2, r2, a2 = Color from backbuffer.
// b3, g3, r3, a3 = Color to be stored.

int bldisp_putpixel0 (unsigned long msg_buf)
{
    unsigned long *msg = (unsigned long *) msg_buf;

    // Invalid address
    if ((void*) msg == NULL)
        goto fail;

// #ps
// maybe these operations are slow because of cache issues.

    unsigned char *dc_where  = (unsigned long *) msg[0]; // Address

    unsigned long dc_width   = (unsigned long) msg[1];
    unsigned long dc_height  = (unsigned long) msg[2];
    unsigned long dc_bpp     = (unsigned long) msg[3];    // bits per pixel

    unsigned long dc_pitch   = (unsigned long) msg[4];  // bytes per row

    unsigned int Color       = (unsigned int) (msg[5] & 0xFFFFFFFF);
    unsigned long _x         = (unsigned long) msg[6];  
    unsigned long _y         = (unsigned long) msg[7]; 
    unsigned long _rop_flags = (unsigned long) msg[8];

    
// ----------------------------
// Color
    char b, g, r, a;
    b = (Color & 0xFF);
    g = (Color & 0xFF00) >> 8;
    r = (Color & 0xFF0000) >> 16;
    a = (Color >> 24) & 0xFF;

// The first byte;
// 0 ~ FF
    int Operation = (int) (_rop_flags & 0xFF);

// Positions
    int x = (int) (_x & 0xFFFF);
    int y = (int) (_y & 0xFFFF);


    //if (x >= dc_width || y >= dc_height)
        //return -1;

// 3 = 24 bpp
// 2 = 16 bpp
// ...
    int bytes_count=0;

// Buffer address validation.
/*
    if (buffer_va == 0){
        panic("bldisp_putpixel0: buffer_va\n");
    }
*/
    if ((void*) dc_where == NULL)
        return -1;
        //panic ("bldisp_putpixel0: dc_where");

//
// bpp
//

// #danger
// This is a global variable.
// Esse valor foi herdado do bootloader.

    // Bits per pixel
    switch (dc_bpp)
    {
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
         
        return -1;

        //#todo: Do we have panic ar this moment?
        //panic ("putpixel0: gSavedBPP\n");
        //debug_print_string("bldisp_putpixel0: dc_bpp\n");
        while(1){}
        break;
    };

// #importante
// Pegamos a largura do dispositivo.
    //int width = (int) (gSavedX & 0xFFFF);
    int width = (int) (dc_width & 0xFFFF);
    width = (int) (width & 0xFFFF);

    int offset=0;
    //int offset = (int) ( (bytes_count*width*y) + (bytes_count*x) );

// Offset.

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

// ------------------------------------------
// A cor que estava no framebuffer.
    unsigned char b2, g2, r2, a2;
// Get yhe color.
    b2 = dc_where[offset];
    g2 = dc_where[offset +1];
    r2 = dc_where[offset +2];
    //if (gSavedBPP == 32){ a2 = dc_where[offset +3]; };
    if (dc_bpp == 32){ a2 = dc_where[offset +3]; };

// ------------------------------
// A cor transformada.
// A cor a ser gravada.
    unsigned char b3, g3, r3, a3;

// -------------------------
// 0 - Sem modificação
// A cor a ser registrada é a mesma enviada por argumento.
    if (Operation == ROP_COPY)
    {
        r3=r; g3=g; b3=b; a3=a;
        //goto RegisterColor;
    }
// -------------------------
// 1 = or
    if (Operation == ROP_OR)
    {
        r3 = (r2 | r);
        g3 = (g2 | g);
        b3 = (b2 | b);
        a3 = a2;
    }
// -------------------------
// 2 = and
    if (Operation == ROP_AND)
    {
        r3 = (r2 & r);
        g3 = (g2 & g);
        b3 = (b2 & b);
        a3 = a2;
    }
// -------------------------
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

// -------------------------
// 10 - less red
    if (Operation == ROP_LESS_RED)
    {
        r3 = (r2 & 0xFE);
        g3 = g2;
        b3 = b2; 
        a3 = a2;
    }
// -------------------------
// 11 - less green
    if (Operation == ROP_LESS_GREEN)
    {
        r3 = r2;
        g3 = (g2 & 0xFE);
        b3 = b2; 
        a3 = a2;
    }
// -------------------------
// 12 - less blue
    if (Operation == ROP_LESS_BLUE)
    {
        r3 = r2;
        g3 = g2;
        b3 = (b2 & 0xFE); 
        a3 = a2;
    }

// -------------------------
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

// Luminosity
// Gray: luminosity = R*0.3 + G*0.59 + B *0.11

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

//RegisterColor:

// ----------------------------
// BGR and A
    dc_where[offset]    = b3;
    dc_where[offset +1] = g3;
    dc_where[offset +2] = r3;
    //if (gSavedBPP == 32){ where[offset +3] = a3; };
    if (dc_bpp == 32){ dc_where[offset +3] = a3; };

// Number of changed pixels
    return (int) 1;

// Number of changed pixels
fail:
    return (int) 0;
}


//
// =================================================
//

// :: Step1
// Initialize display information
// Called only by the display server.
unsigned long 
display_initialization_phase1(
    unsigned long param1,
    unsigned long param2,
    unsigned long param3,
    unsigned long param4 )
{

// #todo
// At this moment, maybe we can call the 
// kernel to initialize some components 
// that at this moment live in kernel.
    if (ModuleInitialization.initialized != TRUE)
    {
        // See: kstdio.c
        newm0_initialize(0,0,0);
    }

    if (ModuleInitialization.initialized == TRUE)
    {
        //printk("Parameters: %d | %d | %d | %d\n",
            // param1, param2, param3, param4 );
    }

    // Signature
    if (param4 != 1234)
        goto fail;

// Initializazing the phase 1.
// Initialize displey info structure.
    DisplayInfo.frontbuffer_address = 
        (unsigned long) param2;  // frontbuffer va.
    DisplayInfo.backbuffer_address = 
        (unsigned long) param3;  // backbuffer va.

    DisplayInfo.phase1 = TRUE;
    DisplayInfo.phase2 = FALSE;
    DisplayInfo.initialized = FALSE;

    //#debug
    //printk ("frontbuffer={%x} | backbuffer={%x}\n",
        //DisplayInfo.frontbuffer_address,
        //DisplayInfo.backbuffer_address );
    //while(1){ asm ("hlt"); }

    return 1234;

fail:
    return 4321;
}

// :: Step2
// Initialize displey information
// Called only by the display server.
unsigned long 
display_initialization_phase2(
    unsigned long param1,
    unsigned long param2,
    unsigned long param3,
    unsigned long param4 )
{
            
    if (ModuleInitialization.initialized == TRUE)
    {
        //printk("Parameters: %d | %d | %d | %d\n",
            //param1, param2, param3, param4 );
    }

// We need to initialize the phase 1 first.
    if (DisplayInfo.phase1 != TRUE)
    {
        DisplayInfo.phase1 = FALSE;
        DisplayInfo.phase2 = FALSE;
        DisplayInfo.initialized = FALSE;
        goto fail;
    }

// Initialize display info structure.
    DisplayInfo.width  = (unsigned long) param2;  // w
    DisplayInfo.height = (unsigned long) param3;  // h
    DisplayInfo.bpp    = (unsigned long) param4;  // bits per pixel

    DisplayInfo.phase2 = TRUE;
    DisplayInfo.initialized = TRUE;

    /*
    // #debug
    printk ("w={%d} | h={%d} | bpp={%d}\n",
        DisplayInfo.width, 
        DisplayInfo.height, 
        DisplayInfo.bpp );
    
    while(1){ asm ("hlt"); }
    */

    return 1234;

fail:
    return 4321;
}





