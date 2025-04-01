// libata.c

// IDE controller support.
// 2013 - Created by Fred Nora.

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


#include "../../../bl.h"


/*
 * Externs
 */
 
extern void os_read_sector();
extern void os_write_sector();
extern void reset_ide0();

// Usadas por read e write.
extern unsigned long hd_buffer;
extern unsigned long hd_lba;

// Internal
int hddStatus=0;
int hddError=0;
//...


#define __OPERATION_PIO_READ  1000
#define __OPERATION_PIO_WRITE  2000



static void 
__libata_pio_read ( 
    int p, 
    void *buffer, 
    int bytes );

static void 
__libata_pio_write ( 
    int p, 
    void *buffer, 
    int bytes );


// Read or write a sector using PIO mode.

static int 
__ata_pio_rw_sector ( 
    unsigned long buffer, 
    unsigned long lba, 
    int operation_number, 
    int port_index,
    int slave ); 


// ===================================================================


uint8_t hdd_ata_status_read(int p)
{
// #bugbug: 
//rever o offset

    //return inb(ata[p].cmd_block_base_addr + ATA_REG_STATUS);

    return (uint8_t) in8( (int) ide_ports[p].base_port + 7 );
}

int hdd_ata_wait_not_busy(int p)
{
    while ( hdd_ata_status_read(p) & ATA_SR_BSY )
        if ( hdd_ata_status_read(p) & ATA_SR_ERR )
            return 1;

    return 0;
}

void hdd_ata_cmd_write ( int port, int cmd_val )
{

// no_busy 
    hdd_ata_wait_not_busy(port);

    //outb(ata.cmd_block_base_address + ATA_REG_CMD,cmd_val);

    out8 ( (int) ide_ports[port].base_port + 7 , (int) cmd_val );
    ata_wait (400);  
}

int hdd_ata_wait_no_drq (int p)
{
    while ( hdd_ata_status_read(p) &ATA_SR_DRQ)
        if (hdd_ata_status_read(p) &ATA_SR_ERR)
            return 1;

    return 0;
}

static void 
__libata_pio_read ( 
    int p, 
    void *buffer, 
    int bytes )
{
    asm volatile (\
        "cld;\
         rep; insw":: "D" (buffer),\
         "d" ( ide_ports[p].base_port + 0 ),\
          "c" (bytes/2));
}


static void 
__libata_pio_write ( 
    int p, 
    void *buffer, 
    int bytes )
{
    asm volatile (\
                "cld;\
                rep; outsw"::"S"(buffer),\
                "d"( ide_ports[p].base_port + 0 ),\
                "c"(bytes/2));
}


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


// Invalid operation number.
    if ( operation_number != __OPERATION_PIO_READ && 
         operation_number != __OPERATION_PIO_WRITE )
    {
        return (int) -1;
    }

// #bugbug
// s� funcionaram as portas 0 e 2.
// para primary e secondary.

    if ( port_index < 0 || port_index >= 4 )
    {
        // #todo: Message
        return -1;
    }

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

    tmplba = (unsigned long) (tmplba >> 24);

// no bit 4.
// 0 = master | 1 = slave

// master. bit 4 = 0
    if (slave == 0){
        tmplba = (unsigned long)(tmplba | 0x000000E0);    //1110 0000b;
    }

// slave. bit 4 = 1
    if (slave == 1){
        tmplba = (unsigned long)(tmplba | 0x000000F0);    //1111 0000b;
    }

// In 32bit machine
// int and long has the same size.

    out8( 
        (int) ( ide_ports[port_index].base_port + 6 ), 
        (int) tmplba );

// #test
    //out8( 
        // (int) ide_ports[port_index].base_port + 6, 
        // (int) 0xE0 | (master << 4) | ((tmplba >> 24) & 0x0F));
 
// 0x01F2
// Port to send number of sectors.

    out8( 
        (int) ( ide_ports[port_index].base_port + 2 ), 
        (int) 1 );

