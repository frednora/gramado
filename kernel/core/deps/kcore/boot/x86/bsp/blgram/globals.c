// global.c
// Created by Fred Nora.

#include "bl.h"  


// Variables for LFB Address support. 
unsigned long g_lbf_pa=0;    // LFB physical address.
unsigned long g_lbf_va=0;    // LFB virtual address.

// Variables for Disk signature support.
unsigned long g_DiskSignature=0;  // Base address for the table.
unsigned long *g_disk_sig1;       // Signature 1
unsigned long *g_disk_sig2;       // Signature 2

// Saving boot disk info
struct ata_boot_disk_info_d  ata_boot_disk_info;

// Do we need to initialize the DE/.
int initialize_de = FALSE;




