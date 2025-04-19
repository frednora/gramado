// ata.c
// ATA/AHCI controller.
// Environment:
//     32bit bootloader.
// History:
//     2017 - Ported from Sirius OS, BSD-2-Clause License.
//     This device driver was created by Nelson Cole, for Sirius OS.
//     2021 - Some new changes by Fred Nora.


// See:
// https://wiki.osdev.org/ATA_Command_Matrix

// =====================================================================
//     IDE controller support by Nelson Cole
// =====================================================================

// Programação do ATA a partir do ICH5/9 e suporte a IDE legado.
// ICH5 - Integraçao do SATA e suporte total ACPI 2.0.
// ICH6 - Implementaram os controladores AHCI SATA pela primeira vez.


#ifndef __DD_IDE_H
#define __DD_IDE_H    1


//see: ide.c
extern int ATAFlag;

#define FORCEPIO  1234
//...


// IO Delay.
#define io_delay() \
    asm("out %%al,$0x80"::);


//
// PCI
//

// Return value when initializing PCI bus.
#define PCI_MSG_ERROR       -1
#define PCI_MSG_AVALIABLE   0x80
#define PCI_MSG_SUCCESSFUL  0

//
// DMA
//

// Atenção:
// Esse endereço está na memória baixa.
//#define DMA_PHYS_ADDR0 (0x40000)
//#define DMA_PHYS_ADDR1 (DMA_PHYS_ADDR0 + 0x10000)
//#define DMA_PHYS_ADDR2 (DMA_PHYS_ADDR1 + 0x10000)
//#define DMA_PHYS_ADDR3 (DMA_PHYS_ADDR2 + 0x10000)

// #bugbug: 
// VGA MEMORY?
// We need valid addresses
// user mode 1:1
#define DMA_PHYS_ADDR0  0xa0000
#define DMA_PHYS_ADDR1  0xb0000
#define DMA_PHYS_ADDR2  0xb0000
#define DMA_PHYS_ADDR3  0xb0000 

//
// PCI CLASS (Mass storage device) 
//

// Device class
#define PCI_CLASS_MASS  1

//
// Subclasses
// (Controller type)
//

/*
Here's a list of subclasses for the PCI class 1 (Mass Storage Controller):

0x00: SCSI
0x01: IDE (Integrated Drive Electronics)
0x02: RAID
0x03: Fibre Channel
0x04: IPI (Intelligent Peripheral Interface)
0x05: USB
0x06: SATA (Serial ATA)
0x07: SSA (Serial Storage Architecture)
0x08: NVMe (Non-Volatile Memory Express)
0x09: SAS (Serial Attached SCSI)
0x0A: SD/MMC
0x0B: UFS (Universal Flash Storage)
0x0C: PCIe (PCI Express)
*/

#define CONTROLLER_TYPE_UNKNOWN  (-1)

// 0x00: SCSI
#define __SCSI_CONTROLLER     0x0
// Sub-class 01h = IDE Controller
#define __ATA_CONTROLLER      0x1
// raid
#define __RAID_CONTROLLER     0x4
// Sub-class 05h = ATA Controller with ADMA
#define __ATA_CONTROLLER_DMA  0x5   // (USB ?)
// Sub-class 06h = SATA Controller
#define __AHCI_CONTROLLER     0x6
// 0x08: NVMe (Non-Volatile Memory Express)
#define __NVME_CONTROLLER     0x08
// 0x09: SAS (Serial Attached SCSI)
#define __SAS_CONTROLLER      0x09

// Number of ports.
#define ATA_NUMBER_OF_PORTS  4
//#define SATA_NUMBER_OF_PORTS  ?

// ============================================

// IO Space Legacy BARs IDE. 
#define ATA_IDE_BAR0  0x1F0  // (p) Primary Command Block Base Address.
#define ATA_IDE_BAR1  0x3F6  // (p) Primary Control Block Base Address.
#define ATA_IDE_BAR2  0x170  // (s) Secondary Command Block Base Address.
#define ATA_IDE_BAR3  0x376  // (s) Secondary Control Block Base Address.
#define ATA_IDE_BAR4  0      // Bus Master Base Address.
#define ATA_IDE_BAR5  0      // Usado pelo AHCI.

