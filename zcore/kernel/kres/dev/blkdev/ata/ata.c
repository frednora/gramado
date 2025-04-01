// ata.c
// This is the ata controller driver, embedded into the kernel base.
// ata handshake.
// write command and read status.
// History:
// + Original version Created by Nelson Cole, Sirius OS, BSD license.
// + Document created by Fred Nora.
// + Changed many times by Fred Nora.

// A lot of changes by Fred Nora.
// Suporte à controladora ATA.

#include <kernel.h>

int g_ata_driver_initialized=FALSE;

int ATAFlag=0;

static unsigned short *ata_devinfo_buffer;


// Saving values.
unsigned char ata_record_dev=0;
unsigned char ata_record_channel=0;

// #important
// Qual é o canal e o dispositivo usado no momento
// pela rotina de leitura e escrita.
// See: config.h ata.c hdd.c
int g_current_ide_port_index=0;

// #important
// Qual é o canal e o dispositivo usado no momento do boot 
// pela rotina de leitura e escrita.
// See: config.h ata.c hdd.c
int g_boottime_ide_port_index=0;

// base address 
// BAR0 is the start of the I/O ports used by the primary channel.
// BAR1 is the start of the I/O ports which control the primary channel.
// BAR2 is the start of the I/O ports used by secondary channel.
// BAR3 is the start of the I/O ports which control secondary channel.
// BAR4 is the start of 8 I/O ports controls the primary channel's Bus Master ATA.
// BAR4 + 8 is the Base of 8 I/O ports controls secondary channel's Bus Master ATA.

unsigned long ATA_BAR0_PRIMARY_COMMAND_PORT=0;    // Primary Command Block Base Address
unsigned long ATA_BAR1_PRIMARY_CONTROL_PORT=0;    // Primary Control Block Base Address
unsigned long ATA_BAR2_SECONDARY_COMMAND_PORT=0;  // Secondary Command Block Base Address
unsigned long ATA_BAR3_SECONDARY_CONTROL_PORT=0;  // Secondary Control Block Base Address
unsigned long ATA_BAR4=0;  // Legacy Bus Master Base Address
unsigned long ATA_BAR5=0;  // AHCI Base Address / SATA Index Data Pair Base Address

// O próximo ID de unidade disponível.
static uint32_t __next_sd_id = 0; 

// --------------------------

// Four ports.
// see: ide.h
struct ide_port_d  ide_ports[4];

struct dev_nport  dev_nport;

// Current storage device.
// see: ata.h
struct ata_device_d *current_sd;
// List for storage devices.
// see: ata.h
struct ata_device_d *ready_queue_dev;

// Not a pointer.
// see: ata.h
struct ata_port_d  ata_port[4];  // 32 ports.
struct ata_controller_d AtaController;

//
// == Private functions: prototypes ==============
//

static void __local_io_delay(void);
static void __ata_pio_read ( int p, void *buffer, int bytes );
static void __ata_pio_write ( int p, void *buffer, int bytes );
static unsigned char __ata_config_structure(char nport);
static void __set_ata_addr (int p, int channel);

static void dev_switch(void);
static int getnport_dev(void);
static int nport_ajust(char nport);

//
// $
// INITIALIZATION
//

static int __ide_identify_device(uint8_t nport);
static int ata_initialize_ide_device(char port);
static int __ata_initialize(int ataflag);

// =======================================================

static void __local_io_delay(void)
{
    asm ("xorl %%eax, %%eax" ::);
    asm ("outb %%al, $0x80"  ::);
}

// low level worker
// #todo:
// avoid this for compatibility with another compiler.
static void __ata_pio_read( int p, void *buffer, int bytes )
{
    asm volatile  (\
        "cld;\
        rep; insw"::"D"(buffer),\
        "d"(ata_port[p].cmd_block_base_address + ATA_REG_DATA),\
        "c"(bytes/2) );
}

// low level worker
// #todo:
// avoid this for compatibility with another compiler.
static void __ata_pio_write( int p, void *buffer, int bytes )
{
    asm volatile  (\
        "cld;\
        rep; outsw"::"S"(buffer),\
        "d"(ata_port[p].cmd_block_base_address + ATA_REG_DATA),\
        "c"(bytes/2) );
}

// Global
void ata_wait(int val)
{
    if (val <= 100){
        val = 400;
    }

    val = (int) (val/100);

    while (val--){
        io_delay();
    };
}

// Global
// Forces a 400 ns delay.
// Waste some time.
void ata_delay(void)
{
    long it=0;
    for (it=0; it<5; it++){
        __local_io_delay();
    };
}

// #bugbug
// Lê o status de um disco determinado, 
// se os valores na estrutura estiverem certos.
// #todo: return type.
unsigned char ata_status_read(int p)
{
    //if (p<0)
        //return 0;
    return in8( ata_port[p].cmd_block_base_address + ATA_REG_STATUS );
}

void ata_cmd_write (int p, int cmd_val)
{
    //if (p<0)
        //return;

    // no_busy 
    ata_wait_not_busy(p);

    out8 ( 
        (unsigned short) ((ata_port[p].cmd_block_base_address + ATA_REG_CMD) & 0xFFFF), 
        (unsigned char) (cmd_val & 0xFF) );

    ata_wait(400);  
}

void ata_soft_reset(int p)
{
    unsigned char data = 
        (unsigned char) in8( ata_port[p].ctrl_block_base_address );

    out8( 
        (unsigned short) ata_port[p].ctrl_block_base_address, 
        (unsigned char) (data | 0x4) );

    out8( 
        (unsigned short) ata_port[p].ctrl_block_base_address, 
        (unsigned char) (data & 0xFB) );
}

unsigned char ata_wait_drq(int p)
{
    //if (p<0)
        //return 0;

// #todo: Simplify this routine.
    while ( !(ata_status_read(p) & ATA_SR_DRQ) )
    {
        if ( ata_status_read(p) & ATA_SR_ERR ){
            return 1;
        }
    };
    return 0;
}

unsigned char ata_wait_no_drq (int p)
{
    //if (p<0)
        //return 0;
// #todo: Simplify this routine.
    while ( ata_status_read(p) & ATA_SR_DRQ )
    {
        if ( ata_status_read(p) & ATA_SR_ERR ){
            return 1;
        }
    };
    return 0;
}

unsigned char ata_wait_busy (int p)
{
    //if (p<0)
        //return 0;
// #todo: Simplify this routine.
    while (!(ata_status_read(p) & ATA_SR_BSY ))
    {
        if ( ata_status_read(p) & ATA_SR_ERR ){
            return 1;
        }
    };
    return 0;
}

// #todo: 
// Ao configurar os bits BUSY e DRQ devemos verificar retornos de erros.
unsigned char ata_wait_not_busy (int p)
{
    //if (p<0)
        //return 0;
// #todo: Simplify this routine.
    while ( ata_status_read(p) & ATA_SR_BSY )
    {
        if ( ata_status_read(p) & ATA_SR_ERR ){
            return 1;
        }
    };
    return 0;
}

