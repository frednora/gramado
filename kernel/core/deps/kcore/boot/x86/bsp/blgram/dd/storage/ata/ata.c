// ata.c
// ATA/AHCI device controller implementation for the Gramado OS bootloader.
//
// This file provides low-level routines for initializing and interacting
// with ATA-compatible storage devices (IDE/PATA/SATA), including PCI probing,
// device detection, and basic PIO/DMA data transfer routines.
//
// Environment:
//   - 32-bit protected mode bootloader (not kernel proper)
//   - Direct hardware access (I/O ports, PCI configuration space)
//
// History:
//   - 2017: Original port from Sirius OS (BSD-2-Clause License) by Nelson Cole.
//   - 2021: Updated and maintained by Fred Nora.
//
// References:
//   - https://wiki.osdev.org/ATA_PIO_Mode
//   - https://wiki.osdev.org/PCI_IDE_Controller
//
// Notes:
//   - This driver now supports up to four ATA ports, managed by the ata_port[4] structure.
//   - Each port (0-3) corresponds to Primary Master, Primary Slave, Secondary Master, and Secondary Slave.
//   - All routines reference ata_port[i] for port-specific operations and configuration.
//
// Successfully loading a ~400KB kernel image from a FAT16 partition 
// using your ATA PIO driver for the primary master is a significant achievement, 
// especially in a bootloader or early OS context. 
// This means:
// + PIO sector read path is robust for at least one drive/channel.
// + FAT16 implementation is functional enough to traverse directories, 
//   find files, and read data at a block level.
// + The integration between the file system and the ATA driver is stable 
//   for this scenario.
//
// #todo:
// - Review type usage and switch to standard C types.
// - Use conventional type names for consistency.
// - Focus is on disk device list management (diskList).
//
// Main goals
// - Extend the ATA driver to handle all 4 ports (Primary Master, Primary Slave, Secondary Master, Secondary Slave).
// - Prepare the codebase for future AHCI (SATA) driver support, using the structure found in the ahci/ folder.
//
// Next Steps to Broaden Device Support
// - Extend to Four Ports (Primary/Secondary, Master/Slave)
// - Prepare for Modularization and Future AHCI Support
//
// Testing:
// - Try reading from a FAT16 partition on another port (e.g., secondary master).
// - Adjust device selection logic as needed.
// - Ensure error handling (timeouts, missing devices, etc.) remains robust.
//
/*
IDE (Integrated Drive Electronics) can handle up to four drives, which may be:
  - ATA (Serial): Most modern hard drives.
  - ATA (Parallel): Older hard drives (PATA).
  - ATAPI (Serial): Modern optical drives.
  - ATAPI (Parallel): Older optical drives.
*/

#include "../../../bl.h"


// PCI Support
// Macro to construct a 32-bit PCI configuration space address 
// for accessing device registers via I/O ports.
// Avoid using macros for critical register accesses in production code!
#define CONFIG_ADDR(bus,device,fn,offset)\
                       (\
                       (((uint32_t)(bus)    & 0xff) << 16)|\
                       (((uint32_t)(device) & 0x3f) << 11)|\
                       (((uint32_t)(fn)     & 0x07) <<  8)|\
                       ( (uint32_t)(offset) & 0xfc)|0x80000000 )


#define PCI_PORT_ADDR  0xCF8
#define PCI_PORT_DATA  0xCFC

// Device indices for up to four drives on a standard IDE controller
#define DISK1  1
#define DISK2  2
#define DISK3  3
#define DISK4  4

//
// Global Variables and Data Structures
//

// Global flags and structures for ATA driver state management.
// Driver initialization/config flags. See ata.h for possible values.
int ATAFlag=0;

// Device port configuration (PCI, IO). See ata_pci and dev_nport structures.
struct dev_nport  dev_nport;
struct ata_pci  ata_pci;

// Buffer for device information returned by IDENTIFY DEVICE/PACKET
static _u16 *ata_devinfo_buffer;

// Last used device/channel for command optimization between ports.
_u8 ata_record_dev=0;
_u8 ata_record_channel=0;

// The current port information
struct ata_current_port_d  ATACurrentPort;

// Main controller structure.
struct ata_controller_d  AtaController;

// Array of ATA port structures for all four ports.
// ata_port[0] = Primary Master, ata_port[1] = Primary Slave,
// ata_port[2] = Secondary Master, ata_port[3] = Secondary Slave
struct ata_port_d  ata_port[4];

// Array of IDE port information structures (for diagnostic and reporting).
struct ide_port_d  ide_port[4];

// ===== IDE Port and Channel State Management =====

// IRQ handler address (optional, for custom vectoring)
unsigned long ide_handler_address=0;

// IDE channel structure array (up to 8 possible channels, for expansion/future use)
struct ide_channel_d  idechannelList[8];

// Global IDE controller structure for overall state
struct ide_d  IDE;

// Device queue pointers for scheduling among detected drives
st_dev_t *current_dev;      // Pointer to current device in queue
st_dev_t *ready_queue_dev;  // Head of device linked list (ready queue)

// pid?
// Next available device ID for enumeration and tracking
uint32_t  dev_next_pid = 0;

// DMA transfer buffer (must be physically contiguous for hardware DMA)
_u8 *dma_addr;

// Device type strings (for reporting/diagnostics)
const char *dev_type[] = {
    "ATA",
    "ATAPI"
};

// IRQ support (flag set by IRQ handler, polled by driver code)
static _u32 ata_irq_invoked = 0;

// Human-readable PCI sub-class codes (for reporting detected controllers)
static const char *ata_sub_class_code_register_strings[] = {
    "Unknown",
    "IDE Controller",
    "Unknown",
    "Unknown",
    "RAID Controller",
    "Unknown",
    "AHCI Controller"
};


unsigned long ATA_BAR0_PRIMARY_COMMAND_PORT=0;    // Primary Command Block Base Address
unsigned long ATA_BAR1_PRIMARY_CONTROL_PORT=0;    // Primary Control Block Base Address
unsigned long ATA_BAR2_SECONDARY_COMMAND_PORT=0;  // Secondary Command Block Base Address
unsigned long ATA_BAR3_SECONDARY_CONTROL_PORT=0;  // Secondary Control Block Base Address
unsigned long ATA_BAR4=0;  // Legacy Bus Master Base Address
unsigned long ATA_BAR5=0;  // AHCI Base Address / SATA Index Data Pair Base Address

// =======================================

static void ata_cmd_write(int p, int cmd_val);


static void __ata_pio_read(int p, _void *buffer,_i32 bytes);
static void __ata_pio_write(int p, _void *buffer,_i32 bytes);
static inline void __atapi_pio_read ( int p, void *buffer, uint32_t bytes );

static void ata_soft_reset(int p);
static _u8 ata_status_read(int p);



static _u8 ata_wait_irq(int p);

static int disk_ata_wait_irq(int p);

static _u8 ata_wait_not_busy(int p);
static _u8 ata_wait_busy(int p);

static _u8 ata_wait_no_drq(int p);
static _u8 ata_wait_drq(int p);

static int nport_ajuste(char nport);

static _u8 __ata_assert_dever(char nport);

static void __set_ata_addr(int p, int channel);

//
// PCI support.
//

// Read
uint32_t 
__ataReadPCIConfigAddr ( 
    int bus, 
    int dev,
    int fun, 
    int offset );

// Write
void 
__ataWritePCIConfigAddr ( 
    int bus, 
    int dev,
    int fun, 
    int offset, 
    int data );

static int 
__ataPCIConfigurationSpace ( 
    char bus, 
    char dev, 
    char fun );

static uint32_t __ataPCIScanDevice(int class);

//
// $
// INITIALIZATION
//

static int __detect_device_type(uint8_t nport);

int __ata_identify_device(char port);

static int __ata_probe_boot_disk_signature(void);
static int __ata_initialize_controller(void);

// Inicializa o IDE e mostra informações sobre o disco.
static int __ata_probe_controller(int ataflag);

// Rotina de diálogo com o driver ATA.
static int 
__ata_initialization_dialog ( 
    int msg, 
    unsigned long long1, 
    unsigned long long2 );

//
// =====================================================
//

 /*
 * diskATAIRQHandler1
 *   Handler for IRQ 14 (Primary IDE Channel). Sets the global IRQ flag.
 */
void diskATAIRQHandler1 ()
{
    ata_irq_invoked = 1;  
}

/*
 * diskATAIRQHandler2
 *   Handler for IRQ 15 (Secondary IDE Channel). Sets the global IRQ flag.
 */ 
void diskATAIRQHandler2 ()
{
    ata_irq_invoked = 1;   
}

/*
 * disk_ata_wait_irq
 *   Wait for ATA interrupt indicating operation completion on the specified port.
 *   Returns:
 *     0    = Success (IRQ received)
 *    -1    = Error detected via status register
 *   0x80   = Timeout occurred before IRQ
 *
 *   Note: Always check/clear ata_irq_invoked before and after use!
 */
static int disk_ata_wait_irq(int p)
{
   _u32 tmp = 0x10000;
   _u8 data=0;

// #bugbug
// Em nenhum momento a flag ata_irq_invoked vira TRUE.

    if (p<0)
        return -1;

    while (!ata_irq_invoked)
    {
        data = ata_status_read(p);

        // #bugbug: Review this code.
        if ( (data &ATA_SR_ERR) )
        {
            ata_irq_invoked = 0;
            return (int) -1;
        }

        //ns
        if (tmp--){
            ata_wait (100);
        }else{
            //ok por tempo esperado.
            ata_irq_invoked = 0;
            return (int) 0x80;
        };
    };
 
// ok por status da interrup��o.
    ata_irq_invoked = 0;
// ok 
    return 0;
}

