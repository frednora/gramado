// connect.c
// Registering the gns server.
// Created by Fred Nora.

#include <gnsint.h>

// Is it registered?
static int __ns_registerd = FALSE;
// Let's store a ring0 pointer right here. hahaha.
static void *__ns_cgroup;
// Our PID.
static pid_t __ns_pid=0;

// ==========================================

// Getting the current cgroup and 
// register network server as the current server for this cgroup.
// OUT:
// 0   = Ok   
// < 0 =  fail.
int register_ns(void)
{
    __ns_registerd = FALSE;

// cgroup
    __ns_cgroup = (void *) gramado_system_call(519,0,0,0);
    if ((void *) __ns_cgroup == NULL){
        debug_print ("register_ns: __ns_cgroup fail\n");
        goto fail;
    }

// PID
    __ns_pid = (int) getpid();
    if (__ns_pid < 0){
        debug_print ("register_ns: __ns_pid fail \n");
        goto fail;
    }

// Register this pid as ns.
    gramado_system_call ( 
        (unsigned long) 521, 
        (unsigned long) __ns_cgroup, 
        (unsigned long) (__ns_pid & 0xFFFFFFFF), 
        (unsigned long) (__ns_pid & 0xFFFFFFFF) );

    __ns_registerd = TRUE;
// O = OK.
    return 0;
fail:
    return (int) -1;
}

//
// End
//





