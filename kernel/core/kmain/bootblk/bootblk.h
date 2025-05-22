// bootblk.h
// Created by Fred Nora.


#ifndef __KMAIN_BOOTBLK_H
#define __KMAIN_BOOTBLK_H    1


// --------------------------------------
// The base address of the boot block.
// virtual = physical.
#define BootBlockVA    0x0000000000090000
// --------------------------------------
// Indexes into the boot block.
// Each entry has 64bit.
#define bbOffsetLFB_PA  0  // offset 0
#define bbOffsetX       1  // offset 8
#define bbOffsetY       2  // offset 16
#define bbOffsetBPP     3  // offset 24
#define bbLastValidPA   4  // offset 32  // Last valid physical address.
#define bbGramadoMode   5  // offset 40  // jail, p1, home ...
//#test
#define bb_idePortNumber  6  // offset 48.
// ...


// Display device support.
// It came from the boot loader.
// See: bldisp.c
extern unsigned long gSavedLFB;
extern unsigned long gSavedX;
extern unsigned long gSavedY;
extern unsigned long gSavedBPP;

// Boot block structure.
// Here is where we save the information that comes from the boot system.
struct bootblk_d
{
    int initialized;
    unsigned long lfb_pa;
    unsigned long deviceWidth;    // in pixels
    unsigned long deviceHeight;   // in pixels
    unsigned long bpp;            // bytes per pixel
    unsigned long last_valid_pa;  // Last valid physical address.
    unsigned long gramado_mode;   // system mode.
    // ...

    // #test
    // The IDE port number given by the 32bit boot loader.
    unsigned long ide_port_number;

    // ...
};
extern struct bootblk_d  bootblk;


#endif

