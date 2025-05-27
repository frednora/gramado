// bootblk.h
// Created by Fred Nora.


#ifndef __KMAIN_BOOTBLK_H
#define __KMAIN_BOOTBLK_H    1


// --------------------------------------
// The base address of the boot block.
// virtual = physical.
#define BootBlockVA    0x0000000000090000

/**********************************
 * Boot Block Field Indexes
 * -------------------------
 * There are 8 fields in the boot block.
 * Each field is 64 bits (8 bytes) in size.
 *
 * Byte offsets are computed as:
 *    offset = Field_Index * 8
 **********************************/

// Field 0: Contains the LFB physical address.
// Stored at bytes 0–7.
#define bbOffsetLFB_PA   0   // Offset: 0  (0 * 8)

// Field 1: Contains the X coordinate.
// Stored at bytes 8–15.
#define bbOffsetX        1   // Offset: 8  (1 * 8)

// Field 2: Contains the Y coordinate.
// Stored at bytes 16–23.
#define bbOffsetY        2   // Offset: 16 (2 * 8)

// Field 3: Contains the bits-per-pixel (BPP) value.
// Stored at bytes 24–31.
#define bbOffsetBPP      3   // Offset: 24 (3 * 8)

// Field 4: Contains the last valid physical address information.
// The lower 32 bits store the 32-bit physical address,
// and the upper 32 bits (within the same 8-byte field) hold a complement (typically 0x00000000).
// Stored at bytes 32–39.
#define bbLastValidPA    4   // Offset: 32 (4 * 8)

// Field 5: Specifies the Gramado mode (e.g., jail, p1, home, etc.).
// Stored at bytes 40–47.
#define bbGramadoMode    5   // Offset: 40 (5 * 8)

// Field 6: (Deprecated) Previously held the IDE port number.
// Stored at bytes 48–55.
#define bb_idePortNumber 6   // Offset: 48 (6 * 8)

// Field 7: Holds the boot disk signature provided by the blgram (from bm2).
// Stored at bytes 56–63.
#define bbDiskSignature  7   // Offset: 56 (7 * 8)

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

// The signature per se.
    unsigned long disk_signature;

    // ...
};
extern struct bootblk_d  bootblk;


#endif

