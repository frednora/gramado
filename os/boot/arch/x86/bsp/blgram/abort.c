// abort.c
// Abort the boot loader.
// 2015 - Created by Fred Nora.

#include "bl.h"  
#include "libs/stdio.h"

extern void refresh_screen(); 

void abort(void)
{
// Final message
    printf("BL.BIN: [ABORT] System halted\n");
    refresh_screen();
// Hang
    while (1){
        asm ("cli");
        asm ("hlt");
    };
}

//
// End
//
