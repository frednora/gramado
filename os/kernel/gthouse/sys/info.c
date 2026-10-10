// info.h
// Created by Fred Nora.

#include <kernel.h>

// ================================================
// The data comes from boot block and beyond.

// #test
// Lets use the info that omes from the BootBlock structure.
// This structure was initialized for kmain() in main.c
unsigned long info_get_boot_info(int index)
{
    if (index < 0){
        return 0;
    }

// #bugbug
// If we use the BootBlock structure, maybe the info
// will be accessible only using the sci2. int 0x82.
// Is it initialized?

// #bugbug
// This structure is not initialized yet.
// We are using bootblk in init.c

    //#todo
    if (bootblk.initialized != TRUE){
        panic("info.c: bootblk.initialized #todo\n");
    }

    switch (index){
    case 1:
       //return (unsigned long) bootblk.last_valid_address;
       break;
    case 2:
       //return (unsigned long) bootblk.metafile_address;
       break;
    case 3:
       //return (unsigned long) bootblk.disk_number;
       break;
    case 4:
       //return (unsigned long) bootblk.heads;
       break;
    case 5:
       //return (unsigned long) bootblk.spt;
       break;
    case 6:
       //return (unsigned long) bootblk.cylinders;
       break;
    case 7:
       //return (unsigned long) 
       break;
    case 9:
       //return (unsigned long) 
       break;
    };

//fail 
    return 0;
}

