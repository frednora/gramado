
// 'shutdown' command for Gramado.
// It shuts down the machine via i/o ports
// if we are running on qemu.
// Environment: ring 3 application.
// Created by Fred Nora.

// + It only works on qemu. (TCG or KVM).
// + It works on qemu with TCG.
// + It works on qemu with KVM.
//   But we still can't realise when KVM is running on qemu.
//   It's because is also used by virtualbox.
//   And the shutdown routine only works on qemu.

//#include <sys/types.h>
#include <rtl/gramado.h>
#include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libio.h>

//#test
#include "qemu.h"

// #bugbug
// Actually rtl_is_qemu() is probing for TCG only.
// The shutdown routine also work on qemu with kvm.
#define is_tcg rtl_is_qemu

static int shutdown_verbose = FALSE;

//
// private functions: prototypes ==============
//

//static void __serial_write_char(unsigned char data);


// ===================================

/*
// Vai escrever em uma porta ja inicializada pelo kernel.
static void __serial_write_char (unsigned char data) 
{
    while (( libio_inport8(0x3F8 + 5) & 0x20 ) == 0)
    {
    };

    libio_outport8 ( 0x3F8, (unsigned char) data );
}
*/

// main:
// #test
// Testing shutdown in virtual machines.
// #todo:
// We can ask the system if we are in qemu or not.
// See: https://wiki.osdev.org/Shutdown
// #todo: parameters.
// Or maybe we need a application called poweroff.bin.
// Na verdade essa rotina precisa ser em ring0.
// Pois tem que checar a permissão de superuser,
// acionar os locks, sincronizar os sistemas de arquivo
// montados, etc ...
// #todo
// Podemos testar para outros hv, como kvm ...
int main(int argc, char *argv[])
{
    static int isTCG = FALSE;
    // ...

    register int i=0;
    int fSilent = FALSE;

    for (i=1; i<argc; i++)
    {
        if ( strncmp(argv[i],"--silent",8) == 0 )
            fSilent=TRUE;
        //if ( strncmp(argv[i],"--silent",8) == 0 )
            //fSilent=TRUE;
    };

//
// Flags
//

    shutdown_verbose = TRUE;
    if (fSilent == TRUE)
    {
        shutdown_verbose = FALSE;
        
        //#debug
        //printf("verbose\n");
        //exit(0);
    }

// ==============================
// qemu with tcg.
// + It only works on qemu. (TCG or KVM).
// + It works on qemu with TCG.
// + It works on qemu with KVM.
//   But we still can't realise when KVM is running on qemu.
//   It's because is also used by virtualbox.
//   And the shutdown routine only works on qemu.
    //isTCG = (int) is_tcg();
    isTCG = (int) rtl_get_system_metrics(300); // Is it running on tcg?
    if (isTCG == TRUE){
        do_via_qemu(shutdown_verbose);
    }

// =========================
// virtualbox
// In Virtualbox, you can do shutdown with:
    //if (isVirtualBox == TRUE){
    //    libio_outport16(0x4004, 0x3400);
    //}

    // bochs
    // In Bochs, and older versions of QEMU(than 2.0), you can do the following:
    //if (isBochs == TRUE){
    //    libio_outport16(0xB004, 0x2000);
    //}

fail:
    printf("shutdown.bin: Failed\n");
    if (isTCG != TRUE){
        printf("Not running on qemu with TCG\n");
    }
    return EXIT_FAILURE;
}

