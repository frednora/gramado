
// ata.h
// Created by Nelson Cole.

#ifndef __ATA_ATA_H
#define __ATA_ATA_H    1

#define IDE_ATA    0
#define IDE_ATAPI  1

//#define HDD1_IRQ 14 
//#define HDD2_IRQ 15 


// Bus.
#define ATA_PRIMARY   0x00
#define ATA_SECONDARY 0x01

// Devices.
#define ATA_MASTER    0
#define ATA_SLAVE     1 
#define ATA_MASTER_DEV 0x00
#define ATA_SLAVE_DEV  0x01

// ATA type.
#define ATA_DEVICE_TYPE   0x00
#define ATAPI_DEVICE_TYPE 0x01

// Modo de transferência.
#define ATA_PIO_MODO  0 
#define ATA_DMA_MODO  1
#define ATA_LBA28     28
#define ATA_LBA48     48

// Force mode.
#define FORCEPIO 1234

// #bugbug 
// Precisamos encontrar endereços válidos.
#define DMA_PHYS_ADDR0  0xa0000
#define DMA_PHYS_ADDR1  0xb0000
#define DMA_PHYS_ADDR2  0xb0000
#define DMA_PHYS_ADDR3  0xb0000 

// #bugbug: 
// Usar definição do Gramado.
#define PCI_CLASSE_MASS  1

// Controladores de unidades ATA.
#define ATA_IDE_CONTROLLER   0x1
#define ATA_RAID_CONTROLLER  0x4
#define ATA_AHCI_CONTROLLER  0x6
#define ATA_UNKNOWN_CONTROLLER  0xFF   

// Retorno da inicializacao PCI. 
#define PCI_MSG_ERROR       -1
#define PCI_MSG_AVALIABLE   0x80
#define PCI_MSG_SUCCESSFUL  0

// IO Space Legacy BARs IDE. 
#define ATA_IDE_BAR0_PRIMARY_COMMAND    0x1F0  // Primary Command Block Base Address.
#define ATA_IDE_BAR1_PRIMARY_CONTROL    0x3F6  // Primary Control Block Base Address.
#define ATA_IDE_BAR2_SECONDARY_COMMAND  0x170  // Secondary Command Block Base Address.
#define ATA_IDE_BAR3_SECONDARY_CONTROL  0x376  // Secondary Control Block Base Address.
#define ATA_IDE_BAR4_BUS_MASTER  0      // Bus Master Base Address.
#define ATA_IDE_BAR5  0      // Usado pelo AHCI.

// ATA/ATAPI Command Set.
#define ATA_CMD_CFA_ERASE_SECTORS               0xC0
#define ATA_CMD_CFA REQUEST_EXTENDED_ERROR_CODE 0x03
#define ATA_CMD_CHECK_MEDIA_CARD_TYPE           0xD1
#define ATA_CMD_CHECK_POWER_MODE                0xE5
#define ATA_CMD_DEVICE_RESET                    0x08
#define ATA_CMD_EXECUTE_DEVICE_DIAGNOSTIC       0x90
#define ATA_CMD_FLUSH_CACHE                     0xE7
#define ATA_CMD_FLUSH_CACHE_EXT                 0xEA
#define ATA_CMD_IDENTIFY_DEVICE                 0xEC
#define ATA_CMD_IDENTIFY_PACKET_DEVICE          0xA1
#define ATA_CMD_PACKET                          0xA0
#define ATA_CMD_READ_BUFFER                     0xE4
#define ATA_CMD_READ_DMA                        0xC8
#define ATA_CMD_READ_DMA_EXT                    0x25
#define ATA_CMD_READ_SECTORS                    0x20
#define ATA_CMD_READ_SECTORS_EXT                0x24
#define ATA_CMD_WRITE_BUFFER                    0xE8
#define ATA_CMD_WRITE_DMA                       0xCA
#define ATA_CMD_WRITE_DMA_EXT                   0x35
#define ATA_CMD_WRITE_SECTORS                   0x30
#define ATA_CMD_WRITE_SECTORS_EXT               0x34

