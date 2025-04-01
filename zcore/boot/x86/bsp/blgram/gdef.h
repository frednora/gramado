// gdef.h
// Global definitions for the boot loader.
// #ps Put this in the top of the includes.
// 2016 - Created by Fred Nora.

#ifndef __GDEF_H
#define __GDEF_H    1

// Do we need to initialize the DE/.
extern int initialize_de;


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

