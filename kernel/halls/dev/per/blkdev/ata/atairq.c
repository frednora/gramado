// atairq.c
// Created by Fred Nora.

#include <kernel.h>  

static int ata_irq_invoked = FALSE;

//
// == Private functions: Prototypes ======================
//

// Handlers for irqs.
static void DeviceInterface_PrimaryIDE(void);
static void DeviceInterface_SecondaryIDE(void);

static int disk_get_ata_irq_invoked (void);
static void disk_reset_ata_irq_invoked (void);

// ========================================================

static void DeviceInterface_PrimaryIDE(void)
{
    // Device not initialized.
    if ( __breaker_ata1_initialized == FALSE )
        return;

// Profiler: Counting interrupts.
    g_profiler_ints_irq14++;

// Set flag.
    ata_irq_invoked = TRUE;

/*
// Read all four ports' status registers
    unsigned char s0 = ata_status_read(0);
    unsigned char s1 = ata_status_read(1);
    unsigned char s2 = ata_status_read(2);
    unsigned char s3 = ata_status_read(3);
*/
}


static void DeviceInterface_SecondaryIDE(void)
{
    // Device not initialized.
    if ( __breaker_ata2_initialized == FALSE )
        return;

// Profiler: Counting interrupts.
    g_profiler_ints_irq15++;

// Set flag.
    ata_irq_invoked = TRUE;

/*
// Read all four ports' status registers
    unsigned char s0 = ata_status_read(0);
    unsigned char s1 = ata_status_read(1);
    unsigned char s2 = ata_status_read(2);
    unsigned char s3 = ata_status_read(3);
*/
}


//local
static int disk_get_ata_irq_invoked (void)
{
    return (int) ata_irq_invoked;
}

//local
static void disk_reset_ata_irq_invoked (void)
{
    ata_irq_invoked = FALSE;
}

// Interrupt handler.
__VOID_IRQ 
irq14_PRIMARY_IDE (void)
{
    //debug_print("irq14_PRIMARY_IDE:\n");
    DeviceInterface_PrimaryIDE();

    // #test
    // apic eoi
    if (CONFIG_INITIALIZE_IOAPIC_UNMASK_PRIMARY_IDE == 1)
    {
        local_apic_eoi(0);  // BSP
        printk ("IDE 14: EOI\n");
    }
}

// Interrupt handler.
__VOID_IRQ 
irq15_SECONDARY_IDE (void)
{
    //debug_print("irq15_SECONDARY_IDE:\n");
    DeviceInterface_SecondaryIDE();

    // #test
    // apic eoi
    if (CONFIG_INITIALIZE_IOAPIC_UNMASK_SECONDARY_IDE == 1)
    {
        //printk ("IDE 15: EOI\n");
        local_apic_eoi(0);  // BSP
        printk ("IDE 15: EOI\n");
    }
}


/*
 * ata_wait_irq:
 * 
 */

// #bugbug
// Veja se é possível mudar o retorno para 'int'.
// #obs: Tem uma função semelhante logo abaixo.

unsigned char ata_wait_irq (int p)
{
    unsigned long tmp = 0x10000;
    unsigned char data=0;


    while (!ata_irq_invoked)
    {
        data = ata_status_read(p);

        if ( (data & ATA_SR_ERR) )
        {
            ata_irq_invoked = FALSE;

            // #bugbug: 
            // Returning -1 on unsigned char.

            return -1;
        }

        // ns
        if (--tmp){ 
            ata_wait(400);
        }else{
            ata_irq_invoked = FALSE;
            return 0x80;
        };

    };

    disk_reset_ata_irq_invoked();
    //ata_irq_invoked = FALSE;
    
    return 0;
}


// disk_ata_wait_irq:
// Esperando pela interrupção.
// #obs: Tem uma função semelhante logo acima.
// OUT:
//     0    = ok por status da interrupção. 
//     -1   = ok por status do controlador.
//     0x80 = ok por tempo esperado.

int disk_ata_wait_irq (int p)
{
    unsigned long Timeout = 10000;
    unsigned char Data=0;

    printk("disk_ata_wait_irq: Start\n");

// Ok, recebemos uma interrupção e retornaremos 0 erros.
    if (ata_irq_invoked == TRUE){ 
        goto done; 
    }

/*
// Wait for interrupt
    while ( ata_irq_invoked != TRUE )
    {
        // Read status
        Data = (unsigned char) ata_status_read(p);

        // O status indicou um erro
        if (Data & ATA_SR_ERR)
        {
            //ata_irq_invoked = FALSE;   
            //return (int) -1;
        }

        // Não temos erro.
        // Se o tempo acabou, saímos indcando com 0x80.
        if (Timeout == 0)
        {
            //ata_irq_invoked = FALSE;
            //return (int) 0x80;
        }

        // Continua a contagem.
        ata_wait(400);
        Timeout--;
    };
*/

// Saímos do while. 
// Isso indica que a flag foi acionada durante o loop.
// OK por status da interrupção.
done:
    ata_irq_invoked = FALSE;
    printk("disk_ata_wait_irq: End\n");
    return 0;
}

