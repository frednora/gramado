// clone.h
// Support for clone.c
// Created by Fred Nora.

#ifndef __INTAKE_CLONE_H
#define __INTAKE_CLONE_H    1


extern int copy_process_in_progress;


// ==============================================

int 
copy_process_struct(
    struct process_d *p1,
    struct process_d *p2 );

pid_t 
copy_process( 
    const char *filename, 
    pid_t pid, 
    unsigned long clone_flags );

#endif   

