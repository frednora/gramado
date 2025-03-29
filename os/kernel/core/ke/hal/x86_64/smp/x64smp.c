// x64smp.c
// Symmetric multiprocessing

// Initialization
// Probing if smp is supported.
// + Via MP.
// + Via ACPI.
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


// see: mp.h
struct rsdp_d *rsdp;
struct xsdp_d *xsdp;

struct rsdt_d *rsdt;
struct xsdt_d *xsdt;


// see: x64smp.h
struct mp_floating_pointer_structure_d *MPTable;
// see: x64smp.h
struct mp_configuration_table_d *MPConfigurationTable;

// The main structure for SMP initialization
// see: x64smp.h
struct smp_info_d  smp_info;


// ------------------------------------
static int __acpi_check_header(unsigned int *ptr, char *sig);

static int __x64_probe_smp_via_acpi(void);
static int __x64_probe_smp_via_mptable(void);
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

    if (smp_info.probe_via_apci_failed == TRUE)
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


// Checks for a given header and validates checksum.
// Credits:
// https://forum.osdev.org/viewtopic.php?t=16990
static int __acpi_check_header(unsigned int *ptr, char *sig)
{
    char *checkPtr = (char *) ptr;
    int len=0;
    char Sum=0;

    // Match
    if (kstrncmp(ptr, sig, 4) == 0)
    {
        len = *(ptr + 1);
        while (0 < len--)
        {
            Sum += *checkPtr;
            checkPtr++;
        };

        //OK, The sun is not '0'.
        if (Sum == 0)
             return 0;
    }

// Fail
   return (int) (-1);
}


//
// $
// VIA ACPI
//

