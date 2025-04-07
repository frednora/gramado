// dispsrv.h
// This file support the current display server,
// that is a loadable program. (ds00.bin)

// #important
// The input system can send system messages 
// direct to this component.


#ifndef __USER_DISPSRV_H
#define __USER_DISPSRV_H    1


//
// == window server ==================================================
//

// ===============================================================
// This structure handles the display server registration.
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

int 
ds_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg );

int ds_init (void);


#endif  

