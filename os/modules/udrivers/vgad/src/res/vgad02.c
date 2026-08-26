/*
To create a simple VGA driver that allows you to change the resolution, 
you can leverage the existing functions and structures from your vgad.c and vgad.h files.
 Below, I am providing a simplified and structured version of the VGA driver focusing on 
 resolution switching.

 Key Features of the Driver
Resolution Switching:

The driver supports two modes: 320x200x256 and 640x480x16.
You can easily add more modes by updating the VGAInfo.ptr_modes array and defining the required register configurations.
Error Handling:

The code validates mode indices and checks for valid configurations.
Initialization:

The vga_initialize() function sets up the VGA mode by writing the appropriate registers.
Main Functionality:

The vgad_main() function demonstrates initializing the VGA driver and setting a resolution.
Testing Steps
Compile the driver as part of your Gramado OS.
Run the vgad_main() function to test resolution switching.
Verify that the screen resolution changes as expected.
*/



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "vgad.h"

#define FALSE  0
#define TRUE   1

// see: vagd.h
struct vga_info_d  VGAInfo;

// ===============================================================

/*
 * CREATE THE REGISTER ARRAY 
 * TAKEN FROM http://wiki.osdev.org/VGA_Hardware
 */

// graphics mode 0x13, 320x200 256 colors.
unsigned char mode_320_200[] = {

    //
    //  Considering multiples of 10 width and 10 height.
    //
    
    
    /* MISC
     * Miscellaneous Output Register
     * 0x63 => 0110 0011
     * 7 6 5 4 3 2 1 0
     * 1 1 0 0 0 1 1 0
     * VSP HSP - - CS CS ERAM IOS
     * 7,6 - 480 lines
     * 5,4 - free
     * 3,2 - 28,322 MHZ Clock (Clock selection)
     * 1 - Enable Ram
     * 0 - Map 0x3d4 to 0x3b4. (if cleared)
     */
    // 0xE3 - mode 12h (640x480 planar 16 color mode)
    // 0x63 - mode 13h (320x200 linear 256-color mode)
    // 0xE3 - mode X   (320x240 planar 256 color mode)
    
        0x63,    // 0110 0011     // 200, free, hz, enable ram, do use the other port.
        //0xE3,  // 1100 0011   // 480, free, hz, enable ram, do use the other port.
    
    //-----------------------------
    /* SEQ */
    /**
     * Index 0x00: - (Reset).
     * 0x03 = 11
     * Bits 1,0 - Synchronous reset.
     */
        0x01,
    /**
     * Index 0x01: (Clocking mode).
     * bit 0: (8/9 Dot Clocks).
     * Each character equals 8 (9/8 Dot Mode is set) or 
     * 9 (9/8 Dot Mode is clear) pixels.
     */
        0x01,
    /**
     * Index 0x02: (Plane write)(map mask).
     * 0x0F = 1111
     * Enable all 4 Maps Bits 0-3
     * chain 4 mode
     */
        0x0F,
    /**
     *  Index 0x03: (Character map).
     * no character map enabled
     */
        0x00,
    /**
     * Index 0x04: (Sequencer Memory mode).
     * Enables ch4, odd/even, extended memory
     */
        0x0E,
    
    //-----------------------------
    /* CRTC */
    // http://martin.hinner.info/vga/timing.html
    // By programming this unit you can control the resolution of your monitor, 
    // as well as some hardware overlay and panning effects.
    // Registers 0-7 of 0x3D4 
    // are 'write protected' by the protect bit (bit 7 of index 0x11).
    // ----------------
    // Horizontal:
    // Registers involved in horizontal timing: 
    // (0x00~0x05)
    // Horizontal timings are based on character clocks (multiples of 8 or 9 pixels)
    // ----------------
    // Vertical:
    // Registers involved in vertical timing:
    // (0x06,0x07,0x09,0x10,0x11,0x12,0x15,0x16)
    // Vertical timings are per-scanline.
    // Since these easily exceed the 255 limit of one byte, 
    // the Overflow Register is used to store the high-order bits.
    
    // ----------------
    // Horizontal:
    // Char width is '10'.
    
    // h  0x00
    // Horizontal Total
        0x5F,
    
    // h1 0x01  
    // Horizontal Display Enable End 
    // (40*8)=320 | (80*8)=640 |(640/8) = 80 = 0x50 (vm) <<<----
    // 0x39 ... 64*8=512.
    // 0x4F = 800, considering granularity of 10 in width.
    // 0x41 = 640, considering granularity of 10 in width.
        0x41,
    
    // h2 0x02  
    // Horizontal Blanking Start  
        0x50,
    
    // h  0x03  
    // Horizontal Blanking End | 
    // Horizontal Display Skew (5.6) | 
    // Horizontal Blanking End (bits 0..4) 
    // 1000 0010
    // '100' '00010'
        0x82,  
    
    // h3 0x04  
    // Horizontal retrace pulse start
        0x54,
    
    // h  0x05  
    // Horizontal retrace end | 
    // H. Blanking End (bit 5) 7? | 
    // Horizontal Retrace End (0..4)   1000 0000
        0x80,  
    
    // ----------------
    // Vertical:
    // Char width is '?'.
    
    
        0xBF,  // 0x06 v  vertical total
        0x1F,  // 0x07 v  overflow
        0x00,  // 0x08    present row scan
        // 0x41 = 640, considering granularity of 10 in height.
        // 0x30 = 480, considering granularity of 10 in height.
        // Isso mudou a resolução para h=48 h=0x30
        0x41,  // 0x09 v  maximum scanline
        0x00,  // 0x0A    cursor start
        0x00,  // 0x0B    cursor end
        0x00,  // 0x0C    start address high  (#bugbug)
        0x00,  // 0x0D    start address low   (#bugbug)
        0x00,  // 0x0E    cursor location high
        0x00,  // 0x0F    cursor location low
        0x9C,  // 0x10 v3 Vertical Retrace Start (bits 0..7)
        0x8E,  // 0x11 v  Vertical Retrace End
    // 0x12 v1 
    // Vertical Display Enable End 
    // (bits 0..7) (vm) <<<----
    // 0x39 ... 64*8=512.
    // 0x19 ... 32*8=256.
        0x8F, //0x39,  //0x8F,  
        0x28,  // 0x13    offset
        0x40,  // 0x14    underline location
        0x96,  // 0x15 v2 Vertical Blanking Start (bits 0..7)
        0xB9,  // 0x16 v  Vertical Blanking End (bits 0..6)
        0xA3,  // 0x17    CRT mode control
        0xFF,  // 0x18    line compare
    
    //-----------------------------
    /* GC */
        0x00,  // 0x00  Set/Reset (bits 0~3)
        0x00,  // 0x01  Enable Set/Reset (bits 0~3)
        0x00,  // 0x02  color compare
        0x00,  // 0x03  data rotate
        0x00,  // 0x04  read map select
        0x40,  // 0x05  graphics mode: 256-Color Shift (bit6) | Interleaved Shift (bit 5).
        0x05,  // 0x06  Miscellaneous
        0x0F,  // 0x07  color don't care
        0xFF,  // 0x08  Bit Mask
    
    //-----------------------------
    /* AC */
    // 0-15: Palette entry register
        0x00,  // 0x00  | start of palette stuff
        0x01,  // 0x01
        0x02,  // 0x02
        0x03,  // 0x03
        0x04,  // 0x04
        0x05,  // 0x05
        0x06,  // 0x06
        0x07,  // 0x07
        0x08,  // 0x08
        0x09,  // 0x09
        0x0A,  // 0x0A
        0x0B,  // 0x0B
        0x0C,  // 0x0C
        0x0D,  // 0x0D
        0x0E,  // 0x0E
        0x0F,  // 0x0F  | end of palette stuff
        0x41,  // 0x10  Mode control
        0x00,  // 0x11  Overscan color Register
        0x0F,  // 0x12  Color Plane Enable
        0x00,  // 0x13  Horizontal Pixel Panning
        0x00   // 0x14  Color Select
    };
    
    
    // ===============================================================
    
    /*
     * CREATE THE REGISTER ARRAY 
     * TAKEN FROM http://wiki.osdev.org/VGA_Hardware
     */
    
    // graphics mode 0x12, 640x480 16 colors.
    unsigned char mode_640_480[] = {
    
    //
    //  Considering multiples of 10 width and 10 height.
    //
    
    
    /* MISC
     * Miscellaneous Output Register
     * 0x63 => 0110 0011
     * 7 6 5 4 3 2 1 0
     * 1 1 0 0 0 1 1 0
     * VSP HSP - - CS CS ERAM IOS
     * 7,6 - 480 lines
     * 5,4 - free
     * 3,2 - 28,322 MHZ Clock (Clock selection)
     * 1 - Enable Ram
     * 0 - Map 0x3d4 to 0x3b4. (if cleared)
     */
    // 0xE3 - mode 12h (640x480 planar 16 color mode)
    // 0x63 - mode 13h (320x200 linear 256-color mode)
    // 0xE3 - mode X   (320x240 planar 256 color mode)
    
        //0x63,    // 0110 0011     // xlines, free, hz, enable ram, do use the other port.
        0xE3,  // 1100 0011   // 480, free, hz, enable ram, do use the other port.
    
    //-----------------------------
    /* SEQ */
    /**
     * Index 0x00: - (Reset).
     * 0x03 = 11
     * Bits 1,0 - Synchronous reset.
     */
        0x01,
    /**
     * Index 0x01: (Clocking mode).
     * bit 0: (8/9 Dot Clocks).
     * Each character equals 8 (9/8 Dot Mode is set) or 
     * 9 (9/8 Dot Mode is clear) pixels.
     */
        0x01,
    /**
     * Index 0x02: (Plane write)(map mask).
     * 0x0F = 1111
     * Enable all 4 Maps Bits 0-3
     * chain 4 mode
     */
        0x0F,
    /**
     *  Index 0x03: (Character map).
     * no character map enabled
     */
        0x00,
    /**
     * Index 0x04: (Sequencer Memory mode).
     * Enables ch4, odd/even, extended memory
     */
        0x06,
    
    //-----------------------------
    /* CRTC */
    // http://martin.hinner.info/vga/timing.html
    // By programming this unit you can control the resolution of your monitor, 
    // as well as some hardware overlay and panning effects.
    // Registers 0-7 of 0x3D4 
    // are 'write protected' by the protect bit (bit 7 of index 0x11).
    // ----------------
    // Horizontal:
    // Registers involved in horizontal timing: 
    // (0x00~0x05)
    // Horizontal timings are based on character clocks (multiples of 8 or 9 pixels)
    // ----------------
    // Vertical:
    // Registers involved in vertical timing:
    // (0x06,0x07,0x09,0x10,0x11,0x12,0x15,0x16)
    // Vertical timings are per-scanline.
    // Since these easily exceed the 255 limit of one byte, 
    // the Overflow Register is used to store the high-order bits.
    
    // ----------------
    // Horizontal:
    // Char width is '10'.
    
    // 0x00 h   | Horizontal Total
        0x5F,
    
    // 0x01 h1  | Horizontal Display Enable End 
        0x4F,
    
    // 0x02 h2  | Horizontal Blanking Start 
        0x50,  
    
    // 0x03 h    
    // Horizontal Blanking End | 
    // Horizontal Display Skew (5.6) | 
    // Horizontal Blanking End (bits 0..4) 
    // 1000 0010
    // '100' '00010'
        0x82, 
    
    // 0x04 h3   
    // Horizontal retrace pulse start
        0x54,
    
    // 0x05 h    
    // Horizontal retrace end | 
    // H. Blanking End (bit 5) 7? | 
    // Horizontal Retrace End (0..4)   1000 0000
        0x80,  
    
    // ----------------
    // Vertical:
    // Char width is '?'.
    
        0x0B,  // 0x06 v  vertical total
        0x3E,  // 0x07 v  overflow
        0x00,  // 0x08    present row scan
        0x40,  // 0x09 v  maximum scanline
        0x00,  // 0x0A    cursor start
        0x00,  // 0x0B    cursor end
        0x00,  // 0x0C    start address high  (#bugbug)
        0x00,  // 0x0D    start address low   (#bugbug)
        0x00,  // 0x0E    cursor location high
        0x00,  // 0x0F    cursor location low
        0xEA,  // 0x10 v3 Vertical Retrace Start (bits 0..7)
        0x8C,  // 0x11 v  Vertical Retrace End
    // 0x12 v1 
    // Vertical Display Enable End 
    // (bits 0..7) (vm) <<<----
    // 0x39 ... 64*8=512.
    // 0x19 ... 32*8=256.
        0xDF, //0x39,  //0x8F,  
        0x28,  // 0x13    offset
        0x00,  // 0x14    underline location
        0xE7,  // 0x15 v2 Vertical Blanking Start (bits 0..7)
        0x04,  // 0x16 v  Vertical Blanking End (bits 0..6)
        0xE3,  // 0x17    CRT mode control
        0xFF,  // 0x18    line compare
    
    //-----------------------------
    /* GC */
        0x00,  // 0x00  Set/Reset (bits 0~3)
        0x00,  // 0x01  Enable Set/Reset (bits 0~3)
        0x00,  // 0x02  color compare
        0x00,  // 0x03  data rotate
        0x00,  // 0x04  read map select
        0x00,  // 0x05  graphics mode: 256-Color Shift (bit6) | Interleaved Shift (bit 5).
        0x05,  // 0x06  Miscellaneous
        0x0F,  // 0x07  color don't care
        0xFF,  // 0x08  Bit Mask
    
    //-----------------------------
    /* AC */
    // 0-15: Palette entry register
        0x00,  // 0x00  | start of palette stuff
        0x01,  // 0x01
        0x02,  // 0x02
        0x03,  // 0x03
        0x04,  // 0x04
        0x05,  // 0x05
        0x14,  // 0x06
        0x07,  // 0x07
        0x38,  // 0x08
        0x39,  // 0x09
        0x3A,  // 0x0A
        0x3B,  // 0x0B
        0x3C,  // 0x0C
        0x3D,  // 0x0D
        0x3E,  // 0x0E
        0x3F,  // 0x0F  | end of palette stuff
        0x01,  // 0x10  Mode control
        0x00,  // 0x11  Overscan color Register
        0x0F,  // 0x12  Color Plane Enable
        0x00,  // 0x13  Horizontal Pixel Panning
        0x00   // 0x14  Color Select
    };
    
  


