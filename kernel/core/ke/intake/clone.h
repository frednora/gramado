// clone.h
// Support for clone.c
// Created by Fred Nora.

#ifndef __INTAKE_CLONE_H
#define __INTAKE_CLONE_H    1


extern int copy_process_in_progress;

// #todo
// The flags used in copy_process() worker.
#define F_CLONE_GRAMADO_STYLE  0x0000  // rtl_clone_and_execute()
#define F_CLONE_UNIX_STYLE     0x0010  // fork()
// ...

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

