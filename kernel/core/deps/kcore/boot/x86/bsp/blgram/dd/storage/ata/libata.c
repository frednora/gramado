// libata.c
// IDE controller support.
// 2013 - Created by Fred Nora.

// Purpose:
// This file implements low-level IDE/ATA sector read/write routines, 
// using PIO (Programmed I/O) mode, for the 32-bit bootloader environment.

/*
Key Features:
Supports reading and writing a sector from/to disk using port I/O.
Can address up to 4 ports (primary/secondary, master/slave).
Functions: ata_read_sector, ata_write_sector, and their helpers.
Designed to be called from higher-level filesystem code (fs.c).
Heavy usage of per-port addressing via ide_port[p].base_port.
*/

/*
 * Low level routines to read and write a sector into/from the disk.
 * Environment:
 *     32bit bootloader.
 * Two functions are used in this document.
 * >> ata_read_sector() is called by read_lba().
 * >> ata_write_sector() is called by write_lba().
 * read_lba() and write_lba are called by fs.c.
 */

/*
 hd info:
 =======
 PIIX3 ATA: 
 LUN#0: disk, PCHS=963/4/17, total number of sectors 65536. (Oracle Virtualbox)
 estatistica de hoje:
 (apenas leitura, usando PIO mode)
00:01:59.902737 /Devices/IDE0/ATA0/Unit0/AtapiDMA            0 times
00:01:59.902742 /Devices/IDE0/ATA0/Unit0/AtapiPIO            0 times
00:01:59.902747 /Devices/IDE0/ATA0/Unit0/DMA                 0 times
00:01:59.902753 /Devices/IDE0/ATA0/Unit0/PIO              1699 times  <<
00:01:59.902760 /Devices/IDE0/ATA0/Unit0/ReadBytes      869376 bytes  <<
00:01:59.902766 /Devices/IDE0/ATA0/Unit0/WrittenBytes        0 bytes
 */ 

/* 
 * Example disk stats (Oracle VirtualBox, PIIX3 ATA):
 *   - Only PIO mode is used for read operations.
 *   - No DMA or ATAPI operations performed in this environment.
 */

#include "../../../bl.h"

/*
 * Externs:
 * - Functions and variables provided elsewhere, needed for sector I/O.
 */

extern void os_read_sector();
extern void os_write_sector();
extern void reset_ide0();

// Usadas por read e write.
extern unsigned long hd_buffer;
extern unsigned long hd_lba;

// Internals
int hddStatus=0;
int hddError=0;
// ...

// ================================================

/*
 * Local PIO transfer routines.
 * These functions perform the actual word transfers to/from the disk.
 */
static void  __libata_pio_read( int p, void *buffer, int bytes );
static void __libata_pio_write( int p, void *buffer, int bytes );

/*
 * Core low-level function:
 * Reads or writes a sector on the given port using PIO mode.
 * - buffer:     Address of the sector buffer.
 * - lba:        Logical Block Address to access.
 * - operation:  __OPERATION_PIO_READ or __OPERATION_PIO_WRITE.
 * - port_index: Index into ide_port[] (0-3 for four ports).
 * - slave:      0 = master, 1 = slave.
 */
static int 
__ata_pio_rw_sector ( 
    unsigned long buffer, 
    unsigned long lba, 
    int operation_number, 
    int port_index,
    int slave ); 

// ===================================================================

/* ========================
   Register/Status Helpers
   ======================== */

/*
 * hdd_ata_status_read
 *   Reads the status register for the given port.
 *   Returns the raw ATA status byte.
 */
uint8_t hdd_ata_status_read(int p)
{
// #bugbug: Rever o offset

    // The ATA status register is at offset 7 from base port.
    return (uint8_t) in8( (int) ide_port[p].base_port + 7 );
    //return inb(ata[p].cmd_block_base_addr + ATA_REG_STATUS);
}

/*
 * hdd_ata_wait_not_busy
 *   Waits for the BSY (busy) bit to clear on the given port.
 *   Returns 0 if successful, 1 if the ERR (error) bit is set.
 */
int hdd_ata_wait_not_busy(int p)
{
    while ( hdd_ata_status_read(p) & ATA_SR_BSY )
        if ( hdd_ata_status_read(p) & ATA_SR_ERR )
            return 1;

    return 0;
}

/*
 * hdd_ata_cmd_write
 *   Writes a command byte to the command register for the given port.
 *   Waits for the device to become not busy, then writes the command.
 *   After writing, waits ~400ns (as per ATA spec) for command acceptance.
 */