// Constants for supported modes
#define MODE_320x200x256 0
#define MODE_640x480x16  1

// Function to write VGA registers for a specific mode
static void write_registers(int mode_index) {

    unsigned int i=0;

    if (mode_index < 0 || mode_index >= 2) {
        printf("Invalid mode index\n");
        return;
    }

// Setup the table of modes
    VGAInfo.ptr_modes[0] = (unsigned long) &mode_320_200[0];
    VGAInfo.ptr_modes[1] = (unsigned long) &mode_640_480[0]; 

    unsigned char *regs = (unsigned char *)VGAInfo.ptr_modes[mode_index];
    if (!regs) {
        printf("Invalid mode configuration\n");
        return;
    }

    // Step 1: Write MISCELLANEOUS Register
    libio_outport8(VGA_MISC_WRITE, *regs++);
    
    // Step 2: Write SEQUENCER Registers
    for (i = 0; i < VGA_NUM_SEQ_REGS; i++) {
        libio_outport8(VGA_SEQ_INDEX, i);
        libio_outport8(VGA_SEQ_DATA, *regs++);
    }

    // Step 3: Unlock CRTC Registers and Write CRTC Registers
    libio_outport8(VGA_CRTC_INDEX, 0x03);
    libio_outport8(VGA_CRTC_DATA, libio_inport8(VGA_CRTC_DATA) | 0x80); // Unlock
    libio_outport8(VGA_CRTC_INDEX, 0x11);
    libio_outport8(VGA_CRTC_DATA, libio_inport8(VGA_CRTC_DATA) & ~0x80); // Unlock

    for (i = 0; i < VGA_NUM_CRTC_REGS; i++) {
        libio_outport8(VGA_CRTC_INDEX, i);
        libio_outport8(VGA_CRTC_DATA, *regs++);
    }

    // Step 4: Write GRAPHICS CONTROLLER Registers
    for (i = 0; i < VGA_NUM_GC_REGS; i++) {
        libio_outport8(VGA_GC_INDEX, i);
        libio_outport8(VGA_GC_DATA, *regs++);
    }

    // Step 5: Write ATTRIBUTE CONTROLLER Registers
    for (i = 0; i < VGA_NUM_AC_REGS; i++) {
        (void)libio_inport8(VGA_INSTAT_READ); // Reset flip-flop
        libio_outport8(VGA_AC_INDEX, i);
        libio_outport8(VGA_AC_WRITE, *regs++);
    }
    (void)libio_inport8(VGA_INSTAT_READ); // Reset flip-flop
    libio_outport8(VGA_AC_INDEX, 0x20);  // Lock palette
}