// local worker
static void __set_ata_addr (int p, int channel)
{
// Select the i/o ports given the channel.
// The ports we found in the BARs.

// #todo
// filtrar limites.

    //if (p<0)
        //panic();

    //if (channel<0)
        //panic();

    switch (channel){

    case ATA_PRIMARY:
        ata_port[p].cmd_block_base_address = 
            ATA_BAR0_PRIMARY_COMMAND_PORT;
        ata_port[p].ctrl_block_base_address = 
            ATA_BAR1_PRIMARY_CONTROL_PORT;
        ata_port[p].bus_master_base_address = 
            ATA_BAR4;
        break;

    case ATA_SECONDARY:
        ata_port[p].cmd_block_base_address = 
            ATA_BAR2_SECONDARY_COMMAND_PORT;
        ata_port[p].ctrl_block_base_address = 
            ATA_BAR3_SECONDARY_CONTROL_PORT;
        ata_port[p].bus_master_base_address = 
            ATA_BAR4 + 8;
        break;

    //default:
        //PANIC
        //break;
    };
}

// local
// __ata_config_structure:
// Set up the ata_port.xxx structure.
// #todo: Where is that structure defined?
// See: ata.h
// De acordo com a porta, saberemos se é 
// primary ou secondary e se é
// master ou slave.

static unsigned char __ata_config_structure (char nport)
{
// Selecting the channel(primary or secondary) and 
// the device number(master or slave) 
// given the port number.

// todo
// filtrar limits.

    // 0~3
    switch (nport){
    case 0:
        ata_port[nport].channel = ATA_PRIMARY;    //0;  // primary
        ata_port[nport].dev_num = ATA_MASTER;     //0;  // not slave ATA_MASTER
        break;
    case 1: 
        ata_port[nport].channel = ATA_PRIMARY;    //0;  // primary
        ata_port[nport].dev_num = ATA_SLAVE;      //1;  // slave    ATA_SLAVE
        break;
    case 2:
        ata_port[nport].channel = ATA_SECONDARY;  //1;  // secondary
        ata_port[nport].dev_num = ATA_MASTER;     //0;  // not slave ATA_MASTER
        break;
    case 3:
        ata_port[nport].channel = ATA_SECONDARY;  //1;  // secondary
        ata_port[nport].dev_num = ATA_SLAVE;      //1;  // slave    ATA_SLAVE
        break;
    default:
        // panic ?
        printk ("Port %d not used\n", nport );
        return -1;
        break;
    };

// local worker.
// Select the i/o ports given the channel.
// The ports we found in the BARs.
    __set_ata_addr(
        nport,
        ata_port[nport].channel );

    return 0;
}

// atapi_pio_read:
// #todo
// Avoid this for compatibility with another compiler.
inline void atapi_pio_read ( int p, void *buffer, uint32_t bytes )
{
// No checks!
// #todo: Port number via parameter.

    //if (p<0)
        //return;

    if (bytes == 0){
        return;
    }

    asm volatile  (\
        "cld;\
        rep; insw"::"D"(buffer),\
        "d"(ata_port[p].cmd_block_base_address + ATA_REG_DATA),\
        "c"(bytes/2) );
}

void ata_set_boottime_ide_port_index(unsigned int port_index)
{
    g_boottime_ide_port_index = (int) port_index;
}

int ata_get_boottime_ide_port_index(void)
{
    return (int) g_boottime_ide_port_index;
}

void ata_set_current_ide_port_index(unsigned int port_index)
{
    g_current_ide_port_index = (int) port_index;
}

int ata_get_current_ide_port_index(void)
{
    return (int) g_current_ide_port_index;
}

// dev_switch:
static void dev_switch(void)
{
// Worker

// ??
// Pula, se ainda não tiver nenhuma unidade.

    if ( !current_sd )
    {
        return;
    }

// Obter a próxima tarefa a ser executada.
// Se caímos no final da lista vinculada, 
// comece novamente do início.
    current_sd = current_sd->next;    
    if ( !current_sd ){
        current_sd = ready_queue_dev;
    }
}

static int getnport_dev(void)
{
    if ((void *) current_sd == NULL){
        return -1;
    }
    return (int) current_sd->dev_nport;
}

// #todo
// Explain it better.
static int nport_ajust(char nport)
{
    char i = 0;

    // #todo
    // Simplify this thing.
 
    while ( nport != getnport_dev() )
    {
        if (i == 4){
            return (int) 1; 
        }
        dev_switch();
        i++;
    };

    if ( getnport_dev() == -1 )
    { 
        return (int) 1; 
    }

    return 0;
}

// #todo ioctl
int 
ata_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg )
{
    debug_print("ata_ioctl: #todo\n");
    if(fd<0){
        return -1;
    }
    return -1;
}


// __ide_identify_device:
// O número da porta identica qual disco queremos pegar informações.
// Salvaremos algumas informações na estrutura de disco.
// OUT:
// 0xFF = erro ao identificar o número.
// 0    = PATA ou SATA
// 0x80 = ATAPI ou SATAPI

