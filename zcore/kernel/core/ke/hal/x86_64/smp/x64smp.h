// x64smp.h
// Symmetric multiprocessing (SMP) support for x86_64.
// Created by Fred Nora.

// See:
// https://wiki.osdev.org/Symmetric_Multiprocessing
// https://wiki.osdev.org/User:Shikhin/Tutorial_SMP
// apic.h
// https://www.cheesecake.org/sac/smp.html
// https://en.wikipedia.org/wiki/Symmetric_multiprocessing


#ifndef __SMP_X64SMP_H
#define __SMP_X64SMP_H    1


//----------------------------------------------------

#define LAPIC_DEFAULT_ADDRESS  0xFEE00000

#define SMP_VIA_ACPI   1000
#define SMP_VIA_MP_TABLE  2000

// see: x64smp.c
struct smp_info_d
{

// The SMP is initialized.
    int initialized;

// Step1:
// Probing:
// First via ACPI.
// if it fails, use MP table.

    int probe_via;
    int probe_via_acpi_failed;
    int probe_via_mp_failed;

// ACPI
    unsigned long RSD_PTR_address;

// MP table
    struct mp_floating_pointer_structure_d *mp_floating_point;
    struct mp_configuration_table_d *mp_configuration_table;

// How many processors did we find?
// Array of pointers.
// Pointer for this structure: (struct entry_processor_d)
    unsigned long processors[32];
    unsigned int number_of_processors;

// Step2:
// Disable PIC for BSP.

// The BSP's PIC is disabled.
    int bsp_pic_is_disabled;

// The IOAPIC was initialized.
    //int ioapic_is_initialized;

// Step3:
// Setup the LAPIC for BSP.

// The BSP is using the his LAPIC.
    int bsp_is_using_lapic;

// The BSP processor will comunicate with the AP
// and update these counters.

// Number of AP running.
// total -bsp
    //int nr_ap;
// Number of AP running.
    int nr_ap_running;
};
// see: x64smp.c
extern struct smp_info_d  smp_info;

// Entry types
#define ENTRY_IS_PROCESSOR  0
#define ENTRY_IS_BUS        1
#define ENTRY_IS_IOAPIC     2
// ..

//
// prototypes ==========
//


// See: x64smp.c
void x64smp_show_info(void);
int x64smp_initialization(void);

#endif   

//
// End
//


