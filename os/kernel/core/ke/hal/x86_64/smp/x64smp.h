/*
 * File: mp.h 
 *     Multi-processor (MP) support.
 *     Symmetric multiprocessing (SMP) 
 */

// See:
// https://wiki.osdev.org/Symmetric_Multiprocessing
// https://wiki.osdev.org/User:Shikhin/Tutorial_SMP
// apic.h
// https://www.cheesecake.org/sac/smp.html
// https://en.wikipedia.org/wiki/Symmetric_multiprocessing


#ifndef ____MP_H
#define ____MP_H    1


/*
First, you need to find the floating pointer structure. 
According to the spec, it can be in one of four places: 
(1) in the first kilobyte of the extended BIOS data area, 
(2) the last kilobyte of base memory, 
(3) the top of physical memory, or 
(4) the BIOS read-only memory space between 0xe0000 and 0xfffff. 
You need to search these areas for the four-byte 
signature "_MP_" which denotes the start of the 
floating pointer structure. 
*/

//----------------------------------------------------
// See
// https://pdos.csail.mit.edu/6.828/2008/readings/ia32/MPspec.pdf
// https://wiki.osdev.org/Symmetric_Multiprocessing
struct mp_floating_pointer_structure_d 
{

// The Signature, must contain _MP_, and 
// is present on a 16 byte boundary.
    char signature[4];

// The address of the MP Configuration Table.
    uint32_t configuration_table;
    
// The length of the floating point structure table, in 16 byte units. 
// This field *should* contain 0x01, meaning 16-bytes.
// In 16 bytes (e.g. 1 = 16 bytes, 2 = 32 bytes)
    uint8_t length; 
    
// The version number of the MP Specification. 
// A value of 1 indicates 1.1, 4 indicates 1.4, and so on.
    uint8_t mp_specification_revision;
    
// The checksum of the Floating Point Structure.
// This value should make all bytes in the table equal 0 when added together
    uint8_t checksum; 

// If this is not zero then configuration_table should be 
// ignored and a default configuration should be loaded instead.
    uint8_t default_configuration; 

// Few feature bytes.
// If bit 7 is set then 
// the IMCR is present and PIC mode is being used, otherwise 
// virtual wire mode is; 
// all other bits are reserved.
    uint32_t features; 
};
// see: x64.c
extern struct mp_floating_pointer_structure_d *MPTable;

//----------------------------------------------------
// See:
// https://wiki.osdev.org/Symmetric_Multiprocessing
struct mp_configuration_table_d 
{
// "PCMP"
    char signature[4];
    uint16_t length;
    uint8_t mp_specification_revision;
// Again, the byte should be all bytes in the table add up to 0.
    uint8_t checksum;

// OEM ID STRING
    char oem_id[8];

// PRODUCT ID STRING
    char product_id[12];

    uint32_t oem_table;
    uint16_t oem_table_size;
    uint16_t entry_count; // This value represents how many entries are following this table

// This is the memory mapped address of the local APICs
// The base address by which each processor accesses
// its local APIC.

    // MEMORY-MAPPED ADDRESS OF LOCAL APIC
    uint32_t lapic_address;

    uint16_t extended_table_length;
    uint8_t extended_table_checksum;
    uint8_t reserved;
};
// see: x64.c
extern struct mp_configuration_table_d *MPConfigurationTable;



//----------------------------------------------------
// #
// Logo abaixo da estrutura acima segue uma
// sequencia de entadas de tipo e tamanhos diferentes.
// As que descrevem processadores possuem 20bytes
// as outras possuem 8 bytes.

/*
entry info:
Description | Type | Length | Comments
Processor  |     0 |     20 | One entry per processor.
Bus        |     1 |      8 | One entry per bus.
I/O APIC   |     2 |      8 | One entry per I/O APIC.
I/O 
Interrupt 
Assignment |     3 |      8 | One entry per bus interrupt source.
Local 
Interrupt 
Assignment |     4 |      8 | One entry per system interrupt source.
*/

//# size: 20 bytes
struct entry_processor_d 
{
// #todo:
// apic stuff. Move the structure to apic.h?

// Always 0 for 'processor'.
    uint8_t type; 