// After you've gathered the information, 
// you'll need to disable the PIC and prepare for I/O APIC. 
// You also need to setup BSP's local APIC. 
// Then, startup the APs using SIPIs.
// You should be able to find a MADT table 
// in the RSDT table or in the XSDT table.
// #todo
// Probe the acpi table.
// + 'RSDP signature'
static int __x64_probe_smp_via_acpi(void)
{

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
    unsigned char c5=0;
    unsigned char c6=0;
    unsigned char c7=0;
    unsigned char c8=0;

//
// Probe ebda address at bda base.
//

    printk("EBDA short Address: %x\n", bda[0] ); 
    ebda_address = (unsigned long) ( bda[0] << 4 );
    ebda_address = (unsigned long) ( ebda_address & 0xFFFFFFFF );
    printk("EBDA Address: %x\n", ebda_address ); 

// base
// between 0xF0000 and 0xFFFFF.
// #todo: filter
    p = ebda_address;

// The signature was found?
    static int Found = FALSE;

// Probe for the signature. // "RSD PTR "
    int max = (int) (0xFFFFF - ebda_address);
    for (i=0; i<max; i++){
        c1 = p[i+0];
        c2 = p[i+1];
        c3 = p[i+2];
        c4 = p[i+3];
        c5 = p[i+4];
        c6 = p[i+5];
        c7 = p[i+6];
        c8 = p[i+7];

        // "RSD PTR "
        if ( c1 == 'R' && 
             c2 == 'S' && 
             c3 == 'D' && 
             c4 == ' ' &&
             c5 == 'P' &&
             c6 == 'T' &&
             c7 == 'R' &&
             c8 == ' '  )
        {
            printk (":: Found [RSD PTR ] at index %d. :)\n",i);
            Found=TRUE;
            break;
        }
    };

// Signature not found.
    if (Found != TRUE){
        printk("__x64_probe_smp_via_acpi: [RSD PTR ] wasn't found!\n");
        goto fail;
    }

// Get address
    unsigned long __rsdp_Pointer = 
        (unsigned long) (ebda_address + i);

// Save the address
    smp_info.RSD_PTR_address = (unsigned long) __rsdp_Pointer;

// --------------------------------------------------------------
// To find the RSDT you need first to locate and check the RSDP, 
// then use the RsdtPointer for ACPI Version < 2.0 an XsdtPointer for any other case. 

// The root table.
    rsdp = (struct rsdp_d *) __rsdp_Pointer;  // 1.0
    xsdp = (struct xsdp_d *) __rsdp_Pointer;  // 2.0

// #debug ok
    printk("RSDP signature: \n");
    printk ("%c %c %c %c\n",
        rsdp->Signature[0],
        rsdp->Signature[1],
        rsdp->Signature[2],
        rsdp->Signature[3] );
    printk ("%c %c %c %c\n",
        rsdp->Signature[4],
        rsdp->Signature[5],
        rsdp->Signature[6],
        rsdp->Signature[7] );
    // #debug
    // #breakpoint
    //while(1){ asm("hlt"); }

    unsigned long __rsdt_address=0;
    unsigned long __xsdt_address=0;

    // Use xsdp
    if (rsdp->Revision == 0x02){
        printk("ACPI version 2.0\n");
        // #maybe 0xFFF00000 ?
        __rsdt_address = 
            (unsigned long) (rsdp->RsdtAddress & 0xFFFFFFFF);
        __xsdt_address = 
            (unsigned long) (xsdp->XsdtAddress & 0xFFFFFFFF);
        
        // #breakpoint
        //panic("x64smp.c: Revision 2.0 #todo\n");
        printk("x64smp.c: Revision 2.0 #todo\n");
        goto valid_revision;

    // Use rsdp
    }else if (rsdp->Revision == 0){
        printk("ACPI version 1.0\n");
        __rsdt_address = 
            (unsigned long) (rsdp->RsdtAddress & 0xFFFFFFFF);
        __xsdt_address = 0;
    }else{
        panic("x64smp.c: acpi revision\n");
    };

// Mapping the rsdt address.
// Used in both revisions.
    unsigned long address_pa = 0;
    unsigned long address_va = 0;
    int mapStatus = -1;


// ++
// ----------------------------------
// Revision 1.0.

// ---------------------------------------
// RSDT (Root System Description Table)
// This table contains pointers to all the other System Description Tables.
// Validating the RSDT:
// You only need to sum all the bytes in the table and compare the result to 0 (mod 0x100). 
// https://wiki.osdev.org/RSDT

    if (rsdp->Revision == 0x02){
        //panic("x64smp.c: #todo 2.0\n");
        printk("x64smp.c: #todo 2.0\n");
        goto valid_revision;
    }else if (rsdp->Revision == 0){
        // Print the address we have.
        //printk("rsdt address: %x \n", __rsdt_address);
        //while(1){ asm("hlt"); }

        // Mapping the rsdt address.
        address_pa = __rsdt_address;
        address_va = RSDT_VA;
        // OUT: 0=ok | -1=fail
        // see: pages.c
        mapStatus = (int) mm_map_2mb_region(address_pa,address_va);
        if (mapStatus != 0){
            panic("mapStatus\n");
        }
        // Now we have a valid pointer.
        rsdt = (struct rsdt_d *) RSDT_VA;
        // #debug
        printk("RSDT signature: \n");
        printk ("%c %c %c %c\n",
            rsdt->Signature[0],
            rsdt->Signature[1],
            rsdt->Signature[2],
            rsdt->Signature[3] );
        
        // #todo
        // Mesmo que a tabela rsdt nao seja satifatoria,
        // o melhor a fazer eh ignorar e procurar por "FACP"
        // e tocar o abrco.
        
    }else{
        panic("x64smp.c: ACPI Revision\n");
    };

// ----------------------------------
// --

// ---------------------------------------
// Multiple APIC Description Table (MADT)
// You should be able to find a MADT table in the RSDT table or in the XSDT table. 
// The table has a list of local-APICs, 
// https://wiki.osdev.org/MADT
// ...

valid_revision:
    return TRUE;

//-------------------
// Initialize lapic
// #test: initializing lapic right after the return.
//do_lapic:

    // #debug
    // #breakpoint

    //printk("#breakpoint\n");
    //while(1){ asm("hlt"); }

    /*
    // 0xFEE00000
    lapic_initializing(LAPIC_DEFAULT_ADDRESS);

    if (LAPIC.initialized == TRUE){
        printk("__x64_probe_smp_via_acpi: lapic initialization ok\n");
        return TRUE;
    }else if (LAPIC.initialized != TRUE){
        printk("__x64_probe_smp_via_acpi: lapic initialization fail\n");
        return FALSE;
    };
    */

fail:
    return FALSE;
}


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
static int __x64_probe_smp_via_mptable(void)
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

    printk("\n");
    printk("\n");
    printk("== SMP VIA ACPI ===================\n");

    // #test #todo
    // Using the ACPI tables.
    smp_info.probe_via = SMP_VIA_ACPI;
    smp_status = (int) __x64_probe_smp_via_acpi();
    if (smp_status != TRUE)
    {
        smp_info.probe_via_apci_failed = TRUE;
        printk("x64smp_initialization: [x64_probe_smp_via_acpi] failed\n");
    }
    if (smp_status == TRUE)
    {
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
    }

    // #debug
    //#breakpoint
    //while (1){ asm("hlt"); };

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
    if (smp_status != TRUE)
    {
        smp_info.probe_via_mp_failed = TRUE;
        printk("x64smp_initialization: [__x64_probe_smp_via_mptable] failed\n");
    }
    if (smp_status == TRUE)
    {
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
    }

    printk("---- SMP END ----\n");
    printk("\n");

    // #debug
    // #breakpoint
    // x64smp_show_info();
    // while (1){ asm("hlt"); };

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