/*
 * show_ide_info
 *   Prints diagnostic information for each detected IDE port.
 *   Useful for debugging and verifying device/port initialization.
 */
void show_ide_info()
{
    register int i=0;

    printf ("show_ide_info:\n");

    // four ports.
    for ( i=0; i<4; i++ ){
        printf ("\n");
        printf ("id        = %d \n", ide_port[i].id );
        printf ("used      = %d \n", ide_port[i].used );
        printf ("magic     = %d \n", ide_port[i].magic );
        printf ("type      = %d \n", ide_port[i].type );
        printf ("name      = %s \n", ide_port[i].name );
        printf ("base_port = %x \n", ide_port[i].base_port );
    };

	/*
	// Estrutura 'ata'
	// Qual lista ??
	
	//pegar a estrutura de uma lista.
	
	//if( ata != NULL )
	//{
		printf("ata:\n");
 	    printf("type={%d}\n", (int) AtaController.chip_control_type);
	    printf("channel={%d}\n", (int) ata.channel);
	    printf("devType={%d}\n", (int) ata.dev_type);
	    printf("devNum={%d}\n", (int) ata.dev_num);
	    printf("accessType={%d}\n", (int) ata.access_type);
	    printf("cmdReadMode={%d}\n", (int) ata.cmd_read_modo);
	    printf("cmdBlockBaseAddress={%d}\n", (int) ata.cmd_block_base_address);
	    printf("controlBlockBaseAddress={%d}\n", (int) ata.ctrl_block_base_address);
		printf("busMasterBaseAddress={%d}\n", (int) ata.bus_master_base_address);
		printf("ahciBaseAddress={%d}\n", (int) ata.ahci_base_address);
	//};
	*/

	// Estrutura 'atapi'
	// Qual lista ??

	// Estrutura 'st_dev'
	// Est�o na lista 'ready_queue_dev'

    //...
}

// =========================
// Low-Level Data Transfer Routines
// =========================

/*
 * __ata_pio_read
 *   Reads data from the ATA data register into a buffer using PIO (Programmed I/O).
 *   - p: Port index (0-3), refers to ata_port[p].
 *   - buffer: Destination buffer for read data.
 *   - bytes: Number of bytes to read (must be even, as insw reads 16 bits).
 *   Uses 'rep; insw' to efficiently read 'bytes/2' words from the port's data register.
 */
static void __ata_pio_read ( int p, void *buffer, _i32 bytes )
{

    /*
    clear_backbuffer();
    printf("Root: >>>>>>>>>>>>>>>>>>>>> [%d]\n", p);
    refresh_screen();
    while(1){}
    */

    asm volatile (\
        "cld;\
        rep; insw"::"D"(buffer),\
        "d"(ata_port[p].cmd_block_base_address + ATA_REG_DATA),\
        "c"(bytes/2) );
}

/*
 * __ata_pio_write
 *   Writes data from a buffer to the ATA data register using PIO.
 *   - p: Port index (0-3), refers to ata_port[p].
 *   - buffer: Source buffer for write data.
 *   - bytes: Number of bytes to write (must be even).
 *   Uses 'rep; outsw' to efficiently write 'bytes/2' words to the port's data register.
 */
static void __ata_pio_write ( int p, void *buffer, _i32 bytes )
{
    asm volatile (\
        "cld;\
        rep; outsw"::"S"(buffer),\
        "d"(ata_port[p].cmd_block_base_address + ATA_REG_DATA),\
        "c"(bytes/2) );
}

/*
 * __atapi_pio_read
 *   PIO read routine specialized for ATAPI devices (e.g., CD-ROM).
 *   - p: Port index (0-3), refers to ata_port[p].
 *   - buffer: Destination buffer.
 *   - bytes: Number of bytes to read.
 *   Uses 'rep; insw' for efficient transfer.
 */
static inline void __atapi_pio_read ( int p, void *buffer, uint32_t bytes )
{
    asm volatile (\
        "cld;\
        rep; insw"::"D"(buffer),\
        "d"(ata_port[p].cmd_block_base_address + ATA_REG_DATA),\
        "c"(bytes/2) );
}

// ====================

/*
 * disk_get_ata_irq_invoked
 *   Returns the current value of the global ATA IRQ flag.
 *   Used to check if an ATA interrupt has occurred.
 */
int disk_get_ata_irq_invoked()
{
    return (int) ata_irq_invoked;
}

/*
 * disk_reset_ata_irq_invoked
 *   Clears the global ATA IRQ flag, resetting interrupt state.
 */
void disk_reset_ata_irq_invoked ()
{
    ata_irq_invoked = 0;
}

/*
 * ata_wait
 *   Simple delay loop for timing gaps between I/O operations.
 *   - val: Approximate microseconds to wait (divided by 100).
 */
void ata_wait(int val)
{
    val /= 100;

    if (val < 0){
        val=1;
    }

    while (val--){
        io_delay();
    };
}

// #TODO: 
// Nelson, ao configurar os bits BUSY e DRQ. 
// Devemos verificar retornos de erros.

/*
 * ata_wait_not_busy
 *   Waits until the BSY (busy) bit is cleared for the specified port.
 *   Returns 0 if BSY clears, 1 if error.
 *   Use this after issuing a command to ata_port[p].
 */
static _u8 ata_wait_not_busy(int p)
{
    if (p<0)
        return 1;

    while ( ata_status_read(p) & ATA_SR_BSY ){
        if ( ata_status_read(p) & ATA_SR_ERR ){
            return 1;
        }
    };

    return 0;
}

/*
 * ata_wait_busy
 *   Waits for the BSY (busy) bit to be set for the port.
 *   Returns 0 if successful, 1 if error.
 *   Less commonly used; for port p (0-3).
 */
static _u8 ata_wait_busy(int p)
{
    if (p<0)
        return 1;

    while ( !(ata_status_read(p) & ATA_SR_BSY ) )
    {
        if ( ata_status_read(p) & ATA_SR_ERR ){
            return 1;
        }
    };

    return 0;
}

/*
 * ata_wait_no_drq
 *   Waits until the DRQ (Data Request) bit is cleared for the port.
 *   Returns 0 if successful, 1 if error.
 */
static _u8 ata_wait_no_drq(int p)
{
    if (p<0)
        return 1;

    while ( ata_status_read(p) & ATA_SR_DRQ )
    {
        if (ata_status_read(p) & ATA_SR_ERR){
            return 1;
        }
    }

    return 0;
}

/*
 * ata_wait_drq
 *   Waits until the DRQ (Data Request) bit is set for the port.
 *   Returns 0 if successful, 1 if error.
 */
static _u8 ata_wait_drq(int p)
{
    if (p<0)
        return 1;

    while ( !(ata_status_read(p) & ATA_SR_DRQ) )
    {
        if (ata_status_read(p) & ATA_SR_ERR){
            return 1;
        }
    };

    return 0;
}

/*
 * ata_wait_irq
 *   Waits for an ATA IRQ (interrupt) or times out for the specified port.
 *   Returns 0 on IRQ, -1 on error, 0x80 on timeout.
 */
static _u8 ata_wait_irq(int p)
{
   _u8 Data=0;
   _u32 tmp = 0x10000;

   if (p<0)
       return -1;

    while (!ata_irq_invoked)
    {
        Data = ata_status_read(p);
        
        if ( (Data & ATA_SR_ERR) )
        {
            ata_irq_invoked = 0;
            return -1;
        }

        // ns
        
        if (--tmp){ 
            ata_wait (100); 
        }else{
            ata_irq_invoked = 0;
            return 0x80;
        };
    };
 
    ata_irq_invoked = 0;

    return 0;
}

/*
 * ata_soft_reset
 *   Issues a soft reset to the ATA device on the specified port (ata_port[p]).
 *   Toggles the SRST (Soft Reset) bit in the control register.
 *   Used for recovery and device signature detection per port.
 */
static void ata_soft_reset(int p)
{
    _u8 Data=0;

    if (p<0)
        return;

    Data = in8(ata_port[p].ctrl_block_base_address + 2);
    out8( 
        ata_port[p].ctrl_block_base_address, 
        Data | 0x4 );
    out8( 
        ata_port[p].ctrl_block_base_address, 
        Data & 0xfb ); 
}

// #bugbug
// Read the status of a 'given disk'.
// If everything is ok with the structure.

/*
 * ata_status_read
 *   Reads the ATA status register for the given port (ata_port[p]).
 *   Returns the raw status byte.
 */
static _u8 ata_status_read(int p)
{
    _u8 Value=0;

    //if (p<0)
        //return 1;

    Value = (_u8) in8( ata_port[p].cmd_block_base_address + ATA_REG_STATUS );
    return (_u8) Value;
}

/*
 * ata_cmd_write
 *   Issues a command byte to the ATA command register for port ata_port[p].
 *   Waits for the device to be not busy before issuing.
 *   After writing, a short delay ensures command acceptance.
 */
static void ata_cmd_write(int p, int cmd_val)
{
    ata_wait_not_busy(p);
    out8( ata_port[p].cmd_block_base_address + ATA_REG_CMD, cmd_val );
    ata_wait(400);  
}

/*
 * __ata_assert_dever
 *   Set up ata_port[nport] for the selected port (0-3).
 *   Maps nport to standard legacy ATA meaning:
 *     0=Primary Master, 1=Primary Slave, 2=Secondary Master, 3=Secondary Slave.
 *   Must be called before port-specific register access.
 *   Returns 0 on success, -1 on invalid port.
 */
