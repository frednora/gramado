
// bootinfo.h

// Display device support.
// Imported from bl64.
// Change to gSavedLFB ...
// See: screen.c

#ifndef __SYSTEM_BOOTINFO_H
#define __SYSTEM_BOOTINFO_H    1

// Display device support.
// It came from the boot loader.
// See: bldisp.c
extern unsigned long gSavedLFB;
extern unsigned long gSavedX;
extern unsigned long gSavedY;
extern unsigned long gSavedBPP;

#endif   

