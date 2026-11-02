// fsbp.h
// File System Boot Partition.
// Created by Fred Nora.

#ifndef __FS_FSBP_H
#define __FS_FSBP_H    1

// Canonical:
// The list of the main directories in the system.
// These are the first directories searched when
// we're looking for a file.
// see: fs.c
extern struct system_directory_d  sdROOT;      // '/'
extern struct system_directory_d  sdGRAMADO;   // '/GRAMADO/'
extern struct system_directory_d  sdDE;        // '/DE/'

extern struct system_fat_d  bpFAT;


// ================================

// For boot partition
void fsbp_initialize_fat(void);
// For boot partition
int fsbp_initialize_bp_directories(void);


#endif   