static _u8 __ata_assert_dever(char nport)
{
    if (nport>3){
        return -1;
    }

    switch (nport){

    // Primary master.
    case 0:
        ata_port[nport].channel = ATA_PRIMARY; //0;  // Primary
        ata_port[nport].dev_num = ATA_MASTER_DEV;  //0;  // Not slave
        break;

    //  Primary slave.
    case 1:   
        ata_port[nport].channel = ATA_PRIMARY;  //0;  // Primary
        ata_port[nport].dev_num = ATA_SLAVE_DEV;  //1;  // Slave
        break;

    // Secondary master.
    case 2:
        ata_port[nport].channel = ATA_SECONDARY;  //1;  // Secondary
        ata_port[nport].dev_num = ATA_MASTER_DEV;  //0;  // Not slave
        break;

    // Secondary slave.
    case 3:
        ata_port[nport].channel = ATA_SECONDARY;  //1;  // Secondary
        ata_port[nport].dev_num = ATA_SLAVE_DEV;  //1;  // Slave
        break;

    // Fail
    default:
        printf("blgram: __ata_assert_dever()\n");
        printf("Port %d, value not used\n", nport );
        goto fail;
        break;
    };

    __set_ata_addr (
        nport, 
        ata_port[nport].channel );

    return 0;

fail:
    return (_u8) -1;
}

/*
 * __set_ata_addr
 *   Sets the ATA base address registers for the selected channel (0=Primary, 1=Secondary).
 *   Populates ata_port[p] fields for command/control/bus master addresses.
 *   For AHCI in the future, this would be replaced by port selection and MMIO base.
 */
static void __set_ata_addr(int p, int channel)
{
    if (channel < 0){
        printf ("__set_ata_addr: [FAIL] channel\n");
        return;
    }

// #bugbug
// Porque estamos checando se � prim�rio ou secundario?

    switch (channel){

        case ATA_PRIMARY:
            // 0 and 1
            ata_port[p].cmd_block_base_address  = ATA_BAR0_PRIMARY_COMMAND_PORT;
            ata_port[p].ctrl_block_base_address = ATA_BAR1_PRIMARY_CONTROL_PORT;
            ata_port[p].bus_master_base_address = ATA_BAR4;
            break;

        case ATA_SECONDARY:
            // 2 and 3
            ata_port[p].cmd_block_base_address  = ATA_BAR2_SECONDARY_COMMAND_PORT;
            ata_port[p].ctrl_block_base_address = ATA_BAR3_SECONDARY_CONTROL_PORT;
            ata_port[p].bus_master_base_address = ATA_BAR4 + 8;
            break;

        //default:
            //PANIC
            //break;
    };
}

/*
 * dev_switch
 *   Advances to next device in the ready_queue_dev linked list.
 *   Wraps to start if at end. Used for cycling through all detected devices.
 *   Ensures driver can cycle through all detected ports/devices.
 */
static inline void dev_switch(void)
{
    // Pula, se ainda não tiver nenhuma unidade?
    if (!current_dev){
        return;
    }
    // Advance to next device, or wrap to start of list
    current_dev = current_dev->next;    

    if (!current_dev){
        current_dev = ready_queue_dev;
    }
}

/*
 * getpid_dev
 *   Returns the device ID of the currently selected device.
 *   Used for process management or higher-level device enumeration.
 */
static inline int getpid_dev()
{
    if ((void*) current_dev == NULL){
        printf("getpid_dev: [FAIL] Invalid pointer\n");
        return -1;
    }
    return current_dev->dev_id;
}

/*
 * getnport_dev
 *   Returns the port index (0-3) of the currently selected device.
 */
static inline int getnport_dev()
{
    if ((void*) current_dev == NULL){
        printf("getnport_dev: [FAIL] Invalid pointer\n");
        return -1;
    }
    return current_dev->dev_nport;
}

/*
 * nport_ajuste
 *   Advances the device queue until the selected device matches requested port (nport).
 *   Returns 0 on success, -1 on failure (e.g., port not found after 4 tries).
 *   Ensures correct device is active before issuing commands.
 */
static int nport_ajuste(char nport)
{
    _i8 i=0;

// #todo: 
// Simplify.

    while ( nport != getnport_dev() )
    {
        if (i == 4){ 
            goto fail; 
        }
        dev_switch();
        i++;
    };

    if ( getnport_dev() == -1 )
    { 
        goto fail;
    }

// ok
    return 0;
fail:
    return (int) -1;
}

/*
 * ata_set_device_and_sector
 *   Prepares ATA registers in ata_port[nport] to specify device, sector (LBA), and access mode.
 *   Supports both LBA28 and LBA48 addressing.
 *   - count:      Number of sectors to transfer.
 *   - addr:       LBA of the first sector.
 *   - access_type: 28 for LBA28, 48 for LBA48.
 *   - nport:      ATA port index (0-3, see ata_port[4]).
 *   Always call this before issuing a command, passing the correct nport.
 */
static inline void ata_set_device_and_sector ( 
    _u32 count, 
    _u64 addr,
    _i32 access_type, 
    _i8 nport )
{
    __ata_assert_dever(nport);  // Ensure channel/device is selected

// Access type.

    switch (access_type){
        // 28-bit LBA mode
        case 28:
            out8 ( ata_port[nport].cmd_block_base_address + ATA_REG_SECCOUNT, count );   // Sector count (7:0)
            out8 ( ata_port[nport].cmd_block_base_address + ATA_REG_LBA0, addr );        // LBA bits 7-0   
            out8 ( ata_port[nport].cmd_block_base_address + ATA_REG_LBA1, addr >> 8 );   // LBA bits 15-8
            out8 ( ata_port[nport].cmd_block_base_address + ATA_REG_LBA2, addr >> 16 );  // LBA bits 23-16

            // Set device register: LBA mode, device, and LBA bits 27-24
            out8 ( ata_port[nport].cmd_block_base_address + ATA_REG_DEVSEL, 
                0x40 | (ata_port[nport].dev_num << 4) | (addr >> 24 &0x0f) );
     
            // If a different device/channel is being used, wait 400ns as per ATA spec
            if ( ata_record_dev != ata_port[nport].dev_num && 
                 ata_record_channel != ata_port[nport].channel )
            {
                ata_wait(400);
                
                //verifique erro
                ata_record_dev = ata_port[nport].dev_num;
                ata_record_channel  = ata_port[nport].channel;
            }
            break;

        // 48-bit LBA mode (large disks, modern drives)
        case 48:
            // First: higher bytes of sector count and LBA
            out8( ata_port[nport].cmd_block_base_address + ATA_REG_SECCOUNT,0);       // Sector Count 15:8
            out8( ata_port[nport].cmd_block_base_address + ATA_REG_LBA0,addr >> 24);  // LBA 31-24   
            out8( ata_port[nport].cmd_block_base_address + ATA_REG_LBA1,addr >> 32);  // LBA 39-32
            out8( ata_port[nport].cmd_block_base_address + ATA_REG_LBA2,addr >> 40);  // LBA 47-40
            // Then: lower bytes (sector count and LBA)
            out8( ata_port[nport].cmd_block_base_address + ATA_REG_SECCOUNT,count);   // Sector Count 7:0
            out8( ata_port[nport].cmd_block_base_address + ATA_REG_LBA0,addr);        // LBA 7-0   
            out8( ata_port[nport].cmd_block_base_address + ATA_REG_LBA1,addr >> 8);   // LBA 15-8
            out8( ata_port[nport].cmd_block_base_address + ATA_REG_LBA2,addr >> 16);  // LBA 23-16

            out8 ( ata_port[nport].cmd_block_base_address + ATA_REG_DEVSEL,
                0x40 | ata_port[nport].dev_num << 4 );   // Modo LBA active, Select device,        

            // Wait 400ns if switching device/channel
            // Verifique se e a mesma unidade para nao esperar pelos 400ns.
            if ( ata_record_dev     != ata_port[nport].dev_num && 
                 ata_record_channel != ata_port[nport].channel )
            {
                ata_wait(400);
                ata_record_dev     = ata_port[nport].dev_num;
                ata_record_channel = ata_port[nport].channel;
            }
            break;

        // CHS mode (obsolete, not supported)
        case 0:
            break;

        // Default ??
    };
}

// ==========================
// O que segue � um suporte ao controlador de DMA 
// para uso nas rotinas de IDE.

//
// DMA support
//

// ============
// Legacy Bus Master Base Address
// #todo Nelson, N�o se esque�a de habiliatar o // Bus Master Enable
// no espa�o de configura�ao PCI (offset 0x4 Command Register)

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


// ==========================
// DMA (Direct Memory Access) Support
// ==========================

/*
 * ide_dma_prdt
 *   Physical Region Descriptor Table (PRDT) for IDE DMA operations.
 *   Each entry describes a memory region for DMA transfer.
 *   For up to 4 ports/devices.
 */
struct {
    uint32_t addr;  // Physical address of buffer
    uint32_t len;   // Length and flags (bit 31 = end of table)
}ide_dma_prdt[4];

/*
 * ide_dma_data
 *   Prepares the PRDT and bus master registers for a DMA transfer.
 *
 *   addr:      Virtual address of the DMA buffer.
 *   byte_count: Number of bytes to transfer.
 *   flg:       Direction flag (0=read, 1=write); overridden to write in this code.
 *   nport:     Port index (0-3).
 *
 *   This sets up the hardware for a subsequent DMA transfer.
 *   For multi-port: each device/port uses its own PRDT entry.
 */
