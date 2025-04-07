// dispsrv.c
// This file support the current display server,
// that is a loadable program. (ds00.bin)

// #important
// The input system can send system messages 
// direct to this component.

// dispsrv.c
// Handle the usermode display server initialization.
// Created by Fred Nora.

// #bugbug
// Remember: We already have a device drivers for display device
// in dev/.

// Who is the active display server?

#include <kernel.h>

//see: dispsrv.h
struct ds_info_d  DisplayServerInfo;

// ==================================================


int 
ds_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg )
{    
    debug_print("ds_ioctl: [TODO]\n");
    return -1;
}

int ds_init(void)
{
    //debug_print ("ds_init:\n");
    return -1;
}

//
// End
//

