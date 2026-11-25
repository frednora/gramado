// gr.c

#include "../bl.h"


#define VGA_VRAM 0xA0000
#define PITCH 320

void 
grDirectPutPixel256 ( 
    unsigned int x, 
    unsigned int y,
    unsigned char color );

// 'One' byte per pixel.
void 
grDirectPutPixel256 ( 
    unsigned int x, 
    unsigned int y,
    unsigned char color ) 
{
    unsigned char *lfb = (unsigned char *) VGA_VRAM;
    
    unsigned int offset = (unsigned int) ( (y * PITCH) + x );
    
    // Plot.
    lfb[offset] = color;
}


//
// ===========================================
//

//The following code plots a 8 bit pixel in a planar mode.

void 
grPutPixel8bitPlanarMode (
    unsigned int x, 
    unsigned int y,
    unsigned char color );

void 
grPutPixel8bitPlanarMode (
    unsigned int x, 
    unsigned int y,
    unsigned char color )
{

    unsigned int *lfb = (unsigned int *) 0xA0000;
   
    unsigned int offset = (unsigned int) ( (y*pitch) + (x/8) );

    // bank
    // We don't like banks.
    bankSwitch (offset >> 16);

   /* 
    * writes to bit mask register of graphics controller 
    * to select plane 
    */
   
   // Select the plane.
   outportb (0x3CE,8);
   outportb (0x3CF,0x80 >> (x & 7));
   
   // put.
   lfb[bankOffset] = color;
}


/*
//The following C code plots a 16 bit red-green-blue (RGB) pixel to display.
void pixel_16RGB (unsigned short color, unsigned short x, unsigned short y) {
   unsigned short* fb = (unsigned short*) _modeInfo.physBasePtr;
   unsigned short offset = x + y * (_modeInfo.bytesPerScanLine / 2);
   fb [offset] = color;
}
*/


/*
//The following code plots an 8 bit RGB pixel in a bank switching mode.
void pixel_8RGB (unsigned char color, unsigned short x, unsigned short y) {
   unsigned char* fb = (unsigned char*) 0xa0000;
   unsigned int offset = x + (long)y * _modeInfo.bytesPerScanLine;
   vbe_bankSwitch (offset >> 16);
   fb [offset & 0xffff] = color;
}
*/




















