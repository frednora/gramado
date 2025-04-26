// x64smp.c
// Symmetric multiprocessing

// Initialization
// Probing if smp is supported.
// + Via MP.   | see: mpfloat.c
// + Via ACPI. | see: acpi.c
// #todo
// This is a work in progress.
// >> we need to disable PIC, and init the first AP.
// The routines are found in another place. 
// see: apic/ioapic routines.
// 2022: Created by Fred Nora.

// see:
// https://en.wikipedia.org/wiki/Symmetric_multiprocessing
// https://forum.osdev.org/viewtopic.php?t=16990

/*
"RSD PTR "
  ||
RsdtAddress pointer at offset 16
  ||
  \/
"RSDT"
  ||
pointer at offset 36 + 4 * n (check the target for the sig "FACP" to get the right n)
  ||
  \/
"FACP"
  ||
  ||=====\
  ||   ||
  ||   PM1a_CNT_BLK; offset: 64   (see section 4.7.3.2)
  ||   PM1b_CNT_BLK; offset: 68
  ||      ||
  ||      \/
  ||      SLP_TYPx; bit 10-12
  ||      SLP_EN;     bit 13
  ||
DSDT pointer at offset 40
  ||
  \/
"DSDT"   (export the \_S5 object somehow.)
*/

/*
bytecode of the \_S5 object
-----------------------------------------
        | (optional) |    |    |    |
NameOP | \          | _  | S  | 5  | _
08     | 5A         | 5F | 53 | 35 | 5F

-----------------------------------------------------------------------------------------------------------
           |           |              | ( SLP_TYPa   ) | ( SLP_TYPb   ) | ( Reserved   ) | (Reserved    )
PackageOP | PkgLength | NumElements  | byteprefix Num | byteprefix Num | byteprefix Num | byteprefix Num
12        | 0A        | 04           | 0A         05  | 0A          05 | 0A         05  | 0A         05

----this-structure-was-also-seen----------------------
PackageOP | PkgLength | NumElements |
12        | 06        | 04          | 00 00 00 00
*/

#include <kernel.h>

/*
  The AP initialization. AP startup.

  + After you've gathered the information, you'll need to 
  + disable the PIC and 
  + prepare for I/O APIC. 
  + You also need to setup BSP's local APIC. 
  + Then, startup the APs using SIPIs. 
*/

// The main structure for SMP initialization
// see: x64smp.h
struct smp_info_d  smp_info;



// ------------------------------------


//
// ======================================================
//

void x64smp_show_info(void)
{
    if (smp_info.initialized != TRUE)
    {
        printk ("smp_info not initialized!\n");
        return;
    }

// Probe via

    if (smp_info.probe_via == SMP_VIA_ACPI)
        printk("Probe via ACPI\n");
    if (smp_info.probe_via == SMP_VIA_MP_TABLE)
        printk("Probe via MP TABLE\n");

    if (smp_info.probe_via_acpi_failed == TRUE)
        printk("Probe via ACPI failed\n");
    if (smp_info.probe_via_mp_failed == TRUE)
        printk("Probe via MP failed\n");


    printk("Number of processors: %d\n",
        smp_info.number_of_processors );

    printk("NR AP running: %d\n",
        smp_info.nr_ap_running );

    printk("PIC disabled: %d\n",
        smp_info.bsp_pic_is_disabled );

    printk("Using LACPI: %d\n",
        smp_info.bsp_is_using_lapic );

// ...
}


//
// $
// SMP INITIALIZATION
//