/*
#define IDE_CMD_READ    0x20
#define IDE_CMD_WRITE   0x30
#define IDE_CMD_RDMUL   0xC4
#define IDE_CMD_WRMUL   0xC5
*/

/*
 //#todo: esse deslocamento é em bytes,
 // mas usamos um ponteiro em short ... entao precisamos dividir por dois.
#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200
*/

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

// --------------------------
// Registers
// See:
// https://wiki.osdev.org/ATA_PIO_Mode

// Read/Write PIO data bytes
#define ATA_REG_DATA      0x00
// R, Error Register,    Used to retrieve any error generated by the last ATA command executed.
#define ATA_REG_ERROR     0x01
// W, Features Register, Used to control command specific interface features.
#define ATA_REG_FEATURES  0x01
// Sector Count Register  Number of sectors to read/write (0 is a special value).
#define ATA_REG_SECCOUNT  0x02

// Sector Number Register (LBAlo) This is CHS / LBA28 / LBA48 specific.
#define ATA_REG_LBA0      0x03
// Cylinder Low Register / (LBAmid)	Partial Disk Sector address.
#define ATA_REG_LBA1      0x04
// Cylinder High Register / (LBAhi)	Partial Disk Sector address.
#define ATA_REG_LBA2      0x05

// Drive/Head Register Used to select a drive and/or head. Supports extra address/flag bits.
#define ATA_REG_DEVSEL    0x06
// W, Command Register	Used to send ATA commands to the device.
#define ATA_REG_CMD       0x07
// R, Status Register	Used to read the current status.
#define ATA_REG_STATUS    0x07

/*
 #todo
 testar esse outros registradores.
#define ATA_REG_SECCOUNT1  0x08
#define ATA_REG_LBA3       0x09
#define ATA_REG_LBA4       0x0A
#define ATA_REG_LBA5       0x0B
#define ATA_REG_CONTROL    0x0C
#define ATA_REG_ALTSTATUS  0x0C
#define ATA_REG_DEVADDRESS 0x0D
*/

// ===========================================================

extern int g_ata_driver_initialized;

//
// Variables
//

// See: ata.c
extern int ATAFlag;
extern unsigned short *ata_identify_dev_buf;
extern unsigned char ata_record_dev;
extern unsigned char ata_record_channel;
// #important
// Qual é o canal e o dispositivo usado no momento
// pela rotina de leitura e escrita.
// See: config.h ata.c hdd.c
extern int g_current_ide_port_index; 
// #important
// Qual é o canal e o dispositivo usado no momento do boot 
// pela rotina de leitura e escrita.
// See: config.h ata.c hdd.c
extern int g_boottime_ide_port_index; 


/*
 * PCIDeviceATA:
 * Estrutura de dispositivos pci para um disco ata.
 * #bugbug: E se tivermos mais que um instalado ???
 * #importante
 * Essa é uma estrutura de dispositivos pci 
 * criada para o gramado, 
 * definida em pci.h e criada em atapci.c
 */
extern struct pci_device_d *PCIDeviceATA;
// extern struct pci_device_d *PCIDeviceATA2;
// ...


/*
 * dev_nport:
 *     AHCI ports;
 */

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

// see: ata.c
extern struct dev_nport  dev_nport;

// ----------------
// History:
// Programação do ATA a partir do ICH5/9 e suporte a IDE legado.
// ICH5 integraçao do SATA e suporte total ACPI 2.0.
// ICH6 implementaram os controladores AHCI SATA pela primeira vez.
// Nelson Cole.

struct ata_port_info_d
{
// helper structure for ata_controller_d.

    uint8_t channel;  // Primary or secondary.
    uint8_t dev_num;  // Master or slave.
};


/*
 * ata:
 * Estrutura para o controle de execução do programa.
 */ 
