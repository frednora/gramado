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



/*
 * struct storage_controller_d
 * ----------------------------------------------------------------------
 * Main controller-level state for the ATA (or compatible) storage controller.
 * This structure tracks high-level information about the detected controller,
 * including what type of storage controller was found (IDE/ATA, RAID, AHCI/SATA, etc.)
 * and whether it has been properly initialized.
 *
 * - 'initialized': Set to nonzero after the controller and all ports (ata_port[4])
 *    are successfully probed and configured.
 * - 'controller_type': Identifies the controller type, such as IDE, RAID, or AHCI,
 *    using one of the __ATA_CONTROLLER, __RAID_CONTROLLER, or __AHCI_CONTROLLER constants.
 *
 * Note: Per-port and per-device information is tracked in ata_port[4] and other structures;
 * storage_controller_d is only for the general controller overview and global state.
 */
struct storage_controller_d
{
// 0: Not initialized; 1: Controller and ports are ready.
    int initialized;
// See __ATA_CONTROLLER, __RAID_CONTROLLER, __AHCI_CONTROLLER, etc.
    uint8_t controller_type;
};
extern struct storage_controller_d  StorageController;



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