void 
ide_dma_data ( 
    void *addr, 
    uint16_t byte_count,
    uint8_t flg,
    uint8_t nport )
{
    _u8 data=0;
    uint32_t phy=0;

// #todo: Check limits.

    // Set physical buffer and size (with end-of-table flag)
    ide_dma_prdt[nport].addr = (_u32) addr;  //@todo: (&~1)sera que e necessario?
    ide_dma_prdt[nport].len  = byte_count | 0x80000000;

    phy = (uint32_t) &ide_dma_prdt[nport];

    // Write PRDT physical address to bus master register
    out32 ( ata_port[nport].bus_master_base_address + ide_dma_reg_addr, phy );
 
// (bit 3 read/write)
// 0 = Memory reads.
// 1 = Memory writes.

    // Set direction (bit 3); 0=read from disk, 1=write to disk
    data = in8 ( ata_port[nport].bus_master_base_address + ide_dma_reg_cmd ) &~8;

// TODO bit 8 Conflito no Oracle VirtualBox
// Obs: Isso foi enviado via argumento e agora foi alerado.

    // Always set to write in this code; for reads use 0
    flg = 1; 

    out8 ( 
        ata_port[nport].bus_master_base_address + ide_dma_reg_cmd, 
        data | flg << 3 );

    // Clear interrupt and error bits in status register
    data = in8 ( ata_port[nport].bus_master_base_address + ide_dma_reg_status );    
    out8 ( 
        ata_port[nport].bus_master_base_address + ide_dma_reg_status, 
        data & ~6 );
}

/*
 * ide_dma_start
 *   Starts the DMA transfer by setting the bus master 'start' bit.
 */
void ide_dma_start(int p)
{
    _u8 Data = 0; 
    
    Data = in8 ( ata_port[p].bus_master_base_address + ide_dma_reg_cmd );
    out8 ( 
        ata_port[p].bus_master_base_address + ide_dma_reg_cmd, 
        Data | 1 );
}

/*
 * ide_dma_stop
 *   Stops the DMA transfer and clears interrupt/error bits.
 */
void ide_dma_stop(int p)
{
    _u8 Data=0;

    Data = in8 ( ata_port[p].bus_master_base_address + ide_dma_reg_cmd );  
    out8( 
        ata_port[p].bus_master_base_address + ide_dma_reg_cmd, 
        Data & ~1);

    Data = in8 ( ata_port[p].bus_master_base_address + ide_dma_reg_status );
    out8( 
        ata_port[p].bus_master_base_address + ide_dma_reg_status, 
        Data & ~6);
}

/*
 * ide_dma_read_status
 *   Worker
 *   Reads the IDE DMA status register to check the result of a DMA operation.
 *   Returns the raw status byte for interpretation by higher-level routines.
 */
int ide_dma_read_status(int p)
{
    int Status=0;
    Status = in8 ( ata_port[p].bus_master_base_address + ide_dma_reg_status );
    return Status;
}

// pci support
// #todo:
// Checar se temos uma lista dessa no suporte a PCI.
// #bugbug
// That 'Unknown' thing in the bottom of the list.

const char *pci_classes[] = {
    "Unknown [old]",
    "Mass storage",
    "Network",
    "Display",
    "Multimedia device",
    "Memory",
    "Bridge device",
    "Simple Communication",
    "Base System Peripheral",
    "Input Device",
    "Docking Station",
    "Processor",
    "Serial Bus",
    "Wireless",
    "Inteligent I/O",
    "Satellite Communications",
    "Encrypt/Decrypt",
    "Data acquisition and signal processing",
    [255]="Unknown"
};

/*
 * __ataReadPCIConfigAddr
 *   Reads a 32-bit word from the PCI configuration space at the specified location.
 *   Used for reading device IDs, BARs, class codes, etc. during controller probing.
 *   Args: bus, dev, fun, offset (all integers)
 */
uint32_t 
__ataReadPCIConfigAddr ( 
    int bus, 
    int dev,
    int fun, 
    int offset )
{

// #bugbug
// Do not use macros.
// Expand this macro outside the function.
 
    out32 ( 
        PCI_PORT_ADDR, 
        CONFIG_ADDR( bus, dev, fun, offset ) );

    return (uint32_t) in32 (PCI_PORT_DATA);
}

/*
 * __ataWritePCIConfigAddr
 *   Writes a 32-bit word to the PCI configuration space at the specified location.
 *   Used for enabling/disabling features (e.g., DMA, interrupts) in the controller.
 */
void 
__ataWritePCIConfigAddr ( 
    int bus, 
    int dev,
    int fun, 
    int offset, 
    int data )
{

// #bugbug
// Do not use macros.
// Expand this macro outside the function.

    out32 ( 
        PCI_PORT_ADDR, 
        CONFIG_ADDR( bus, dev, fun, offset ) );
    out32 ( PCI_PORT_DATA, data );
}

