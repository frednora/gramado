// gramldr.c
// Created by Fred Nora.

#include "../../bl.h"


//
// GRAMADO/ (Normal system)
//

// Load this one
const char *image_pathname = "/GRAMADO/KERNEL.BIN";
// or try this one. (BACKUP)
const char *image_default_pathname = "/GRAMADO/KRNL.BIN";

//
// DE/ (Desktop Environment)
//

// Load this one
const char *image_pathname_de = "/DE/KERNEL.BIN";
// or try this one. (BACKUP)
const char *image_default_pathname_de = "/DE/KRNL.BIN";



// Kernel image
static int __bl_load_kernel_image(void);
static int __bl_load_kernel_image_for_de(void);


// =============================================================

//
// $
// LOAD KERNEL IMAGE
//

/*
 * __bl_load_kernel_image: 
 *     It loads the kernel image at 0x00100000.
 *     The entry point is at 0x00101000.
 */ 
// #todo
// This way can chose the filename from a
// configuration file.
// This routine will try to load the default filename
// if the provide name fail.
// This routine will build the pathname
// to search in the default folder.
static int __bl_load_kernel_image(void)
{
// Called by bl_main().

    int Status = -1;

// #bugbug
// Precisamos que essa rotina retorne
// para termos a change de inicializarmos o
// rescue shell. Mas acontece que por enquanto
// essa função aborta ao primeiro sinal de perigo.
// See: loader.c

    Status = (int) elfLoadKernelImage(image_pathname,image_default_pathname);
    if (Status < 0){
        printf ("__bl_load_kernel_image: elfLoadKernelImage fail\n");
        goto fail;
    }

    // OK
    return (int) Status;

fail:
    refresh_screen();
    return (int) (-1);
}

/*
 * __bl_load_kernel_image_for_de: 
 *     It loads the kernel image at 0x00100000.
 *     The entry point is at 0x00101000.
 */ 
// #todo
// This way can chose the filename from a
// configuration file.
// This routine will try to load the default filename
// if the provide name fail.
// This routine will build the pathname
// to search in the default folder.
static int __bl_load_kernel_image_for_de(void)
{
// Called by bl_main().

    int Status = -1;

// #bugbug
// Precisamos que essa rotina retorne
// para termos a change de inicializarmos o
// rescue shell. Mas acontece que por enquanto
// essa função aborta ao primeiro sinal de perigo.
// See: gramldr.c.c

    Status = 
        (int) elfLoadKernelImage (
                image_pathname_de, 
                image_default_pathname_de );


// fail?
    if (Status < 0){
        printf ("__bl_load_kernel_image_for_de: on elfLoadKernelImage()\n");
        goto fail;
    }

    return (int) Status;  // OK

fail:
    refresh_screen();
    return (int) (-1);
}


// Load kernel image from Gramado OS.
int gramado_load_kernel_image(void)
{
    int Status = -1;

// Initialize the de kernel environment.
// #todo:
// At this moment we need to update the bootblock
// to tell the kernel that we're loading him from a new perpective.
    if (initialize_de == TRUE){
        printf("bl_main: Initializing de kernel\n");
        refresh_screen();
        Status = __bl_load_kernel_image_for_de();

// Initialize the normal kernel environment.
    } else {
        Status = __bl_load_kernel_image();
    }

    return (int) Status;
}