/*
// Read and write registers
#define REG_CMD_BASE0	0x1F0	// (p) command base register of controller 0 
#define REG_CMD_BASE1	0x170	// (s) command base register of controller 1 
#define REG_CTL_BASE0	0x3F6	// (p) control base register of controller 0 
#define REG_CTL_BASE1	0x376	// (s) control base register of controller 1 
*/


// ATA/ATAPI Command Set.
#define ATA_CMD_CFA REQUEST_EXTENDED_ERROR_CODE 0x03
#define ATA_CMD_DEVICE_RESET                    0x08
#define ATA_CMD_READ_SECTORS                    0x20
#define ATA_CMD_READ_SECTORS_EXT                0x24
#define ATA_CMD_READ_DMA_EXT                    0x25
#define ATA_CMD_WRITE_SECTORS                   0x30
#define ATA_CMD_WRITE_SECTORS_EXT               0x34
#define ATA_CMD_WRITE_DMA_EXT                   0x35
#define ATA_CMD_EXECUTE_DEVICE_DIAGNOSTIC       0x90
#define ATA_CMD_PACKET                          0xA0
#define ATA_CMD_IDENTIFY_PACKET_DEVICE          0xA1
#define ATA_CMD_CFA_ERASE_SECTORS               0xC0
#define ATA_CMD_READ_DMA                        0xC8
#define ATA_CMD_WRITE_DMA                       0xCA
#define ATA_CMD_CHECK_MEDIA_CARD_TYPE           0xD1
#define ATA_CMD_READ_BUFFER                     0xE4
#define ATA_CMD_CHECK_POWER_MODE                0xE5
#define ATA_CMD_FLUSH_CACHE                     0xE7
#define ATA_CMD_WRITE_BUFFER                    0xE8
#define ATA_CMD_FLUSH_CACHE_EXT                 0xEA
#define ATA_CMD_IDENTIFY_DEVICE                 0xEC


// ATAPI descrito no SCSI.
#define ATAPI_CMD_READ  0xA8
#define ATAPI_CMD_EJECT 0x1B

// ATA bits de status control (alternativo).
#define ATA_SC_HOB  0x80    // High Order Byte.
#define ATA_SC_SRST 0x04    // Soft Reset.
#define ATA_SC_nINE 0x02    // INTRQ.

// ATA bits de status. 
#define ATA_SR_BSY  0x80    // Busy
#define ATA_SR_DRDY 0x40    // Device Ready
#define ATA_SR_DF   0x20    // Device Fault
#define ATA_SR_DSC  0x10    // Device Seek Complete
#define ATA_SR_DRQ  0x08    // Data Request
#define ATA_SR_SRST 0x04    // 
#define ATA_SR_IDX  0x02    // Index
#define ATA_SR_ERR  0x01    // Error

// ATA bits de errro após a leitura.
#define ATA_ER_BBK   0x80    // 
#define ATA_ER_UNC   0x40    //
#define ATA_ER_MC    0x20    //
#define ATA_ER_IDNF  0x10    //
#define ATA_ER_MCR   0x08    //
#define ATA_ER_ABRT  0x04    //
#define ATA_ER_TK0NF 0x02    //
#define ATA_ER_AMNF  0x01    //

// Registers
#define ATA_REG_DATA     0x00
#define ATA_REG_ERROR    0x01
#define ATA_REG_FEATURES 0x01
#define ATA_REG_SECCOUNT 0x02
#define ATA_REG_LBA0     0x03
#define ATA_REG_LBA1     0x04
#define ATA_REG_LBA2     0x05
#define ATA_REG_DEVSEL   0x06
#define ATA_REG_CMD      0x07
#define ATA_REG_STATUS   0x07

// Devices
#define ATA_MASTER_DEV  0x00
#define ATA_SLAVE_DEV   0x01

// Bus
#define ATA_PRIMARY    0x00
#define ATA_SECONDARY  0x01

// ATA type.
#define ATA_DEVICE_TYPE    0x00
#define ATAPI_DEVICE_TYPE  0x01

// Modo de transferência.
#define ATA_PIO_MODO  0 
#define ATA_DMA_MODO  1
#define ATA_LBA28     28
#define ATA_LBA48     48


//
// variables
//

//see: ide.c
extern _u16 *ata_identify_dev_buf;
extern _u8 ata_record_dev;
extern _u8 ata_record_channel;