// VGA initialization function
int vga_initialize(int mode_number) {
    printf("Initializing VGA...\n");
    VGAInfo.initialized = FALSE;

    // Set up the Linear Frame Buffer (LFB) address
    VGAInfo.lfb_address = (unsigned long)(0xA0000 & 0xFFFFF);

    // Configure mode-specific parameters
    switch (mode_number) {
        case MODE_320x200x256:
            VGAInfo.width = 320;
            VGAInfo.height = 200;
            VGAInfo.bpp = 256;
            write_registers(MODE_320x200x256);
            break;

        case MODE_640x480x16:
            VGAInfo.width = 640;
            VGAInfo.height = 480;
            VGAInfo.bpp = 16;
            write_registers(MODE_640x480x16);
            break;

        default:
            printf("Invalid mode number\n");
            return -1;
    }

    // Successfully initialized
    VGAInfo.initialized = TRUE;
    printf("VGA initialized successfully\n");
    return 0;
}

// Main entry point for the VGA driver
int vgad_main(void) {
    printf("VGAD Driver: Starting...\n");

    // Initialize VGA in 640x480x16 mode
    if (vga_initialize(MODE_640x480x16) == 0) {
        printf("Resolution set to 640x480x16.\n");
    } else {
        printf("Failed to set resolution.\n");
    }

    // Add any additional functionality here

    printf("VGAD Driver: Done.\n");
    return 0;
}


