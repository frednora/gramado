// mpfloat.c
// MP Floating Pointer table support.
// Created by Fred Nora.

// https://wiki.osdev.org/Symmetric_Multiprocessing

// #test
// The routines are running on Qemu.
// Some routines here are not running on Virtualbox,
// maybe there is no support for the old MP in there.

#include <kernel.h>


// see: mpfloat.h
struct mp_floating_pointer_structure_d *MPTable;
struct mp_configuration_table_d *MPConfigurationTable;


//
// $
// VIA MP TABLE
//

// x64_probe_smp:
// MP Floating Point Structure:
// To use these tables, the MP Floating Point Structure 
// must first be found. As the name suggests, 
// it is a Floating Point Structure and must be searched for.
// can't be found in this area, 
// then the area between 0xF0000 and 0xFFFFF should be searched. 
// To find the table, the following areas must be searched in:
// :: a) In the first kilobyte of Extended BIOS Data Area (EBDA), or
// :: b) Within the last kilobyte of system base memory 
// (e.g., 639K-640K for systems with 640KB of base memory 
// or 511K-512K for systems with 512 KB of base memory) 
// if the EBDA segment is undefined, or
// :: c) In the BIOS ROM address space between 0xF0000 and 0xFFFFF.
// See:
// https://wiki.osdev.org/Symmetric_Multiprocessing
// OUT:
// TRUE = OK.
// FALSE = FAIL.

