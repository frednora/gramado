// dispsrv.h
// This file support the current display server,
// that is a loadable program. (ds00.bin)

// #important
// The input system can send system messages 
// direct to this component.


#ifndef __GDI_DISPSRV_H
#define __GDI_DISPSRV_H    1


//
// == display server ==================================================
//

// This structure handles the display server registration.
// The default display server is ds00.bin for now.
// See: net.c
struct ds_info_d
{
    int initialized;

// Process info
    pid_t pid;
    int pid_personality;

// Thread indo
    tid_t tid;
};
// see: dispsrv.c
extern struct ds_info_d  DisplayServerInfo;

// ===============================================================

int dispsrv_setup_ds_info(pid_t pid);

int 
ds_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg );

int ds_init (void);


#endif  

