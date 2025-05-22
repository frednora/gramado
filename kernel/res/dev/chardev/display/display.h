// display.h
// Main header for the display manager.
// Created by Fred Nora.

#ifndef __DISPLAY_DISPLAY_H
#define __DISPLAY_DISPLAY_H    1


// #test
// We can use only an area of the screen.
// Or multiple areas.
struct virtual_screen_d
{
// Relative to the actual screen.
    unsigned long left;
    unsigned long top;
    unsigned long width;
    unsigned long height;
};

struct display_device_d
{
    int used;
    int magic;

// PID of the owner.
// It means that only this process is able to 
// call routines affecting this structure.
    pid_t owner_pid;

// The file to handle this device.
    file *_file;
    //char name[64];

// Structure initialization.
    int initialized;

// LFB - Linear Frame Buffer
    unsigned long framebuffer_pa;
    unsigned long framebuffer_va;
    //unsigned long framebuffer_end_pa;
    //unsigned long framebuffer_end_va;

// End of installed memory
    //unsigned long memory_end_pa;
    //unsigned long memory_end_va;

// #bugbug
// Actually it is about the visible area,
// But we also need the info about all the available memory
// inside the display device.

    unsigned long framebuffer_width;
    unsigned long framebuffer_height;
    unsigned long framebuffer_bpp;

// How many bytes in a single row.
    unsigned long framebuffer_pitch;

// Screen size in bytes.
    unsigned long framebuffer_size_in_bytes;
// Screen size in KB.
    unsigned long framebuffer_size_in_kb;

    //unsigned long screen_rows;
    //unsigned long buffer_rows;
    
    //int mode;
    //unsigned long flags;
    
    //struct tty_d *console_tty
    // FILE *fp;

    //...   

// #test
// We can use only an area of the screen.
// Or multiple areas.
    struct virtual_screen_d virtual_screen;

    struct display_device_d *next;
};

//
// Graphic mode support.
//
 
struct tagGraphics
{
    unsigned long LFB;
    //unsigned long Backbuffer;
    //...
};
struct tagGraphics *Graphics;

//
// Video support
//

/*
 * video_d:
 *     Estrutura para informações sobre parâmetros de vídeo.
 */
struct video_d
{
    object_type_t  objectType;
    object_class_t objectClass;

	//call back

    int useGui;   //se estamos ou não usando gui. text mode, video mode.
    int vgaMode;  //número do modo de vídeo se estiver usando vga.
    int vesaMode; //número do modo vesa.
    //...

	//unsigned long flag;  //
	//unsigned long error; //

    // currente video memory. 
    // Ponteiro para a base atual da memoria de video em text mode.
    unsigned long currentVM; 
    char ch;         // The char
    char chAtrib;    // The attributes 
    // ...

    //unsigned long Display[32];    //Ponteiros para as telas.
};
struct video_d  VideoBlock;

//
// == prototypes =================================
//

void bldisp_show_info(void);


int displayInitialize(void);

#endif    


