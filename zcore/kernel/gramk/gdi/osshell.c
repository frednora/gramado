// osshell.c
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

// Who is the active os shell?

#include <kernel.h>


// see: osshell.h
struct osshell_info_d OSShell_Info;