    uint8_t local_apic_id;
    uint8_t local_apic_version;

// flags:
// If bit 0 is clear then the processor must be ignored.
// If bit 1 is set then the processor is the bootstrap processor.
    uint8_t flags;

// Stepping, model, family
    uint32_t signature;
    
    uint32_t feature_flags;
    
    unsigned long reserved;
    //uint64_t reserved;
};


//----------------------------------------------------
/*
// # size: 8 bytes
struct entry_io_apic_d 
{
    uint8_t type; // Always 2
    uint8_t id;
    uint8_t version;
    uint8_t flags; // If bit 0 is set then the entry should be ignored
    
    uint32_t address; // The memory mapped address of the IO APIC is memory
};
*/

//
// == ACPI stuff ====================================
//

//----------------------------------------------------
// https://wiki.osdev.org/RSDP
struct rsdp_d
{
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;
} __attribute__ ((packed));
extern struct rsdp_d *rsdp;

//----------------------------------------------------
// Since version 2.0 it has been extended: 
// https://wiki.osdev.org/XSDP
struct xsdp_d 
{
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;  // Deprecated since version 2.0
    uint32_t Length;
    uint64_t XsdtAddress;
    uint8_t ExtendedChecksum;
    uint8_t reserved[3];
} __attribute__ ((packed));
extern struct xsdp_d *xsdp;

//----------------------------------------------------
// RSDT (Root System Description Table) 
// https://wiki.osdev.org/RSDT
struct rsdt_d 
{
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
    // unsigned int *entry; // 4*n
};
// see: x64smp.c
extern struct rsdt_d *rsdt;

//----------------------------------------------------
// eXtended System Descriptor Table (XSDT) - 
// the 64-bit version of the ACPI RSDT 
// https://wiki.osdev.org/XSDT
struct xsdt_d 
{
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
    //unsigned long *entry; // 8*n
};
// see: x64smp.c
extern struct xsdt_d *xsdt;


//----------------------------------------------------
// MADT entry type.
// Based on the Entry Type field value, 
// the rest of the record layout can be determined.

// Entry Type 0: Processor Local APIC 
#define MADT_RECORD_TYPE_LAPIC          0
// Entry Type 1: I/O APIC 
#define MADT_RECORD_TYPE_IOAPIC         1
// Entry Type 2: IO/APIC Interrupt Source Override 
#define MADT_RECORD_TYPE_ISO            2
// Entry type 3: IO/APIC Non-maskable interrupt source 
//#define MADT_RECORD_ ...
// Entry Type 4: Local APIC Non-maskable interrupts 
#define MADT_RECORD_TYPE_NMI            4
// Entry Type 5: Local APIC Address Override
#define MADT_RECORD_TYPE_LAPIC_AO       5
// Entry Type 9: Processor Local x2APIC 
//#define MADT_RECORD_ ...


//----------------------------------------------------
// Flag
//#define MADT_LAPIC_FLAG_ENABLED         (1 << 0)
//#define MADT_LAPIC_FLAG_ONLINE_CAPABLE  (1 << 1)


//----------------------------------------------------
// The MADT describes all of the interrupt controllers in the system. 
// It can be used to enumerate the processors currently available. 
// https://wiki.osdev.org/MADT

struct madt_d 
{

// -------------------
// Header
// ACPI header?
    char signature[4];              //00  4  Signature 'APIC' 
    unsigned int length;            //04  4  Length 
    unsigned char revision;         //08  1  Revision 
    unsigned char checksum;         //09  1  Checksum 
    char oem_id[6];                 //0a  6  OEMID 
    char oem_table_id[8];           //10  8  OEM Table ID 
    unsigned int oem_revision;      //18  4  OEM Revision 
    unsigned int creator_id;        //1c  4  Creator ID 
    unsigned int creator_revision;  //20  4  Creator Revision 

// -------------------
// Info
    unsigned int lapic;  //24  4  Local APIC Address 
    unsigned int flag;   //28  4  Flags (1 = Dual 8259 Legacy PICs Installed) 

// -------------------
// Based on the Entry Type field value, 
// the rest of the record layout can be determined.
// Entry Type 0: Processor Local APIC 
// Entry Type 1: I/O APIC 
// Entry Type 2: IO/APIC Interrupt Source Override 
// Entry type 3: IO/APIC Non-maskable interrupt source 
// Entry Type 4: Local APIC Non-maskable interrupts 
// Entry Type 5: Local APIC Address Override
// Entry Type 9: Processor Local x2APIC 

