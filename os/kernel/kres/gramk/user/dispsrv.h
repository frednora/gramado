// dispsrv.h

#ifndef __USER_DISPSRV_H
#define __USER_DISPSRV_H    1


//
// == window server ==================================================
//

// ===============================================================

struct ds_info_d
{
    int initialized;

// Process info
    pid_t pid;
    int pid_personality;

// Thread indo
    tid_t tid;
};
// see: graphics.c
extern struct ds_info_d  DisplayServerInfo;

// ===============================================================


// #todo:
// Change this term to display server.
// types of window server.
typedef enum {
    WindowServerTypeNull,
    WindowServerTypeEmbedded,       // Embedded inside the base kernel.
    WindowServerTypeRing0Process,
    WindowServerTypeRing3Process
    // ...
    // #todo
    //DisplayServerTypeNull,
    //DisplayServerTypeEmbedded,       // Embedded inside the base kernel.
    //DisplayServerTypeRing0Process,
    //DisplayServerTypeRing3Process
    // ...
} window_server_t;


extern int WindowServer_initialized;
    
extern window_server_t WindowServer_type;  // tipo de window server.
    
// the pid for loadable window servers.
// When the window server is the embedded so this pid needs to be
// the pid of the kernel process.
   
extern pid_t WindowServer_pid; 

// Limiting the nale to 64.
extern char WindowServer_name[64];
   
// the virtual console used by this window server.
extern int WindowServer_virtual_console;

//==========

void ws_show_info(void);

int 
ws_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg );


int ws_init (void);


#endif  

