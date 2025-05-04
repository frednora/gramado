// ata.c
// ATA/AHCI controller.
// 'ATA is the interface' and 'IDE is the protocol'.
// Environment:
//     32bit bootloader.
// History:
//     2017 - Ported from Sirius OS, BSD-2-Clause License.
//     This device driver was created by Nelson Cole, for Sirius OS.
//     2021 - Some new changes by Fred Nora.

// See:
// https://wiki.osdev.org/ATA_PIO_Mode


// #todo:
// Rever a tipagem.
// Usar os nomes tradicionais para os tipos.
// Obs: 
// O foco est� na lista de discos. 
// diskList

/*
IDE can connect up to 4 drives. Each drive can be one of the following:
  ATA (Serial): Used for most modern hard drives.
  ATA (Parallel): Commonly used for hard drives.
  ATAPI (Serial): Used for most modern optical drives.
  ATAPI (Parallel): Commonly used for optical drives.
*/

#include "../../../bl.h"


// pci support
// #bugbug
// No macros, please
#define CONFIG_ADDR(bus,device,fn,offset)\
                       (\
                       (((uint32_t)(bus)    & 0xff) << 16)|\
                       (((uint32_t)(device) & 0x3f) << 11)|\
                       (((uint32_t)(fn)     & 0x07) <<  8)|\
                       ( (uint32_t)(offset) & 0xfc)|0x80000000 )


#define PCI_PORT_ADDR  0xCF8
#define PCI_PORT_DATA  0xCFC
#define DISK1  1
#define DISK2  2
#define DISK3  3
#define DISK4  4


//
// globals
//

//see: ata.h
int ATAFlag=0;

//see: ata.h
struct dev_nport  dev_nport;
struct ata_pci  ata_pci;
struct ata  ata;

static _u16 *ata_devinfo_buffer;

_u8 ata_record_dev=0;
_u8 ata_record_channel=0;

//
// IDE ports
//

// Channel and device number
int g_current_ide_channel=0;  // Primary/Secondary
int g_current_ide_device=0;   // Master/Slave  
// Port number.
int g_current_ide_port = 0;


// see: ata.h
// #todo: Initialize these structure before using them.
struct ide_ports_d  ide_ports[4];

unsigned long ide_handler_address=0;

//see: ide.h
struct ide_channel_d  idechannelList[8];
struct ide_d  IDE;

// A unidade atualmente selecionada.
st_dev_t *current_dev;

// O in�cio da lista.
st_dev_t *ready_queue_dev;

// O pr�ximo ID de unidade dispon�vel. 
uint32_t  dev_next_pid = 0;

_u8 *dma_addr;

const char *dev_type[] = {
    "ATA",
    "ATAPI"
};

// IRQ support.
//static _u32 ata_irq_invoked = 1; 
static _u32 ata_irq_invoked = 0;

static const char *ata_sub_class_code_register_strings[] = {
    "Unknown",
    "IDE Controller",
    "Unknown",
    "Unknown",
    "RAID Controller",
    "Unknown",
    "AHCI Controller"
};

// base address 
static _u32 ATA_BAR0=0;    // Primary Command Block Base Address
static _u32 ATA_BAR1=0;    // Primary Control Block Base Address
static _u32 ATA_BAR2=0;    // Secondary Command Block Base Address
static _u32 ATA_BAR3=0;    // Secondary Control Block Base Address
static _u32 ATA_BAR4=0;    // Legacy Bus Master Base Address
static _u32 ATA_BAR5=0;    // AHCI Base Address / SATA Index Data Pair Base Address


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
 *     irq 14 handler
 */
void diskATAIRQHandler1 ()
{
    ata_irq_invoked = 1;  
}

/*
 * diskATAIRQHandler2
 *     irq 15 handler
 */ 
void diskATAIRQHandler2 ()
{
    ata_irq_invoked = 1;   
}

/*
 * disk_ata_wait_irq:
 *     Esperando pela interrup��o.
 * OUT:
 *     0    = ok por status da interrup��o. 
 *     -1   = ok por status do controlador.
 *     0x80 = ok por tempo esperado.
 */