static int __ide_identify_device(uint8_t nport)
{
// Called by ata_initialize_ide_device().
// + Identify the device.
// + Register device info into the structure.

    // Signature bytes.
    unsigned char sig_byte_1=0xFF;
    unsigned char sig_byte_2=0xFF;
    unsigned char status=0;
    char name_buffer[32];
    struct disk_d  *disk;

    //debug_print("__ide_identify_device:\n");

// #todo
// What is the limit?
// The limit here is 4.
// See: ide.h
    if (nport >= 4){
        panic("__ide_identify_device: nport\n");
    }

// Selecting the channel(primary or secondary) and 
// the device number(master or slave) 
// given the port number.
    __ata_config_structure(nport);

// ??
// Ponto flutuante
// Sem unidade conectada ao barramento
// #todo
// Precisamos de uma mensagem aqui.
    if ( ata_status_read(nport) == 0xFF ){
        debug_print("__ide_identify_device: ata_status_read(nport)\n");
        goto fail;
    }


// Select device
// 0xA0 or 0xE0 ?
// #todo:
// Review the data sent to the port.
    out8( 
        (unsigned short) ( ata_port[nport].cmd_block_base_address + ATA_REG_DEVSEL), 
        (unsigned char) 0xE0 | ata_port[nport].dev_num << 4 );

// Sector count
    out8 ( ata_port[nport].cmd_block_base_address + ATA_REG_SECCOUNT, 0 );  // Sector Count 7:0

// Reset values for LBA
    out8 ( ata_port[nport].cmd_block_base_address + ATA_REG_LBA0,     0 );  // LBA  7- 0
    out8 ( ata_port[nport].cmd_block_base_address + ATA_REG_LBA1,     0 );  // LBA 15- 8
    out8 ( ata_port[nport].cmd_block_base_address + ATA_REG_LBA2,     0 );  // LBA 23-16


// Identify
// Get information

// cmd
    ata_wait(400);
    ata_wait(400);
    ata_cmd_write (nport,ATA_CMD_IDENTIFY_DEVICE); 

// fail?
    ata_wait(400);
    ata_wait(400);
    if (ata_status_read(nport) == 0){
        debug_print("__ide_identify_device: ata_status_read(nport)\n");
        goto fail;
    }

// #todo
// Use esses registradores para pegar mais informações.
// See:
// hal/dev/blkdev/ata.h
// See
// https://wiki.osdev.org/ATA_PIO_Mode
// https://wiki.osdev.org/PCI_IDE_Controller

/*
    // #exemplo:
    // get the "signature bytes" 
    unsigned cl=inb(ctrl->base + REG_CYL_LO);
    unsigned ch=inb(ctrl->base + REG_CYL_HI);
    differentiate ATA, ATAPI, SATA and SATAPI 
    if (cl==0x14 && ch==0xEB) return ATADEV_PATAPI;
    if (cl==0x69 && ch==0x96) return ATADEV_SATAPI;
    if (cl==0 && ch == 0) return ATADEV_PATA;
    if (cl==0x3c && ch==0xc3) return ATADEV_SATA;
 */

// Saving.
    // lba1 = in8( ata_port[nport].cmd_block_base_address + ATA_REG_LBA1 );
    // lba2 = in8( ata_port[nport].cmd_block_base_address + ATA_REG_LBA2 );

    // REG_CYL_LO = 4
    // REG_CYL_HI = 5

// Signature:
// Getting signature bytes.
    sig_byte_1 = in8( ata_port[nport].cmd_block_base_address + 4 );
    sig_byte_2 = in8( ata_port[nport].cmd_block_base_address + 5 );

// #todo
// In this moment we're trying to find 
// the size of the disk given in sectors.

/*
//===============

// #test
// Isso provavelmente é o número de setores envolvidos em
// uma operação de leitura ou escrita.

    unsigned long rw_size_in_sectors=0;
    rw_size_in_sectors = in8( ata_port[nport].cmd_block_base_address + ATA_REG_SECCOUNT );
    if (rw_size_in_sectors>0){
       printk("::#breakpoint NumberOfSectors %d \n",rw_size_in_sectors);
       refresh_screen();
       while(1){}
    }
    //===============
*/

/*

// Sector Number Register 
// (LBAlo) This is CHS / LBA28 / LBA48 specific.
#define ATA_REG_LBA0      0x03

// Cylinder Low Register 
// (LBAmid) Partial Disk Sector address.
#define ATA_REG_LBA1      0x04

// Cylinder High Register 
// (LBAhi) Partial Disk Sector address.
#define ATA_REG_LBA2      0x05
*/

// See:
// https://forum.osdev.org/viewtopic.php?f=1&t=22563

/*
#define DRDY  0x40
#define BSY   0x80
#define HOB   0x80
#define Sector_Count    2
#define LBA_Low         3
#define LBA_Mid         4
#define LBA_High        5
#define Device          6
#define Status          7
#define Command         7
    unsigned long Max_LBA=0;
    unsigned int port = (ata_port[nport].cmd_block_base_address & 0xFFFF);
*/    

//#atençao
//Isso foi chamado logo acima.
    //out8(port+Command, 0xF8);  //READ NATIVE MAX ADDRESS
    //out8(port+Command, 0xEC);  //ATA_CMD_IDENTIFY_DEVICE

// Wait 
    //while ( in8(port+Status) &  BSY != 0)
    //{
        // wait command completed
    //}

// Wait
    //int w=0;
    //for(w=0;w<400000;w++){}

/*
// Not lba48
    Max_LBA  = (unsigned long long )in8(port+LBA_Low);
    Max_LBA += (unsigned long long )in8(port+LBA_Mid)  <<8;
    Max_LBA += (unsigned long long )in8(port+LBA_High) <<16;
    Max_LBA += ((unsigned long long )in8(port+Device) & 0xF) <<24;
*/

    //unsigned char buffer[512+1];
    //unsigned short buffer[512+1];

    // ata_port[nport].cmd_block_base_address?
    // Isso foi configurado logo acima,
    // então a função hdd_ata_pio_read
    // vai usar a mesma porta que estamos 
    // usando nessa função.

    //debug_print("ide_identify_device: hdd_ata_pio_read()\n");

/*
    hdd_ata_pio_read ( 
        (unsigned int) nport, 
        (void *) buffer, 
        (int) 512 );
*/

/*
    ata_wait_drq(p);
    __ata_pio_read ( p, buffer, 512 );

    ata_wait_not_busy(p);
    ata_wait_no_drq(p);
*/

/*  
    //#todo: 60 se for words e 120 se for bytes.
    Max_LBA = buffer[60] &0xffff;
    Max_LBA += buffer[61] << 16 &0xffffffff;

    if( Max_LBA > 0 )
    {
        printk("::#breakpoint Max_LBA %d \n",Max_LBA);
        //refresh_screen();
        //while(1){}
        
        ide_ports[nport].size_in_sectors = (unsigned long) Max_LBA;
    }
*/

// =============================================================

// #test
// vamos pegar mais informações. 

//
// # Type
//

/*
	if (lba1==0x00 && lba2==0x00) = ATADEV_PATA
	if (lba1==0x3c && lba2==0xc3) = ATADEV_SATA
	if (lba1==0x14 && lba2==0xEB) = ATADEV_PATAPI
	if (lba1==0x69 && lba2==0x96) = ATADEV_SATAPI
*/

// ==========================
// # PATA
    if ( sig_byte_1 == ATADEV_PATA_SIG1 && sig_byte_2 == ATADEV_PATA_SIG2 )
    {
        // kputs("Unidade PATA\n");
        // aqui esperamos pelo DRQ
        // e lemos 256 word de dados PIO

        ata_wait_drq(nport);
        __ata_pio_read( nport, ata_devinfo_buffer, 512 );

        ata_wait_not_busy(nport);
        ata_wait_no_drq(nport);

        // See: ide.h

        ide_ports[nport].id = (uint8_t) nport;           // Port index.
        ide_ports[nport].type = (int) idedevicetypesPATA;  // Device type.
        ide_ports[nport].name = "PATA";
        ide_ports[nport].channel = ata_port[nport].channel;  // Primary or secondary.
        ide_ports[nport].dev_num = ata_port[nport].dev_num;  // Master or slave.
        ide_ports[nport].used = (int) TRUE;
        ide_ports[nport].magic = (int) 1234;

        // Disk
        // See: disk.h
        disk = (struct disk_d *) kmalloc( sizeof(struct disk_d) );
        if ((void*) disk == NULL){
            debug_print("__ide_identify_device: disk on PATA\n");
            goto fail;
        }
        if ((void *) disk != NULL)
        {
            memset (disk, 0, sizeof(struct disk_d) );

            // Object header.
            // #todo
            //disk->objectType = ?
            //disk->objectClass = ?

            disk->used = TRUE;
            disk->magic = 1234;
            // type and class.
            disk->diskType = DISK_TYPE_PATA;
            // disk->diskClass = ? // #todo:

            // ID and index.
            disk->id = nport;
            disk->boot_disk_number = 0; // ?? #todo

            // name
 
            // name = "sd?"
            //disk->name = "PATA-TEST";
            ksprintf ( (char *) name_buffer, "PATA-TEST-%d",nport);
            disk->name = (char *) strdup ( (const char *) name_buffer);  

            // Security
            disk->pid = (pid_t) get_current_process(); //current_process;
            disk->gid = current_group;
            // ...

            disk->channel = ata_port[nport].channel;  // Primary or secondary.
            disk->dev_num = ata_port[nport].dev_num;  // Master or slave.

            // disk->next = NULL;
            
            // #bugbug
            // #todo: Check overflow.
            diskList[nport] = (unsigned long) disk;
        }

        // It's a PATA device.
        // 0 = PATA or SATA.
        return (int) 0;
    }

// ==========================
// # PATAPI
    if ( sig_byte_1 == ATADEV_PATAPI_SIG1 && sig_byte_2 == ATADEV_PATAPI_SIG2 )
    {
        //kputs("Unidade PATAPI\n");   
        ata_cmd_write(nport,ATA_CMD_IDENTIFY_PACKET_DEVICE);
        ata_wait(400);
        ata_wait_drq(nport); 
        __ata_pio_read( nport, ata_devinfo_buffer, 512 );
        ata_wait_not_busy(nport);
        ata_wait_no_drq(nport);

        // See: ide.h

        ide_ports[nport].id = (uint8_t) nport;
        ide_ports[nport].type = (int) idedevicetypesPATAPI;
        ide_ports[nport].name = "PATAPI";
        ide_ports[nport].channel = ata_port[nport].channel;  // Primary or secondary.
        ide_ports[nport].dev_num = ata_port[nport].dev_num;  // Master or slave.
        ide_ports[nport].used = (int) TRUE;
        ide_ports[nport].magic = (int) 1234;

        // Disk
        // See: disk.h
        disk = (struct disk_d *) kmalloc(sizeof(struct disk_d));
        if ((void*) disk == NULL){
            debug_print("__ide_identify_device: disk on PATAPI\n");
            goto fail;
        }
        if ((void *) disk != NULL)
        {
            memset (disk, 0, sizeof(struct disk_d) );
            disk->used = TRUE;
            disk->magic = 1234;
            disk->diskType = DISK_TYPE_PATAPI;
            // disk->diskClass = ?

            disk->id = nport;  
                        
            // name
            
            //disk->name = "PATAPI-TEST";
            ksprintf ( (char *) name_buffer, "PATAPI-TEST-%d",nport);
            disk->name = (char *) strdup ( (const char *) name_buffer);  

            disk->channel = ata_port[nport].channel;  // Primary or secondary.
            disk->dev_num = ata_port[nport].dev_num;  // Master or slave.

            // #todo: Check overflow.
            diskList[nport] = (unsigned long) disk;
        }

        // It's a PATAPI device.
        // 0x80 = PATAPI or SATAPI.
        return (int) 0x80;
    }

// ==========================
// #SATA
    if ( sig_byte_1 == ATADEV_SATA_SIG1 && sig_byte_2 == ATADEV_SATA_SIG2 )
    {
        //kputs("Unidade SATA\n");   
        // O dispositivo responde imediatamente um erro ao cmd Identify device
        // entao devemos esperar pelo DRQ ao invez de um BUSY
        // em seguida enviar 256 word de dados PIO.

        ata_wait_drq(nport); 
        __ata_pio_read ( nport, ata_devinfo_buffer, 512 );
        ata_wait_not_busy(nport);
        ata_wait_no_drq(nport);

        // See: ide.h

        ide_ports[nport].id = (uint8_t) nport;
        ide_ports[nport].type = (int) idedevicetypesSATA;  // Device type.
        ide_ports[nport].name = "SATA";                    // Port name.
        ide_ports[nport].channel = ata_port[nport].channel;  // Primary or secondary.
        ide_ports[nport].dev_num = ata_port[nport].dev_num;  // Master or slave.
        ide_ports[nport].used = (int) TRUE;
        ide_ports[nport].magic = (int) 1234;

        // Disk
        // See: disk.h
        disk = (struct disk_d *) kmalloc(sizeof(struct disk_d));
        if ((void*) disk == NULL){
            debug_print("__ide_identify_device: disk on SATA\n");
            goto fail;
        }
        if ((void *) disk != NULL)
        {
            memset (disk, 0, sizeof(struct disk_d) );
            // Object header.
            // #todo
            //disk->objectType = ?
            //disk->objectClass = ?

            disk->used = TRUE;
            disk->magic = 1234;
            disk->diskType = DISK_TYPE_SATA;
            // disk->diskClass = ?

            // ID and index.
            disk->id = nport;
            disk->boot_disk_number = 0; // ?? #todo

            // name

            //disk->name = "SATA-TEST";
            ksprintf( (char *) name_buffer, "SATA-TEST-%d",nport);
            disk->name = (char *) strdup((const char *) name_buffer);  

            // Security
            disk->pid = (pid_t) get_current_process();  //current_process;
            disk->gid = current_group;
              
            disk->channel = ata_port[nport].channel;  // Primary or secondary.
            disk->dev_num = ata_port[nport].dev_num;  // Master or slave.
 
            // disk->next = NULL;
            
            // #todo: Check overflow.
            diskList[nport] = (unsigned long) disk;
        }

        // It's a SATA device.
        // 0 = PATA or SATA.
        return (int) 0;
    }

// ==========================
// # SATAPI
    if (sig_byte_1 == ATADEV_SATAPI_SIG1  && sig_byte_2 == ATADEV_SATAPI_SIG2)
    {
        //kputs("Unidade SATAPI\n");   
        ata_cmd_write(nport,ATA_CMD_IDENTIFY_PACKET_DEVICE);
        ata_wait(400);
        ata_wait_drq(nport); 
        __ata_pio_read(nport,ata_devinfo_buffer,512);
        ata_wait_not_busy(nport);
        ata_wait_no_drq(nport);

        // See: ide.h
        ide_ports[nport].id = (uint8_t) nport;
        ide_ports[nport].type = (int) idedevicetypesSATAPI;
        ide_ports[nport].name = "SATAPI";
        ide_ports[nport].channel = ata_port[nport].channel;  // Primary or secondary.
        ide_ports[nport].dev_num = ata_port[nport].dev_num;  // Master or slave.
        ide_ports[nport].used = (int) TRUE;
        ide_ports[nport].magic = (int) 1234;

        // Disk
        // See: disk.h
        disk = (struct disk_d *) kmalloc(sizeof(struct disk_d));
        if ((void*) disk == NULL){
            debug_print("__ide_identify_device: disk on SATAPI\n");
            goto fail;
        }
        if ((void *) disk != NULL)
        {
            memset (disk, 0, sizeof(struct disk_d) );
            disk->used = TRUE;
            disk->magic = 1234;
            disk->diskType = DISK_TYPE_SATAPI;
            //disk->diskClass = ?

            disk->id = nport;  

            // name
            
            //disk->name = "SATAPI-TEST";
            ksprintf((char *) name_buffer, "SATAPI-TEST-%d",nport);
            disk->name = (char *) strdup((const char *) name_buffer);  

            disk->channel = ata_port[nport].channel;  // Primary or secondary.
            disk->dev_num = ata_port[nport].dev_num;  // Master or slave.

            // #todo: Check overflow.
            diskList[nport] = (unsigned long) disk;
        }

        // It's a SATAPI device.
        // 0x80 = PATAPI or SATAPI.
        return (int) 0x80;
    }

// fail ??
// Is something wrong here?
    // #debug
    //panic("ide_identify_device: type not defined");
// 0xFF: Unknown device class.
fail:
    debug_print("__ide_identify_device: fail\n");
    return (int) 0xFF;
}

