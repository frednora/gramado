// display.h
// Created by Fred Nora.

#ifndef __DISPLAY_H
#define __DISPLAY_H    1



struct display_info_d
{
    int initialized;

    // Initialization phase 1.
    int phase1;
    unsigned long frontbuffer_address;
    unsigned long backbuffer_address;

    // Initialization phase 2.
    int phase2;
    unsigned long width;
    unsigned long height;
    unsigned long bpp;   // bits per pixel.

    // ...
};
extern struct display_info_d  DisplayInfo;


//
// ================================================
//

// Step1
unsigned long 
display_initialization_phase1(
    unsigned long param1,
    unsigned long param2,
    unsigned long param3,
    unsigned long param4 );

// Step2
unsigned long 
display_initialization_phase2(
    unsigned long param1,
    unsigned long param2,
    unsigned long param3,
    unsigned long param4 );

#endif   