// 0x1F3  
// Port to send bit 0 - 7 of LBA.

    tmplba = lba;
    tmplba = tmplba & 0x000000FF;
    out8( (int) ide_ports[port_index].base_port + 3 , (int) tmplba );

// 0x1F4
// Port to send bit 8 - 15 of LBA.

    tmplba = lba;
    tmplba = tmplba >> 8;
    tmplba = tmplba & 0x000000FF;
    out8( (int) ide_ports[port_index].base_port + 4 , (int) tmplba );

// 0x1F5:
// Port to send bit 16 - 23 of LBA

    tmplba = lba;
    tmplba = tmplba >> 16;
    tmplba = tmplba & 0x000000FF;
    out8( (int) ide_ports[port_index].base_port + 5 , (int) tmplba );

// =================================================

    /*
    if (_lba >= 0x10000000) 
    {
        Port = (unsigned short) (ide_ports[port_index].base_port);  // Base port 
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

    Port = (unsigned short) (ide_ports[port_index].base_port + ATA_REG_CMD); 

    //if (lba >= 0x10000000) {
    //    if (operation_number == __OPERATION_PIO_READ){
    //        out8 ( (unsigned short) port, (unsigned char) 0x24 );
    //    }
    //    if (operation_number == __OPERATION_PIO_WRITE){
    //        out8 ( (unsigned short) port, (unsigned char) 0x34 );
    //    }
    //} else {
        if (operation_number == __OPERATION_PIO_READ){
            out8 ( (unsigned short) Port, (unsigned char) 0x20 );
        }
        if (operation_number == __OPERATION_PIO_WRITE){
            out8 ( (unsigned short) Port, (unsigned char) 0x30 );
        }
    //}


// PIO or DMA ??
// If the command is going to use DMA, set the Features Register to 1, otherwise 0 for PIO.
    // outb (0x1F1, isDMA)

// timeout sim, n�o podemos esperar para sempre.
// #todo
// Colocar essas declara��es no in�cio da fun��o.

    unsigned char c=0;
    unsigned long timeout = (4444*512);

again:

    c = (unsigned char) in8( (int) ide_ports[port_index].base_port + 7);

// Select a bit.
    c = (c & 8);

    if (c == 0)
    {
        timeout--;
        if (timeout == 0)
        {
            printf("__ata_pio_rw_sector: [FAIL] rw sector timeout\n");
            return -3;
        }

        // #bugbug: 
        // Isso pode enrroscar aqui.

        goto again;
    }

//
// read or write.
//

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

            //Flush Cache

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

/*
 * ata_read_sector:
 * eax - buffer
 * ebx - lba
 * ecx - null
 * edx - null
 */
void 
ata_read_sector ( 
    unsigned long ax, 
    unsigned long bx, 
    unsigned long cx, 
    unsigned long dx )
{
    static int Operation = __OPERATION_PIO_READ;  //0x20;  // Read

    // Channel and device number
    int ideChannel = g_current_ide_channel;
    int isSlave = g_current_ide_device;
	
// #bugbug 
// We have 4 valid ports.
// We do not have the IDE port, so, we are using the ide channel.
	int idePort = g_current_ide_port;
	


//====================== WARNING ==============================
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
        (int) idePort,    // We have 4 valid ports.
        (int) isSlave );  // Slave or not.

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
 * ata_write_sector:
 * eax - buffer
 * ebx - lba
 * ecx - null
 * edx - null
 */

void 
ata_write_sector ( 
    unsigned long ax, 
    unsigned long bx, 
    unsigned long cx, 
    unsigned long dx )
{
    static int Operation = __OPERATION_PIO_WRITE; //0x30;  // Read

    // Channel and device number
    int ideChannel = g_current_ide_channel;
    int isSlave = g_current_ide_device;
	
// #bugbug 
// We have 4 valid ports.
// We do not have the IDE port, so, we are using the ide channel.
	int idePort = g_current_ide_port;
	

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
        (int) idePort,    // We have 4 valid ports.
        (int) isSlave );  // Slave or not.

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