    unsigned int struc;
    //uint8_t records[];
};

//----------------------------------------------------

// Record: Local APIC
struct madt_record_lapic_d 
{
    uint8_t type;
    uint8_t len;

    uint8_t proc_id;
    uint8_t apic_id;
    uint32_t flags;
} __attribute__ ((packed));

// Record: I/O APIC 
struct madt_record_ioapic_d 
{
    uint8_t type;
    uint8_t len;

    uint8_t id;
    uint8_t reserved;
    uint32_t addr;
    uint32_t gsi_base;
} __attribute__ ((packed));

// Record: Interrupt Source Override
struct madt_record_iso_d 
{
    uint8_t type;
    uint8_t len;

    uint8_t bus_src;
    uint8_t irq_src;
    uint32_t gsi;
    uint16_t flags;
} __attribute__ ((packed));

// Record: Non Maskable Interrupt
struct madt_record_nmi_d 
{
    uint8_t type;
    uint8_t len;

    uint8_t proc_id;
    uint16_t flags;
    uint8_t lint;
} __attribute__ ((packed));



//----------------------------------------------------
// Used in the table below
struct GenericAddressStructure 
{
    uint8_t AddressSpace;
    uint8_t BitWidth;
    uint8_t BitOffset;
    uint8_t AccessSize;

    //uint64_t Address;
    unsigned long Address;
};

//----------------------------------------------------
// see:
// https://wiki.osdev.org/FADT
struct FADT
{
    //struct   ACPISDTHeader h;
    struct rsdt_d  h;

    uint32_t FirmwareCtrl;
    uint32_t Dsdt;

// field used in ACPI 1.0; no longer in use, for compatibility only
    uint8_t  Reserved;
 
    uint8_t  PreferredPowerManagementProfile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CommandPort;
    uint8_t  AcpiEnable;
    uint8_t  AcpiDisable;
    uint8_t  S4BIOS_REQ;
    uint8_t  PSTATE_Control;
    uint32_t PM1aEventBlock;
    uint32_t PM1bEventBlock;
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
    uint32_t PM2ControlBlock;
    uint32_t PMTimerBlock;
    uint32_t GPE0Block;
    uint32_t GPE1Block;
    uint8_t  PM1EventLength;
    uint8_t  PM1ControlLength;
    uint8_t  PM2ControlLength;
    uint8_t  PMTimerLength;
    uint8_t  GPE0Length;
    uint8_t  GPE1Length;
    uint8_t  GPE1Base;
    uint8_t  CStateControl;
    uint16_t WorstC2Latency;
    uint16_t WorstC3Latency;
    uint16_t FlushSize;
    uint16_t FlushStride;
    uint8_t  DutyOffset;
    uint8_t  DutyWidth;
    uint8_t  DayAlarm;
    uint8_t  MonthAlarm;
    uint8_t  Century;
 
    // reserved in ACPI 1.0; used since ACPI 2.0+
    uint16_t BootArchitectureFlags;
 
    uint8_t  Reserved2;
    uint32_t Flags;
 
    // 12 byte structure; see below for details
    struct GenericAddressStructure ResetReg;
 
    uint8_t  ResetValue;
    uint8_t  Reserved3[3];

// 64bit pointers - Available on ACPI 2.0+
    //uint64_t  X_FirmwareControl;
    //uint64_t  X_Dsdt;
    unsigned long X_FirmwareControl;
    unsigned long X_Dsdt;

    struct GenericAddressStructure X_PM1aEventBlock;
    struct GenericAddressStructure X_PM1bEventBlock;
    struct GenericAddressStructure X_PM1aControlBlock;
    struct GenericAddressStructure X_PM1bControlBlock;
    struct GenericAddressStructure X_PM2ControlBlock;
    struct GenericAddressStructure X_PMTimerBlock;
    struct GenericAddressStructure X_GPE0Block;
    struct GenericAddressStructure X_GPE1Block;
};

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
    int probe_via_apci_failed;
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

void x64smp_show_info(void);

// See: x64smp.c
int x64smp_initialization(void);

#endif   

//
// End
//