void hdd_ata_cmd_write ( int port, int cmd_val )
{
    // no_busy 
    hdd_ata_wait_not_busy(port);
    //outb(ata.cmd_block_base_address + ATA_REG_CMD,cmd_val);
    out8 ( (int) ide_port[port].base_port + 7 , (int) cmd_val );
    ata_wait (400);  
}

/*
 * hdd_ata_wait_no_drq
 *   Waits for the DRQ (Data Request) bit to clear on the given port.
 *   Returns 0 if successful, 1 if the ERR (error) bit is set.
 */
int hdd_ata_wait_no_drq(int p)
{
    while ( hdd_ata_status_read(p) &ATA_SR_DRQ)
        if (hdd_ata_status_read(p) &ATA_SR_ERR)
            return 1;

    return 0;
}

/* ========================
   PIO Data Transfer
   ======================== */


/*
 * __libata_pio_read
 *   Reads 'bytes' from the disk data register (port p) into 'buffer' using PIO.
 *   - Uses the 'rep insw' instruction for high-speed 16-bit word transfer.
 */
static void __libata_pio_read( int p, void *buffer, int bytes )
{
    asm volatile (\
        "cld;\
         rep; insw":: "D" (buffer),\
         "d" (ide_port[p].base_port + 0),\
          "c" (bytes/2));
}

/*
 * __libata_pio_write
 *   Writes 'bytes' from 'buffer' to the disk data register (port p) using PIO.
 *   - Uses 'rep outsw' for high-speed 16-bit word transfer.
 */
static void __libata_pio_write( int p, void *buffer, int bytes )
{
    asm volatile (\
                "cld;\
                rep; outsw"::"S"(buffer),\
                "d"(ide_port[p].base_port + 0),\
                "c"(bytes/2));
}

/* ========================
   Main Sector R/W Routine
   ======================== */

/*
 * __ata_pio_rw_sector
 * IN:
 *     buffer - Buffer address
 *     lba    - LBA number 
 *     rw     - Flag read or write.
 *   //inline unsigned char inportb (int port)
 *   //out8 ( int port, int data )
 *   (IDE PIO)
 */
// IN:
// port_index = We have 4 valid ports.
// slave = slave or not.
/*
Handles the sequence for a PIO sector read/write:
Drive/head/lba selection (master/slave, LBA bits)
Sector count and address (set registers for LBA28)
Issue command:
0x20 = read
0x30 = write
Wait for device ready (DRQ set, with timeout)
Transfer data:
If read: calls __libata_pio_read
If write: calls __libata_pio_write, then flushes cache
Error/timeout handling
*/
/*
 * __ata_pio_rw_sector
 *   Reads or writes a single sector using PIO mode, on the specified port/device.
 *   Handles:
 *     - Master/slave selection
 *     - LBA addressing (28-bit)
 *     - Register setup (sector count, LBA)
 *     - Command issue (0x20=read, 0x30=write)
 *     - Data transfer to/from buffer
 *     - Status/error checking and cache flush (for write)
 *
 *   Returns:
 *     0 on success
 *    -1 for invalid operation or port
 *    -3 for timeout
 */
/*
 * __ata_pio_rw_sector
 *   Reads or writes a single sector using PIO mode on the specified port/device.
 * 
 *   Parameters:
 *     buffer          - Address of the 512-byte data buffer.
 *     lba             - Logical Block Address (LBA) of the sector to access (28-bit addressing).
 *     operation_number- __OPERATION_PIO_READ (read) or __OPERATION_PIO_WRITE (write).
 *     port_index      - ATA port index (0=Primary Master, 1=Primary Slave, 2=Secondary Master, 3=Secondary Slave).
 *     slave           - 0 for master, 1 for slave (bit 4 in device/head register).
 * 
 *   Returns:
 *     0   - Success
 *     -1  - Invalid arguments/operation/port
 *     -3  - Timeout waiting for device ready
 *
 *   Process:
 *     1. Selects target device (master/slave) and sets LBA bits 24-27 in Drive/Head register (0x1F6).
 *     2. Sets sector count to 1.
 *     3. Sets LBA low/mid/high registers (0x1F3-0x1F5).
 *     4. Issues ATA command (0x20=read, 0x30=write) to Command register (0x1F7).
 *     5. Waits for DRQ=1 (device ready for data transfer).
 *     6. Transfers 512 bytes using rep insw/outsw.
 *     7. (If write) Flushes drive cache.
 */
/*
+ Wait for device to be not busy (BSY=0) using hdd_ata_wait_not_busy().
+ Set up registers for sector count, LBA low/mid/high, and device/head (with slave/master bit).
+ Write the command (READ_SECTORS or WRITE_SECTORS) using hdd_ata_cmd_write().
+ Wait for DRQ (data request) and perform the PIO transfer.
+ Optionally, check for errors after the transfer.
*/

