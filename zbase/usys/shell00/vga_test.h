
//This is a test
//credits:
//https://forum.osdev.org/viewtopic.php?f=1&t=19158
//http://files.osdev.org/mirrors/geezer/osd/graphics/modes.c

#include <libio.h>

//define the ports , taken from http://files.osdev.org/mirrors/geezer/osd/graphics/modes.c
#define   VGA_AC_INDEX      0x3C0
#define   VGA_AC_WRITE      0x3C0
#define   VGA_AC_READ         0x3C1
#define   VGA_MISC_WRITE      0x3C2
#define VGA_SEQ_INDEX      0x3C4
#define VGA_SEQ_DATA      0x3C5
#define   VGA_DAC_READ_INDEX   0x3C7
#define   VGA_DAC_WRITE_INDEX   0x3C8
#define   VGA_DAC_DATA      0x3C9
#define   VGA_MISC_READ      0x3CC
#define VGA_GC_INDEX       0x3CE
#define VGA_GC_DATA       0x3CF
#define VGA_CRTC_INDEX      0x3D4      /* 0x3B4 */
#define VGA_CRTC_DATA      0x3D5      /* 0x3B5 */
#define   VGA_INSTAT_READ      0x3DA
#define   VGA_NUM_SEQ_REGS   5
#define   VGA_NUM_CRTC_REGS   25
#define   VGA_NUM_GC_REGS      9
#define   VGA_NUM_AC_REGS      21
#define   VGA_NUM_REGS      (1+VGA_NUM_SEQ_REGS+VGA_NUM_CRTC_REGS+VGA_NUM_GC_REGS+VGA_NUM_AC_REGS)

//the vga identifiers
unsigned int VGA_width;
unsigned int VGA_height;
unsigned int VGA_bpp;
unsigned char *VGA_address;

/**
* CREATE THE REGISTER ARRAY TAKEN FROM http://wiki.osdev.org/VGA_Hardware
*/
unsigned char mode_320_200_256[]={
   /* MISC
    *
    * 0x63 => 01100011
    * 7 6 5 4 3 2 1 0
    * 1 1 0 0 0 1 1 0
    * VSP HSP - - CS CS ERAM IOS
    * 7,6 - 480 lines
    * 5,4 - free
    * 3,2 - 28,322 MHZ Clock
    * 1 - Enable Ram
    * 0 - Map 0x3d4 to 0x3b4
    */
   0x63,
   /* SEQ */
   /**
    * index 0x00 - Reset
    * 0x03 = 11
    * Bits 1,0 Synchronous reset
    */
   0x03,
   /**
    * index 0x01
    * Clocking mode register
    * 8/9 Dot Clocks
    */
   0x01,
   /**
    * Map Mask Register, 0x02
    * 0x0F = 1111
    * Enable all 4 Maps Bits 0-3
    * chain 4 mode
    */
   0x0F,
   /**
    * map select register, 0x03
    * no character map enabled
    */
   0x00,
   /**
    * memory mode register 0x04
    * enables ch4,odd/even,extended memory
    */
   0x0E,
   /* CRTC */
   0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
   0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x9C, 0x0E, 0x8F, 0x28,   0x40, 0x96, 0xB9, 0xA3,
   0xFF,
   /* GC */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
   0xFF,
   /* AC */
   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
   0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
   0x41, 0x00, 0x0F, 0x00,   0x00
};

void write_registers(unsigned char *regs){
   unsigned i;

   /* write MISCELLANEOUS reg */
   libio_outport8(VGA_MISC_WRITE, *regs);
   regs++;
   /* write SEQUENCER regs */
   for(i = 0; i < VGA_NUM_SEQ_REGS; i++)
   {
      libio_outport8(VGA_SEQ_INDEX, i);
      libio_outport8(VGA_SEQ_DATA, *regs);
      regs++;
   }
   /* unlock CRTC registers */
   libio_outport8(VGA_CRTC_INDEX, 0x03);
   libio_outport8(VGA_CRTC_DATA, libio_inport8(VGA_CRTC_DATA) | 0x80);
   libio_outport8(VGA_CRTC_INDEX, 0x11);
   libio_outport8(VGA_CRTC_DATA, libio_inport8(VGA_CRTC_DATA) & ~0x80);
   /* make sure they remain unlocked */
   regs[0x03] |= 0x80;
   regs[0x11] &= ~0x80;
   /* write CRTC regs */
   for(i = 0; i < VGA_NUM_CRTC_REGS; i++)
   {
      libio_outport8(VGA_CRTC_INDEX, i);
      libio_outport8(VGA_CRTC_DATA, *regs);
      regs++;
   }
   /* write GRAPHICS CONTROLLER regs */
   for(i = 0; i < VGA_NUM_GC_REGS; i++)
   {
      libio_outport8(VGA_GC_INDEX, i);
      libio_outport8(VGA_GC_DATA, *regs);
      regs++;
   }
   /* write ATTRIBUTE CONTROLLER regs */
   for(i = 0; i < VGA_NUM_AC_REGS; i++)
   {
      (void)libio_inport8(VGA_INSTAT_READ);
      libio_outport8(VGA_AC_INDEX, i);
      libio_outport8(VGA_AC_WRITE, *regs);
      regs++;
   }
   
   /* lock 16-color palette and unblank display */
   (void)libio_inport8(VGA_INSTAT_READ);
   libio_outport8(VGA_AC_INDEX, 0x20);
}

/**
* Clears the VGA screen
*/
void VGA_clear_screen(void)
{
   unsigned int x=0;
   unsigned int y=0;

   for(y=0; y<VGA_height; y++){
      for(x=0; x<VGA_width; x++){
         VGA_address[VGA_width*y+x]=0x0f;
      }
   }
}


/**
* Note here the vga struct must have the width 320 and height of 200
* color mode is 256
*/

void VGA_init(int width, int height, int bpp)
{
   //setup the vga struct
   VGA_width=(unsigned int)width;
   VGA_height=(unsigned int)height;
   VGA_bpp=bpp;
   VGA_address=0xA0000;

   //enables the mode 13 state
   write_registers(mode_320_200_256);


// clears the screen
// #bugbug: We cant write into that memory position
// been in ring3.

   //VGA_clear_screen();
}
