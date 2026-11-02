// globals.h
// Global definitions for the boot loader.
// #ps Put this in the top of the includes.
// 2016 - Created by Fred Nora.

#ifndef __GLOBALS_H
#define __GLOBALS_H    1

// Do we need to initialize the DE/.
extern int initialize_de;


// Variables for LFB Address support. 
// See: globals.c
extern unsigned long g_lbf_pa;    // LFB physical address.
extern unsigned long g_lbf_va;    // LFB virtual address.

// Variables for Disk signature support.
// See: globals.c
extern unsigned long g_DiskSignature;  // Base address for the table.
extern unsigned long *g_disk_sig1;       // Signature 1
extern unsigned long *g_disk_sig2;       // Signature 2

// #todo #test
struct ata_boot_disk_info_d
{
    // Current IDE port index (0-3, see ata_port[4])
    int port;

    // Current selected channel (0=Primary, 1=Secondary) and device (0=Master, 1=Slave)
    int channel;  // 0: Primary, 1: Secondary
    int device;   // 0: Master,  1: Slave
};
extern struct ata_boot_disk_info_d  ata_boot_disk_info;


//
// The boot block structure.
//

// #todo
// It needs to be the same shape of the bootblock structure
// found in the boot manager (BM.BIN), in bootloader.inc.

struct boot_block_d
{
// 32bit mode.
    unsigned long lfb;
    unsigned long x;
    unsigned long y;
    unsigned long bpp;
    unsigned long last_valid_address;
    unsigned long metafile_address;
    unsigned long disk_number;
    unsigned long heads; 
    unsigned long spt; 
    unsigned long cylinders; 
    unsigned long boot_mode; 
    unsigned long gramado_mode;
    //...
};

extern struct boot_block_d  BootBlock;


//
// =====================================================
//


// main flags.
extern int gdefLegacyBIOSBoot;
extern int gdefEFIBoot;
extern int gdefSafeBoot;
extern int gdefShowLogo;
extern int gdefShowProgressBar;
//...


// + Objects.
// + Global structs.
// ...


// Essa flag ser� lida pela rotina de falta de p�gina.
// para ignorar e n�o parar o sistema.
//int ____testing_memory_size_flag;
extern int ____testing_memory_size_flag;

//salvando o �lltimo endere�o v�lido
//unsigned long __last_valid_address; 
extern unsigned long __last_valid_address;

#endif   

//
// End
//

