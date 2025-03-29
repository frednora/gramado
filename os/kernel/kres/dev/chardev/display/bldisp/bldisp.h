// bldisp.h
// Bootloader display device.
// Created by Fred Nora.

#ifndef __BLDISP_BLDISP_H
#define __BLDISP_BLDISP_H    1

// See:
// http://www.brokenthorn.com/Resources/OSDevVid2.html
/*
    3C0-3CF  EGA/VGA
	3C0 VGA attribute and sequencer register
	3C1 Other video attributes
	3C2 EGA, VGA, CGA input status 0
	3C3 Video subsystem enable
	3C4 CGA, EGA, VGA sequencer index
	3C5 CGA, EGA, VGA sequencer
	3C6 VGA video DAC PEL mask
	3C7 VGA video DAC state
	3C8 VGA video DAC PEL address
	3C9 VGA video DAC
	3CA VGA graphics 2 position
	3CC VGA graphics 1 position
	3CD VGA feature control
	3CE VGA graphics index
	3CF Other VGA graphics

	3D0-3DF Color Graphics Monitor Adapter (ports 3D0-3DB are
		write only, see 6845)
	3D0 port address decodes to 3D4
	3D1 port address decodes to 3D5
	3D2 port address decodes to 3D4
	3D3 port address decodes to 3D5
	3D4 6845 index register, selects which register [0-11h]
	    is to be accessed through port 3D5
	3D5 6845 data register [0-11h] selected by port 3D4,
	    registers 0C-0F may be read.  If a read occurs without
	    the adapter installed, FFh is returned.  (see 6845)
	3D6 port address decodes to 3D4
	3D7 port address decodes to 3D5
	3D8 6845 Mode control register (CGA, EGA, VGA, except PCjr)
	3D9 color select palette register (CGA, EGA, VGA, see 6845)
	3DA status register (read only, see 6845, PCjr VGA access)
	3DB Clear light pen latch (any write)
	3DC Preset Light pen latch
	3DF CRT/CPU page register (PCjr only)
*/ 
 
/*
MDA	B0000 to B0FFF	3B0 to 3BB
CGA	B8000 to BBFFF	3D0 to 3DF
Mono text	B0000 to B7FFF	3B0 to 3BB, 3C0 to 3CF
Mono graphics	A0000 to AFFFF	3B0 to 3BB, 3C0 to 3CF
Colour text	B8000 to BFFFF	3C0 to 3CF, 3D0 to 3DF
Colour graphics	A0000 to AFFFF or B8000 to BFFFF	3C0 to 3CF, 3D0 to 3DF
 */


/*
 #test: The format of the structure we got with vesa bios.
 bl needs to give us this pointer.
//vbeInfoBlock structure has the following format.
typedef struct _vbeInfoBlock {
        uint8_t  signature[4];     // “VESA”
        uint16_t version;          // Either 0x0200 (VBE 2.0) or 0x0300 (VBE 3.0)
        uint32_t oemString;        // Far pointer to OEM name
        uint8_t  capabilities[4];  // capabilities
        uint32_t videoModesPtr;    // Far pointer to video mode list
        uint16_t totalMemory;      // Memory size in 64K blocks
        uint16_t oemSoftwareRev;
        uint32_t oemVenderNamePtr;
        uint32_t oemProductNamePtr;
        uint32_t oemProductRevPtr;
        uint8_t  reserved [222];
        uint8_t  oemData [256];
}vbeInfoBlock;
*/

#define COLUMNS  80
#define ROWS     25
#define SCREEN_WIDTH   COLUMNS
#define SCREEN_HEIGHT  ROWS
#define SCREEN_DEFAULT_LEFT  0
#define SCREEN_DEFAULT_TOP   0


extern int refresh_device_screen_flag;
extern int refresh_valid_screen_flag;


extern unsigned long screen_size_in_kb;
// hack: for debuging purpose
extern int g_use_fake_screen_size;
extern unsigned long fake_screen_size_in_kb;

// The refresh is enabled we mapped
// memory enough for that.
extern int refresh_screen_enabled;
// We need to flush the whole screen into the frontbuffer.
extern int screen_is_dirty;

extern unsigned long gSavedLFB;
extern unsigned long gSavedX;
extern unsigned long gSavedY;
extern unsigned long gSavedBPP;


// The info comes from the gramado boot loader.
extern struct display_device_d *bl_display_device;

// ----------------------

void bldisp_flush(unsigned long flags);
void bldisp_vsync(void);

int 
bldisp_putpixel0 ( 
    unsigned int  _color,
    unsigned long _x, 
    unsigned long _y, 
    unsigned long _rop_flags,
    unsigned long buffer_va );


// The whole screen is dirty.
// It can be flushed into the framebuffer.
void invalidate_screen(void);
void screenSetSize( unsigned long width, unsigned long height );
unsigned long screenGetWidth (void);
unsigned long screenGetHeight (void);
unsigned long screenGetBPP (void);
unsigned long screenGetPitch (void);
unsigned long screen_scanline_va( int scanline );
int screenInit (void);

unsigned long videoGetMode (void);
void videoSetMode (unsigned long mode);
void videoSetupCGAStartAddress (unsigned long address);

// #todo ioctl
int 
bldisp_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg );

int Video_initialize(void);

// Device driver initialization
int DDINIT_bldisp(void);

#endif    