// #bugbug
// Both master and slave on the same channel (primary or secondary) always 
// use the same command/base port (0x1F0 for primary, 0x170 for secondary).
// The control port (0x3F6 or 0x376) is only for device control (reset, nIEN, etc.), 
// not for read/write/data/status commands.

static int 
__ata_pio_rw_sector ( 
    unsigned long buffer, 
    unsigned long lba, 
    int operation_number, 
    int port_index,     
    int slave )  
{
    unsigned long tmplba = (unsigned long) lba;
    unsigned short Port;

    // Only accept valid operation codes
    if ( operation_number != __OPERATION_PIO_READ && 
         operation_number != __OPERATION_PIO_WRITE )
    {
        return (int) -1;
    }

    // Only accept valid port indices (0-3)
    if ( port_index < 0 || port_index >= 4 )
    {
        // #todo: Message
        return -1;
    }

    /*
    // #debug
    // Printing the base port
    clear_backbuffer();
    printf("base port: %x\n",ide_port[port_index].base_port);
    refresh_screen();
    while(1){}
    */

// Selecionar se � master ou slave.
//  outb (0x1F6, slavebit<<4)
// 0 - 3    In CHS addressing, bits 0 to 3 of the head. 
//          In LBA addressing, bits 24 to 27 of the block number
// 4  DRV  Selects the drive number.
// 5  1	Always set.
// 6  LBA Uses CHS addressing if clear or LBA addressing if set.
// 7  1 Always set.
// 0x01F6; 
// Port to send drive and bit 24 - 27 of LBA

    // Select master/slave and set LBA bits 24-27

    // ---- Device/Head Register setup (0x1F6/6) ----
    // Bits: 7=1, 6=LBA=1, 5=1, 4=DRV (slave), 3-0=LBA(24-27)
    // 0xA0 = 1010 0000 = base for master, LBA mode
    // 0xB0 = 1011 0000 = base for slave, LBA mode
    //tmplba = (unsigned long) (tmplba >> 24); // LBA bits 24-27 only
    tmplba = (lba >> 24) & 0x0F; // LBA bits 24-27 only

// -------------------------------------------------------
// no bit 4.
// 0 = master | 1 = slave

    //int isSlave = (int) (slave & 0xF);
    // Quick fix
    uint8_t isSlave = (uint8_t) ata_port[port_index].dev_num;

    // 0xE0 = 1110 0000 = LBA=1, DEV=0 (master)
    // 0xF0 = 1111 0000 = LBA=1, DEV=1 (slave)

/*
// NOT a slave
// master. bit 4 = 0
    if (isSlave == 0)
    {
        //#debug
        //printf ("MASTER\n");
        //refresh_screen();
        //while(1){}
        tmplba = (unsigned long)(tmplba | 0x000000E0);    //1110 0000b;
    }
*/

/*
// slave. bit 4 = 1
    if (isSlave == 1)
    {
        //#debug
        //printf ("SLAVE\n");
        //refresh_screen();
        //while(1){}
        tmplba = (unsigned long)(tmplba | 0x000000F0);    //1111 0000b;
    }
// -------------------------------------------------------
*/

// In 32bit machine
// int and long has the same size.
/*
Bits 7:5 = 1 1 1
Bit 6: LBA (should be 1 for LBA)
Bit 5: Always 1
Bit 4: 0=Master, 1=Slave
Bits 3-0: LBA bits 24-27
*/

    // 0xE0 = 1110 0000 = LBA=1, DEV=0 (master)
    // 0xF0 = 1111 0000 = LBA=1, DEV=1 (slave)

    //unsigned char dev_head = 0xE0 | (isSlave << 4) | tmplba;
    unsigned char dev_head = 0xE0 | (isSlave << 4) | ((lba >> 24) & 0x0F);

    out8( 
        (int) (ide_port[port_index].base_port + 6), 
        (int) (dev_head & 0xFF) );
    //out8( 
        //(int) (ide_port[port_index].base_port + 6), 
        //(int) tmplba );

// After writing Device/Head register, 
// ATA spec requires a 400ns delay (usually 4 status reads).
    io_delay();

// #test
    //out8( 
        // (int) ide_port[port_index].base_port + 6, 
        // (int) 0xE0 | (master << 4) | ((tmplba >> 24) & 0x0F));
 
// 0x01F2
// Port to send number of sectors.
// Set sector count to 1 (we are reading/writing one sector)

    // ---- Sector Count (0x1F2/2): Set to 1 sector ----
    out8( 
        (int) (ide_port[port_index].base_port + 2), 
        (int) 1 );
    io_delay();

//
// Set LBA 0-23
//

// ---- LBA Registers (0x1F3-0x1F5/3-5): LBA bits 0-23 ----

// 0x1F3  
// Port to send bit 0 - 7 of LBA.

    tmplba = lba;
    tmplba = tmplba & 0x000000FF;
    out8( (int) ide_port[port_index].base_port + 3 , (int) tmplba );
    //io_delay();

// 0x1F4
// Port to send bit 8 - 15 of LBA.

    tmplba = lba;
    tmplba = tmplba >> 8;
    tmplba = tmplba & 0x000000FF;
    out8( (int) ide_port[port_index].base_port + 4 , (int) tmplba );
    //io_delay();

// 0x1F5:
// Port to send bit 16 - 23 of LBA

    tmplba = lba;
    tmplba = tmplba >> 16;
    tmplba = tmplba & 0x000000FF;
    out8( (int) ide_port[port_index].base_port + 5 , (int) tmplba );
    //io_delay();

// =================================================

    /*
    if (_lba >= 0x10000000) 
    {
        Port = (unsigned short) (ide_port[port_index].base_port);  // Base port 
		out8 (Port + ATA_REG_SECCOUNT, 0);																// Yes, so setup 48-bit addressing mode
		out8 (Port + ATA_REG_LBA3, ((_lba & 0xFF000000) >> 24));
		out8 (Port + ATA_REG_LBA4, 0);
		out8 (Port + ATA_REG_LBA5, 0);
    }
    */

// =================================================
// 0x1F7:
// Command port
// Operation: read or write

    // Issue read or write command (0x20 or 0x30)
    Port = (unsigned short) (ide_port[port_index].base_port + ATA_REG_CMD); 

    //if (lba >= 0x10000000) {
    //    if (operation_number == __OPERATION_PIO_READ){
    //        out8 ( (unsigned short) port, (unsigned char) 0x24 );
    //    }
    //    if (operation_number == __OPERATION_PIO_WRITE){
    //        out8 ( (unsigned short) port, (unsigned char) 0x34 );
    //    }
    //} else {
        if (operation_number == __OPERATION_PIO_READ){
            out8 ( (unsigned short) Port, (unsigned char) 0x20 ); // READ SECTOR
            io_delay();
        }
        if (operation_number == __OPERATION_PIO_WRITE){
            out8 ( (unsigned short) Port, (unsigned char) 0x30 ); // WRITE SECTOR
            io_delay();
        }
    //}

// PIO or DMA ??
// If the command is going to use DMA, set the Features Register to 1, otherwise 0 for PIO.
    // outb (0x1F1, isDMA)

// Wait for DRQ (Data Request) bit to be set, with timeout
// ---- Wait for DRQ (bit 3) set ----
    unsigned char c=0;
    unsigned long timeout = (4444*1024); // Arbitrary large timeout; tune as needed
again:
    c = (unsigned char) in8( (int) ide_port[port_index].base_port + 7 );

// Select a bit.
    c = (c & 8);  // DRQ (bit 3) set: ready for data transfer

    if (c & 0x08) // DRQ (bit 3) set: ready for data transfer
        goto __OK;

    if (c == 0)
    {
        timeout--;
        if (timeout == 0){
            printf("__ata_pio_rw_sector: [FAIL] rw sector timeout\n");
            return -3;
        }
        // #bugbug: 
        // Isso pode enrroscar aqui.
        goto again;
    }

__OK:

//
// Read or write.
//

// ---- Data Transfer ----


    // Perform the data transfer
    switch (operation_number){

        // read
        case __OPERATION_PIO_READ:
            __libata_pio_read ( 
                (int)    port_index, 
                (void *) buffer, 
                (int)    512 );
            return 0;
            break;

        // write
        case __OPERATION_PIO_WRITE:
 
            __libata_pio_write ( 
                (int)    port_index, 
                (void *) buffer, 
                (int)    512 );

            // Flush cache after write

            //if (lba >= 0x10000000) {
            //    hdd_ata_cmd_write ( 
            //        (unsigned short) port_index, 
            //        (unsigned char) ATA_CMD_FLUSH_CACHE_EXT );
            //} else {
                hdd_ata_cmd_write ( 
                    (unsigned short) port_index, 
                    (unsigned char) ATA_CMD_FLUSH_CACHE );
            //}    

            hdd_ata_wait_not_busy(port_index);
            if ( hdd_ata_wait_no_drq(port_index) != 0)
            {
                // #todo: Message.
                return -1;
            }
            return 0;
            break;

        // fail
        default:
            printf ("__ata_pio_rw_sector: default\n");
            bl_die();
            break;
    };

    return 0;
}

