// storage.h 
// Created by Fred Nora.

#ifndef __DD_STORAGE_H
#define __DD_STORAGE_H    1


// --------------------------------------------------
// Controller mode

#define STORAGE_CONTROLLER_MODE_SCSI     0x00
#define STORAGE_CONTROLLER_MODE_ATA      0x01
#define STORAGE_CONTROLLER_MODE_RAID     0x04
// Sub-class 05h = ATA Controller with ADMA
#define STORAGE_CONTROLLER_MODE_DMA      0x05   // (USB ?)
#define STORAGE_CONTROLLER_MODE_AHCI     0x06
// 0x08: NVMe (Non-Volatile Memory Express)
#define STORAGE_CONTROLLER_MODE_NVME     0x08
// 0x09: SAS (Serial Attached SCSI)
#define STORAGE_CONTROLLER_MODE_SAS      0x09
#define STORAGE_CONTROLLER_MODE_UNKNOWN  0xFF

// --------------------------------------------------
// Interface standard
// ...


//
// Signature for interface standard.
//

// ATA controller mode handles PATA and SATA interface standards.
// AHCI controller mode handles only SATA interface standards,

// PATA
#define STORAGE_INTERFACE_STANDARD_PATA_SIG1    0
#define STORAGE_INTERFACE_STANDARD_PATA_SIG2    0
#define STORAGE_INTERFACE_STANDARD_PATAPI_SIG1  0x14
#define STORAGE_INTERFACE_STANDARD_PATAPI_SIG2  0xEB

// SATA
#define STORAGE_INTERFACE_STANDARD_SATA_SIG1    0x3C
#define STORAGE_INTERFACE_STANDARD_SATA_SIG2    0xC3
#define STORAGE_INTERFACE_STANDARD_SATAPI_SIG1  0x69
#define STORAGE_INTERFACE_STANDARD_SATAPI_SIG2  0x96




// Internal use
#define CONTROLLER_TYPE_ATA   1000
#define CONTROLLER_TYPE_AHCI  2000

struct boot_disk_d
{
    int initialized;
    int controller_type;
};
extern struct boot_disk_d  BootDisk;


// Read lba on ide device.
void read_lba( unsigned long address, unsigned long lba );
// White lba on ide device.
void write_lba( unsigned long address, unsigned long lba );


int storage_initialize(void);

#endif    

