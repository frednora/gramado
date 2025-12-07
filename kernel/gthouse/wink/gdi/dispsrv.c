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

// Setup DisplayServerInfo global structure.
// See: dispsrv.h
int dispsrv_setup_ds_info(pid_t pid)
{
    struct te_d *p;
    struct thread_d *t;
    pid_t current_process = -1;

    //debug_print ("__initialize_ds_info:\n");

// Maybe we can just emit an error message and return.
    if (DisplayServerInfo.initialized == TRUE)
    {
        // #debug
        printk("dispsrv_setup_ds_info: Another display server is on\n");
        goto fail;
    }
    DisplayServerInfo.initialized = FALSE;

// -----------------
// PID: The process pointer.

    if (pid < 0 || pid >= PROCESS_COUNT_MAX){
        goto fail;
    }
    current_process = (pid_t) get_current_process();
    if (pid != current_process)
    {
        // #debug
        printk("dispsrv_setup_ds_info: pid != current_process\n");
        goto fail;
    }
    p = (struct te_d *) teList[pid];
    if ((void*) p == NULL){
        goto fail;
    }
    if (p->magic != 1234){
        goto fail;
    }
    DisplayServerInfo.pid = (pid_t) pid;

// -----------------
// TID: The flower thread

    t = (struct thread_d *) p->flower;
    if ((void*) t == NULL){
        goto fail;
    }
    if (t->magic != 1234){
        goto fail;
    }
    DisplayServerInfo.tid = (tid_t) t->tid;

// ----------------
// Process Personality
    p->personality = (int) PERSONALITY_GUI;
    DisplayServerInfo.pid_personality = (int) PERSONALITY_GUI;

// ----------------
// Security Access token

    // users
    // ...

    // group of users.
    p->token.gid  = (gid_t) GID_DEFAULT;
    p->token.rgid = (gid_t) GID_DEFAULT;  // real
    p->token.egid = (gid_t) GID_DEFAULT;  // effective
    p->token.sgid = (gid_t) GID_DEFAULT;  // saved

    DisplayServerInfo.initialized = TRUE;
// OK
    return 0;
fail:
    return (int) -1;
}

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