// __ataPCIConfigurationSpace:
// Getting information:
// Let's fill the structure with some information about the device.
// Mass storage device only.
static int 
__ataPCIConfigurationSpace ( 
    char bus, 
    char dev, 
    char fun )
{
    uint32_t data=0;

// Controller type.
    AtaController.chip_control_type = STORAGE_CONTROLLER_MODE_UNKNOWN;


    printf ("Initializing PCI Mass Storage support..\n");

// Indentification Device e
// Salvando configura��es.

    data = (uint32_t) __ataReadPCIConfigAddr( bus, dev, fun, 0 );

    ata_pci.vendor_id = data       & 0xffff;
    ata_pci.device_id = data >> 16 & 0xffff;

    //printf ("\nDisk info:\n");
    printf("[ Vendor ID: %X,Device ID: %X ]\n", 
        ata_pci.vendor_id, 
        ata_pci.device_id );

// ======================================================
// Getting information about the PCI device.
// class, subclass, prog if and revision id.

    data = (uint32_t) __ataReadPCIConfigAddr( bus, dev, fun, 8 );

// Class and subclass
    ata_pci.class       = data >> 24 & 0xff;
    ata_pci.subclass    = data >> 16 & 0xff;

// prog if
    ata_pci.prog_if     = data >>  8 & 0xff;
// revision id
    ata_pci.revision_id = data       & 0xff;

// ===========================================
// Detecting the device subclass base on the information above.


// SCSI
    if ( ata_pci.class == PCI_CLASS_MASS && 
         ata_pci.subclass == STORAGE_CONTROLLER_MODE_SCSI ){

        AtaController.chip_control_type = STORAGE_CONTROLLER_MODE_SCSI; // :)
        printf ("SCSI not supported\n");
        return (int) PCI_MSG_ERROR;

//
//  ## IDE ##
//

    // 1:1 = ATA
    } else if ( ata_pci.class == PCI_CLASS_MASS && 
         ata_pci.subclass == STORAGE_CONTROLLER_MODE_ATA ){

        AtaController.chip_control_type = STORAGE_CONTROLLER_MODE_ATA; // :)

        // IDE
        //#debug
        //printf (">>> IDE \n");
        //refresh_screen();
        //while(1){}
        //refresh_screen();

        // Compatibilidade e nativo, primary.
        data  = __ataReadPCIConfigAddr( bus, dev, fun, 8 );
        if (data & 0x200)
        {
            __ataWritePCIConfigAddr ( 
                bus, dev, fun, 
                8, (data | 0x100) ); 
        }

        // Compatibilidade e nativo, secundary.
        data = __ataReadPCIConfigAddr( bus, dev, fun, 8 );
        if (data & 0x800)
        { 
            __ataWritePCIConfigAddr ( 
                bus, dev, fun, 
                8, (data | 0x400) ); 
        }

        data = __ataReadPCIConfigAddr( bus, dev, fun, 8 );
        if (data & 0x8000)
        {
            // Bus Master Enable
            data = __ataReadPCIConfigAddr(bus,dev,fun,4);
            __ataWritePCIConfigAddr(bus,dev,fun,4,data | 0x4);
        } 

        // Habilitar interrupcao (INTx#)
        data = __ataReadPCIConfigAddr( bus, dev, fun, 4 );
        __ataWritePCIConfigAddr( bus, dev, fun, 4, data & ~0x400);

        // IDE Decode Enable
        data = __ataReadPCIConfigAddr( bus, dev, fun, 0x40 );
        __ataWritePCIConfigAddr( bus, dev, fun, 0x40, data | 0x80008000 );

        // Synchronous DMA Control Register
        // Enable UDMA
        data = __ataReadPCIConfigAddr( bus, dev, fun, 0x48 );
        __ataWritePCIConfigAddr( bus, dev, fun, 0x48, data | 0xf);

        printf("[ Sub Class Code %s Programming Interface %d Revision ID %d ]\n",\
            ata_sub_class_code_register_strings[AtaController.chip_control_type],
            ata_pci.prog_if,
            ata_pci.revision_id );

//
//  ## RAID ##
//

    // 1:4 = RAID
    } else if ( ata_pci.class == PCI_CLASS_MASS && 
                ata_pci.subclass == STORAGE_CONTROLLER_MODE_RAID ){

        AtaController.chip_control_type = STORAGE_CONTROLLER_MODE_RAID; // :)
        printf ("RAID not supported\n");
        return (int) PCI_MSG_ERROR;

//
//  ## ATA DMA ## 
//

    // 1:5 = ATA with dma
    } else if ( ata_pci.class == PCI_CLASS_MASS && 
                ata_pci.subclass == STORAGE_CONTROLLER_MODE_DMA ){

        AtaController.chip_control_type = STORAGE_CONTROLLER_MODE_DMA; // :)
        printf ("ATA DMA not supported\n");
        return (int) PCI_MSG_ERROR;

//
//  ## ACHI ##  SATA
//

    // 1:6 = SATA
    } else if ( ata_pci.class == PCI_CLASS_MASS && 
                ata_pci.subclass == STORAGE_CONTROLLER_MODE_AHCI ){

        AtaController.chip_control_type = STORAGE_CONTROLLER_MODE_AHCI; // :)

        // ACHI
        //#debug
        //printf (">>> SATA \n");
        //while(1){}
        //refresh_screen();

        // Compatibilidade e nativo, primary.
        data = __ataReadPCIConfigAddr ( bus, dev, fun, 8 );
        if (data & 0x200)
        {
            __ataWritePCIConfigAddr ( 
                bus, dev, fun, 
                8, data | 0x100 ); 
        }

        // Compatibilidade e nativo, secundary.
        data = __ataReadPCIConfigAddr ( bus, dev, fun, 8 );
        if (data & 0x800)
        {
            __ataWritePCIConfigAddr ( 
                bus, dev, fun, 
                8, data | 0x400 ); 
        }

        // ??
        data = __ataReadPCIConfigAddr ( bus, dev, fun, 8 );
        if (data & 0x8000) 
        {
            // Bus Master Enable.
            data = __ataReadPCIConfigAddr ( bus, dev, fun, 4 );
            __ataWritePCIConfigAddr ( bus, dev, fun, 4, data | 0x4 );
        } 

        // IDE Decode Enable
        data = __ataReadPCIConfigAddr ( bus, dev, fun, 0x40 );
        __ataWritePCIConfigAddr ( bus, dev, fun, 0x40, data | 0x80008000 );

        // Habilitar interrupcao (INTx#)
        data = __ataReadPCIConfigAddr ( bus, dev, fun, 4 );
        __ataWritePCIConfigAddr ( bus, dev, fun, 4, data & ~0x400);

        printf("[ Sub Class Code %s Programming Interface %d Revision ID %d ]\n",\
            ata_sub_class_code_register_strings[AtaController.chip_control_type], 
            ata_pci.prog_if,
            ata_pci.revision_id );


    // 1:8 = NVME
    } else if ( ata_pci.class == PCI_CLASS_MASS && 
                ata_pci.subclass == STORAGE_CONTROLLER_MODE_NVME ){

        AtaController.chip_control_type = STORAGE_CONTROLLER_MODE_NVME; // :)
        printf ("NVME not supported\n");
        return (int) PCI_MSG_ERROR;

    // 1:9 = SAS
    } else if ( ata_pci.class == PCI_CLASS_MASS && 
                ata_pci.subclass == STORAGE_CONTROLLER_MODE_SAS ){

        AtaController.chip_control_type = STORAGE_CONTROLLER_MODE_SAS; // :)
        printf ("SAS not supported\n");
        return (int) PCI_MSG_ERROR;

    // Fail
    // ?:? = Class/subclass not supported.
    } else {

        AtaController.chip_control_type = STORAGE_CONTROLLER_MODE_UNKNOWN; // :)
        printf ("Unknown controller type. Class=%d Subclass=%d\n", 
            ata_pci.class, ata_pci.subclass);
        return (int) PCI_MSG_ERROR;
    };

// #obs:
// Nesse momento j� sabemos se � IDE, RAID, AHCI.
// Vamos pegar mais informa��es,
// Salvaremos as informa��es na estrutura.
// PCI cacheline, Latancy, Headr type, end BIST

    data = __ataReadPCIConfigAddr ( bus, dev, fun, 0xC );

    ata_pci.primary_master_latency_timer = data >>  8 & 0xff;
    ata_pci.header_type                  = data >> 16 & 0xff;
    ata_pci.BIST                         = data >> 24 & 0xff;

// ========================
// BARs
    ata_pci.bar0 = __ataReadPCIConfigAddr( bus, dev, fun, 0x10 );
    ata_pci.bar1 = __ataReadPCIConfigAddr( bus, dev, fun, 0x14 );
    ata_pci.bar2 = __ataReadPCIConfigAddr( bus, dev, fun, 0x18 );
    ata_pci.bar3 = __ataReadPCIConfigAddr( bus, dev, fun, 0x1C );
    ata_pci.bar4 = __ataReadPCIConfigAddr( bus, dev, fun, 0x20 );
    ata_pci.bar5 = __ataReadPCIConfigAddr( bus, dev, fun, 0x24 );
// ========================
// Interrupt
    data = __ataReadPCIConfigAddr( bus, dev, fun, 0x3C );
    ata_pci.interrupt_line = data      & 0xff;
    ata_pci.interrupt_pin  = data >> 8 & 0xff;

// ========================
// PCI command and status.
    data = __ataReadPCIConfigAddr( bus, dev, fun, 4 );
    ata_pci.command = data       & 0xffff; 
    ata_pci.status  = data >> 16 & 0xffff;

// ------------------------

    // #debug
    //printf ("[ Command %x Status %x ]\n", 
        //ata_pci.command, 
        //ata_pci.status );

    // #debug
    //printf ("[ Interrupt Line %x Interrupt Pin %x ]\n", 
        //ata_pci.interrupt_pin, 
        //ata_pci.interrupt_line );

// ================
// Get Synchronous DMA Control Register.

    //??
    data = __ataReadPCIConfigAddr(bus,dev,fun,0x48);

    //printf ("[ Synchronous DMA Control Register %X ]\n", data );
    //refresh_screen();

    return (int) PCI_MSG_SUCCESSFUL;
}

// __ataPCIScanDevice:
// Get the bus/dev/fun for a device given the class.
static uint32_t __ataPCIScanDevice(int class)
{
    uint32_t data = -1;
    int bus=0; 
    int dev=0; 
    int fun=0;

// =============
// Probe

    for ( bus=0; bus < 256; bus++ )
    {
        for ( dev=0; dev < 32; dev++ )
        {
            for ( fun=0; fun < 8; fun++ )
            {
                out32 ( PCI_PORT_ADDR, CONFIG_ADDR( bus, dev, fun, 0x8) );
                
                data = in32 (PCI_PORT_DATA);
                
                // #todo
                // We need a class variable outside the if statement.
                // ex: ClassValue = data >> 24 & 0xff;
                
                if ( ( data >> 24 & 0xff ) == class )
                {
                    // #todo: Save this information.
                    printf ("[ Detected PCI device: %s ]\n", 
                        pci_classes[class] );

                    // Done
                    
                    // #todo
                    // Put this into a variable.
                    
                    // XXXValue = ( fun + (dev*8) + (bus*32) );
                    // return (uint32_t) XXXValue;
                    
                    return (uint32_t) ( fun + (dev*8) + (bus*32) );
                }
            };
        };
    };

// Fail
    printf ("[ PCI device NOT detected ]\n");
    refresh_screen ();
    return (uint32_t) (-1);
}

/*
 * __detect_device_type
 *   Determines the type of device connected to ata_port[nport] (0-3).
 *   Reads the device signature after a soft reset and IDENTIFY command.
 *   Returns a constant indicating device type (ATADEV_PATA, ATADEV_PATAPI, etc).
 *   Essential for distinguishing hard drives, ATAPI devices, SATA, etc.
 *   Called during enumeration for each port.
 */
