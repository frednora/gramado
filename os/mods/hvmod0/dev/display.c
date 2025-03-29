// display.c
// Display support for ring0 lodable module.
// Created by Fred Nora.

#include <kernel.h>

// See:
struct display_info_d  DisplayInfo;


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
    if (ModuleInitialization.initialized != TRUE){
        newm0_initialize();
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