int disk_ata_wait_irq()
{
   _u32 tmp = 0x10000;
   _u8 data=0;

// #bugbug
// Em nenhum momento a flag ata_irq_invoked vira TRUE.

    while (!ata_irq_invoked)
    {
        data = ata_status_read();

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
 * show_ide_info:
 *     Mostrar as informa��es obtidas na 
 * inicializa��es do controlador.
 */
void show_ide_info()
{
    register int i=0;

    printf ("show_ide_info:\n");

    // four ports.
    for ( i=0; i<4; i++ ){
        printf ("\n");
        printf ("id        = %d \n", ide_ports[i].id );
        printf ("used      = %d \n", ide_ports[i].used );
        printf ("magic     = %d \n", ide_ports[i].magic );
        printf ("type      = %d \n", ide_ports[i].type );
        printf ("name      = %s \n", ide_ports[i].name );
        printf ("base_port = %x \n", ide_ports[i].base_port );
    };

	/*
	// Estrutura 'ata'
	// Qual lista ??
	
	//pegar a estrutura de uma lista.
	
	//if( ata != NULL )
	//{
		printf("ata:\n");
 	    printf("type={%d}\n", (int) ata.chip_control_type);
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

// low level worker
void __ata_pio_read ( void *buffer, _i32 bytes )
{
    asm volatile (\
        "cld;\
        rep; insw"::"D"(buffer),\
        "d"(ata.cmd_block_base_address + ATA_REG_DATA),\
        "c"(bytes/2) );
}

// low level worker
void __ata_pio_write ( void *buffer, _i32 bytes )
{
    asm volatile (\
        "cld;\
        rep; outsw"::"S"(buffer),\
        "d"(ata.cmd_block_base_address + ATA_REG_DATA),\
        "c"(bytes/2) );
}

// low level worker
// atapi_pio_read:
static inline void __atapi_pio_read ( void *buffer, uint32_t bytes )
{
    asm volatile (\
        "cld;\
        rep; insw"::"D"(buffer),\
        "d"(ata.cmd_block_base_address + ATA_REG_DATA),\
        "c"(bytes/2) );
}

// ====================

int disk_get_ata_irq_invoked()
{
    return (int) ata_irq_invoked;
}

void disk_reset_ata_irq_invoked ()
{
    ata_irq_invoked = 0;
}

void ata_wait (_i32 val)
{
    val /= 100;

    if (val<0){
        val=1;
    }

    while (val--)
    {
        io_delay();
    };
}

// #TODO: 
// Nelson, ao configurar os bits BUSY e DRQ. 
// Devemos verificar retornos de erros.

_u8 ata_wait_not_busy()
{
    while ( ata_status_read() & ATA_SR_BSY ){
        if ( ata_status_read() & ATA_SR_ERR ){
            return 1;
        }
    };

    return 0;
}

_u8 ata_wait_busy()
{
    while ( !(ata_status_read() & ATA_SR_BSY ) )
    {
        if ( ata_status_read() & ATA_SR_ERR ){
            return 1;
        }
    };

    return 0;
}

_u8 ata_wait_no_drq()
{
    while ( ata_status_read() & ATA_SR_DRQ )
    {
        if (ata_status_read() & ATA_SR_ERR){
            return 1;
        }
    }

    return 0;
}

_u8 ata_wait_drq()
{
    while ( !(ata_status_read() & ATA_SR_DRQ) )
    {
        if (ata_status_read() & ATA_SR_ERR){
            return 1;
        }
    };

    return 0;
}

_u8 ata_wait_irq (){

   _u8 Data=0;
   _u32 tmp = 0x10000;

    while (!ata_irq_invoked)
    {
        Data = ata_status_read();
        
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

// Soft reset.
void ata_soft_reset()
{
    _u8 Data=0;

    Data = in8(ata.ctrl_block_base_address + 2);
    out8( 
        ata.ctrl_block_base_address, 
        Data | 0x4 );
    out8( 
        ata.ctrl_block_base_address, 
        Data & 0xfb ); 
}

//#bugbug
//L� o status de um disco determinado, se os valores  
//na estrutura estiverem certos.

_u8 ata_status_read()
{
    _u8 Value=0;

    Value = (_u8) in8( ata.cmd_block_base_address + ATA_REG_STATUS );

    return (_u8) Value;
}

void ata_cmd_write (_i32 cmd_val)
{
    ata_wait_not_busy();
    out8( ata.cmd_block_base_address + ATA_REG_CMD, cmd_val );
    ata_wait(400);  
}

// ----------------------------------------------
// __ata_assert_dever:
//     Check if it is 
//     (primary or secondary) and (master or slave).

_u8 __ata_assert_dever(char nport)
{
    if (nport>3){
        return -1;
    }

    switch (nport){

    case 0:
        ata.channel = 0;  // Primary
        ata.dev_num = 0;  // Not slave
        break;

    case 1:   
        ata.channel = 0;  //  Primary
        ata.dev_num = 1;  // Slave
        break;

    case 2:
        ata.channel = 1;  // Secondary
        ata.dev_num = 0;  // Not slave
        break;

    case 3:
        ata.channel = 1;  // Secondary
        ata.dev_num = 1;  // Slave
        break;

    // Fail.
    default:
        printf ("bl-__ata_assert_dever: [FAIL] Port %d, value not used\n", 
            nport );
        return -1;
        break;
    };

    set_ata_addr (ata.channel);

    return 0;
fail:
    return -1;
}

// Set address.
// Primary or secondary.
void set_ata_addr(int channel)
{
    if (channel<0){
        printf ("set_ata_addr: [FAIL] channel\n");
        return;
    }

// #bugbug
// Porque estamos checando se � prim�rio ou secundario?

    switch (channel){

        case ATA_PRIMARY:
            ata.cmd_block_base_address  = ATA_BAR0;
            ata.ctrl_block_base_address = ATA_BAR1;
            ata.bus_master_base_address = ATA_BAR4;
            break;

        case ATA_SECONDARY:
            ata.cmd_block_base_address  = ATA_BAR2;
            ata.ctrl_block_base_address = ATA_BAR3;
            ata.bus_master_base_address = ATA_BAR4 + 8;
            break;

        //default:
            //PANIC
            //break;
    };
}

// dev_switch:
// Porque esse tipo?
static inline void dev_switch(void)
{

// ??
// Pula, se ainda n�o tiver nenhuma unidade.

    if ( !current_dev )
    {
        return;
    }

// Obter a pr�xima tarefa a ser executada.
// Se ca�mos no final da lista vinculada, 
// comece novamente do in�cio.

    current_dev = current_dev->next;    

    if ( !current_dev )
    {
        current_dev = ready_queue_dev;
    }
}

/*
 * getpid_dev:
 *     ?? Deve ser algum suporte a Processos.
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
 * getnport_dev:
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
 * nport_ajuste:
 */
// #todo
// Change this name.
// OUT:
//  0 = ok 
// -1 = fail
int nport_ajuste(char nport)
{
    _i8 i=0;

// #todo: 
// Simplify.

    while ( nport != getnport_dev() )
    {
        if (i == 4){ 
            goto fail; 
        }
        dev_switch ();
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

// ===============================================
// Obs: 
// O que segue s�o rotinas de suporte a ATA.

// ata_set_device_and_sector:
static inline void ata_set_device_and_sector ( 
    _u32 count, 
    _u64 addr,
    _i32 access_type, 
    _i8 nport )
{
    __ata_assert_dever(nport);

// Access type.

    switch (access_type){

        // Mode LBA28
        case 28:
            out8 ( ata.cmd_block_base_address + ATA_REG_SECCOUNT, count );   // Sector Count 7:0
            out8 ( ata.cmd_block_base_address + ATA_REG_LBA0, addr );        // LBA 7-0   
            out8 ( ata.cmd_block_base_address + ATA_REG_LBA1, addr >> 8 );   // LBA 15-8
            out8 ( ata.cmd_block_base_address + ATA_REG_LBA2, addr >> 16 );  // LBA 23-16

            // Modo LBA active, Select device, and LBA 27-24
            out8 ( ata.cmd_block_base_address + ATA_REG_DEVSEL, 
                0x40 | (ata.dev_num << 4) | (addr >> 24 &0x0f) );
     
            // Verifique se e a mesma unidade para n�o esperar pelos 400ns.

            if ( ata_record_dev != ata.dev_num && 
                 ata_record_channel != ata.channel )
            {
                ata_wait(400);
                
                //verifique erro
                ata_record_dev = ata.dev_num;
                ata_record_channel  = ata.channel;
            }
            break;

        //Mode LBA48
        case 48:

            out8( ata.cmd_block_base_address + ATA_REG_SECCOUNT,0);       // Sector Count 15:8
            out8( ata.cmd_block_base_address + ATA_REG_LBA0,addr >> 24);  // LBA 31-24   
            out8( ata.cmd_block_base_address + ATA_REG_LBA1,addr >> 32);  // LBA 39-32
            out8( ata.cmd_block_base_address + ATA_REG_LBA2,addr >> 40);  // LBA 47-40
            out8( ata.cmd_block_base_address + ATA_REG_SECCOUNT,count);   // Sector Count 7:0
            out8( ata.cmd_block_base_address + ATA_REG_LBA0,addr);        // LBA 7-0   
            out8( ata.cmd_block_base_address + ATA_REG_LBA1,addr >> 8);   // LBA 15-8
            out8( ata.cmd_block_base_address + ATA_REG_LBA2,addr >> 16);  // LBA 23-16

            out8 ( ata.cmd_block_base_address + ATA_REG_DEVSEL,
                0x40 | ata.dev_num << 4 );   // Modo LBA active, Select device,        

            // Verifique se e a mesma unidade para nao esperar pelos 400ns.
            if ( ata_record_dev     != ata.dev_num && 
                 ata_record_channel != ata.channel )
            {
                ata_wait(400);
                ata_record_dev     = ata.dev_num;
                ata_record_channel = ata.channel;
            }
            break;

        // Modo CHS
        // not supported
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


/* 
 * ide_dma_prdt: 
 */
struct {
    uint32_t addr;
    uint32_t len;
}ide_dma_prdt[4];


/* 
 * ide_dma_data: 
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

// @todo: 
// Check limits.

    ide_dma_prdt[nport].addr = (_u32) addr;  //@todo: (&~1)sera que e necessario?
    ide_dma_prdt[nport].len  = byte_count | 0x80000000;

    phy = (uint32_t) &ide_dma_prdt[nport];

    // prds physical.
    out32 ( ata.bus_master_base_address + ide_dma_reg_addr, phy );
 
// (bit 3 read/write)
// 0 = Memory reads.
// 1 = Memory writes.

    data = in8 ( ata.bus_master_base_address + ide_dma_reg_cmd ) &~8;

// TODO bit 8 Confilito no Oracle VirtualBox
// Obs: Isso foi enviado via argumento e agora foi alerado.

    flg = 1; 

    out8 ( 
        ata.bus_master_base_address + ide_dma_reg_cmd, 
        data | flg << 3 );

// Limpar o bit de interrup��o e 
// o bit de erro no registo de status.

    data = in8 ( ata.bus_master_base_address + ide_dma_reg_status );
    
    out8 ( 
        ata.bus_master_base_address + ide_dma_reg_status, 
        data & ~6 );

// #todo: 
//   Deletar retorno.

done:
    return;
}

/*
 * ide_dma_start:
 */
void ide_dma_start ()
{
    _u8 Data = 0; 
    
    Data = in8 ( ata.bus_master_base_address + ide_dma_reg_cmd );
    out8 ( 
        ata.bus_master_base_address + ide_dma_reg_cmd, 
        Data | 1);
}

/*
 * ide_dma_stop:
 */
void ide_dma_stop()
{
    _u8 Data=0;

    Data = in8 ( ata.bus_master_base_address + ide_dma_reg_cmd );  
    out8( 
        ata.bus_master_base_address + ide_dma_reg_cmd, 
        Data & ~1);

    Data = in8 ( ata.bus_master_base_address + ide_dma_reg_status );
    out8( 
        ata.bus_master_base_address + ide_dma_reg_status, 
        Data & ~6);
}

// DMA read status.
int ide_dma_read_status ()
{
    int Status=0;
    Status = in8 ( ata.bus_master_base_address + ide_dma_reg_status );
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
 * __ataReadPCIConfigAddr:
 *     READ
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
 * __ataWritePCIConfigAddr:
 *     WRITE
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
    ata.chip_control_type = CONTROLLER_TYPE_UNKNOWN;


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
         ata_pci.subclass == __SCSI_CONTROLLER ){

        ata.chip_control_type = __SCSI_CONTROLLER; // :)
        printf ("SCSI not supported\n");
        return (int) PCI_MSG_ERROR;

//
//  ## IDE ##
//

    // 1:1 = ATA
    } else if ( ata_pci.class == PCI_CLASS_MASS && 
         ata_pci.subclass == __ATA_CONTROLLER ){

        ata.chip_control_type = __ATA_CONTROLLER; // :)

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
            ata_sub_class_code_register_strings[ata.chip_control_type],
            ata_pci.prog_if,
            ata_pci.revision_id );

//
//  ## RAID ##
//

    // 1:4 = RAID
    } else if ( ata_pci.class == PCI_CLASS_MASS && 
                ata_pci.subclass == __RAID_CONTROLLER ){

        ata.chip_control_type = __RAID_CONTROLLER; // :)
        printf ("RAID not supported\n");
        return (int) PCI_MSG_ERROR;

//
//  ## ATA DMA ## 
//

    // 1:5 = ATA with dma
    } else if ( ata_pci.class == PCI_CLASS_MASS && 
                ata_pci.subclass == __ATA_CONTROLLER_DMA ){

        ata.chip_control_type = __ATA_CONTROLLER_DMA; // :)
        printf ("ATA DMA not supported\n");
        return (int) PCI_MSG_ERROR;

//
//  ## ACHI ##  SATA
//

    // 1:6 = SATA
    } else if ( ata_pci.class == PCI_CLASS_MASS && 
                ata_pci.subclass == __AHCI_CONTROLLER ){

        ata.chip_control_type = __AHCI_CONTROLLER; // :)

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
            ata_sub_class_code_register_strings[ata.chip_control_type], 
            ata_pci.prog_if,
            ata_pci.revision_id );


    // 1:8 = NVME
    } else if ( ata_pci.class == PCI_CLASS_MASS && 
                ata_pci.subclass == __NVME_CONTROLLER ){

        ata.chip_control_type = __NVME_CONTROLLER; // :)
        printf ("NVME not supported\n");
        return (int) PCI_MSG_ERROR;

    // 1:9 = SAS
    } else if ( ata_pci.class == PCI_CLASS_MASS && 
                ata_pci.subclass == __SAS_CONTROLLER ){

        ata.chip_control_type = __SAS_CONTROLLER; // :)
        printf ("SAS not supported\n");
        return (int) PCI_MSG_ERROR;

    // Fail
    // ?:? = Class/subclass not supported.
    } else {

        ata.chip_control_type = CONTROLLER_TYPE_UNKNOWN; // :)
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


// OUT:
// see: ata.h for values.
static int __detect_device_type(uint8_t nport)
{
    _u8 status=0;

// Signature bytes
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

    if ( ata_status_read() == 0xFF )
    {
        printf ("0xFF: Bus with no devices.\n");
        printf ("Floating Bus?\n");
        //refresh_screen();
        goto fail;
    }

    // #todo
    ata_soft_reset();
    ata_wait_not_busy();

    out8 ( ata.cmd_block_base_address + ATA_REG_SECCOUNT, 0 );  // Sector Count 7:0
    out8 ( ata.cmd_block_base_address + ATA_REG_LBA0, 0 );      // LBA 7-0
    out8 ( ata.cmd_block_base_address + ATA_REG_LBA1, 0 );      // LBA 15-8
    out8 ( ata.cmd_block_base_address + ATA_REG_LBA2, 0 );      // LBA 23-16

// Select device.

    // #suspended
    //out8 ( 
        //ata.cmd_block_base_address + ATA_REG_DEVSEL, 
        //0xE0 | ata.dev_num << 4 );
    // #test
    out8 ( 
        ata.cmd_block_base_address + ATA_REG_DEVSEL, 
        0xA0 | ata.dev_num << 4 );
    ata_wait(400);

    //#todo
	ata_wait_not_busy();

    // cmd
    ata_cmd_write (ATA_CMD_IDENTIFY_DEVICE); 
    ata_wait (400);


/*
// #old
// Channel with no device
    if ( ata_status_read() == 0 ){
        printf ("0x00: Channel with no devices.\n");
        goto fail;
    }
*/

	spin = 1000000;
	// Ignora bit de erro
    while (spin--) 
    { 	
		if ( !(ata_status_read() & ATA_SR_BSY) )
            break;
	};

	st = ata_status_read();
	if (!st)
    {
		return (int) ATADEV_UNKNOWN;
	}

// Get signature bytes.
    sigbyte1 = (unsigned char) in8( ata.cmd_block_base_address + ATA_REG_LBA1 );
    sigbyte2 = (unsigned char) in8( ata.cmd_block_base_address + ATA_REG_LBA2 );
    // more 2 bytes ...

//
// # type # 
//

    // Invalid
    if ( sigbyte1 == 0x7F && sigbyte2 == 0x7F )
    {
        goto fail0;
    }
    if ( sigbyte1 == 0xFF && sigbyte2 == 0xFF )
    {
        goto fail0;
    }


// ================================
// PATA
    if ( sigbyte1 == ATADEV_PATA_SIG1 && sigbyte2 == ATADEV_PATA_SIG2 )
    {
        return (int) ATADEV_PATA;
    }

// ================================
// PATAPI
    if ( sigbyte1 == ATADEV_PATAPI_SIG1 && sigbyte2 == ATADEV_PATAPI_SIG2 )
    {
        return (int) ATADEV_PATAPI;
    }

// ================================
// SATA
    if ( sigbyte1 == ATADEV_SATA_SIG1 && sigbyte2 == ATADEV_SATA_SIG2 )
    {
        return (int) ATADEV_SATA;
    }

// ================================
// SATAPI

    if ( sigbyte1 == ATADEV_SATAPI_SIG1 && sigbyte2 == ATADEV_SATAPI_SIG2 )
    {
        return (int) ATADEV_SATAPI;
    }

fail0:
    printf("Invalid signature sig1={%x} sig2={%x}\n",
       sigbyte1, sigbyte2 );
fail:
    return (int) ATADEV_UNKNOWN;
}

/*
 * __ata_identify_device:
 *    Obtendo informa��es sobre um dos dispositivos.
 */
// OUT: 
// 0 = ok 
// -1 = fail
int __ata_identify_device(char port)
{
    int DevType = ATADEV_UNKNOWN;
    st_dev_t *new_dev;

    //#debug
    printf ("\n");
    printf (":: Port %d:\n",port);


// Storage device structure.
    new_dev = (struct st_dev *) malloc( sizeof(struct st_dev) );
    if ((void *) new_dev == NULL){
        printf ("__ata_identify_device: [FAIL] new_dev\n");
        bl_die();
    }

    DevType = (int) __detect_device_type(port);
    if (DevType == ATADEV_UNKNOWN){
        printf("Port %d: Unknown\n",port);
        goto fail;
    }

// ========================
// ATA device.

    if (DevType == ATADEV_PATA){
        printf("Port %d: PATA device\n",port);

        // kputs("Unidade PATA\n");
        // aqui esperamos pelo DRQ
        // e eviamos 256 word de dados PIO

        __ata_pio_read ( ata_devinfo_buffer, 512 );
        ata_wait_not_busy();
        ata_wait_no_drq();

        // Salvando o tipo em estrutura de porta.
        ide_ports[port].id = (uint8_t) port;
        ide_ports[port].name = "PATA";
        ide_ports[port].type = (int) idedevicetypesPATA;

        ide_ports[port].used = (int) TRUE;
        ide_ports[port].magic = (int) 1234;

        new_dev->dev_type = 
            (ata_devinfo_buffer[0] &0x8000)?    0xffff: ATA_DEVICE_TYPE;
        new_dev->dev_access = 
            (ata_devinfo_buffer[83]&0x0400)? ATA_LBA48: ATA_LBA28;

        if (ATAFlag == FORCEPIO){
            //com esse s� funciona em pio
            new_dev->dev_modo_transfere = 0;
        }else{
            //com esse pode funcionar em dma
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
        ata_cmd_write(ATA_CMD_IDENTIFY_PACKET_DEVICE);

        ata_wait(400);
        ata_wait_drq(); 
        __ata_pio_read ( ata_devinfo_buffer, 512 );
        //ata_wait_not_busy();
        //ata_wait_no_drq();

        // Salvando o tipo em estrutura de porta.
        ide_ports[port].id = (uint8_t) port;
        ide_ports[port].name = "PATAPI";
        ide_ports[port].type = (int) idedevicetypesPATAPI;

        ide_ports[port].used = (int) TRUE;
        ide_ports[port].magic = (int) 1234;

        new_dev->dev_type = 
              (ata_devinfo_buffer[0]&0x8000)? ATAPI_DEVICE_TYPE: 0xffff;

        new_dev->dev_access = ATA_LBA28;

        if (ATAFlag == FORCEPIO){
            // com esse s� funciona em pio  
            new_dev->dev_modo_transfere = 0; 
        }else{
            //com esse pode funcionar em dma
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

        ata_wait_drq(); 
        __ata_pio_read ( ata_devinfo_buffer, 512 );
        //ata_wait_not_busy();
        //ata_wait_no_drq();

        //salvando o tipo em estrutura de porta.
        ide_ports[port].id = (uint8_t) port;
        ide_ports[port].name = "SATA";
        ide_ports[port].type = (int) idedevicetypesSATA;

        ide_ports[port].used = (int) TRUE;
        ide_ports[port].magic = (int) 1234;

        printf ("__ata_identify_device: [FAIL] SATA not supported :)\n");
        goto fail;

    }else if (DevType == ATADEV_SATAPI){
        printf("Port %d: SATAPI device\n",port);

        //kputs("Unidade SATAPI\n");   
        ata_cmd_write(ATA_CMD_IDENTIFY_PACKET_DEVICE);
        ata_wait(400);
        ata_wait_drq(); 
        __ata_pio_read(ata_devinfo_buffer,512);
        //ata_wait_not_busy();
        //ata_wait_no_drq();

        //salvando o tipo em estrutura de porta.
        ide_ports[port].id = (uint8_t) port;
        ide_ports[port].name = "SATAPI";
        ide_ports[port].type = (int) idedevicetypesSATAPI;

        ide_ports[port].used = (int) TRUE;
        ide_ports[port].magic = (int) 1234;

        printf ("__ata_identify_device: [FAIL] SATAPI not supported :)\n");
        goto fail;

    }else{
        printf("Port %d: Invalid device type\n",port);
        //printf ("__ata_identify_device: [FAIL] Invalid device type\n");
        goto fail;
    };

// ----------------------------------

// Dados em comum
    new_dev->dev_id      = dev_next_pid++;
    new_dev->dev_num     = ata.dev_num;
    new_dev->dev_channel = ata.channel;  // 
    new_dev->dev_nport   = port;

// port
// No estilo do Nelson?
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

// Queue
// Add no fim da lista (ready_queue_dev).

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

static int __ata_initialize_controller(void)
{
    int port=0;

    // #test
    // Suspending that 'reset', this way the 0xFF test
    // will be the first interaction with the controller.
    // See: __ata_identify_device() bellow.
    //Soft Reset, defina IRQ
    //out8 ( ATA_BAR1, 0xff );
    //out8 ( ATA_BAR3, 0xff );
    //out8 ( ATA_BAR1, 0x00 );
    //out8 ( ATA_BAR3, 0x00 );

    ata_record_dev = 0xff;
    ata_record_channel = 0xff;

    // Vamos trabalhar na lista de dispositivos.

    // Iniciando a lista.
    ready_queue_dev = (struct st_dev *) malloc( sizeof(struct st_dev) );
    if ((void *) ready_queue_dev == NULL){
        printf("__ata_initialize_controller: ready_queue_dev struct fail\n");
        bl_die();
    }

    // #todo:
    // Checar a validade da estrutura.

    current_dev = (struct st_dev *) ready_queue_dev;

    current_dev->dev_id = dev_next_pid++;

    current_dev->dev_type    = -1;
    current_dev->dev_num     = -1;
    current_dev->dev_channel = -1;
    current_dev->dev_nport   = -1;
    current_dev->next = NULL;

    // ??
    ata_devinfo_buffer = (_u16 *) malloc(4096);
    if ((void *) ata_devinfo_buffer == NULL){
        printf("__ata_initialize_controller: ata_devinfo_buffer fail\n");
        bl_die();
    }

// Sondando dispositivos
// As primeiras quatro portas do controlador IDE.
// Inicializa estrutura de dispositivo e coloca na lista
// encadeada de dispositivo.
// #test
// Nesse hora conseguiremos saber mais informaçoes sobre o dispositivo.

// Let's initilize all the four ports for the ATA controller.
    for (port=0; port<ATA_NUMBER_OF_PORTS; port++)
    {
        __ata_identify_device(port);
    };
}

//
// $
// INITIALIZATION
//

/*
 * __ata_probe_controller:
 *     Initialize the IDE controller e show some information about the disk.
 *     #bugbug: It uses some predefined values for port and device.
 *              see: config.h
 * Credits: Nelson Cole.
 */
static int __ata_probe_controller(int ataflag)
{
// Sondando a interface PCI para encontrarmos um dispositivo
// que seja de armazenamento de dados.
// #bugbug: 
// E se tivermos mais que um dispositivo desse tipo.
// #todo
// Talvez essa sondagem pode nos dizer se o dispositivo 
// é primary/secondary e master/slave.

    int Status = 1;  //error
    _u32 data=0;

    _u8 bus=0;
    _u8 dev=0;
    _u8 fun=0;

// Clear
// Just to avoid sosme dirty values.
    bzero( &ide_ports[0], sizeof(struct ide_ports_d) );
    bzero( &ide_ports[1], sizeof(struct ide_ports_d) );
    bzero( &ide_ports[2], sizeof(struct ide_ports_d) );
    bzero( &ide_ports[3], sizeof(struct ide_ports_d) );

// ================================================

// #importante 
// HACK HACK
// usando as defini��es feitas em config.h
// at� que possamos encontrar o canal e o dispositivo certos.
// __IDE_PORT indica qual � o canal.
// __IDE_SLAVE indica se � master ou slave.
// ex: primary/master.
// See: config.h

// #importante:
// Veja no kernel.
// Fizemos de um jeito diferente no driver que est'a no kernel.

    // We have 4 valid port number.
	// We get this value from the configuration file.
    g_current_ide_port = __CONFIG_IDE_PORT;

    /*
    //#debug
    printf ("bl CONFIG:     IDE port: %d\n",g_current_ide_port);   // from config.h
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
    bb[0] = (unsigned long) (g_current_ide_port & 0xff);
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

	
	switch (g_current_ide_port)
	{
        // Primary master
		case 0:
		    g_current_ide_channel = 0;
            g_current_ide_device = 0;
		    break;
        // Primary slave
		case 1:
		    g_current_ide_channel = 0; 
            g_current_ide_device = 1;
		    break;
        // Secondary master
		case 2:
		    g_current_ide_channel = 1;
            g_current_ide_device = 0;
		    break;
        // SEcondary slave
		case 3:
		    g_current_ide_channel = 1; 
            g_current_ide_device = 1;
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

// PCI:  b,d,f
    bus = ( data >> 8 & 0xff );
    dev = ( data >> 3 & 31 );
    fun = ( data      & 7 );

// ------------------------------
// Getting information:
// Let's fill the structure with some information about the device.
// Mass storage device only.
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

    ATA_BAR0 = ( ata_pci.bar0 & ~7 )   + ATA_IDE_BAR0 * ( !ata_pci.bar0 ); 
    ATA_BAR1 = ( ata_pci.bar1 & ~3 )   + ATA_IDE_BAR1 * ( !ata_pci.bar1 );       

    ATA_BAR2 = ( ata_pci.bar2 & ~7 )   + ATA_IDE_BAR2 * ( !ata_pci.bar2 );
    ATA_BAR3 = ( ata_pci.bar3 & ~3 )   + ATA_IDE_BAR3 * ( !ata_pci.bar3 );

    ATA_BAR4 = ( ata_pci.bar4 & ~0x7 ) + ATA_IDE_BAR4 * ( !ata_pci.bar4 );
    ATA_BAR5 = ( ata_pci.bar5 & ~0xf ) + ATA_IDE_BAR5 * ( !ata_pci.bar5 );

// Saving the 'port addresses' into the structure 
// for future use.
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

    // Primary
    ide_ports[0].base_port = (unsigned short) ATA_BAR0;
    ide_ports[1].base_port = (unsigned short) ATA_BAR1;

    // Secondary
    ide_ports[2].base_port = (unsigned short) ATA_BAR2;
    ide_ports[3].base_port = (unsigned short) ATA_BAR3;

// TODO: Tem ainda a porta do dma na bar4 e bar5
    // ATA_BAR4
    // ATA_BAR5

//
// given the controller type
//


    BootDisk.initialized = FALSE;
    BootDisk.controller_type = -1;

//
// Se for IDE.
//

// =========================
// Type ATA
// Sub-class 01h = IDE Controller
    if (ata.chip_control_type == __ATA_CONTROLLER){
        
        BootDisk.controller_type = CONTROLLER_TYPE_ATA;
        __ata_initialize_controller();

// =========================
// Type RAID
    } else if (ata.chip_control_type == __RAID_CONTROLLER){

        BootDisk.controller_type = -1;
        printf ("__ata_probe_controller: RAID not supported\n");
        bl_die();

// =========================
// Type ATA DMA.
    } else if (ata.chip_control_type == __ATA_CONTROLLER_DMA){

        BootDisk.controller_type = -1;
        printf ("__ata_probe_controller: ATA DMA not supported\n");
        bl_die();

// =========================
// Type AHCI.
// Sub-class 06h = SATA Controller
    } else if (ata.chip_control_type == __AHCI_CONTROLLER){

        BootDisk.controller_type = CONTROLLER_TYPE_AHCI;
        printf ("__ata_probe_controller: AHCI not supported\n");
        bl_die();

// =========================
// Controller type not supported
// Not IDE and Not AHCI
    }else{
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
 * __ata_initialization_dialog:
 *     Rotina de di�logo com o driver ATA.
 */
int 
__ata_initialization_dialog ( 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{
    int Status = 1;    //Error.

    switch (msg){

    //ATAMSG_INITIALIZE
    //Initialize driver.
    case 1:
        __ata_probe_controller((int) long1);
        Status = 0;
        goto done;
        break;

    //ATAMSG_REGISTER
    //registra o driver. 
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
 * ata_initialize:
 *     Inicializa o driver de hd.
 */
// Called by OS_Loader_Main in main.c.
int ata_initialize(void)
{

// #todo: 
// We need to do something here. haha

// See: ide.c
    __ata_initialization_dialog( 1, FORCEPIO, FORCEPIO );
    g_driver_hdd_initialized = (int) TRUE;

    return 0;
}


//
// End
//

