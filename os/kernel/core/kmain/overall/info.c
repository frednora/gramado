
// info.h

#include <kernel.h>

// ================================================
// The data comes from boot block and beyond.

// #test
// Lets use the info that omes from the BootBlock structure.
// This structure was initialized for kmain() in main.c

unsigned long info_get_boot_info(int index)
{

    if (index<0){
        return 0;
    }


// #bugbug
// If we use the BootBlock structure, maybe the info
// will be accessible only using the sci2. int 0x82.
// Is it initialized?

// #bugbug
// This structure is not initialized yet.
// We are using xBootBlock in init.c

    //#todo
    if (xBootBlock.initialized != TRUE){
        panic("info.c: BootBlock.initialized #todo\n");
    }

    switch (index){
    case 1:
       //return (unsigned long) xBootBlock.last_valid_address;
       break;
    case 2:
       //return (unsigned long) xBootBlock.metafile_address;
       break;
    case 3:
       //return (unsigned long) xBootBlock.disk_number;
       break;
    case 4:
       //return (unsigned long) xBootBlock.heads;
       break;
    case 5:
       //return (unsigned long) xBootBlock.spt;
       break;
    case 6:
       //return (unsigned long) xBootBlock.cylinders;
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