// ata_initialize_ide_device:
// ?? Alguma rotina de configuração de dispositivos.
// This routine was called by ata_initialize.
// #todo
// Agora essa função precisa receber um ponteiro 
// para a estrutura de disco usada pelo gramado.
// Para salvarmos os valores que pegamos nos registradores.
// Called by ata_initialize.
static int ata_initialize_ide_device(char port)
{
// Inicializa uma porta e coloca numa lista encadeada de
// estruturas de ata_device.

// This routine is too long.
// And it is gonna setup the values in the structure
// given the port.
// #bugbug
// The structure is gonna stay with the
// values for the last time we called this routine.
// So, it's gonna work only if we're 
// using a single device.
// #todo
// We gotta save the device independent info
// in a proper ata device information structure.
// Not in the controller information structure.

    struct ata_device_d  *new_dev;

    int isBootTimeIDEPort=FALSE;
    int data=0;
    unsigned long value1=0;
    unsigned long value2=0;
    unsigned long value3=0;
    unsigned long value4=0;

// Limits
// 4 ports only    
    if (port < 0 || port >= 4){
        printk("ata_initialize_ide_device: port\n");
        goto fail;
    }

// storage
// We need this structure.
    if ((void*) storage == NULL){
        panic("ata_initialize_ide_device: storage\n");
    }

// boot disk
// We need this structure.
    if ((void*) ____boot____disk == NULL){
        panic("ata_initialize_ide_device: ____boot____disk\n");
    }
    if ( ____boot____disk->magic != 1234 ){
        panic("ata_initialize_ide_device: ____boot____disk->magic\n");
    }

// boot partition
// We need this structure.
    if ((void*) volume_bootpartition == NULL){
        panic("ata_initialize_ide_device: volume_bootpartition\n");
    }
    if (volume_bootpartition->magic != 1234){
        panic("ata_initialize_ide_device: volume_bootpartition->magic\n");
    }

// Is it the boottime ide port?
    int boottime_ideport = ata_get_boottime_ide_port_index();
   
    // YES!
    if (port == boottime_ideport){
        isBootTimeIDEPort = TRUE;
    }

//
// new_dev
//

// See: ata.h
// Ata device structure.

    new_dev = (struct ata_device_d *) kmalloc( sizeof(struct ata_device_d) );
    if ((void *) new_dev ==  NULL){
        printk("ata_initialize_ide_device: new_dev\n");
        goto fail;
    }
    memset ( new_dev, 0, sizeof(struct ata_device_d) );

// Validation
    new_dev->used = TRUE;
    new_dev->magic = 1234;
// Is this structure already initialized or not?
    new_dev->initialized = FALSE;

// ??
// #todo: 
// Explain it.
// This routine is too long.
// And it is gonna setup the values in the structure
// given the port.

// Worker
// + Identify the device.
// + Register device info into the structure.
// ================
// Unidade de classe desconhecida.
// Erro
// 0xFF = erro ao identificar o número.
// 0    = PATA ou SATA
// 0x80 = ATAPI ou SATAPI

    data = (int) __ide_identify_device(port);
    if (data == 0xFF){
        debug_print ("ata_initialize_ide_device: Invalid device type\n");
        return (int) -1;
    }

// ================
// Unidades ATA.
// 0 = PATA ou SATA
    if (data == 0){

        // Is it an ata device?
        new_dev->dev_type = 
            (ata_devinfo_buffer[0] & 0x8000) ? 0xffff : ATA_DEVICE_TYPE;

        // What kind of lba?
        new_dev->dev_access = 
            (ata_devinfo_buffer[83] & 0x0400) ? ATA_LBA48 : ATA_LBA28;

        // Let's set up the PIO support.
        // Where ATAFlag was defined?
        // Where FORCEPIO was defined?

        // Com esse só funciona em pio
        if (ATAFlag == FORCEPIO){
            new_dev->dev_modo_transfere = 0;
        // Com esse pode funcionar em dma
        }else{
            new_dev->dev_modo_transfere = 
                 ( ata_devinfo_buffer[49] & 0x0100 ) 
                 ? ATA_DMA_MODO 
                 : ATA_PIO_MODO;
        };

        //old
        //new_dev->dev_total_num_sector  = ata_devinfo_buffer[60];
        //new_dev->dev_total_num_sector += ata_devinfo_buffer[61];

        //#test
        //new_dev->dev_total_num_sector  = 
        //    *(unsigned long*) (short *) &ata_devinfo_buffer[60];

        // #bugbug
        // We can not do this.
        // We need to check the supported sector size.
        // 512 or 4096 ?

        new_dev->dev_byte_per_sector = 512;

        //new_dev->dev_total_num_sector_lba48  = ata_devinfo_buffer[100];
        //new_dev->dev_total_num_sector_lba48 |= ata_devinfo_buffer[101];
        //new_dev->dev_total_num_sector_lba48 |= ata_devinfo_buffer[102];
        //new_dev->dev_total_num_sector_lba48 |= ata_devinfo_buffer[103];

        value1 = ata_devinfo_buffer[103];  
        new_dev->lba48_value1 = (unsigned short) value1;
        value1 = ( value1 << 48 );           
        value1 = ( value1 & 0xFFFF000000000000 );    
        
        value2 = ata_devinfo_buffer[102];  
        new_dev->lba48_value2 = (unsigned short) value2;
        value2 = ( value2 << 32 );
        value2 = ( value2 & 0x0000FFFF00000000 );

        value3 = ata_devinfo_buffer[101];
        new_dev->lba48_value3 = (unsigned short) value3;  
        value3 = ( value3 << 16 );
        value3 = ( value3 & 0x00000000FFFF0000 );

        value4 = ata_devinfo_buffer[100];
        new_dev->lba48_value4 = (unsigned short) value4;  
        //value4 = ( value4 << 0 );
        value4 = ( value4 & 0x000000000000FFFF );
          
        new_dev->dev_total_num_sector_lba48 = 
            (unsigned long) (value1 | value2 | value3 | value4);

        // #bugbug
        // We can not do this.
        // We need to check the supported sector size.
        // 512 or 4096 ?

        new_dev->dev_size = 
            (new_dev->dev_total_num_sector_lba48 * 512);

        // Pegando o size.
        // Quantidade de setores.
        // Uma parte está em 61 e outra em 60.

        value1 = ata_devinfo_buffer[61];  // First part
        new_dev->lba28_value1 = (unsigned short) value1;
        value1 = ( value1 << 16 );
        value1 = ( value1 & 0xFFFF0000 );
        value2 = ata_devinfo_buffer[60];  // Second part
        new_dev->lba28_value2 = (unsigned short) value2;
        value2 = ( value2 & 0x0000FFFF );
        // Total number of blocks.
        new_dev->dev_total_num_sector = (value1 | value2);
        new_dev->_MaxLBA = new_dev->dev_total_num_sector;

        //if ( new_dev->dev_total_num_sector > 0 )
        //{
        //     printk ("#debug: >>>> ata Size %d\n", 
        //         new_dev->dev_total_num_sector );
        //     refresh_screen();
        //     while(1){}
        //}

// #todo
// Agora essa função precisa receber um ponteiro 
// para a estrutura de disco usada pelo gramado.

// ================
// Unidades ATAPI. 
// 0x80 = ATAPI ou SATAPI
    }else if (data == 0x80){

        // Is this an ATAPI device ?
        new_dev->dev_type = 
            (ata_devinfo_buffer[0] & 0x8000) ? ATAPI_DEVICE_TYPE : 0xffff;

        // What kind of lba?
        new_dev->dev_access = ATA_LBA28;

        // Let's set up the PIO support.
        // Where ATAFlag was defined?
        // Where FORCEPIO was defined?

        // Com esse só funciona em pio 
        if (ATAFlag == FORCEPIO){
            new_dev->dev_modo_transfere = 0; 

        // Com esse pode funcionar em dma
        }else{
            new_dev->dev_modo_transfere = (ata_devinfo_buffer[49] & 0x0100) ? ATA_DMA_MODO : ATA_PIO_MODO;
        };

        // ?
        new_dev->dev_total_num_sector  = 0;
        new_dev->dev_total_num_sector += 0;

        // #bugbug
        // 2024
        // Is this standard for all kind of CD?
        // We need to get this information in some place.

        new_dev->dev_byte_per_sector = 2048; 

        //#test
        //new_dev->dev_total_num_sector  = 
            //*(unsigned long*) (short *) &ata_devinfo_buffer[60];

        new_dev->dev_total_num_sector_lba48  = 0;
        new_dev->dev_total_num_sector_lba48 |= 0;
        new_dev->dev_total_num_sector_lba48 |= 0;
        new_dev->dev_total_num_sector_lba48 |= 0;

        value1 = ata_devinfo_buffer[103];  
        new_dev->lba48_value1 = (unsigned short) value1;
        value1 = ( value1 << 48 );           
        value1 = ( value1 & 0xFFFF000000000000 );    

        value2 = ata_devinfo_buffer[102];  
        new_dev->lba48_value2 = (unsigned short) value2;
        value2 = ( value2 << 32 );
        value2 = ( value2 & 0x0000FFFF00000000 );

        value3 = ata_devinfo_buffer[101];
        new_dev->lba48_value3 = (unsigned short) value3;  
        value3 = ( value3 << 16 );
        value3 = ( value3 & 0x00000000FFFF0000 );

        value4 = ata_devinfo_buffer[100];
        new_dev->lba48_value4 = (unsigned short) value4;  
        //value4 = ( value4 << 0 );
        value4 = ( value4 & 0x000000000000FFFF );

        new_dev->dev_total_num_sector_lba48 = 
            (unsigned long) (value1 | value2 | value3 | value4);

        // #bugbug
        // 2024
        // Is this standard for all kind of CD?
        // We need to get this information in some place.

        new_dev->dev_size = (new_dev->dev_total_num_sector_lba48 * 2048);

        // Pegando o size.
        // Quantidade de setores.
        // Uma parte está em 61 e outra em 60.
 
        value1 = ata_devinfo_buffer[61];
        new_dev->lba28_value1 = (unsigned short) value1;  
        value1 = ( value1 << 16 );           
        value1 = ( value1 & 0xFFFF0000 );    
        value2 = ata_devinfo_buffer[60];
        new_dev->lba28_value2 = (unsigned short) value2;  
        value2 = ( value2 & 0x0000FFFF );  
        new_dev->dev_total_num_sector = (value1 | value2);
        new_dev->_MaxLBA = new_dev->dev_total_num_sector;

        //if ( new_dev->dev_total_num_sector > 0 )
        //{
        //   printk ("#debug: >>>> atapi Size %d\n", 
        //       new_dev->dev_total_num_sector );
        //   refresh_screen();
        //   while(1){}
        //}

        // #todo
        // Agora essa função precisa receber um ponteiro 
        // para a estrutura de disco usada pelo gramado.

// ================
// Unidade de classe desconhecida.
// #bugbug: Not panic()
    } else {
        debug_print("ata_initialize_ide_device: [ERROR] not ATA, not ATAPI.\n");
        return (int) -1;
    };

//
// Dados em comum.
//

    new_dev->dev_id = __next_sd_id++;

// Salvando na estrutura de dispositivo as
// informações sobre a porta ide.
// channel and device.
// ex: primary/master.
// #bugbug
// Mas temos um problema. Talvez quando essa função
// foi chamada o dev_num ainda não tenha cido inicializado.

    new_dev->dev_channel = ata_port[port].channel;
    new_dev->dev_num     = ata_port[port].dev_num;

    new_dev->dev_nport = port;

//
// bootitme device?
//

// This is the boottime ide port.
// So we're gonna save the pointer
// into the boot disk structure.

// Not a boottime device.
    new_dev->boottime_device = FALSE;

    if (isBootTimeIDEPort == TRUE)
    {
        if ((void*) ____boot____disk != NULL)
        {
            if (____boot____disk->magic == 1234)
            {
                // ??
                // Only for ATA devices.
                ____boot____disk->ata_device = 
                    (struct ata_device_d *) new_dev;

                new_dev->disk = (struct disk_d *) ____boot____disk;

                // YES, it's a boottime device.
                new_dev->boottime_device = TRUE;
            }
        }
    }

//
// == port ====================================
//

// #bugbug
// Nao devemos confundir esses numeros com os numeros
// gerados pelo BIOS, pois bios tambem considera
// outras midias alem do ATA.

// #atenção
// Essa estrutura é para 32 portas.
// para listar as portas AHCI.
// Mas aqui está apenas listando as 4 portas ATA.

    switch (port){
    case 0:  dev_nport.dev0 = 0x81;  break;
    case 1:  dev_nport.dev1 = 0x82;  break;
    case 2:  dev_nport.dev2 = 0x83;  break;
    case 3:  dev_nport.dev3 = 0x84;  break;
    // #bugbug
    // Return?
    default:
        debug_print("ata_initialize_ide_device: [ERROR] default port number\n");
        break;
    };

// Linked list.
    new_dev->next = NULL;

    // #debug
    // printk("[ Detected Disk type: %s ]\n", dev_type[new_dev->dev_type] );
    // refresh_screen ();

// =========================================

//
// Add no fim da lista (ready_queue_dev).
//
    struct ata_device_d *tmp;
    tmp = (struct ata_device_d *) ready_queue_dev;
    if ((void *) tmp ==  NULL){
        printk("ata_initialize_ide_device: [FAIL] tmp\n");
        goto fail;
    }

// Linked list. Walk.
// Colocaremos esse novo ponteiro no fim da lista.
    while (tmp->next){
        tmp = tmp->next;
    };
// Save at the end of th elist.
    tmp->next = new_dev;

// Validation
    new_dev->used = TRUE;
    new_dev->magic = 1234;
// Is this structure already initialized or not?
    new_dev->initialized = TRUE;
//done:
    //debug_print ("ata_initialize_ide_device: done\n");
    return 0;

fail:
    refresh_screen();
    panic("ata_initialize_ide_device: fail\n");
    return -1;  // Not reached.
}