// ?
// dev_nport structure.
struct dev_nport 
{ 
    unsigned char dev0;
    unsigned char dev1;
    unsigned char dev2;
    unsigned char dev3;
    unsigned char dev4;
    unsigned char dev5;
    unsigned char dev6;
    unsigned char dev7;
    unsigned char dev8;
    unsigned char dev9;
    unsigned char dev10;
    unsigned char dev11;
    unsigned char dev12;
    unsigned char dev13;
    unsigned char dev14;
    unsigned char dev15;
    unsigned char dev16;
    unsigned char dev17;
    unsigned char dev18;
    unsigned char dev19;
    unsigned char dev20;
    unsigned char dev21;
    unsigned char dev22;
    unsigned char dev23;
    unsigned char dev24;
    unsigned char dev25;
    unsigned char dev26;
    unsigned char dev27;
    unsigned char dev28;
    unsigned char dev29;
    unsigned char dev30;
    unsigned char dev31;
};
//see: ide.c
extern struct dev_nport  dev_nport;


// ata_pci:
// Suporta a IDE Controller.
// Essa é uma estrutura de superte a discos ata.
struct ata_pci
{
    _u16 vendor_id;
    _u16 device_id;
    _u16 command;
    _u16 status;
    _u8  prog_if;
    _u8  revision_id;

    _u8  class;
    _u8  subclass;

    _u8  primary_master_latency_timer;
    _u8  header_type;
    _u8  BIST;
    _u32 bar0;
    _u32 bar1; 
    _u32 bar2;
    _u32 bar3;
    _u32 bar4;
    _u32 bar5;
    _u16 subsystem_vendor_id;
    _u16 subsystem_id;  
    _u32 capabilities_pointer;
    _u8  interrupt_line;
    _u8  interrupt_pin;
};
//see: ata.c
extern struct ata_pci  ata_pci;

// ata:
// Estrutura para o controle de execução do programa. 
struct ata
{
    //int used;
    //int magic;
    uint8_t  chip_control_type;
    uint8_t  channel;
    uint8_t  dev_type;
    uint8_t  dev_num;
    uint8_t  access_type;
    uint8_t  cmd_read_modo;
    uint32_t cmd_block_base_address;
    uint32_t ctrl_block_base_address;
    uint32_t bus_master_base_address;
    uint32_t ahci_base_address;
};
//see: ide.c
extern struct ata  ata;

// ??
// st_dev:
typedef struct st_dev 
{
    _u32 dev_id;
    _u8  dev_nport;
    _u8  dev_type;            // ATA or ATAPI
    _u8  dev_num;
    _u8  dev_channel;
    _u8  dev_access;          // LBA28 or LBA48
    _u8  dev_modo_transfere;
    _u32 dev_byte_per_sector;
    _u32 dev_total_num_sector;
    _u64 dev_total_num_sector_lba48;
    _u32 dev_size;
       
    struct st_dev *next;
}st_dev;
typedef struct st_dev  st_dev_t;




//
// IDE support 
//

// Channel and device number
extern int g_current_ide_channel;
extern int g_current_ide_device;
// Port number.
extern int g_current_ide_port;


// 0 primary master 
// 1 primary slave 
// 2 secondary master 
// 3 secondary slave.
typedef enum {
    ideportsPrimaryMaster,      // 0
    ideportsPrimarySlave,       // 1
    ideportsSecondaryMaster,    // 2
    ideportsSecondarySlave      // 3
}ide_ports_t;

typedef enum {
    idetypesPrimaryMaster,      // 0
    idetypesPrimarySlave,       // 1
    idetypesSecondaryMaster,    // 2
    idetypesSecondarySlave      // 3
}ide_types_t;

typedef enum {
    idedevicetypesPATA,    // 0
    idedevicetypesPATAPI,  // 1
    idedevicetypesSATA,    // 2
    idedevicetypesSATAPI   // 3
}ide_device_types_t;


//
// DEVICE TYPES
//

#define ATADEV_UNKNOWN  (-1)

// PATA
#define ATADEV_PATA    idedevicetypesPATA
#define ATADEV_PATAPI  idedevicetypesPATAPI

// SATA
#define ATADEV_SATA    idedevicetypesSATA
#define ATADEV_SATAPI  idedevicetypesSATAPI


//
// SIGNATURE
//

// PATA
#define ATADEV_PATA_SIG1    0
#define ATADEV_PATA_SIG2    0
#define ATADEV_PATAPI_SIG1  0x14
#define ATADEV_PATAPI_SIG2  0xEB