// Wrapper
int 
libata_pio_rw_sector ( 
    unsigned long buffer, 
    unsigned long lba, 
    int port_index,     
    int slave )  
{

// #todo: Filters

    int rv=0;
    static int Operation = __OPERATION_PIO_READ;

    // Read from the curent port.
    rv = 
    (int) __ata_pio_rw_sector ( 
        (unsigned long) buffer,  // Buffer
        (unsigned long) lba,        // LBA
        (int) Operation, 
        (int) port_index,               // We have 4 valid ports.
        (int) slave );             // Slave or not.

    return (int) rv;
}

/* ========================
   Entry Points
   ======================== */

/*
 * ata_read_sector
 *   Reads a single 512-byte sector from disk into the buffer at 'ax' using LBA 'bx'.
 *   - ax: Buffer address
 *   - bx: LBA address
 *   - cx, dx: unused
 *   Uses current global port and device selection.
 */
void 
ata_read_sector ( 
    unsigned long ax, 
    unsigned long bx, 
    unsigned long cx, 
    unsigned long dx )
{
    static int Operation = __OPERATION_PIO_READ;

    int idePort = ATACurrentPort.g_current_ide_port;     // Port index (0-3)

    // Channel and device number
// #bugbug 
// We have 4 valid ports.
// We do not have the IDE port, so, we are using the ide channel.
    int ideChannel = ATACurrentPort.g_current_ide_channel;  // 2 channels
    int isSlave    = ATACurrentPort.g_current_ide_device;   // 0=master, 1=slave

// ====================== WARNING ==============================
// #IMPORTANTE:
// #todo
// So falta conseguirmos as variaveis que indicam o canal e 
// se eh master ou slave.

// IN:
// (buffer, lba, rw flag, port number, master )

    __ata_pio_rw_sector ( 
        (unsigned long) ax,  // Buffer
        (unsigned long) bx,  // LBA
        (int) Operation, 
        (int) idePort,       // We have 4 valid ports.
        (int) isSlave );     // Slave or not.

/*
//antigo.

    // Passando os argumentos.	
	hd_buffer = (unsigned long) ax;    //arg1 = buffer. 
	hd_lba = (unsigned long) bx;       //arg2 = lba.

	// Read sector. (ASM)
	os_read_sector();

	//#todo: deletar esse return.
	//testar sem ele antes.
*/

}