// ++
//----------------------------------------------

// __ata_initialize:
// Inicializa o ATA e mostra informações sobre o disco.
// Sondando na lista de dispositivos encontrados 
// pra ver se tem algum controlador de disco ATA.
// IN: ?
// Configurando flags do driver.
// FORCEPIO = 1234
// Called by ataDialog in atainit.c

static int __ata_initialize(int ataflag)
{
// Called by DDINIT_ata().
// Here we're gonna know some things about the ata controller.
// + What is the type of ata controller we have: ATA, RAID or AHCI.
// + For ATA controller we're gonna initialize the 'ports', or the
//   ATA devices we have in this controller.
// + It's gonna fail for RAID and AHCI.

/*
	#todo
	Me parece que em nenhum momento na rotina de inicialização
	o dispositivo foi registrado, como acontece na sondagem dos 
	outros dispositivos pci.
	Precisa registrar na estrutura de dispositivos e na
	lista de dispositivos. Colocando informações como
	a classe do dispositivo. (char, block, network). No caso, block.
*/

    int Status = -1;  //error
    int Value = -1;
    // Iterator.
    int iPortNumber=0;

    PROGRESS("__ata_initialize:\n");

// Setup interrupt breaker.
    //debug_print("ata_initialize: Turn on interrupt breaker\n");
    __breaker_ata1_initialized = FALSE;
    __breaker_ata2_initialized = FALSE;

// Driver status.
    g_ata_driver_initialized = FALSE;
    AtaController.initialized = FALSE;

// A estrutura ainda nao foi configurada.
    //ata_port.used = FALSE;
    //ata_port.magic = 0;

// ======================================
// #importante 
// HACK HACK
// Usando as definições feitas em config.h
// até que possamos encontrar dinamicamente 
// o canal e o dispositivo certos.
// __IDE_PORT indica qual é o indice de porta.
// Configumos o atual como sendo o mesmo
// usado durante o boot.
// #todo
// Poderemos mudar o atual conforme nossa intenção
// de acessarmos outros discos.
// See: config.h


    unsigned int boottime_ideport_index = __IDE_PORT;
    unsigned int current_ideport_index = __IDE_PORT;

/*
    //#debug
    printk ("kernel CONFIG:     ATA port: %d\n",boottime_ideport_index);   // from config.h
    printk ("kernel xBootBlock: ATA port: %d\n",xBootBlock.ide_port_number);  // from bootblock, from bl.bin
    refresh_screen();
    while(1){}
*/

    ata_set_boottime_ide_port_index(boottime_ideport_index);
    ata_set_current_ide_port_index(current_ideport_index);
// ============================================

// ??
// Configurando flags do driver.
// FORCEPIO = 1234

    ATAFlag = (int) ataflag;

//
// Messages
//


    // #debug
    // printk ("Initializing ATA/AHCI support ...\n");

// #test
// Sondando na lista de dispositivos encontrados 
// pra ver se tem algum controlador de disco ATA.
// #importante:
// Estamos sondando uma lista que contruimos quando fizemos
// uma sondagem no começo da inicializaçao do kernel.
// #todo: 
// Podemos salvar essa lista.
// #todo
// O que é PCIDeviceATA?
// É uma estrutura para dispositivos pci. (pci_device_d)
// Vamos mudar de nome.

// pci device.
    PCIDeviceATA = 
        (struct pci_device_d *) scan_pci_device_list2 ( 
                                    (unsigned char) PCI_CLASSCODE_MASS, 
                                    (unsigned char) PCI_SUBCLASS_IDE );

    if ((void *) PCIDeviceATA == NULL){
        printk("__ata_initialize: PCIDeviceATA\n");
        Status = (int) -1;
        goto fail;
    }
    if ( PCIDeviceATA->used != TRUE || PCIDeviceATA->magic != 1234 ){
        printk ("__ata_initialize: PCIDeviceATA validation\n");
        Status = (int) -1;
        goto fail;
    }

//#debug
    // printk (": ATA device found\n");
    // printk ("[ Vendor=%x Device=%x ]\n", PCIDeviceATA->Vendor, PCIDeviceATA->Device );

// Vamos saber mais sobre o dispositivo encontrado. 
// #bugbug: 
// Esse retorno é só um código de erro.
// Nessa hora configuramos os valores na estrutura 'ata_port.xxx'
// see: atapci.c

    Value = 
        (unsigned long) atapciSetupMassStorageController( (struct pci_device_d*) PCIDeviceATA );

    if (Value == PCI_MSG_ERROR){
        printk ("__ata_initialize: Error Driver [%x]\n", Value );
        Status = (int) -1;
        goto fail;
    }

// Explicando:
// Aqui estamos pegando nas BARs o número das portas.
// Logo em seguida salvaremos esses números e usaremos
// eles para fazer uma rotina de soft reset.
// See:
// https://wiki.osdev.org/PCI_IDE_Controller

// base address 
// BAR0 is the start of the I/O ports used by the primary channel.
// BAR1 is the start of the I/O ports which control the primary channel.
// BAR2 is the start of the I/O ports used by secondary channel.
// BAR3 is the start of the I/O ports which control secondary channel.
// BAR4 is the start of 8 I/O ports controls the primary channel's Bus Master ATA.
// BAR4 + 8 is the Base of 8 I/O ports controls secondary channel's Bus Master ATA.

    ATA_BAR0_PRIMARY_COMMAND_PORT   = ( PCIDeviceATA->BAR0 & ~7 ) + ATA_IDE_BAR0_PRIMARY_COMMAND   * ( !PCIDeviceATA->BAR0 );
    ATA_BAR1_PRIMARY_CONTROL_PORT   = ( PCIDeviceATA->BAR1 & ~3 ) + ATA_IDE_BAR1_PRIMARY_CONTROL   * ( !PCIDeviceATA->BAR1 );
    ATA_BAR2_SECONDARY_COMMAND_PORT = ( PCIDeviceATA->BAR2 & ~7 ) + ATA_IDE_BAR2_SECONDARY_COMMAND * ( !PCIDeviceATA->BAR2 );
    ATA_BAR3_SECONDARY_CONTROL_PORT = ( PCIDeviceATA->BAR3 & ~3 ) + ATA_IDE_BAR3_SECONDARY_CONTROL * ( !PCIDeviceATA->BAR3 );

    ATA_BAR4 = ( PCIDeviceATA->BAR4 & ~0x7 ) + ATA_IDE_BAR4_BUS_MASTER * ( !PCIDeviceATA->BAR4 );
    ATA_BAR5 = ( PCIDeviceATA->BAR5 & ~0xf ) + ATA_IDE_BAR5            * ( !PCIDeviceATA->BAR5 );

// Get the base i/o port for 
// all the 4 ATA ports.
// Colocando nas estruturas.
    ide_ports[0].base_port = 
        (unsigned short) (ATA_BAR0_PRIMARY_COMMAND_PORT   & 0xFFFF);
    ide_ports[1].base_port = 
        (unsigned short) (ATA_BAR1_PRIMARY_CONTROL_PORT   & 0xFFFF);
    ide_ports[2].base_port = 
        (unsigned short) (ATA_BAR2_SECONDARY_COMMAND_PORT & 0xFFFF);
    ide_ports[3].base_port = 
        (unsigned short) (ATA_BAR3_SECONDARY_CONTROL_PORT & 0xFFFF);
    // Tem ainda a porta do dma na bar4.

//
// De acordo com o tipo.
//

// #??
// Em que momento setamos os valores dessa estrutura.
// Precisamos de uma flag que diga que a estrutura ja esta inicializada.
// Caso contrario nao podemos usar os valores dela.
// >> Isso acontece  logo acima quando chamamos
// a funçao atapciConfigurationSpace()

    //if ( ata_port.used != TRUE || ata_port.magic != 1234 ){
    //    printk("__ata_initialize: ata_port structure validation\n");
    //    goto fail;
    //}

// Que tipo de controlador?
// A rotina atapciConfigurationSpace() configurou
// a estrutura 'ata' com o tipo de controlador encontrado.

// ==============================================
// ATA controller type.

    // Type
    if (AtaController.controller_type == ATA_IDE_CONTROLLER){

        printk ("__ata_initialize: [ATA] Initialize ports\n");
        //while(1){}
        
        // Soft Reset, defina IRQ.
        out8(
            (unsigned short) (ATA_BAR1_PRIMARY_CONTROL_PORT & 0xFFFF),
            0xff );
        out8( 
            (unsigned short) (ATA_BAR3_SECONDARY_CONTROL_PORT & 0xFFFF), 
            0xff );
        out8( 
            (unsigned short) (ATA_BAR1_PRIMARY_CONTROL_PORT & 0xFFFF), 
            0x00 );
        out8( 
            (unsigned short) (ATA_BAR3_SECONDARY_CONTROL_PORT & 0xFFFF), 
            0x00 );

        // ??
        ata_record_dev     = 0xff;
        ata_record_channel = 0xff;

        //printk ("Initializing ATA Mass Storage device ...\n");
        //refresh_screen ();

        // As estruturas de disco serão colocadas em uma lista encadeada.
        
        //#deprecated
        //ide_mass_storage_initialize();

        // ready_queue_dev
        // Vamos trabalhar na lista de dispositivos.
        // Iniciando a lista.
        // ata_device_d structure.

        ready_queue_dev = 
            (struct ata_device_d *) kmalloc( sizeof(struct ata_device_d) );

        if ((void*) ready_queue_dev == NULL){
            printk("__ata_initialize: ready_queue_dev\n");
            Status = (int) -1;
            goto fail;
        }

        // current_sd
        // Initialize the head of the list.
        // Not used.
        // There is a loop to reinitialize the
        // structure of each of all ports.

        current_sd = (struct ata_device_d *) ready_queue_dev;
        memset( current_sd, 0, sizeof(struct ata_device_d) );
        current_sd->dev_id   = __next_sd_id++;
        current_sd->dev_type = -1;
        // Channel and device.
        // ex: primary/master.
        current_sd->dev_channel = -1;
        current_sd->dev_num     = -1;
        current_sd->dev_nport   = -1;
        current_sd->next = NULL;

        // #bugbug
        // Is this a buffer? For what?
        // Is this buffer enough?
        ata_devinfo_buffer = (unsigned short *) kmalloc(4096);
        if ((void *) ata_devinfo_buffer == NULL){
            printk ("ata_initialize: ata_devinfo_buffer\n");
            Status = (int) -1;
            goto fail;
        }
        memset (ata_devinfo_buffer, 0, 4096);

        // Sondando dispositivos e imprimindo na tela.
        // As primeiras quatro portas do controlador ATA. 
        // #todo
        // Create a constant for 'max'.
        // We're gonna create the structure for each 
        // of the devices.

        // This routine is too long.
        // And it is gonna setup the values in the structure
        // given the port.

        // #bugbug
        // The structure is gonna stay with the
        // values for the last time we called this routine.
        // So, it's gonna work only if we're 
        // using a single device.
        // But we're saving the device info into another structure.

        // Inicializa uma estrutura de ata_device e 
        // coloca o ponteiro numa lista encadeada.

        for ( 
            iPortNumber=0; 
            iPortNumber < 4; 
            iPortNumber++ )
        {
            ata_initialize_ide_device(iPortNumber);
        };

        // Ok
        Status = 0;
        goto done;
    }


// ==============================================
// RAID controller.

    if (AtaController.controller_type == ATA_RAID_CONTROLLER)
    {
        // #debug
        printk ("__ata_initialize: [RAID] Unsupported type\n");
        die();
        while (1){
        };

        Status = (int) -1;
        goto fail;
    }

// ==============================================
// AHCI controller type.
// #todo:
// On qemu: Feature '-s -machine q35' uses ahci.
// It emulates ICH9 not I440FX.
// see: https://wiki.qemu.org/Features/Q35

    if (AtaController.controller_type == ATA_AHCI_CONTROLLER){
        printk ("__ata_initialize: [AHCI] Unsupported type\n");
        while(1){}
        Status = (int) -1;
        goto fail;
    }

// ==============================================
// Unknown controller type.

    if (AtaController.controller_type == ATA_UNKNOWN_CONTROLLER){
        printk ("__ata_initialize: [UNKNOWN] Unsupported type\n");
        while(1){}
        Status = (int) -1;
        goto fail;
    }

// ==============================================
// Nem ATA, nem RAID, nem AHCI.
    Status = (int) -1;
    printk("__ata_initialize: ATA, RAID or AHCI were not found\n");
fail:
    printk ("__ata_initialize: fail\n");
    return -1;
    //return (int) Status;

done:
// Setup interrupt breaker.
// Só liberamos se a inicialização fncionou.
    if ( Status == 0 ){
        //debug_print("__ata_initialize: Turn off interrupt breaker\n");
        __breaker_ata1_initialized = TRUE; 
        __breaker_ata2_initialized = TRUE; 
    }

// Driver status.
    g_ata_driver_initialized = TRUE;
    AtaController.initialized = TRUE;

    return (int) Status;
}

