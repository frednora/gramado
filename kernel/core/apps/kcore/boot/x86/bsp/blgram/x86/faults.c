// faults.c
// Faults and exceptions for the bootloader.
// 2015 - Created by Fred Nora.

#include "../bl.h"


void faultsShowRegisters();

/*
 * cpu_faults:
 *     Tratamento de faults do Boot Loader.
 *     Trata a fault de acordo com o numero.
 * Obs: Isso pertence ao Boot Loader. Nao ha threads para serem fechadas.
 * @todo: Exibir as falhas de inicializacao.
 * 2015 - Created.
 */
void cpu_faults(unsigned long fault_number)
{
    g_cursor_x = 0;
    g_cursor_y = 0;

    printf("faults-cpu_faults: %d\n", fault_number );

// If we were testing the memory during the fault.
// See: gdef.h
    if (____testing_memory_size_flag == 1)
    {
        // Final message
        printf("cpu_faults: Testing memory size\n");
        printf("cpu_faults: Out of range with exception %d\n", fault_number );
        printf("cpu_faults: address = %x\n", __last_valid_address);
        refresh_screen();

        // Hang the system
        while (1){
            asm ("cli");
            asm ("hlt");
        };
    }

// Mostra o erro de acordo com o numero.
    switch (fault_number)
    {
        // EXCEPTION
        case 1:
        case 3:
            printf("EXCEPTION \n");
            break;

        // DOUBLE FAULT
        case 8:
            printf("DOUBLE FAULT \n");
            break;

        // STACK
        case 12:
            printf("STACK FAULT \n");
            break;

        // GP
        case 13:
            printf("GP \n");
            break;

        // PAGE FAULT
        case 14:
            printf("PAGE FAULT \n");
            break;

        default:
            printf("Default number\n");
            break;
    };

// More ?! 
// Tratamento, informacoes.

//done:
    faultsShowRegisters();
    bl_die();

//  No return
}

void faultsShowRegisters()
{
    //#todo
}

//
// End
//