// Probe for smp support and initialize lapic.
// see:
// https://wiki.osdev.org/SMP
int x64smp_initialization(void)
{
// Called I_kmain() in kmain.c
// Probing if smp is supported.
// + Via ACPI.
// + Via MP. And then initialize lapic.

    int smp_status = FALSE;  // fail

    smp_info.initialized = FALSE;
    smp_info.probe_via = 0;

//
// The SMP support.
//

    //PROGRESS("x64smp_initialization:\n");

    // #debug
    printk("\n");
    printk("---- SMP START ----\n");

    printk("x64smp_initialization:\n");

// ----------------------
// ACPI

//
// == ACPI ===========================================================
//

    printk("\n");
    printk("\n");
    printk("== SMP VIA ACPI ===================\n");

    // #test #todo
    // Using the ACPI tables.
    // See: acpi.c

    smp_info.probe_via = SMP_VIA_ACPI;
    smp_status = (int) __x64_probe_smp_via_acpi();

    // #bugbug
    // We gotta find this table and check the elements.
    //printk("ACPI: Breakpoint\n");
    //refresh_screen();
    //while(1){}

    // ACPI OK.
    if (smp_status == TRUE){

        // If the LAPIC is not initialized
        if (LAPIC.initialized != TRUE)
        {
            // #todo
            // Disable PIC for BSP.

            // Enable LAPIC for BSP.
            // see: apic.c
            // 0xFEE00000
            lapic_initializing(LAPIC_DEFAULT_ADDRESS);
            if (LAPIC.initialized == TRUE){
                printk("x64smp_initialization: lapic initialization ok\n");
            }else if (LAPIC.initialized != TRUE){
                printk("x64smp_initialization: lapic initialization fail\n");
            };
        }
    } else {
        smp_info.probe_via_acpi_failed = TRUE;
        printk("x64smp_initialization: [x64_probe_smp_via_acpi] failed\n");
    };

    // #debug
    //#breakpoint
    //printk("x64smp.c Breakpoint\n");
    //refresh_screen();
    //while (1){ asm("hlt"); };

//
// == MP ===========================================================
//

// ----------------------
// MP Table
// Initialize smp support via MP Floating point structure. (kinda).
// Testando a inicializaçao do lapic.
// Tentando ler o id e a versao.
// It works on qemu and qemu/kvm.
// It doesn't work on Virtualbox. (Table not found).

    printk("\n");
    printk("\n");
    printk("== SMP VIA MP ===================\n");

    smp_info.probe_via = SMP_VIA_MP_TABLE;
    smp_status = (int) __x64_probe_smp_via_mptable();

    if (smp_status == TRUE){

        // If the LAPIC is not initialized
        if (LAPIC.initialized != TRUE)
        {
            // We need a valid address,
            // this time we get the value from the MP table.

            // Initialize LAPIC based on the address we found before.
            if ((void*) MPConfigurationTable != NULL)
            {
                if (MPConfigurationTable->lapic_address != 0)
                {
                    // #todo
                    // Disable PIC for BSP.

                    // Enable LAPIC for BSP.
                    // see: apic.c
                    lapic_initializing( MPConfigurationTable->lapic_address );
                    if (LAPIC.initialized == TRUE){
                        printk("??: lapic initialization ok\n");
                    }else if (LAPIC.initialized != TRUE){
                        printk("??: lapic initialization fail\n");
                    };
                }
            }
        }
    } else {
        smp_info.probe_via_mp_failed = TRUE;
        printk("x64smp_initialization: [__x64_probe_smp_via_mptable] failed\n");
    };

    printk("---- SMP END ----\n");
    printk("\n");

    // #debug
    // #breakpoint
    // x64smp_show_info();
    // while (1){ asm("hlt"); };

    // #debug
    //#breakpoint
    //printk("x64smp.c Breakpoint\n");
    //refresh_screen();
    //while (1){ asm("hlt"); };

    return (int) smp_status;
}

/*
// #todo
void x64_show_smp_into(void);
void x64_show_smp_into(void)
{
// Processor entry.
    struct entry_processor_d *p_entry;
    int i=0;

    if (smp_info.initialized != TRUE)
        return;
    // #debug
    if (smp_info.number_of_processors > 16)
        return;

    for (i=0; i<smp_info.number_of_processors; i++)
    {
        p_entry = (struct entry_processor_d *) smp_info.processors[i];
        if ( (void*) p_entry != NULL )
        {
            //#todo
            // Print info.
        }
    };
}
*/