// SATA
#define ATADEV_SATA_SIG1    0x3c
#define ATADEV_SATA_SIG2    0xc3
#define ATADEV_SATAPI_SIG1  0x69
#define ATADEV_SATAPI_SIG2  0x96



// ->state field
// #define ATA_DEVICE_STATE_INITIALIZING  1000
// ...
// #define ATA_DEVICE_STATE_DEAD          2000


// IDE ports support
struct ide_ports_d 
{
    int used;
    int magic;

// Identification
    uint8_t id;
    // PATA, SATA, PATAPI, SATAPI
    int type;
    char *name;

    //int state;

// Ports
    unsigned short base_port;
    //unsigned short base_cmd;		//command base register 
    //unsigned short base_ctl;		//control base register 

    //...
};
// see: ata.c
// #todo: Initialize these structure before using them.
extern struct ide_ports_d  ide_ports[4];


#define IDE_ATA    0
#define IDE_ATAPI  1

#define ATA_MASTER  0
#define ATA_SLAVE   1 

//#define HDD1_IRQ 14 
//#define HDD2_IRQ 15 

#define IDE_CMD_READ    0x20
#define IDE_CMD_WRITE   0x30
#define IDE_CMD_RDMUL   0xC4
#define IDE_CMD_WRMUL   0xC5

extern unsigned long ide_handler_address; 

// Estrutura para canais da controladora IDE. 
struct ide_channel_d
{
    int id;
    int used;
    int magic;
    char name[8];

    // Cada canal vai ter uma porta diferente.
    // ex: canal 0, maste e canal 0 slave tem portas diferentes.	

    unsigned short port_base;
    unsigned char interrupt_number;

	//@todo: lock stuff.
	//@todo: semaphore
	//...
};
typedef struct ide_channel_d ide_channel_t; 

extern struct ide_channel_d  idechannelList[8];


// Estrutura para discos controlados pela controladora ide.
struct ide_disk_d
{
    int id;    // id do disco ide.
    int used;
    int magic;
    char name[8];         // #todo: bigger.
    unsigned short Type;  // 0: ATA | 1:ATAPI.

// O canal usado pelo disco.
// pode ser 0 ou 1, master ou slave ou outroscanais.
    struct ide_channel_d *channel; 

// #todo: estrutura para parti��es.
// Podemos ter muitos elementos aqui.
};
typedef struct ide_disk_d  ide_disk_t;

 
/*
 * ide_d:
 * #IMPORTANTE
 * Estrutura para configurar a interface IDE. 
 * Essa ser� a estrutura raiz para gerenciamento do controlador de IDE.
 */

struct ide_d
{
    // devemos colocar aqui um ponteiro para estrutura de informa��es 
    // sobre o dispositivo controlador de ide.	

    int current_port;
    struct ide_ports_d *primary_master; 
    struct ide_ports_d *primary_slave; 
    struct ide_ports_d *secondary_master; 
    struct ide_ports_d *secondary_slave; 
};

typedef struct ide_d ide_t;

//see: ide.c
extern struct ide_d  IDE;

 

//
// Prototypes =================================
//

void show_ide_info();


// ata_dev.c
int nport_ajuste(char nport);

// ata_main.c
void set_ata_addr(int channel);

// ata.c
void ata_wait(int val);
_u8 ata_wait_not_busy();
_u8 ata_wait_busy();
_u8 ata_wait_no_drq();
_u8 ata_wait_drq();
_u8 ata_wait_irq();
_u8 ata_status_read();
void ata_cmd_write(int cmd_val);

int disk_ata_wait_irq();

// worker
_u8 __ata_assert_dever(char nport);

// low level workers.
void __ata_pio_read(_void *buffer,_i32 bytes);
void __ata_pio_write(_void *buffer,_i32 bytes);
static inline void __atapi_pio_read ( void *buffer, uint32_t bytes );

//ide_dma.c
void 
ide_dma_data ( 
    void *addr, 
    uint16_t byte_count, 
    uint8_t flg, 
    uint8_t nport );
void ide_dma_start();
void ide_dma_stop();
int ide_dma_read_status();

//ahci.c
void ahci_mass_storage_init();


//
// $
// INITIALIZATION
//

int ata_initialize(void);

#endif    

//
// End
//