static int __detect_device_type(uint8_t nport)
{
    _u8 status=0;

    // Signature bytes returned by IDENTIFY command
    unsigned char sigbyte1=0;
    unsigned char sigbyte2=0;

	int spin=0;
    int st=0;

    if (nport > 3){
        goto fail;
    }

    __ata_assert_dever(nport);

// Bus with no devices.
// See:
// https://wiki.osdev.org/ATA_PIO_Mode
// Before sending any data to the IO ports, read the Regular Status byte. 
// The value 0xFF is an illegal status value, and indicates that the bus has no drives. 

    // Check for floating bus (no device present)
    // Per ATA spec, status register returns 0xFF if no device is attached
    if ( ata_status_read(nport) == 0xFF )
    {
        printf ("0xFF: Bus with no devices.\n");
        printf ("Floating Bus?\n");
        //refresh_screen();
        goto fail;
    }

    // Reset the device and wait for it to become ready
    ata_soft_reset(nport);
    ata_wait_not_busy(nport);

    // Clear registers before sending IDENTIFY
    out8 ( ata_port[nport].cmd_block_base_address + ATA_REG_SECCOUNT, 0 );  // Sector Count 7:0
    out8 ( ata_port[nport].cmd_block_base_address + ATA_REG_LBA0, 0 );      // LBA 7-0
    out8 ( ata_port[nport].cmd_block_base_address + ATA_REG_LBA1, 0 );      // LBA 15-8
    out8 ( ata_port[nport].cmd_block_base_address + ATA_REG_LBA2, 0 );      // LBA 23-16

// Select device.

    // #suspended
    //out8 ( 
        //ata.cmd_block_base_address + ATA_REG_DEVSEL, 
        //0xE0 | ata.dev_num << 4 );

    // Select device (master/slave)
    out8 ( 
        ata_port[nport].cmd_block_base_address + ATA_REG_DEVSEL, 
        0xA0 | ata_port[nport].dev_num << 4 );
    ata_wait(400);

    //#todo
	ata_wait_not_busy(nport);

    // Send IDENTIFY DEVICE command
    ata_cmd_write (nport,ATA_CMD_IDENTIFY_DEVICE); 
    ata_wait (400);

/*
// #old
// Channel with no device
    if ( ata_status_read(nport) == 0 ){
        printf ("0x00: Channel with no devices.\n");
        goto fail;
    }
*/

    // Wait for device to clear BSY (not busy)
	// Ignora bit de erro
	spin = 1000000;
    while (spin--) { 	
		if ( !(ata_status_read(nport) & ATA_SR_BSY) )
            break;
	};

	st = ata_status_read(nport);
	if (!st){
		return (int) ATADEV_UNKNOWN;
	}

    // Read signature bytes
    sigbyte1 = (unsigned char) in8( ata_port[nport].cmd_block_base_address + ATA_REG_LBA1 );
    sigbyte2 = (unsigned char) in8( ata_port[nport].cmd_block_base_address + ATA_REG_LBA2 );
    // more 2 bytes ...

//
// # type # 
//

    // Check for invalid signatures
    if ( sigbyte1 == 0x7F && sigbyte2 == 0x7F )
    {
        goto fail0;
    }
    if ( sigbyte1 == 0xFF && sigbyte2 == 0xFF )
    {
        goto fail0;
    }

//
// Identify device by signature
//

// ATA handle two interface standard, PATA and SATA.

// -----------------------

// PATA
    if ( sigbyte1 == STORAGE_INTERFACE_STANDARD_PATA_SIG1 && 
         sigbyte2 == STORAGE_INTERFACE_STANDARD_PATA_SIG2 )
    {
        return (int) ATADEV_PATA;
    }
// PATAPI
    if ( sigbyte1 == STORAGE_INTERFACE_STANDARD_PATAPI_SIG1 && 
         sigbyte2 == STORAGE_INTERFACE_STANDARD_PATAPI_SIG2 )
    {
        return (int) ATADEV_PATAPI;
    }

// -----------------------

// SATA
    if ( sigbyte1 == STORAGE_INTERFACE_STANDARD_SATA_SIG1 && 
         sigbyte2 == STORAGE_INTERFACE_STANDARD_SATA_SIG2 )
    {
        return (int) ATADEV_SATA;
    }
// SATAPI
    if ( sigbyte1 == STORAGE_INTERFACE_STANDARD_SATAPI_SIG1 && 
         sigbyte2 == STORAGE_INTERFACE_STANDARD_SATAPI_SIG2 )
    {
        return (int) ATADEV_SATAPI;
    }

fail0:
    printf("Invalid signature sig1={%x} sig2={%x}\n", sigbyte1, sigbyte2 );
fail:
    return (int) ATADEV_UNKNOWN;
}

/*
 * __ata_identify_device
 *   Issues IDENTIFY command on ata_port[port] and fills device structures with details.
 *   Handles both ATA and ATAPI, and records capabilities.
 *   Returns 0 on success, -1 on failure.
 *   This is the main per-port device setup routine.
 */
int __ata_identify_device(char port)
{
    int DevType = ATADEV_UNKNOWN;
    st_dev_t *new_dev;

    //#debug
    printf ("\n");
    printf (":: Port %d:\n",port);

    // Allocate new device structure
    new_dev = (struct st_dev *) malloc( sizeof(struct st_dev) );
    if ((void *) new_dev == NULL){
        printf ("__ata_identify_device: [FAIL] new_dev\n");
        bl_die();
    }

    // Detect device type
    DevType = (int) __detect_device_type(port);
    if (DevType == ATADEV_UNKNOWN){
        printf("Port %d: Unknown\n",port);
        goto fail;
    }

//
// Handle different device types
//

// ========================
// ATA device.

    if (DevType == ATADEV_PATA){
        printf("Port %d: PATA device\n",port);

        // kputs("Unidade PATA\n");
        // aqui esperamos pelo DRQ
        // e eviamos 256 word de dados PIO

        // Wait for DRQ and read IDENTIFY data
        __ata_pio_read ( port, ata_devinfo_buffer, 512 );
        ata_wait_not_busy(port);
        ata_wait_no_drq(port);

        // Fill per-port info structure
        ide_port[port].id = (uint8_t) port;
        ide_port[port].name = "PATA";
        ide_port[port].type = (int) idedevicetypesPATA;
        ide_port[port].used = (int) TRUE;
        ide_port[port].magic = (int) 1234;

        // Fill device structure with details from IDENTIFY data
        new_dev->dev_type = 
            (ata_devinfo_buffer[0] &0x8000)?    0xffff: ATA_DEVICE_TYPE;
        new_dev->dev_access = 
            (ata_devinfo_buffer[83]&0x0400)? ATA_LBA48: ATA_LBA28;

        if (ATAFlag == FORCEPIO){
            // Only PIO mode
            new_dev->dev_modo_transfere = 0;
        }else{
            // Com esse pode funcionar em dma
            new_dev->dev_modo_transfere = 
                (ata_devinfo_buffer[49]&0x0100)? ATA_DMA_MODO: ATA_PIO_MODO;
        };

        new_dev->dev_total_num_sector  = ata_devinfo_buffer[60];
        new_dev->dev_total_num_sector += ata_devinfo_buffer[61];
        new_dev->dev_byte_per_sector = 512;
        new_dev->dev_total_num_sector_lba48  = ata_devinfo_buffer[100];
        new_dev->dev_total_num_sector_lba48 += ata_devinfo_buffer[101];
        new_dev->dev_total_num_sector_lba48 += ata_devinfo_buffer[102];
        new_dev->dev_total_num_sector_lba48 += ata_devinfo_buffer[103];
        new_dev->dev_size = (new_dev->dev_total_num_sector_lba48 * 512);

// ========================
// Unidades ATAPI.
    }else if (DevType == ATADEV_PATAPI){
        printf("Port %d: PATAPI device\n",port);

        //kputs("Unidade PATAPI\n");   
        ata_cmd_write(port, ATA_CMD_IDENTIFY_PACKET_DEVICE);

        ata_wait(400);
        ata_wait_drq(port); 
        __ata_pio_read ( port, ata_devinfo_buffer, 512 );
        //ata_wait_not_busy();
        //ata_wait_no_drq();

        // Salvando o tipo em estrutura de porta.
        ide_port[port].id = (uint8_t) port;
        ide_port[port].name = "PATAPI";
        ide_port[port].type = (int) idedevicetypesPATAPI;
        ide_port[port].used = (int) TRUE;
        ide_port[port].magic = (int) 1234;

        new_dev->dev_type = 
              (ata_devinfo_buffer[0]&0x8000)? ATAPI_DEVICE_TYPE: 0xffff;

        new_dev->dev_access = ATA_LBA28;

        if (ATAFlag == FORCEPIO){
            // Only PIO mode
            new_dev->dev_modo_transfere = 0; 
        }else{
            // Com esse pode funcionar em dma
            new_dev->dev_modo_transfere = 
                (ata_devinfo_buffer[49]&0x0100)? ATA_DMA_MODO: ATA_PIO_MODO;
        };

        new_dev->dev_total_num_sector  = 0;
        new_dev->dev_total_num_sector += 0;

        new_dev->dev_byte_per_sector = 2048; 

        new_dev->dev_total_num_sector_lba48  = 0;
        new_dev->dev_total_num_sector_lba48 += 0;
        new_dev->dev_total_num_sector_lba48 += 0;
        new_dev->dev_total_num_sector_lba48 += 0;

        new_dev->dev_size = 
            (new_dev->dev_total_num_sector_lba48 * 2048);


    }else if (DevType == ATADEV_SATA){
        printf("Port %d: SATA device\n",port);

        //kputs("Unidade SATA\n");   
        // O dispositivo responde imediatamente um erro ao cmd Identify device
        // entao devemos esperar pelo DRQ ao invez de um BUSY
        // em seguida enviar 256 word de dados PIO.

        ata_wait_drq(port); 
        __ata_pio_read ( port, ata_devinfo_buffer, 512 );
        //ata_wait_not_busy();
        //ata_wait_no_drq();

        // Salvando o tipo em estrutura de porta.
        ide_port[port].id = (uint8_t) port;
        ide_port[port].name = "SATA";
        ide_port[port].type = (int) idedevicetypesSATA;
        ide_port[port].used = (int) TRUE;
        ide_port[port].magic = (int) 1234;

        printf ("__ata_identify_device: [FAIL] SATA not supported :)\n");
        goto fail;

    }else if (DevType == ATADEV_SATAPI){
        printf("Port %d: SATAPI device\n",port);

        //kputs("Unidade SATAPI\n");   
        ata_cmd_write(port, ATA_CMD_IDENTIFY_PACKET_DEVICE);
        ata_wait(400);
        ata_wait_drq(port); 
        __ata_pio_read(port, ata_devinfo_buffer,512);
        //ata_wait_not_busy();
        //ata_wait_no_drq();

        // Salvando o tipo em estrutura de porta.
        ide_port[port].id = (uint8_t) port;
        ide_port[port].name = "SATAPI";
        ide_port[port].type = (int) idedevicetypesSATAPI;
        ide_port[port].used = (int) TRUE;
        ide_port[port].magic = (int) 1234;

        printf ("__ata_identify_device: [FAIL] SATAPI not supported :)\n");
        goto fail;

    } else {
        printf("Port %d: Invalid device type\n",port);
        //printf ("__ata_identify_device: [FAIL] Invalid device type\n");
        goto fail;
    };

// ----------------------------------

    // Fill common device fields
    new_dev->dev_id      = dev_next_pid++;
    new_dev->dev_num     = ata_port[port].dev_num;
    new_dev->dev_channel = ata_port[port].channel;
    new_dev->dev_nport   = port;

    // Mark port in dev_nport struct for fast lookup (if used elsewhere)
    switch (port){
    case 0:  dev_nport.dev0 = 0x81;  break;
    case 1:  dev_nport.dev1 = 0x82;  break;
    case 2:  dev_nport.dev2 = 0x83;  break;
    case 3:  dev_nport.dev3 = 0x84;  break;
    default: 
        printf ("__ata_identify_device: [FAIL] Invalid port?\n");
        break; 
    };

// #todo:
// This verbose belongs to the boot loader, not kernel.
// #ifdef BL_VERBOSE
#ifdef KERNEL_VERBOSE
    printf ("[DEBUG]: Detected Disk type: {%s}\n", 
        dev_type[new_dev->dev_type] );
    refresh_screen ();
#endif

    new_dev->next = NULL;

// Add new device to end of ready queue (linked list)
    st_dev_t *tmp_dev;
    tmp_dev = (struct st_dev *) ready_queue_dev;
    if ((void*) tmp_dev == NULL){
        printf("__ata_identify_device: [FAIL] tmp_dev\n");
        bl_die();
    }

    while (tmp_dev->next){
        tmp_dev = tmp_dev->next;
    };
    tmp_dev->next = new_dev;

// OK
    return 0;
fail:
    return (int) -1;
}

