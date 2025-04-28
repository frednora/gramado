// vgad.h
// Created by Fred Nora.

#ifndef __SERVER_VGAD_H
#define __SERVER_VGAD_H    1


// Define the ports.
// Taken from: 
// http://files.osdev.org/mirrors/geezer/osd/graphics/modes.c

// ===============================
// Step1:
// Bit 0 of this register controls the location of several other registers: 
// If cleared, port 0x3D4 is mapped to 0x3B4 ...
#define VGA_MISC_WRITE  0x3C2  // Miscellaneous Output Register.
#define VGA_MISC_READ   0x3CC

// ===============================
// Step2:
// Sequencer.
#define VGA_SEQ_INDEX  0x3C4
#define VGA_SEQ_DATA   0x3C5
/* VGA sequencer register indices */
#define VGA_SEQ_RESET          0x00
#define VGA_SEQ_CLOCK_MODE     0x01
#define VGA_SEQ_PLANE_WRITE    0x02
#define VGA_SEQ_CHARACTER_MAP  0x03
#define VGA_SEQ_MEMORY_MODE    0x04
/* VGA sequencer register bit masks */
#define VGA_SR01_CHAR_CLK_8DOTS  0x01 /* bit 0: character clocks 8 dots wide are generated */
#define VGA_SR01_SCREEN_OFF      0x20 /* bit 5: Screen is off */
#define VGA_SR02_ALL_PLANES      0x0F /* bits 3-0: enable access to all planes */
#define VGA_SR04_EXT_MEM         0x02 /* bit 1: allows complete mem access to 256K */
#define VGA_SR04_SEQ_MODE        0x04 /* bit 2: directs system to use a sequential addressing mode */
#define VGA_SR04_CHN_4M          0x08 /* bit 3: selects modulo 4 addressing for CPU access to display memory */

// ===============================
// Step3:
// The CRTC registers allow us 
// to control the pixel clock timings of the 
// horizontal and vertical retrace and 
// blanking periods of the display.
// These timing periods help 
// control the output of the display (the video resolution), and 
// the refresh rate.
#define VGA_CRTC_INDEX  0x3D4      /* 0x3B4 */
#define VGA_CRTC_DATA   0x3D5      /* 0x3B5 */

// ===============================
// Step4:
// Graphics Controller.
#define VGA_GC_INDEX  0x3CE
#define VGA_GC_DATA   0x3CF
/* VGA graphics controller register indices */
#define VGA_GFX_SR_VALUE       0x00
#define VGA_GFX_SR_ENABLE      0x01
#define VGA_GFX_COMPARE_VALUE  0x02
#define VGA_GFX_DATA_ROTATE    0x03
#define VGA_GFX_PLANE_READ     0x04
#define VGA_GFX_MODE           0x05
#define VGA_GFX_MISC           0x06
#define VGA_GFX_COMPARE_MASK   0x07
#define VGA_GFX_BIT_MASK       0x08

// ===============================
// Step5:
// This port is a confusing one: 
// You write both the index and data bytes to the same port. 
#define VGA_AC_INDEX  0x3C0
#define VGA_AC_WRITE  0x3C0
#define VGA_AC_READ   0x3C1

// DAC
#define VGA_DAC_READ_INDEX   0x3C7
#define VGA_DAC_WRITE_INDEX  0x3C8
#define VGA_DAC_DATA         0x3C9

// input
#define VGA_INSTAT_READ  0x3DA

#define VGA_NUM_SEQ_REGS    5
#define VGA_NUM_CRTC_REGS  25
#define VGA_NUM_GC_REGS     9
#define VGA_NUM_AC_REGS    21
#define VGA_NUM_REGS \
    ( 1 + VGA_NUM_SEQ_REGS + VGA_NUM_CRTC_REGS + VGA_NUM_GC_REGS + VGA_NUM_AC_REGS )



struct vga_info_d
{
    int initialized;

    int current_mode;
    unsigned long ptr_modes[32];
    
    // w, h, bpp.
    unsigned int width;
    unsigned int height;
    unsigned int bpp;

// #bugbug
// We can't write on this address from ring3.
    unsigned long lfb_address;
};
extern struct vga_info_d  VGAInfo;

//
// =====================================================
//

int vgad_main(void);


#endif   