// It works on qemu and qemu/kvm.
// It doesn't work on Virtualbox. (Table not found).
int __x64_probe_smp_via_mptable(void)
{
// Probe using the MP Floating Point structure.
// Called by x64_initialize_smp().
// + Find the processor entries using the MP Floating point table.
// + Initialize local apic.

// 0x040E - The base address.
// Get a short value.
    unsigned short *bda = (unsigned short*) BDA_BASE;
    unsigned long ebda_address=0;
    register int i=0;
    unsigned char *p;
// Signature elements.
    unsigned char c1=0;
    unsigned char c2=0;
    unsigned char c3=0;
    unsigned char c4=0;

    printk("__x64_probe_smp:\n");

// #todo
// We can use a structure and put all these variable together,
    g_smp_initialized = FALSE;
    smp_info.initialized = FALSE;

// At this point we gotta have a lot of information
// in the structure 'processor'.
    if ((void*) processor == NULL){
        panic("__x64_probe_smp_via_mptable: processor\n");
    }
// Is APIC supported?
    if (processor->hasAPIC != TRUE){
        panic("__x64_probe_smp_via_mptable: No APIC\n");
    }

// Probe ebda address at bda base.
    printk("EBDA short Address: %x\n", bda[0] ); 
    ebda_address = (unsigned long) ( bda[0] << 4 );
    ebda_address = (unsigned long) ( ebda_address & 0xFFFFFFFF);
    printk("EBDA Address: %x\n", ebda_address ); 

    // #debug
    // refresh_screen();

//
// Probe 0x5F504D5F signature. 
// "_MP_".
//

// base
// between 0xF0000 and 0xFFFFF.
// #todo: filter
    p = ebda_address;

// The signature was found?
    static int mp_found = FALSE;

// Probe for the signature."_MP_"
// This signature is the first element of the table.
// MP Floating Pointer Structure
    int max = (int) (0xFFFFF - ebda_address);
    for (i=0; i<max; i++){
        c1 = p[i+0];
        c2 = p[i+1];
        c3 = p[i+2];
        c4 = p[i+3];
        if ( c1 == '_' && c2 == 'M' && c3 == 'P' && c4 == '_' )
        {
            printk (":: Found _MP_ at index %d. :)\n",i);
            mp_found=TRUE;
            break;
        }
    };

// Signature not found.
    if (mp_found != TRUE){
        printk("__x64_probe_smp_via_mptable: MP table wasn't found!\n");
        goto fail;
    }

//mp_table_found:

// ==============================================
// MPTable
// MP Floating Point Structure
// base + offset.
// This is the base of the structure.
// See:
// https://wiki.osdev.org/User:Shikhin/Tutorial_SMP
// hal/mp.h

    unsigned long table_address = (unsigned long) (ebda_address + i);

// Saving
    MPTable = (struct mp_floating_pointer_structure_d *) table_address;
    smp_info.mp_floating_point = 
        (struct mp_floating_pointer_structure_d *) MPTable;

// ---------------------------------------------
// Print table info

// Signature
// "_MP_" 
// + OK on qemu.
// + OK on kvm.
// + FAIL on Virtualbox. #todo: try APIC.
    printk("Signature: %c %c %c %c\n",
        MPTable->signature[0],
        MPTable->signature[1],
        MPTable->signature[2],
        MPTable->signature[3] );

    //#debug
    //refresh_screen();
    //while(1){}

// ------------------------------------------
// Getting the address of the MP Configuration Table. 
// Pointed by th MP Floating point structure.
// 32bit address.
    unsigned long configurationtable_address = 
        (unsigned long) (MPTable->configuration_table & 0xFFFFFFFF);

// Pointer for the configuration table.
    printk("Configuration table address: {%x}\n",
        configurationtable_address);
// Lenght: n*16 bytes
// The length of the floating point structure table, 
// in 16 byte units. 
// This field *should* contain 0x01, meaning 16-bytes.
    printk("Lenght: {%d}\n", MPTable->length);
// Revision: 1.x
// The version number of the MP Specification. 
// A value of 1 indicates 1.1, 4 indicates 1.4, and so on.
    printk("Revision: {%d}\n", MPTable->mp_specification_revision);
// Checksum
// The checksum of the Floating Point Structure. 
    printk("Checksum: {%d}\n", MPTable->checksum);
// Default configuration flag.
// If this is not zero then configuration_table should be 
// ignored and a default configuration should be loaded instead.
    printk("Default configuration flag: {%d}\n",
        MPTable->default_configuration );

    if ( MPTable->default_configuration != 0 ){
        printk("todo: The configuration table should be ignored\n");
    }

// Features
// Few feature bytes.
// If bit 7 is set 
// then the IMCR is present and PIC mode is being used, 
// otherwise virtual wire mode is. 
// All other bits are reserved.
    printk("Features: {%d}\n", MPTable->features);
// Se o bit 7 está ligado.
    if ( MPTable->features & (1 << 7) ){
        printk("The IMCR is present and PIC mode is being used.\n");
    }
// Se o bit 7 está desligado.
    if ( (MPTable->features & (1 << 7)) == 0 ){
        printk("Using the virtual wire mode.\n");
    }

    //#debug
    //refresh_screen();
    //while(1){}

// ==============================================
// MPConfigurationTable structure.
// Pointed by th MP Floating point structure.

// Struture pointer.
// MP Configuration table.
    MPConfigurationTable = 
        (struct mp_configuration_table_d *) configurationtable_address;

    if ((void*) MPConfigurationTable == NULL){
        printk("__x64_probe_smp: Invalid Configuration table address\n");
        goto fail;
    }
// Saving
    smp_info.mp_configuration_table = 
        (struct mp_configuration_table_d *) MPConfigurationTable;

// Signature
// "PCMP"
    printk("Signature: %c %c %c %c\n",
        MPConfigurationTable->signature[0],
        MPConfigurationTable->signature[1],
        MPConfigurationTable->signature[2],
        MPConfigurationTable->signature[3] );

// Intel strings: 
// "OEM00000" "PROD00000000"

// OEM ID STRING
    char oemid_string[8+1];
    for (i=0; i<8; i++){
        oemid_string[i] = MPConfigurationTable->oem_id[i];
    };
    oemid_string[8]=0;  // finish
    printk("OEM ID STRING: {%s}\n",oemid_string);

// PRODUCT ID STRING
    char productid_string[12+1];
    for (i=0; i<12; i++){
        productid_string[i] = MPConfigurationTable->product_id[i];
    };
    productid_string[12]=0;  // finish
    printk("PRODUCT ID STRING: {%s}\n",productid_string);

// Lapic address
    printk("lapic address: {%x}\n",
        MPConfigurationTable->lapic_address );

// Is this the standard lapic address?
    if ( MPConfigurationTable->lapic_address != LAPIC_BASE )
    {
        printk("fail: Not standard lapic address\n");
        printk("Found={%x} Standard{%x}\n",
            MPConfigurationTable->lapic_address,
            LAPIC_BASE );
    }

//
// Entries
//

    printk("\n");
    printk("------------------\n");

// Probing the entries right below the MPConfigurationTable.

// -------------------------------
// Max number of entries.

    // This field tell us how many entries we have to probe.
    register int EntryCount = (int) MPConfigurationTable->entry_count;
    printk("Entry count: {%d}\n", MPConfigurationTable->entry_count);

    //#debug
    //refresh_screen();
    //while(1){};

// #bugbug
// Talvez essas entradas estão erradas.
// Talvez não haja entrada alguma nesse endereço aqui.

// =======================================================
// Entries
// ACPI processor, Local APIC.

// Logo abaixo da configuration table.
// começa uma sequência de entradas de tamanhos diferentes.
// Se a entrada for para descrever um processador, então 
// a entrada tem 20 bytes, caso contrario tem 8 bytes.
// see: mp.h

// Base address
// The address of the first entry.
    unsigned long entry_base = 
    (unsigned long) ( configurationtable_address + 
                      sizeof(struct mp_configuration_table_d) );

/*
------------------------------------------------------
entry info:
Description | Type | Length | Comments

Processor * |    0 |     20 | One entry per processor.

Bus         |    1 |      8 | One entry per bus.

I/O APIC  * |    2 |      8 | One entry per I/O APIC. :)

I/O 
Interrupt 
Assignment  |    3 |      8 | One entry per bus interrupt source.

Local 
Interrupt 
Assignment  |    4 |      8 | One entry per system interrupt source.
------------------------------------------------------
*/

// #remember:
// One type of entry is IOAPIC.

// Estrutura para entradas que descrevem um processador.
// Processor = type 0.

    struct entry_processor_d *e;

// This routine gets the number of processors.
// #todo:
// We can create a method for that routine.
// Register this number into the global data.
    unsigned int NumberOfProcessors=0;
    g_processor_count = NumberOfProcessors;

// Clean the list
    for (i=0; i<32; i++){
        smp_info.processors[i] = 0;
    };
    smp_info.number_of_processors = 0;

// #test
// #bugbug
// EntryCount has the max number of entries.
    if (EntryCount > 32)
    {
        printk("#bugbug: EntryCount > 32\n");
        EntryCount = 32;
    }

// --------------------------------------
// LOOP:
// Check all the n entries indicated in the table above.
    for (i=0; i<EntryCount; i++)
    {
        // Tracing
        //printk("\n");
        //printk(":::: Entry %d:\n",i);
        
        // #bugbug
        // Actually, here we need to get not only the processor
        // entries. But also the i/o apic entry, to know
        // what is the address for the i/o apic registers.

        // Looking for processors.
        e = (struct entry_processor_d *) entry_base;

        // ---------------------------
        // [ PROCESSOR ] ~> Size 20
        // It is a processor entry.
        if (e->type == ENTRY_IS_PROCESSOR){

            //printk("\n");
            //printk("------------------\n");
            //printk(">>>>> PROCESSOR found! in entry %d\n",i);
            printk("Entry %d: Type %d [PROCESSOR]\n", i, e->type );

            smp_info.processors[NumberOfProcessors] = (unsigned long) e;
            NumberOfProcessors += 1;

            // apic id.
            printk("local_apic_id %d\n", e->local_apic_id);
            // apic version
            printk("local_apic_version %d\n", e->local_apic_version);
            // Flags:
            // If bit 0 is clear then the processor must be ignored.
            // If bit 1 is set then the processor is the bootstrap processor.
            // Ignore the processor.
            if ( (e->flags & (1<<0)) == 0 ){
                printk("Processor must be ignored\n");
            }
            // BSP processor.
            if ( e->flags & (1<<1) ){
                printk("__x64_probe_smp: The processor is a BSP\n");
            }
            printk ("stepping: %d\n", (e->signature & 0x00F));
            printk ("   model: %d\n",((e->signature & 0x0F0) >> 4) );
            printk ("  family: %d\n",((e->signature & 0xF00) >> 8) );

            entry_base = (unsigned long) (entry_base + 20);

        // ---------------------------
        // [ NOT A PROCESSOR ] ~> Size 8
        // Not a processor entry.
        } else if (e->type != ENTRY_IS_PROCESSOR){
            //printk ("Device type %d in entry %d\n", e->type, i );

            // #todo
            // OK, at this moment, we know that this entry is not
            // a processor entry.
            // Let's save the address for the i/o apic 
            // if we find an entry for i/o apic.
            
            // [ IOAPIC ]
            // This is the type for i/o apic entries.
            if (e->type == ENTRY_IS_BUS){
                printk("Entry %d: Type %d [BUS]\n", i, e->type );
            }else if (e->type == ENTRY_IS_IOAPIC){
                printk("Entry %d: Type %d [IOAPIC]\n", i, e->type );
            }else if (e->type == 3){
                printk("Entry %d: Type %d [IO INTERRUPT]\n", i, e->type );
            }else if (e->type == 4){
                printk("Entry %d: Type %d [LOCAL INTERRUPT]\n", i, e->type );
            }else{
                printk("Entry %d: Type %d [?]\n", i, e->type );
            }
            
            //...
            
            entry_base = (unsigned long) (entry_base + 8);
        };
    };

//done:

    printk("\n");
    printk("------------------\n");

// Global number of processors.
    g_processor_count = (unsigned int) NumberOfProcessors;
// smp number of processors.
    smp_info.number_of_processors = (unsigned int) NumberOfProcessors;
    printk("Processor count: {%d}\n", smp_info.number_of_processors );

// smp done.
    smp_info.initialized = TRUE;
    printk("__x64_probe_smp_via_mptable: done\n");

    // #debug
    //refresh_screen();
    //while(1){
    //};

    // g_smp_initialized = TRUE;
    return TRUE;
fail:
    g_smp_initialized = FALSE;
    smp_info.initialized = FALSE;
    return FALSE;
}