//
// bytecode of the \_S5 object
// -----------------------------------------
//        | (optional) |    |    |    |   
// NameOP | \          | _  | S  | 5  | _
// 08     | 5A         | 5F | 53 | 35 | 5F
//
// -----------------------------------------------------------------------------------------------------------
//           |           |              | ( SLP_TYPa   ) | ( SLP_TYPb   ) | ( Reserved   ) | (Reserved    )
// PackageOP | PkgLength | NumElements  | byteprefix Num | byteprefix Num | byteprefix Num | byteprefix Num
// 12        | 0A        | 04           | 0A         05  | 0A          05 | 0A         05  | 0A         05
//
//----this-structure-was-also-seen----------------------
// PackageOP | PkgLength | NumElements |
// 12        | 06        | 04          | 00 00 00 00
//
// (Pkglength bit 6-7 encode additional PkgLength bytes [shouldn't be the case here])
//

/*
// Credits:
// https://forum.osdev.org/viewtopic.php?t=16990
int initAcpi(void)
{
   unsigned int *ptr = acpiGetRSDPtr();

   // check if address is correct  ( if acpi is available on this pc )
   if (ptr != NULL && acpiCheckHeader(ptr, "RSDT") == 0)
   {
      // the RSDT contains an unknown number of pointers to acpi tables
      int entrys = *(ptr + 1);
      entrys = (entrys-36) /4;
      ptr += 36/4;   // skip header information

      while (0<entrys--)
      {
         // check if the desired table is reached
         if (acpiCheckHeader((unsigned int *) *ptr, "FACP") == 0)
         {
            entrys = -2;
            struct FACP *facp = (struct FACP *) *ptr;
            if (acpiCheckHeader((unsigned int *) facp->DSDT, "DSDT") == 0)
            {
               // search the \_S5 package in the DSDT
               char *S5Addr = (char *) facp->DSDT +36; // skip header
               int dsdtLength = *(facp->DSDT+1) -36;
               while (0 < dsdtLength--)
               {
                  if ( memcmp(S5Addr, "_S5_", 4) == 0)
                     break;
                  S5Addr++;
               }
               // check if \_S5 was found
               if (dsdtLength > 0)
               {
                  // check for valid AML structure
                  if ( ( *(S5Addr-1) == 0x08 || ( *(S5Addr-2) == 0x08 && *(S5Addr-1) == '\\') ) && *(S5Addr+4) == 0x12 )
                  {
                     S5Addr += 5;
                     S5Addr += ((*S5Addr &0xC0)>>6) +2;   // calculate PkgLength size

                     if (*S5Addr == 0x0A)
                        S5Addr++;   // skip byteprefix
                     SLP_TYPa = *(S5Addr)<<10;
                     S5Addr++;

                     if (*S5Addr == 0x0A)
                        S5Addr++;   // skip byteprefix
                     SLP_TYPb = *(S5Addr)<<10;

                     SMI_CMD = facp->SMI_CMD;

                     ACPI_ENABLE = facp->ACPI_ENABLE;
                     ACPI_DISABLE = facp->ACPI_DISABLE;

                     PM1a_CNT = facp->PM1a_CNT_BLK;
                     PM1b_CNT = facp->PM1b_CNT_BLK;
                     
                     PM1_CNT_LEN = facp->PM1_CNT_LEN;

                     SLP_EN = 1<<13;
                     SCI_EN = 1;

                     return 0;
                  } else {
                     wrstr("\\_S5 parse error.\n");
                  }
               } else {
                  wrstr("\\_S5 not present.\n");
               }
            } else {
               wrstr("DSDT invalid.\n");
            }
         }
         ptr++;
      }
      wrstr("no valid FACP present.\n");
   } else {
      wrstr("no acpi.\n");
   }

   return -1;
}
*/