// Called to probe for the boot disk signature into all the 4 ports. 
static int __ata_probe_boot_disk_signature(void)
{
    register int i=0;

// #test
// Is this a good moment to check what is the ATA port 
// where is the disk that we booted from?
// Goal:
// + Read a given sector from all the 4 ports.
// + Check if the signature found in the disk 
//   maches with the own we got from the boot manager.
//   In this case, it means that we are in the correct boot disk.

    int idePort = ATACurrentPort.g_current_ide_port;          // Port index (0-3)
    //int ideChannel = ATACurrentPort.g_current_ide_channel;  // 2 channels
    int isSlave = ATACurrentPort.g_current_ide_device;        // 0=master, 1=slave

    static char sig_buffer[512];

    clear_backbuffer();
    refresh_screen();

    for (i=0; i<4; i++){

    bzero(sig_buffer,512); // Crear the local buffer for current use.

    idePort = i;
    ATACurrentPort.g_current_ide_port = i;
    switch (idePort)
    {
        // Primary master
        case 0:
            ATACurrentPort.g_current_ide_channel = 0;
            ATACurrentPort.g_current_ide_device = 0;  // Not slave
		    break;
        // Primary slave
        case 1:
            ATACurrentPort.g_current_ide_channel = 0; 
            ATACurrentPort.g_current_ide_device = 1;  // Slave
            break;
        // Secondary master
        case 2:
            ATACurrentPort.g_current_ide_channel = 1;
            ATACurrentPort.g_current_ide_device = 0;  // Not slave
            break;
        // Secondary slave
        case 3:
            ATACurrentPort.g_current_ide_channel = 1; 
            ATACurrentPort.g_current_ide_device = 1;  // Slave
            break;
        default:
            // #debug
            printf ("__ata_probe_boot_disk_signature: Invalid port number\n");
            refresh_screen();
            while(1){}
            break;
    };
    isSlave = ATACurrentPort.g_current_ide_device;

    // Read from the curent port.
    // see: libata.c
    libata_pio_rw_sector ( 
        (unsigned long) sig_buffer,  // Buffer
        (unsigned long) 0x04 -1,     // LBA ok
        (int) idePort,               // We have 4 valid ports.
        (int) isSlave );             // Slave or not.

    // Print the buffer.
    printf("\n");
    unsigned long *s1 = (unsigned long *) &sig_buffer[0];
    unsigned long *s2 = (unsigned long *) &sig_buffer[4];
    printf("Port %d: Disk signature 1: %x\n", i, s1[0] );// first dword
    printf("Port %d: Disk signature 2: %x\n", i, s2[0] );// second dword
    refresh_screen();

    // Disk found!
    if ( g_disk_sig1[0] == s1[0] &&
         g_disk_sig2[0] == s2[0] )
    {
        // Update the structure to identify the boot disk and its done.
        ata_boot_disk_info.port    = (int) ATACurrentPort.g_current_ide_port;
        ata_boot_disk_info.channel = (int) ATACurrentPort.g_current_ide_channel;
        ata_boot_disk_info.device  = (int) ATACurrentPort.g_current_ide_device;

        printf("Signature found in: port=%d ch=%d dev=%d\n",
            ata_boot_disk_info.port,
            ata_boot_disk_info.channel,
            ata_boot_disk_info.device );
        
        refresh_screen();      
        //while(1){}

        // Done
        // #todo:
        // OK, it's working,
        // now we gotta recreate this routine into the 
        // kernel image too.
        return 0;
    }

    };

fail:
    return (int) -1;
}

/*
 * __ata_initialize_controller
 *   Initializes the ATA controller and enumerates all possible devices on all 4 ports.
 *   Allocates ready queue, prepares info buffer, and calls __ata_identify_device() for each port.
 *   This is crucial for multi-port support: it sets up the device list for later I/O.
 */
static int __ata_initialize_controller(void)
{
    register int i=0;
    int port=0;

    // #test
    // Suspending that 'reset', this way the 0xFF test
    // will be the first interaction with the controller.
    // See: __ata_identify_device() bellow.
    //Soft Reset, defina IRQ
    //out8 ( ATA_BAR1_PRIMARY_CONTROL_PORT, 0xff );
    //out8 ( ATA_BAR3_SECONDARY_CONTROL_PORT, 0xff );
    //out8 ( ATA_BAR1_PRIMARY_CONTROL_PORT, 0x00 );
    //out8 ( ATA_BAR3_SECONDARY_CONTROL_PORT, 0x00 );

    ata_record_dev = 0xff;
    ata_record_channel = 0xff;

// Vamos trabalhar na lista de dispositivos.

    // Start the device linked list (ready_queue_dev)
    ready_queue_dev = (struct st_dev *) malloc(sizeof(struct st_dev));
    if ((void *) ready_queue_dev == NULL){
        printf("__ata_initialize_controller: ready_queue_dev struct fail\n");
        bl_die();
    }

// #todo:
// Checar a validade da estrutura.

    // Initialize the first (dummy) device node
    current_dev = (struct st_dev *) ready_queue_dev;
    current_dev->dev_id = dev_next_pid++;
    current_dev->dev_type    = -1;
    current_dev->dev_num     = -1;
    current_dev->dev_channel = -1;
    current_dev->dev_nport   = -1;
    current_dev->next = NULL;

    // Allocate buffer for IDENTIFY responses and device info
    ata_devinfo_buffer = (_u16 *) malloc(4096);
    if ((void *) ata_devinfo_buffer == NULL){
        printf("__ata_initialize_controller: ata_devinfo_buffer fail\n");
        bl_die();
    }

// Let's initilize all the four ports for the ATA controller.
// Sondando dispositivos
// As primeiras quatro portas do controlador IDE.
// Inicializa estrutura de dispositivo e coloca na lista
// encadeada de dispositivo.
// #test
// Nesse hora conseguiremos saber mais informaçoes sobre o dispositivo.

    // Enumerate all 4 ports: attempt to identify a device on each one
    for (port=0; port<ATA_NUMBER_OF_PORTS; port++)
    {
        __ata_identify_device(port);
    };

//=======================================================
// Probe for the boot disk signature into all the 4 ports. 
    int disk_ok = -1;
    disk_ok = (int) __ata_probe_boot_disk_signature();
    if (disk_ok < 0){
        printf("__ata_initialize_controller: Boot disk not found\n");
        bl_die();
    }

    //printf("breakpoint\n");
    //refresh_screen();
    //while(1){}

    return 0; // ok
}

//
// $
// INITIALIZATION
//

/*
 * __ata_probe_controller
 *   Probes PCI bus for ATA controller, sets up BARs, initializes controller for all ports.
 *   Sets up ide_port[] and ata_port[] for all 4 ATA ports.
 *   Calls __ata_initialize_controller() for multi-port device enumeration.
 *   Prepares for future AHCI/SATA and RAID support.
 *   Credits: Nelson Cole.
 */