/*
 * ata_write_sector
 *   Writes a single 512-byte sector from buffer at 'ax' to disk at LBA 'bx'.
 *   - ax: Buffer address
 *   - bx: LBA address
 *   - cx, dx: unused
 *   Uses current global port and device selection.
 */
void 
ata_write_sector ( 
    unsigned long ax, 
    unsigned long bx, 
    unsigned long cx, 
    unsigned long dx )
{
    static int Operation = __OPERATION_PIO_WRITE;

    int idePort = ATACurrentPort.g_current_ide_port;            // Port index (0-3)

// Channel and device number
// #bugbug 
// We have 4 valid ports.
// We do not have the IDE port, so, we are using the ide channel.
    int ideChannel = ATACurrentPort.g_current_ide_channel;  // two channels
    int isSlave    = ATACurrentPort.g_current_ide_device;   // 0=master, 1=slave

/*
    printf("ata_write_sector: idePort=%d isSlave=%d\n",
        idePort, 
        isSlave);
    refresh_screen();
    while(1){}
*/

// =========================== WARNING ==============================
// #IMPORTANTE:
// #todo
// So falta conseguirmos as variaveis que indicam o canal e 
// se eh master ou slave.

// #bugbug:
// a rotina de salvar um arquivo invocada pelo shell 
// apresentou problemas. Estamos testando ...

// read test (buffer, lba, rw flag, port number )
    // __ata_pio_rw_sector ( (unsigned long) ax, (unsigned long) bx, (int) 0x30, (int) 0 );

    __ata_pio_rw_sector ( 
        (unsigned long) ax,  // Buffer
        (unsigned long) bx,  // LBA
        (int) Operation, 
        (int) idePort,       // We have 4 valid ports.
        (int) isSlave );     // Slave or not.

/*
// Antigo.
// Passando os argumentos.
	hd_buffer = (unsigned long) ax;    //arg1 = buffer. 
	hd_lba = (unsigned long) bx;       //arg2 = lba.

	// Write sector. (ASM)
    // entry/x86/head/hwlib.inc

	os_write_sector(); 
*/

}

//
// End
//

