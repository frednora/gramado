// connect.c
// Register the window server in the system.
// Created by Fred Nora.

#include "../../gwsint.h"

// Flag:
// The display server is alread registered.
static int __ds_registered = FALSE;
// The pointer for the current cgroup structure.
// #ps: ring 0 address.
static void *__p_cgroup;  // __ws_desktop
// Our PID.
static pid_t __pid=0;

// =============================================

/*
 * registerDS:
 *     This routine is gonna register this window server.
 *     It's gonna change the kernel input mode.
 *     Service 519: Get the current desktop structure pointer.
 *     Service 513: Register this window server.
 */
// OUT:
// 0   = Ok   
// < 0 =  fail.
int registerDS(void)
{
// Valid only for Gramado OS.

// cgroup
// Getting pointer for the current cgroup, ring0 address (hahaha). 
// Register the display server 
// as the current display server for this cgroup.

    __p_cgroup = (void *) gramado_system_call (519,0,0,0);
    if ((void *) __p_cgroup == NULL)
    {
        // #debug
        //server_debug_print ("registerDS: __p_cgroup\n");
        printf             ("registerDS: __p_cgroup\n");
        exit(1);
        goto fail;
    }

// =====================
// register

// PID
// Get the PID of the server.
    __pid = (int) getpid();
    if (__pid < 0)
    {
        // #debug
        //server_debug_print ("registerDS: __pid\n");
        printf             ("registerDS: __pid\n");
        exit(1);
        goto fail;
    }

// Register this PID of the current window server.
// #todo
// #bugbug
// We need to check the return value.
// int Status = -1;

    gramado_system_call ( 
        513, 
        __p_cgroup, 
        __pid, 
        __pid );

    // Flag: Now the display server is registered.
    __ds_registered = TRUE;
    Initialization.register_ds_checkpoint = TRUE;

// 0 = OK.
    return 0;

fail:
    return (int) -1;
}

//
// End
//





