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

#include "endpoint.h"

// Internal library (Creating a template)
#include "../inittask/inittask.h"


struct init_d
{
// This structure is initialized.
    int initialized;
// Save the argc.
    int argc;
// Save the runlevel value.
    int runlevel;
// Running in headless mode.
    int is_headless;
// Save the pid.
    pid_t pid;
// Unlock kernel features.
    int taskswitching_unlocked;
    int scheduler_unlocked;

};
extern struct init_d  Init;


#endif    

