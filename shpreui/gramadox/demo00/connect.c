// connect.c
// Register the window server in the system.
// Created by Fred Nora.

#include "gram3d.h"

// Flag: The display server is registered.
static int __ds_registered = FALSE;
// This is a pointer for the current cgroup.
// It's a ring0 address. hahaha.
static void * __p_cgroup;
// Our PID.
static pid_t __pid=0;

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

    // Desktop
    // Getting current desktop;

    // Register.
    // Register window server as the current server for this
    // desktop.

//
// == cgroup ==================
//

    // cgroup 
    // Getting pointer for the current cgroup.
    __p_cgroup = (void *) gramado_system_call (519,0,0,0);
    if ((void *) __p_cgroup == NULL)
    {
        // #debug
        gwssrv_debug_print ("registerDS: [FAIL] __p_cgroup\n");
        printf             ("registerDS: [FAIL] __p_cgroup\n");
        exit(1);
        
        return (int) (-1);
    }

//
// == Register =====================
//

    // PID
    // Get the PID of the server.
    __pid = (int) getpid();
    if ( __pid < 0 )
    {
        // #debug
        gwssrv_debug_print ("registerDS: [FAIL] __pid\n");
        printf             ("registerDS: [FAIL] __pid\n");
        exit(1);

        return (int) (-1);
    }

// Register this PID of the display server into the cgroup structure.
// #todo
// #bugbug
// We need to check the return value.
// int Status = -1;

    gramado_system_call ( 
        513, 
        __p_cgroup, 
        __pid, 
        __pid );

    // flag.
    __ds_registered = TRUE;

    // O = OK.
    return 0;
}


//
// End.
//





