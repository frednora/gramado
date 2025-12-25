// init.h
// Header for init process.
// Created by Fred Nora.

#ifndef __INIT_H
#define __INIT_H    1

#include <types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <rtl/gramado.h>

#include "config.h"
#include "endpoint.h"

// Internal library (Creating a template)
#include "../inittask/inittask.h"


#define EnvironmentWinuCore   4000
#define EnvironmentWinuHeavy  4001

/*
Standard Linux Runlevels (0-6)
0 (Halt): Shuts down the system.
1 (Single-User Mode): Minimal services, for maintenance or recovery.
2 (Multi-User, No Network): Multi-user mode, but without network services.
3 (Full Multi-User): Standard command-line interface (CLI) mode with networking.
4 (Custom): Unused by default, can be defined by the user.
5 (Graphical): Similar to runlevel 3 but with a graphical user interface (GUI).
6 (Reboot): Reboots the system.
*/

#define __RUNLEVEL_HALT  0
#define __RUNLEVEL_SINGLE_USER  1
#define __RUNLEVEL_MULTI_USER  2
#define __RUNLEVEL_FULL_MULT_USER  3
#define __RUNLEVEL_CUSTOM  4
#define __RUNLEVEL_GRAPHICAL  5
#define __RUNLEVEL_REBOOT  6

struct init_d
{
    int initialized;  // Structure validation
    pid_t pid;
    tid_t tid;

// Save the current runlevel
// syscall: 288
    int __runlevel;

// Options:
// + 1001 - cli (embedded shell)
// + 1002 - shell direct with the kernel console
// + 1003 - operate as a server with system messages
// + 1004 - launch display server
    int __selected_option;

// Winu Core: ds00
// Winu Heavy: demo00 and demo01
    int environment;

    int argc;         // Save argc from main

    int is_headless;  // Headless mode

// It's necessary to nnlock some kernel features like 
// taskswitching and scheduling
    int taskswitching_unlocked;
    int scheduler_unlocked;
};
extern struct init_d  Init;


#endif    

