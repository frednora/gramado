// qemu.c
// Shutdown the machine when we're running on qemu hv.

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

#define QEMU_MAGIC_PORT  0x0604
#define QEMU_SHUTDOWN_COMMAND  0x2000 


static void __save_fat_cache(void);
static void __poweroff(void);

// =================================

// Save FAT cache into the real disk.
static void __save_fat_cache(void)
{
    sc82( 10008, 0, 0, 0);
}

static void __poweroff(void)
{
    libio_outport16(
        (unsigned short) QEMU_MAGIC_PORT, 
        (unsigned short) QEMU_SHUTDOWN_COMMAND );
}


// do_via_qemu:
// Nesse momento temos que gravar 
// o cache da FAT no disco físico
// antes de desligarmos.
// Senão teremos problemas para ler os novos arquivos depois
// que religarmos.
void do_via_qemu(int verbose)
{
    debug_print("SHUTDOWN.BIN: [QEMU] Shutting down\n");

// Save FAT cache into the disk.
    if (verbose == TRUE){
        printf("Saving fat...\n");
    }
    __save_fat_cache();

// Power off using the qemu infra-structure.
    if (verbose == TRUE){
        printf("Poweroff via qemu\n");
    }
    __poweroff();
}