struct ata_controller_d
{
// ATA Mass storage controler structure.

// Structure validation
    int used;
    int magic;

// The structure was initialized.
    //int initialized;

// IDE, RAID, AHCI.
    uint8_t controller_type;


// #bugbug
// Um dispositivo controlador por esse controller?
// #todo
// We gotta save the device independent info
// in a proper ata device information structure.
// Not in the controller information structure.

    // 4 dispositivos.
    //struct ata_port_info_d  port[4];

    uint8_t channel;  // Primary or secondary.
    uint8_t dev_num;  // Master or slave.

    uint8_t dev_type;  
    uint8_t access_type;
    uint8_t cmd_read_modo;
    uint32_t cmd_block_base_address;
    uint32_t ctrl_block_base_address;
    uint32_t bus_master_base_address;
    uint32_t ahci_base_address;

    //#todo
    //struct ata_device_d devs[4];
};

// Not a pointer.
// #todo
// Actually we need a pointer,
// because the system is gonna have more than one
// ata controller. Specially in VMs.
extern struct ata_controller_d  ata_controller;


/*
 * ata_device_d
 * storage device.
 * É uma estrutura para dispositivos de armazenamento.
 */
// #todo
// Change to ata_device_d
// Do jeito que esta, nos da a impressao errada de que 
// todos os dispositivos de armazenamento sao do tipo ata.

struct ata_device_d
{
    unsigned long dev_id;

// Is it the boottime device?
// TRUE or FALSE.
    int boottime_device;

    unsigned char  dev_nport;
    unsigned char  dev_type;            // ATA or ATAPI
    unsigned char  dev_num;
    unsigned char  dev_channel;
    unsigned char  dev_access;          // LBA28 or LBA48
    unsigned char  dev_modo_transfere;
    
    unsigned long dev_byte_per_sector;

    unsigned long dev_total_num_sector;             // lba28
    unsigned long dev_total_num_sector_lba48;  // lba48

    unsigned short lba28_value1;
    unsigned short lba28_value2;

    unsigned short lba48_value1;
    unsigned short lba48_value2;
    unsigned short lba48_value3;
    unsigned short lba48_value4;
    
    unsigned long dev_size;
    
// #test
    unsigned long _MaxLBA;
    unsigned long _MaxLBAExt;

// disk info structure.
// See:
// disk.h
    struct disk_d *disk;

// A list of threads waiting on this device.
    struct thread_d *waiting_list;

    struct ata_device_d  *next;
};
// Current storage device.
extern struct ata_device_d *current_sd;
// List for storage devices.
extern struct ata_device_d *ready_queue_dev;

//======================================================

// Incluindo coisas que estavam em disk1.c

#define DISK1 1
#define DISK2 2
#define DISK3 3
#define DISK4 4

extern unsigned long ATA_BAR0_PRIMARY_COMMAND_PORT;    // Primary Command Block Base Address
extern unsigned long ATA_BAR1_PRIMARY_CONTROL_PORT;    // Primary Control Block Base Address
extern unsigned long ATA_BAR2_SECONDARY_COMMAND_PORT;  // Secondary Command Block Base Address
extern unsigned long ATA_BAR3_SECONDARY_CONTROL_PORT;  // Secondary Control Block Base Address
extern unsigned long ATA_BAR4;  // Legacy Bus Master Base Address
extern unsigned long ATA_BAR5;  // AHCI Base Address / SATA Index Data Pair Base Address


//
// DMA support
//

// Commands dma 
#define dma_bus_start   1
#define dma_bus_stop    0
#define dma_bus_read    0
#define dma_bus_write   1

// Status dma
#define ide_dma_sr_err     0x02

// Registros bus master base address
#define ide_dma_reg_cmd     0x00
#define ide_dma_reg_status  0x02
#define ide_dma_reg_addr    0x04

// channel
#define ide_dma_primary     0x00
#define ide_dma_secundary   0x01

// ide_dma_prdt:
struct {
    uint32_t addr;
    uint32_t len;
}ide_dma_prdt[4];


//
// == IDE ===============================================
//