/*
// Credits:
// https://forum.osdev.org/viewtopic.php?t=16990
void acpiPowerOff(void)
{
   // SCI_EN is set to 1 if acpi shutdown is possible
   if (SCI_EN == 0)
      return;

   acpiEnable();

   // send the shutdown command
   outw((unsigned int) PM1a_CNT, SLP_TYPa | SLP_EN );
   if ( PM1b_CNT != 0 )
      outw((unsigned int) PM1b_CNT, SLP_TYPb | SLP_EN );

   wrstr("acpi poweroff failed.\n");
}
*/


/*
// Credits:
// https://wiki.osdev.org/MADT
// The following code snippet detects and parses MADT table 
// to collect Local APIC data on SMP systems. 
// Works with both RSDT and XSDT, and compiles 
// for both protected mode and long mode.

uint8_t lapic_ids[256]={0}; // CPU core Local APIC IDs
uint8_t numcore=0;          // number of cores detected
uint64_t lapic_ptr=0;       // pointer to the Local APIC MMIO registers
uint64_t ioapic_ptr=0;      // pointer to the IO APIC MMIO registers
 
void detect_cores(uint8_t *rsdt)
{
  uint8_t *ptr, *ptr2;
  uint32_t len;
 
  // iterate on ACPI table pointers
  for(len = *((uint32_t*)(rsdt + 4)), ptr2 = rsdt + 36; ptr2 < rsdt + len; ptr2 += rsdt[0]=='X' ? 8 : 4) {
    ptr = (uint8_t*)(uintptr_t)(rsdt[0]=='X' ? *((uint64_t*)ptr2) : *((uint32_t*)ptr2));
    if(!memcmp(ptr, "APIC", 4)) {
      // found MADT
      lapic_ptr = (uint64_t)(*((uint32_t)(ptr+0x24)));
      ptr2 = ptr + *((uint32_t*)(ptr + 4));
      // iterate on variable length records
      for(ptr += 44; ptr < ptr2; ptr += ptr[1]) {
        switch(ptr[0]) {
          case 0: if(ptr[4] & 1) lapic_ids[numcore++] = ptr[3]; break; // found Processor Local APIC
          case 1: ioapic_ptr = (uint64_t)*((uint32_t*)(ptr+4); break;  // found IOAPIC
          case 5: lapic_ptr = *((uint64_t*)(ptr+4); break;             // found 64 bit LAPIC
        }
      }
      break;
    }
  }
}
*/

/*
// detect SMP cores and print out results
detect_cores(rsd_ptr->rsdt_address);
*/

/*
printk("Found %d cores, IOAPIC %lx, LAPIC %lx, Processor IDs:", numcore, ioapic_ptr, lapic_ptr);
for(i = 0; i < numcore; i++)
  printk(" %d", lapic_ids[i]);
printk("\n");
*/

