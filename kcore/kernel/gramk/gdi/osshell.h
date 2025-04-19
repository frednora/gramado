// osshell.h
// This file support the current OS Shell,
// that is a loadable program. (taskbar.bin)
// #todo:
// This is the OS Shell, 
// resposable for the icons on desktop,
// the taskbar, the start menu, etc.

// #important
// #todo: 
// The input system can send system messages 
// direct to this component.

// ===============================================================
// This structure handles the osshell registration.

#ifndef __USER_OSSHELL_H
#define __USER_OSSHELL_H    1

struct osshell_info_d
{
    int initialized;

// Process info
    pid_t pid;
    int pid_personality;

// Thread indo
    tid_t tid;
};
// see: osshell.c
extern struct osshell_info_d OSShell_Info;

#endif   

