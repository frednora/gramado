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

#ifndef __GDI_OSSHELL_H
#define __GDI_OSSHELL_H    1

// This structure handles the os shell registration.
// The default os shell for now is the Overview application, called taskbar.bin.
// #todo: We gotta implement the methods for the registration based on the 
// methods used for the display server found in net.c
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
extern struct osshell_info_d  OSShell_Info;

#endif   