// IDE ports.
// 0 primary master 
// 1 primary slave 
// 2 secondary master 
// 3 secondary slave.
typedef enum {
	ideportsPrimaryMaster,    // 0
	ideportsPrimarySlave,     // 1
	ideportsSecondaryMaster,  // 2
	ideportsSecondarySlave    // 3
}ide_ports_t;

typedef enum {
	idetypesPrimaryMaster,    // 0
	idetypesPrimarySlave,     // 1
	idetypesSecondaryMaster,  // 2
	idetypesSecondarySlave    // 3
}ide_types_t;

typedef enum {
	idedevicetypesPATA,    // 0
	idedevicetypesSATA,    // 1
	idedevicetypesPATAPI,  // 2
	idedevicetypesSATAPI   // 3
}ide_device_types_t;

// ----------------

// IDE ports support
// Structure for a ide port.
struct ide_port_d 
{
    // #todo
    // Object header ?

    // Structure validation.
    int used;
    int magic;
    // The port number.
    //int id;
    uint8_t id;
// PATA, SATA, PATAPI, SATAPI
    int type;
    char *name;
    unsigned short base_port;
// #test
// The size of the disk given in sectors.
// This is a work in progress, don't trust in this value yet.
    unsigned long size_in_sectors;
// #todo
// Salvar aqui o canal usado pela porta
// e se o dispositivo é master ou slave.
    uint8_t channel;   // #bugbug: penso que seja para primary ou secondary.
    uint8_t dev_num;   // #bugbug: penso que seja para master e slave.
    // ...
// Dá pra colocar aqui mais informações sobre 
// o dispositivo conectado a porta.
// podemos usar ponteiros para estruturas.
};
// Four ports.
extern struct ide_port_d  ide_ports[4];

//
// == Prototypes ==============================================
//

void ata_soft_reset (void);
unsigned char ata_status_read (void);

void ata_cmd_write (int cmd_val);

unsigned char ata_wait_drq (void);
unsigned char ata_wait_no_drq (void);

unsigned char ata_wait_busy (void);
unsigned char ata_wait_not_busy (void);

void ata_wait (int val);
void ata_delay (void);

inline void atapi_pio_read ( void *buffer, uint32_t bytes );

void ata_set_boottime_ide_port_index(unsigned int port_index);
int ata_get_boottime_ide_port_index(void);

void ata_set_current_ide_port_index(unsigned int port_index);
int ata_get_current_ide_port_index(void);

int 
ata_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg );

uint32_t 
diskReadPCIConfigAddr ( 
    int bus, 
    int dev,
    int fun, 
    int offset );

void 
diskWritePCIConfigAddr ( 
    int bus, 
    int dev,
    int fun, 
    int offset, 
    int data );


// Read and write via pio mode.
int 
pio_rw_sector ( 
    unsigned long buffer, 
    unsigned int _lba, 
    int rw, 
    unsigned int port_index );

int 
ataReadSector( 
    unsigned long buffer, 
    unsigned long lba, 
    unsigned long reserved1, 
    unsigned long reserved2 );

int 
ataWriteSector ( 
    unsigned long buffer,
    unsigned long lba,
    unsigned long reserved1,
    unsigned long reserved2 );

int init_hdd (void);

void ide_dma_start (void);
void ide_dma_stop (void);
int ide_dma_read_status (void);

void 
ide_dma_data ( 
    void *addr, 
    uint16_t byte_count,
    uint8_t flg,
    uint8_t nport );

int 
init_ata ( 
    int msg, 
    unsigned long long1 );

uint32_t diskPCIScanDevice (int class);

int atapciSetupMassStorageController(struct pci_device_d *D);

unsigned char ata_wait_irq (void);
int disk_ata_wait_irq (void);
int ide_identify_device ( uint8_t nport );
int ide_dev_init (char port);
static inline void dev_switch (void);
static inline int getnport_dev (void);
int nport_ajust ( char nport );

// Show info:
void ata_show_ide_info(void);
void ata_show_device_list_info(void);

#endif    


