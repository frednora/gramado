// acpi.h
// ACPI tables support.
// RSDP, XSDP, RSDT, XSDT, 
// MADT, FADT,


#ifndef __SMP_ACPI_H
#define __SMP_ACPI_H    1


//----------------------------------------------------
// https://wiki.osdev.org/RSDP
// For ACPI 1.0 (older systems), the RSDP contains a 32-bit physical address 
// that points to the RSDT (Root System Description Table). Since it's a standard 32-bit address, 
// the mask used would generally be 0xFFFFFFFF, meaning all bits are valid.
struct rsdp_d
{
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;  // 32bit
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
// ----------------------------------------------------
// We have more thing here. That depends on the type of rsdt table.
// The header part is the same for all kinds.

    // unsigned int *entry; // 4*n
};
// see: x64smp.c and acpi.c
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

// =========================================

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


// =========================================

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

// ========================================


int acpi_check_header(unsigned int *ptr, char *sig);
int acpi_check_header02(unsigned int *ptr, char *sig);

int __x64_probe_smp_via_acpi(void);

// ...

#endif   