//
// $
// INITIALIZATION
//

//----------------------------------------------
//--

// DDINIT_ata:
// Device driver initialization.
// Rotina de diálogo com o driver ATA. 
// Called by init_executive() in system.c
// #importante
// Nessa hora ja temos as estruturas de disk e volume inicializadas.
// entao as estruturas usadas pelo driver ata, pode
// se registrar em disk ou volume.
// msg:
// 1 = Initialize the driver.
// 2 = Register the driver.
// ...

// IN: ???
int 
DDINIT_ata ( 
    int msg, 
    unsigned long long1 )
{
// Called by zeroInitializeSystemComponents in system.c.
// Do some ata routine given the operation number.

    int Status = 1;  // Error.

    PROGRESS("DDINIT_ata:\n");

    switch (msg)
    {
        // ATAMSG_INITIALIZE
        // Initialize driver.
        // ata.c
        case 1:
            //debug_print("init_ata: Initialize ata support\n");
            if (long1 == FORCEPIO){
                debug_print("ATA in PIO mode\n");
            }
            // IN: forcepio?
            Status = (int) __ata_initialize((int) long1);
            // We can't live without this at the moment.
            if (Status<0){
                panic("DDINIT_ata: ata_initialize failed\n");
            }
            return (int) Status;
            break;

        //ATAMSG_REGISTER
        //registra o driver. 
        //case 2:
        //    break;

        default:
            panic("DDINIT_ata: Unsupported service\n");
            break;
    };

    return (int) Status;
}