static int __ata_probe_controller(int ataflag)
{
    int Status = 1;  //error
    _u32 data=0;

    _u8 bus=0;
    _u8 dev=0;
    _u8 fun=0;

    // Clear per-port info structures
    bzero( &ide_port[0], sizeof(struct ide_port_d) );
    bzero( &ide_port[1], sizeof(struct ide_port_d) );
    bzero( &ide_port[2], sizeof(struct ide_port_d) );
    bzero( &ide_port[3], sizeof(struct ide_port_d) );

// ================================================

// #importante:
// Veja no kernel.
// Fizemos de um jeito diferente no driver que est'a no kernel.

    // We have 4 valid port number.
	// We get this value from the configuration file.

    // #important:
    // Use a pre-set config for which port is primary (from config.h)
    ATACurrentPort.g_current_ide_port = __CONFIG_DEFAULT_ATA_PORT;

    /*
    //#debug
    printf ("bl CONFIG: IDE port: %d\n",ATACurrentPort.g_current_ide_port);   // from config.h
    //printf ("bl xBootBlock: IDE port: %d\n",xBootBlock.ide_port_number);  // from bootblock, from bl.bin
    refresh_screen();
    while(1){}
    */

/*
// #test
// Let's send this value to the kernel
// Using the standard bootblock address
// 9000 + 48
    unsigned long *bb = (unsigned long *) (0x90000 + 48);
    bb[0] = 0; // clean 4 bytes
    bb[1] = 0; // clean 4 bytes
    bb[0] = (unsigned long) (ATACurrentPort.g_current_ide_port & 0xff);
*/

// See:
// https://wiki.osdev.org/PCI_IDE_Controller

// IDE Interface:
// Primary Master Drive.
// Primary Slave Drive.
// Secondary Master Drive.
// Secondary Slave Drive.

// Serial IDE
// Primary Master,   also called SATA1.
// Primary Slave,    also called SATA2.
// Secondary Master, also called SATA3.
// Secondary Slave,  also called SATA4.

    switch (ATACurrentPort.g_current_ide_port)
    {
        // Primary master
        case 0:
            ATACurrentPort.g_current_ide_channel = 0;
            ATACurrentPort.g_current_ide_device = 0;  // Not slave
		    break;
        // Primary slave
        case 1:
            ATACurrentPort.g_current_ide_channel = 0; 
            ATACurrentPort.g_current_ide_device = 1;  // Slave
            break;
        // Secondary master
        case 2:
            ATACurrentPort.g_current_ide_channel = 1;
            ATACurrentPort.g_current_ide_device = 0;  // Not slave
            break;
        // Secondary slave
        case 3:
            ATACurrentPort.g_current_ide_channel = 1; 
            ATACurrentPort.g_current_ide_device = 1;  // Slave
            break;
        default:
            // #debug
            printf ("Invalid IDE port number\n");
            refresh_screen();
            while(1){}
            goto fail;
            break;
    };

// ===============================================================

// Configurando flags do driver.
    ATAFlag = (int) ataflag;

// Messages

//#ifdef KERNEL_VERBOSE
    //printf ("sm-disk-disk-__ata_probe_controller:\n");
    //printf ("Initializing IDE/AHCI support ...\n");
    //refresh_screen();
//#endif

// Sondando a interface PCI para encontrarmos um dispositivo
// que seja de armazenamento de dados.
// #todo
// Talvez essa sondagem pode nos dizer se o dispositivo 
// é primary/secondary e master/slave.

    // Scan PCI for a Mass Storage Controller (class 0x01)
    // Get the bus/dev/fun for a device given the class.
    data = (_u32) __ataPCIScanDevice(PCI_CLASS_MASS);

// Error
    if (data == -1)
    {
        printf ("__ata_probe_controller: pci_scan_device fail. ret={%d}\n", 
            (_u32) data );

        // Abort
        Status = (int) (PCI_MSG_ERROR);
        goto fail;
    }

    // Decode bus/dev/fun from PCI scan result
    bus = ( data >> 8 & 0xff );
    dev = ( data >> 3 & 31 );
    fun = ( data      & 7 );

// ------------------------------
// Getting information:
// Let's fill the structure with some information about the device.
// Mass storage device only.

    // Read PCI config for this controller 
    // (fills ata_pci and sets AtaController.chip_control_type)
    data = (_u32) __ataPCIConfigurationSpace( bus, dev, fun );
    if (data != PCI_MSG_SUCCESSFUL)
    {
        printf ("__ata_probe_controller: Error Driver [%x]\n", data );
        Status = (int) 1;
        goto fail;  
    }

// After call the function above
// now we have a lot of information into the structure.

// ==============================
// BARs
// Getting the base ports's addresses 

    // Set BARs—fall back to legacy addresses if BAR is zero
    ATA_BAR0_PRIMARY_COMMAND_PORT = ( ata_pci.bar0 & ~7 )   + ATA_IDE_BAR0 * ( !ata_pci.bar0 ); 
    ATA_BAR1_PRIMARY_CONTROL_PORT = ( ata_pci.bar1 & ~3 )   + ATA_IDE_BAR1 * ( !ata_pci.bar1 );       

    ATA_BAR2_SECONDARY_COMMAND_PORT = ( ata_pci.bar2 & ~7 )   + ATA_IDE_BAR2 * ( !ata_pci.bar2 );
    ATA_BAR3_SECONDARY_CONTROL_PORT = ( ata_pci.bar3 & ~3 )   + ATA_IDE_BAR3 * ( !ata_pci.bar3 );

    ATA_BAR4 = ( ata_pci.bar4 & ~0x7 ) + ATA_IDE_BAR4 * ( !ata_pci.bar4 );
    ATA_BAR5 = ( ata_pci.bar5 & ~0xf ) + ATA_IDE_BAR5 * ( !ata_pci.bar5 );


// Saving the 'port addresses' into the structure for future use.
// Configure ide_port[] base addresses for all 4 ports
// IDE Interface:
// Primary Master Drive.
// Primary Slave Drive.
// Secondary Master Drive.
// Secondary Slave Drive.
// Serial IDE
// Primary Master,   also called SATA1.
// Primary Slave,    also called SATA2.
// Secondary Master, also called SATA3.
// Secondary Slave,  also called SATA4.
// See: 
// https://wiki.osdev.org/PCI_IDE_Controller

// 0x1F0, Primary Command (Master)
    ide_port[0].base_port = 
        (unsigned short) ATA_BAR0_PRIMARY_COMMAND_PORT;
// 0x1F0, Primary Command (Slave)
    ide_port[1].base_port = 
        (unsigned short) ATA_BAR0_PRIMARY_COMMAND_PORT;

// 0x170, Secondary Command (Master)
    ide_port[2].base_port = 
        (unsigned short) ATA_BAR2_SECONDARY_COMMAND_PORT;
// 0x170, Secondary Command (Slave)
    ide_port[3].base_port = 
        (unsigned short) ATA_BAR2_SECONDARY_COMMAND_PORT;

// #todo: 
// BAR4 and BAR5 for DMA support.
    // ATA_BAR4
    // ATA_BAR5

    /*
    clear_backbuffer();
    printf(">>>>>>>>>>>>>>>>>>>> %x %x %x %x\n", 
    ide_port[0].base_port,
    ide_port[1].base_port,
    ide_port[2].base_port,
    ide_port[3].base_port );
    refresh_screen();
    while(1){}
    */

//
// Now initialize for controller type
//

    BootDisk.initialized = FALSE;
    BootDisk.controller_type = -1;

//
// Se for IDE.
//

// =========================
// Type ATA
// Sub-class 01h = IDE Controller
// IDE controller: proceed with full ATA support (multi-port)
    if (AtaController.chip_control_type == STORAGE_CONTROLLER_MODE_ATA){
        
        BootDisk.controller_type = CONTROLLER_TYPE_ATA;
        __ata_initialize_controller();

// =========================
// Type RAID
    } else if (AtaController.chip_control_type == STORAGE_CONTROLLER_MODE_RAID){

        BootDisk.controller_type = -1;
        printf ("__ata_probe_controller: RAID not supported\n");
        bl_die();

// =========================
// Type ATA DMA.
    } else if (AtaController.chip_control_type == STORAGE_CONTROLLER_MODE_DMA){

        BootDisk.controller_type = -1;
        printf ("__ata_probe_controller: ATA DMA not supported\n");
        bl_die();

// =========================
// Type AHCI.
// Sub-class 06h = SATA Controller
    } else if (AtaController.chip_control_type == STORAGE_CONTROLLER_MODE_AHCI){

        BootDisk.controller_type = CONTROLLER_TYPE_AHCI;
        printf ("__ata_probe_controller: AHCI not supported\n");
        bl_die();

// =========================
// Unknown or unsupported controller
// Not IDE and Not AHCI
    } else {
        BootDisk.controller_type = -1;
        printf ("__ata_probe_controller: Controller type not supported\n");
        bl_die();
    };

    BootDisk.initialized = TRUE;

// Ok

    /*
    // #debug
    // #test
    // Testing channel and device number.
    // Isso ta mostrando o valor do ultimo dispositivo sondado.
    printf ("channel=%d device=%d\n",ata.channel,ata.dev_num);
    refresh_screen();
    while(1){}
    */

    Status = 0;
    goto done;

fail:
    printf ("__ata_probe_controller: fail\n");
    refresh_screen();
done:
    return (int) Status;
}

/*
 * __ata_initialization_dialog
 *   Entry point for driver initialization: 
 * receives a message (msg) and optional arguments. 
 * Handles driver setup (case 1 = initialize).
 * This abstraction allows for future extension (register, shutdown, etc).
 */
int 
__ata_initialization_dialog ( 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{
    int Status = 1;    //Error

    switch (msg){

    // Initialize driver, passing flags (FORCEPIO, etc)
    //ATAMSG_INITIALIZE
    case 1:
        __ata_probe_controller((int) long1);
        Status = 0;
        goto done;
        break;

    //registra o driver. 
    //ATAMSG_REGISTER
    //case 2:
    //    break;

    default:
        goto fail;
        break;
    };

fail:
    printf ("__ata_initialization_dialog: fail\n");
    refresh_screen();
done:
    return (int) Status;
}


//
// $
// INITIALIZATION
//

/*
 * ata_initialize
 *   Main driver initialization entry—called from OS_Loader_Main in main.c.
 *   Sets up controller, all ports/devices, and marks driver initialized.
 *   Always initializes all four ports using ata_port[4] for full multi-device support.
 */
int ata_initialize(void)
{
    // Start the full driver initialization using FORCEPIO for max compatibility.
    // (FORCEPIO disables DMA support for legacy/boot use.)
    __ata_initialization_dialog( 1, FORCEPIO, FORCEPIO );
    g_driver_hdd_initialized = (int) TRUE;

    return TRUE;
}

//
// End
//

