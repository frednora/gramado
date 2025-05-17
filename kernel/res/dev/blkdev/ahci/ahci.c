// ahci.c
// Creted by Fred Nora.

#include <kernel.h>


static int __ahci_initialize(void);


// ==============================================


/*
// #todo ioctl
int 
ahci_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg );
int 
ahci_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg )
{
    debug_print("ahci_ioctl: #todo\n");
    if(fd<0){
        return -1;
    }
    return -1;
}
*/

static int __ahci_initialize(void)
{
    return -1;
}

//
// #
// INITIALIZATION
//

int DDINIT_ahci (void)
{
// Probably the kernel already have a list of found 
// PCI devices at this time, and we're gonna use this list.

    // __ahci_initialize();
    return -1;
}



