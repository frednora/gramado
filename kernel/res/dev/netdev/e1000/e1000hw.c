// e1000hw.c
// e1000 Intel nic driver.
// This part of the driver interacts directly with the hardware.
// Created by Fred Nora.

// Credits: 
// Chicago OS, Italo Matias.
// ...


/*
Key steps involved:

PCI Configuration Access:
Locate the NIC device using PCI configuration space.
Read the Base Address Register (BAR) 0 to obtain the physical base address 
of the device's memory-mapped region.

Memory Mapping:
Allocate a virtual memory region of appropriate size.
Map the physical address range obtained from BAR 0 to the allocated 
virtual memory region using the operating system's memory management functions.

Accessing Device Registers:
Use the mapped virtual addresses to access the device's registers and memory.
*/

#include <kernel.h>

static int e1000_initialized = FALSE;

// see: nicintel.h
struct intel_nic_info_d  *currentNIC;

int e1000_irq_count=0;
unsigned long gE1000InputTime=0;

// Counters.
static unsigned long e1000_tx_counter=0;
static unsigned long e1000_rx_counter=0;

// =======================================

static void __e1000_on_transmit(void);
static void __e1000_on_receive(void);
// NIC device handler.
static void DeviceInterface_e1000(void);

static uint32_t 
__E1000ReadCommand ( 
    struct intel_nic_info_d *d, 
    uint16_t addr );

static void 
__E1000WriteCommand ( 
    struct intel_nic_info_d *d, 
    uint16_t addr, 
    uint32_t val );

static uint32_t 
__E1000ReadEEPROM ( 
    struct intel_nic_info_d *d, 
    uint8_t addr );

static unsigned long 
__E1000AllocCont ( 
    uint32_t amount, 
    unsigned long *virt );

static int 
__mapping_nic1_device_address(
    unsigned long pa,
    unsigned long va);

static void __e1000_enable_interrupt(struct intel_nic_info_d *nic_info);
static void __e1000_setup_irq (int irq_line);
static void __initialize_tx_support(struct intel_nic_info_d *d);
static void __initialize_rx_support(struct intel_nic_info_d *d);
static int __e1000_reset_controller(struct intel_nic_info_d *d);
static void __e1000_linkup(struct intel_nic_info_d *d);

// =====================

void e1000_show_info(void)
{
    printk("e1000 NIC info:\n");
    if ((void*) currentNIC == NULL){
        printk ("currentNIC fail\n");
    }
    if (currentNIC->magic != 1234)
        printk ("currentNIC validation\n");

// Counters
    printk("Counters: TX={%d} RX={%d}\n",
        e1000_tx_counter, e1000_rx_counter );

}

// Read from memory mapped register.
static uint32_t 
__E1000ReadCommand ( 
    struct intel_nic_info_d *d, 
    uint16_t addr )
{
    //if ( (void*) d == NULL ) { return 0; }
    unsigned long address = (d->registers_base_address + addr);
    return *( (volatile unsigned int *) address );
}

// Write to memory mapped register.
static void 
__E1000WriteCommand ( 
    struct intel_nic_info_d *d, 
    uint16_t addr, 
    uint32_t val )
{
    //if ( (void*) d == NULL ) { return 0; }
    unsigned long address = (d->registers_base_address + addr);
    *( (volatile unsigned int *) address ) = val;
}

static uint32_t 
__E1000ReadEEPROM ( 
    struct intel_nic_info_d *d, 
    uint8_t addr )
{
    uint32_t data=0;

// ??
// 00010h EECD EEPROM/Flash Control/Data Register

// ??
// 14H

// #todo
// Check the pointer validation.
    //if ( (void*) d == NULL )
        //return 0;

	// We have the EEPROM?
 
	//#debug
	//printk("E1000ReadEEPROM:\n");

    // Yes :)
    if (d->has_eeprom == TRUE){
        __E1000WriteCommand( d, 0x14, 1 | (addr << 8) );

        // #bugbug
        // #hang
        while (( (data = (uint32_t) __E1000ReadCommand ( d, 0x14)) & 0x10 ) != 0x10 )
        {
        };

    // Nope ...
    } else if (d->has_eeprom == FALSE){
        __E1000WriteCommand ( d, 0x14, 1 | (addr << 2) );

        //#bugbug
        //#obs: loop
        while (( (data = (uint32_t) __E1000ReadCommand(d, 0x14)) & 0x01 ) != 0x01 )
        {
        };
    };

    return (uint32_t) ((data >> 16) & 0xFFFF); 
}

/*
 * E1000AllocCont: ??
 *     Retorna o endereço físico e 
 * coloca o virtual em *virt
 * ah ... então eu vou alocar usando endereços virtuais
 * ... e traduzir para físico 
 * ... colocar o virtual em *virt e retornar o físico.
 */
// Precisamos de um endereço fisico.
// + alocamos um endereço virtual
// + convertemos para fisico
// IN:  size, return virtual address.
// OUT: physical address

static unsigned long 
__E1000AllocCont ( 
    uint32_t amount, 
    unsigned long *virt )  //#todo: 64bit address
{
    unsigned long va=0;
    unsigned long pa=0;

// #bugbug
// What is the limit for this value?

    if (amount == 0){
        panic("__E1000AllocCont: [FAIL] amount\n");
    }
// ============
// va
    va = (unsigned long) kmalloc((size_t) amount);
    *virt = va;
    if (*virt == 0){
        panic("__E1000AllocCont: [FAIL] va allocation\n");
    }
    memset ( va, 0, amount );
    //#debug
    //printk("va=%x\n",va);

// ============
// pa
// ps: Using the kernel page directory.
// see: 

    pa = 
        (unsigned long) virtual_to_physical (
                            va, 
                            gKernelPML4Address ); 
    if (pa == 0){
        panic ("__E1000AllocCont: [FAIL] pa\n");
    }
    //#debug
    //printk("pa=%x\n",pa);

    return (unsigned long) pa;
}

// Enable interrupt
// 0xD0 Message Control (0x0080) Next Pointer (0xE0) Capability ID (0x05)
// 0000 0001  1111 0111  0000   0010  1101   0111
// 0x1F6DC, 1f72d7
static void __e1000_enable_interrupt(struct intel_nic_info_d *nic_info)
{
    uint32_t value=0;
    if ((void*) nic_info == NULL){
        panic("__e1000_enable_interrupt: nic_info\n");
    }
// Enable interrupts
// Set the IMS, Interrupt Mask Set/Read.
    __E1000WriteCommand (nic_info, 0xD0, 0x1F6DC);
    //__E1000WriteCommand(nic_info, 0xD0, 0xFB);
    __E1000WriteCommand(nic_info, 0xD0, 0xFF & ~4);
// Drag the ICR, Interrupt Cause Read.
    value = (uint32_t) __E1000ReadCommand(nic_info, 0xC0);
}

static void __e1000_linkup(struct intel_nic_info_d *d)
{
    uint32_t val=0;

    if ((void*)d == NULL){
        panic("__e1000_linkup: d\n");
    }

    printk("__e1000_linkup:\n");

// CTRL - Device Control Register
    val = (uint32_t) __E1000ReadCommand(d,0);
    __E1000WriteCommand (d, 0, (val | 0x40));

// #debug
    printk("__e1000_linkup: Done\n");
}

static void __initialize_tx_support(struct intel_nic_info_d *d)
{
// 00400h - Transmit Control Register

    register int i=0;

    if ((void*) d == NULL){
        panic("__initialize_tx_support: d\n");
    }

    //printk("[1]:\n");

// And alloc the phys/virt address of the transmit buffer
// tx_descs_phys conterá o endereço físico e
// legacy_tx_descs conterá o endereço virtual.
// IN:  size, return virtual address.
// OUT: physical address

    //unsigned long tx_address = 
    //    (unsigned long) &d->legacy_tx_descs; //#wrong
    //unsigned long tx_address=0; //#bugbug not used  //va
    //printk("tx_address=%x\n",tx_address);
 
// Vamos alocar memória e pagarmos seu endereço físico.
// Retorna um endereço virtual em tx_address e 
// o físico no retorno da função.

    uint32_t size_all_desc = 
        (uint32_t) ((sizeof(struct legacy_tx_desc) * 8) + 16 );

    d->tx_descs_phys = 
        (unsigned long) __E1000AllocCont ( 
               size_all_desc, 
               (unsigned long *)(&d->legacy_tx_descs) );

    if (d->tx_descs_phys == 0){
        panic ("__e1000_reset_controller: [FAIL] d->tx_descs_phys\n");
    }

    //printk("[2]:\n");

// tx
// tmp physical address.
    unsigned long tmp_txaddress_pa=0;

    for ( i=0; i<8; i++ ) 
    {
        // Alloc the phys/virt address of this transmit desc
        // alocamos memória para o buffer, 
        // salvamos o endereço físico do buffer e 
        // obtemos o endereço virtual do buffer.
        // IN:  size, return virtual address.
        // OUT: physical address
        // Vamos alocar memória e pegar seu endereço físico.
        // 8 buffers para envio.
        // O endereço virtual fica salvo no array de ponteiros.
        tmp_txaddress_pa = 
            (unsigned long) __E1000AllocCont ( 
                 E1000_DEFAULT_BUFFER_SIZE, 
                 (unsigned long *) &d->tx_buffers_virt[i] );
        
        if (tmp_txaddress_pa == 0){
            panic("__e1000_reset_controller: tmp_txaddress_pa\n");
        }
        
        // Buffer (PA)
        // A estrutura contém um ponteiro físico para o bufffer.
        // Em duas partes de 32bit.
        // Mas d->tx_buffers_virt[i] contém o ponteiro virtual de 64bit.
        d->legacy_tx_descs[i].addr  = (unsigned int) tmp_txaddress_pa;
        d->legacy_tx_descs[i].addr2 = (unsigned int) (tmp_txaddress_pa>>32);
        if (d->legacy_tx_descs[i].addr == 0){
            panic ("__e1000_reset_controller: [FAIL] d->legacy_tx_descs[i].addr\n");
        }

        // #test: 
        // Configurando o tamanho do buffer
        d->legacy_tx_descs[i].length = (uint16_t) E1000_DEFAULT_BUFFER_SIZE;

        //cmd: bits
        //IDE VLE DEXT RSV RS IC IFCS EOP
        //IDE  (bit 7) - Interrupt Delay Enable
        //VLE  (bit 6) - VLAN Packet Enable
        //DEXT (bit 5) - Descriptor extension (#importante: '0' for legacy mode)
        //RSV  (bit 4) - Reserved
        //RS   (bit 3) - Report status
        //IC   (bit 2) - Insert checksum
        //IFCS (bit 1) - Insert FCS (CRC)
        //EOP  (bit 0) - End of packet

        d->legacy_tx_descs[i].cmd = 0;
        // ?
        // #define TSTA_DD  (1 << 0)    // Descriptor Done
        d->legacy_tx_descs[i].status = 1;
    };

/*
// #debug
    for ( i=0; i<8; i++ ){
        printk ("PA_LOW={%x} PA_HIGH={%x} VA={%x} \n",
            d->legacy_tx_descs[i].addr,
            d->legacy_tx_descs[i].addr2, 
            d->tx_buffers_virt[i] );
    };
    refresh_screen();
    while(1){}
*/

    d->tx_cur = 0;

//========================================
// ## TX ##
//transmit
//Setup the (transmit) ring registers.
// Pass the physical address (and some other informations) 
// of the transmit buffer
//TDBAL	= 0x3800,	/* Tx Descriptor Base Address Low */
//TDBAH	= 0x3804,	/* Tx Descriptor Base Address High */

// low
    __E1000WriteCommand (
        d, 
        0x3800, 
        (unsigned int) d->tx_descs_phys );  //low (endereço do ring)
// high
    __E1000WriteCommand (
        d, 
        0x3804, 
        (unsigned int) (d->tx_descs_phys >> 32) );   //high


/*
// #debug
    printk ("PA_LOW={%x} PA_HIGH={%x} VA={%x} \n",
        d->tx_descs_phys,
        (d->tx_descs_phys >> 32), 
        d->legacy_tx_descs );
    refresh_screen();
    while(1){}
*/

// =================

// Descriptor len.
    __E1000WriteCommand (d, 0x3808, 128);  //8*16

// =================

// Head and tail para tx.
    __E1000WriteCommand (d, 0x3810, 0);    //head
    __E1000WriteCommand (d, 0x3818, 7);    //tail


	//#define E1000_TCTL     0x00400  /* TX Control - RW */
    //• CT = 0x0F (16d collision)
    //• COLD: HDX = 511 (0x1FF); FDX = 63 (0x03F)
    //• PSP = 1b
    //• EN=1b
    //• All other fields 0b	
     /* Transmit Control */
    //#define E1000_TCTL_RST    0x00000001    /* software reset */
    //#define E1000_TCTL_EN     0x00000002    /* enable tx */
    //#define E1000_TCTL_BCE    0x00000004    /* busy check enable */
    //#define E1000_TCTL_PSP    0x00000008    /* pad short packets */
    //#define E1000_TCTL_CT     0x00000ff0    /* collision threshold */
    //#define E1000_TCTL_COLD   0x003ff000    /* collision distance */
    //#define E1000_TCTL_SWXOFF 0x00400000    /* SW Xoff transmission */
    //#define E1000_TCTL_PBE    0x00800000    /* Packet Burst Enable */
    //#define E1000_TCTL_RTLC   0x01000000    /* Re-transmit on late collision */
    //#define E1000_TCTL_NRTU   0x02000000    /* No Re-transmit on underrun */
    //#define E1000_TCTL_MULR   0x10000000    /* Multiple request support */

	//habilita esses dois campos e o resto é zero.
    //• GRAN = 1b (descriptors)
    //• WTHRESH = 1b
    //• All other fields 0b.	
	//#define E1000_TXDCTL_WTHRESH 0x003F0000 /* TXDCTL Writeback Threshold */
	//#define E1000_TXDCTL_GRAN    0x01000000 /* TXDCTL Granularity */
	//#define E1000_TXDCTL   0x03828  /* TX Descriptor Control - RW */

    __E1000WriteCommand (
        d, 
        0x3828, 
        (0x01000000 | 0x003F0000) );


/*
// 0x400
    __E1000WriteCommand ( 
        d, 
        0x400, 
        ( 0x00000ff0 | 0x003ff000 | 0x00000008 | 0x00000002) );
	//E1000WriteCommand(d, 0x400, 0x10400FA);  //TCTL	= 0x0400,	//Transmit Control 
	//E1000WriteCommand(d, 0x400, 0x3003F0FA);
	//E1000WriteCommand(d, 0x400, (1 << 1) | (1 << 3) );
*/


/*
#define TCTL_EN          (1 << 1)    // Transmit Enable
#define TCTL_PSP         (1 << 3)    // Pad Short Packets
#define TCTL_CT_SHIFT    4           // Collision Threshold
#define TCTL_COLD_SHIFT  12          // Collision Distance
#define TCTL_SWXOFF      (1 << 22)   // Software XOFF Transmission
#define TCTL_RTLC        (1 << 24)   // Re-transmit on Late Collision
*/

/*
    __E1000WriteCommand(
        d, 
        0x400,  
        TCTL_EN
        | TCTL_PSP
        | (15 << TCTL_CT_SHIFT)
        | (64 << TCTL_COLD_SHIFT)
        | TCTL_RTLC);
*/

// Transmit Control Register
// bits  1010b
    //uint32_t val = (uint32_t) __E1000ReadCommand (d, 0x400);
    //__E1000WriteCommand (d, 0x400, val | TCTL_EN | TCTL_PSP );
    //__E1000WriteCommand (d, 0x400, val | 0x2 );

    __E1000WriteCommand (
        d, 
        0x400, 
        ( 0x00000ff0 | 0x003ff000 | 0x00000008 | 0x00000002) );

//=====================

// 0x410
// IPGT = 8
// IPGR1 = 2
// IPGR2 = 10
    //#define E1000_TIPG     0x00410  /* TX Inter-packet gap -RW */	

/*
    __E1000WriteCommand ( 
        d,
        0x410,
        (  0x0000000A | 0x00000008 | 0x00000002) ); 
*/


    __E1000WriteCommand (
        d, 
        0x410, 
        0x0060200A );  


// Talvez ja fizemos isso. 
// Initialize the transmit descriptor registers (TDBAL, TDBAH, TDL, TDH, and TDT).

    //eth_write(base_addr, REG_ADDR_MAC_CONF,
	//	  /* Set the RMII speed to 100Mbps */
	//	  MAC_CONF_14_RMII_100M |
	//	  /* Enable full-duplex mode */
	//	  MAC_CONF_11_DUPLEX |
	//	  /* Enable transmitter */
	//	  MAC_CONF_3_TX_EN |
	//	  /* Enable receiver */
	//	  MAC_CONF_2_RX_EN);	
    
	
	//iow32(d, TCTL, TCTL_EN);
	
	//printk("nic_i8254x_reset: Done\n");
	//refresh_screen();	
	
	//endereço físico  dos rings;
	//printk("tx_ring_pa=%x rx_ring_pa=%x \n", 
	//    d->rx_descs_phys, 
	//	d->tx_descs_phys );

}

static void __initialize_rx_support(struct intel_nic_info_d *d)
{
// 00100h - Receive Control Register

    register int i=0;

    if ((void*) d == NULL){
        panic("__initialize_rx_support: d\n");
    }

// And alloc the phys/virt address of the receive buffer.

    uint32_t size_all_desc = 
        (uint32_t) ((sizeof(struct legacy_rx_desc) * 32) + 16 );

    d->rx_descs_phys = 
        __E1000AllocCont (
            size_all_desc, 
            (unsigned long *)(&d->legacy_rx_descs));

    if (d->rx_descs_phys == 0){
        panic ("__e1000_reset_controller: [FAIL] d->rx_descs_phys\n");
    }

// rx
// tmp physical address.
    unsigned long tmp_rxaddress_pa=0;

    for ( i=0; i<32; i++ ) 
    {
        // Alloc the phys/virt address of this transmit desc
        // IN:  size, return virtual address.
        // OUT: physical address
        // vamos alocar memória e retornar o endereço físico.
        // 32 buffers para recebimento.
        // O endereço virtual fica salvo no array de ponteiros.
        tmp_rxaddress_pa = 
            (unsigned long) __E1000AllocCont ( 
                E1000_DEFAULT_BUFFER_SIZE, 
                (unsigned long *) &d->rx_buffers_virt[i] );

        if (tmp_rxaddress_pa == 0){
            panic("__e1000_reset_controller: tmp_rxaddress_pa\n");
        }

        // Buffer (PA)
        // A estrutura contém um ponteiro físico para o bufffer.
        // Em duas partes de 32bit.
        // Mas d->rx_buffers_virt[i] contém o ponteiro virtual de 64bit.
        d->legacy_rx_descs[i].addr  = (unsigned int) tmp_rxaddress_pa;
        d->legacy_rx_descs[i].addr2 = (unsigned int) (tmp_rxaddress_pa>>32);
        if (d->legacy_rx_descs[i].addr == 0){
            panic ("__e1000_reset_controller: [FAIL] d->legacy_rx_descs[i].addr\n");
        }

        // #test: 
        // Configurando o tamanho do buffer
        d->legacy_rx_descs[i].length = (uint16_t) E1000_DEFAULT_BUFFER_SIZE;
        d->legacy_rx_descs[i].status = 0;
    };

/*
// #debug 
// Vamos imprimir os endereços edereços físicos dos buffers 
// e os edereços virtuais dos descritores.
    for ( i=0; i < 32; i++ ){
        printk ("PA_LOW={%x} PA_HIGH={%x} VA={%x} \n",
            d->legacy_rx_descs[i].addr,
            d->legacy_rx_descs[i].addr2, 
            d->rx_buffers_virt[i] );
    };
    refresh_screen();
    while(1){}
*/

    d->rx_cur = 0;

// ===================================
// ## RX ##
// receive
// Setup the (receive) ring registers.
// Pass the physical address (and some other informations) of the receive buffer

// Address: low and high.
    __E1000WriteCommand (
        d, 
        0x2800, 
        (unsigned int) d->rx_descs_phys );  // low
    __E1000WriteCommand (
        d, 
        0x2804, 
        (unsigned int) (d->rx_descs_phys >> 32) );  // high 

/*
// #debug
    printk ("PA_LOW={%x} PA_HIGH={%x} VA={%x} \n",
        d->rx_descs_phys,
        (d->rx_descs_phys >> 32), 
        d->legacy_rx_descs );
    refresh_screen();
    while(1){}
*/

// Buffer
    __E1000WriteCommand (d, 0x2808, 512);  // 32*16

// head and tail para rx.
    __E1000WriteCommand (d, 0x2810, 0);   // head
    __E1000WriteCommand (d, 0x2818, 31);  // tail

// receive control
// RCTL = 0x0100, /* Receive Control */
    __E1000WriteCommand ( d, 0x100, 0x602801E );

// RX Delay Timer Register
    //__E1000WriteCommand (d, 0x2820, 0);
}

// Reset the controller.
static int __e1000_reset_controller(struct intel_nic_info_d *d)
{
    register int i=0;
    //uint32_t value=0;

    // #debug
    //debug_print ("__e1000_reset_controller:\n");
    //printk      ("__e1000_reset_controller:\n");

// structure
    if ((void*) d == NULL){
        panic("__e1000_reset_controller: d\n");
    }
// base address
// 0x0 is an invalid base address.
    if (d->registers_base_address == 0){
        panic ("__e1000_reset_controller: d->registers_base_address\n");
    }
// Clear the Multicast Table Array (MTA).
    for (i=0; i<128; i++){
        __E1000WriteCommand ( d, 0x5200 + (i * 4), 0 );
    };

    __initialize_tx_support(d);
    __initialize_rx_support(d);
    __e1000_linkup(d);
    __e1000_enable_interrupt(d);

    return 0;
}

//=======================================================
// __e1000_setup_irq:
//     Setup nic irq 
// #importante
// Isso é usado por uma hotina em headlib.s para 
// configurar uma nova entrada na idt. 
// Essa função é chamada pelo driver de PCI quando encontrar
// o dispositivo Intel apropriado.
// #bugbug
// Called by pciHandleDevice.

// O assembly tem que pegar aqui.
uint8_t nic_idt_entry_new_number=0;
//na verdade o assembly esta usando outro endereço
//uint32_t nic_idt_entry_new_address;
unsigned long nic_idt_entry_new_address=0; //global

static void __e1000_setup_irq(int irq_line)
{
    //debug_print ("__e1000_setup_irq: [FIXME]\n");

// irq line.
    uint8_t irq = (uint8_t) (irq_line & 0xFF);
// handler address.
// #bugbug: 
// Na verdade o assembly esta usando outro endereço.
// #todo: 64bit.
    unsigned long handler = (unsigned long) &irq_E1000; 
// #importante
// Transformando irq em número de interrupção.
// 9+32=41.
    uint8_t idt_num = (uint8_t) (irq + 32);

//#debug OK (irq=9) 
    printk("__e1000_setup_irq: irq={%d}\n", irq);
    printk("__e1000_setup_irq: idt_num={%d}\n", idt_num);
    printk("__e1000_setup_irq: handler={%x}\n", handler);
	//printk ("PCIRegisterIRQHandler: pin={%d}\n",currentNIC->pci->irq_pin);//shared INTA#
	//refresh_screen();
	//while(1){}
	//#debug interrupção=41 

	//refresh_screen();
	//while(1){}

//
// Creating IDT entry.
//

// Chamando asm:
// número e endereço.
// #obs: Essas variáveis são declaradas nesse arquivo
// o assembly terá que pegar.
// Essa é a rotina em assembly que cria uma entrada na idt para 
// o nic, com base nas variáveis que são importadas pelo assembly.
// headlib.asm
// deveria ir para hwlib.asm

    nic_idt_entry_new_number = 
        (unsigned long) (idt_num & 0xFF);

//not used for now.
//na verdade o assembly esta usando outro endereço
    nic_idt_entry_new_address = (unsigned long) handler; 

    printk("__e1000_setup_irq: interrupt={%d}\n", 
        nic_idt_entry_new_number );
    printk("__e1000_setup_irq: handler={%x}\n", 
        nic_idt_entry_new_address );

// Call assembly.
// #todo: Move this export to the top of this document.
    extern void asm_nic_create_new_idt_entry(void);
    asm_nic_create_new_idt_entry();
}

static int 
__mapping_nic1_device_address(
    unsigned long pa,
    unsigned long va)
{
// OUT: 0=ok | -1=fail
// see: pages.c
    return (int) mm_map_2mb_region(pa,va);
}

int 
e1000hw_send(
    struct intel_nic_info_d *dev, 
    size_t len, 
    const char *data )
{
    uint16_t old=0;
// dev
    struct intel_nic_info_d *d;

// Device structure
    d = (struct intel_nic_info_d *) dev;
    if ((void*) d == NULL){
        printk("e1000_send: d\n");
        goto fail;
    }
    if (d->magic != 1234){
        printk("e1000_send: d validation\n");
        goto fail;
    }

// len
    if (len <= 0)
        panic("e1000_send: len=0\n");
    // #test
    // 8192 
    if (len > E1000_DEFAULT_BUFFER_SIZE)
        panic("e1000_send: len\n");

    if ((void*) data == NULL){
        panic("e1000:_send: data\n");
    }

// current descriptor
    old = d->tx_cur;
    if (old >= SEND_BUFFER_MAX){
        panic("e1000_send: old\n");
    }

// Usando o ponteiro virtual de 64bit.
// Buffer, data, len
    memcpy(
        (void *) d->tx_buffers_virt[old],  // device buffer 
        (const void *) data,               // application buffer (read only)
        (size_t) len );

// lenght
// #todo:
// What is the correct size when sending?
    d->legacy_tx_descs[old].length = (uint16_t) len;
// cmd
// CMD_EOP | CMD_IFCS | CMD_RS;
    d->legacy_tx_descs[old].cmd = (uint8_t) 0x1B;
// status
    d->legacy_tx_descs[old].status = (uint8_t) 0;

// Circula
// Configura qual vai ser o proximo
    d->tx_cur = (uint16_t) ((d->tx_cur + 1) % 8);
    __E1000WriteCommand( d, 0x3818, d->tx_cur );

// Espera no antigo
// #hang
    while ( !(d->legacy_tx_descs[old].status & 0xFF) )
    {
        // Nothing
    };

// done
    return 0;
fail:
    refresh_screen();
    return (int) -1;
}

// We receive an interrupt telling us that
// the transmission is completed. So, here we simply
// update the counters.
static void __e1000_on_transmit(void)
{
    // printk ("__e1000_on_transmit: Transmit completed\n");
    e1000_tx_counter++;
    networkUpdateCounter(1);
}

// Worker:
// Called by DeviceInterface_e1000().
// Frame size?
// + The standard Ethernet (IEEE 802.3) frame size is 1,518 bytes.
// + Ethernet header (14 bytes).
// + The payload (IP packet, usually 1,500 bytes).
// + Frame Check Sequence (FCS) field (4 bytes).
static void __e1000_on_receive(void)
{
// Frame
    unsigned char *frame;
    uint16_t frame_lenght = 0x0000;
// Descriptor index.
    uint16_t old = 0;

// The current NIC.
// (Intel structure?)
// #bugbug:
// We still don't have a pointer for the current device.
// and this structure represents only an Intel device.

    if ((void*) currentNIC == NULL){
        return;
    }
    if (currentNIC->magic != 1234){
        return;
    }

// #todo
// Explain it better.

    //while ( ( currentNIC->legacy_rx_descs[ currentNIC->rx_cur ].status & 0x01) == 0x01 ) 
    while (1)
    {
        // #test
        if (( currentNIC->legacy_rx_descs[ currentNIC->rx_cur ].status & 0x01) != 0x01)
            break;

        // Pega o atual e circula.
        old = currentNIC->rx_cur;
        currentNIC->rx_cur = (currentNIC->rx_cur + 1) % RECEIVE_BUFFER_MAX; 

        if (old >= RECEIVE_BUFFER_MAX){
            panic("__e1000_on_receive: [receive] old\n");
        }

        //#debug
        //printk ("BUFFER ID: gotten from %d\n", old);

        // Get the frame base address.
        // frame addres and frame lenght.
        frame = (unsigned char *) currentNIC->rx_buffers_virt[old];
        frame_lenght = (uint16_t) currentNIC->legacy_rx_descs[old].length;

        // Validation
        if ((void*) frame == NULL){
            panic ("__e1000_on_receive: frame\n");
        }
        if (frame_lenght > E1000_DEFAULT_BUFFER_SIZE){
            panic ("__e1000_on_receive: frame_lenght\n");
        }

        // Somente apagar o primeiro bit
        currentNIC->legacy_rx_descs[old].status &= ~1;

        // ?? Provavelmente seleciona o buffer antes de circular.
        // #bugbug: devemos circular primeiro pra depois chamar essa rotina?
        // RDT - Receive Descriptor Tail
        // REG_RXDESCTAIL
        __E1000WriteCommand ( currentNIC, 0x2818, old );

        //#test buffer
        //eh = (void*) buffer;
        // IN:
        // + frame address
        // + frame lenght
        // see: network.c
        if ((void*) frame != NULL)
        {
            network_on_receiving ( 
                (const unsigned char*) frame, 
                (ssize_t) (frame_lenght & 0xFFFF) );

            e1000_rx_counter++;
            networkUpdateCounter(2);
        }
    };
}

static void DeviceInterface_e1000(void)
{
// Called by the irq_E1000().

    uint32_t InterruptCause=0;

// The current NIC.
// (Intel structure?)
// #bugbug:
// We still don't have a pointer for the current device.
// and this structure represents only an Intel device.

    if ((void*) currentNIC == NULL)
        panic("DeviceInterface_e1000: currentNIC\n");
    if (currentNIC->magic != 1234)
        panic("DeviceInterface_e1000: currentNIC validation\n");

// Status
// 0xC0 - Interrupt Cause Read Register
// ICR Register Bit Description
// 0 - TXDW - Transmit Descriptor Written Back
// 1 - TXQE - Transmit Queue Empty
// 2 - LSC  - Link Status Change
// 3 - RXSEQ - Receive Sequence Error
// 4 - RXDMT0 - Receive Descriptor Minimum Threshold Reached
// 6 - RXO - Receiver Overrun
// 7 - RXT0 - Receiver Timer Interrupt
// 9 - MDAC - MDI/O Access Complete
// 10 - RXCFG - Receiving /C/ ordered sets
// 12 - PHYINT - PHY Interrupt
// 13 - GPI_SDP6 - General Purpose Interrupt on SDP6[2].
// 14 - GPI_SDP7 - General Purpose Interrupt on SDP7[3].
// 15 - TXD_LOW - Transmit Descriptor Low Threshold hit
// 16 - SRPD - Small Receive Packet Detected

// 0xD0 - IMS - Interrupt Mask Set/Read register.
// Without this, the card may spam interrupts.
// bit 0 - TXDW - Sets mask for Transmit Descriptor Written Back.
    __E1000WriteCommand( currentNIC, 0xD0, 1 );

// Status
// ICR - Interrupt Cause Read register.
    InterruptCause = (uint32_t) __E1000ReadCommand( currentNIC, 0xC0 );
    //__E1000WriteCommand( currentNIC, 0xC0, InterruptCause );
// Clear all the bits.
    __E1000WriteCommand( currentNIC, 0xC0, 0xffffffff );

//
// Check the interrupt cause in ICR.
//

    if (InterruptCause == 0){
        goto fail;
    }

    // 0x01 - Transmit completed.
    // INTERRUPT_TXDW
    if (InterruptCause & 0x01){
        __e1000_on_transmit();
        goto done;

    // 0x02
    // INTERRUPT_TXQE
    } else if (InterruptCause & 0x02){
        printk("DeviceInterface_e1000: Transmit queue empty!\n");
        goto done;

    // 0x04 - Linkup
    // INTERRUPT_LSC
    // Start link.
    } else if (InterruptCause & 0x04){
        printk("DeviceInterface_e1000: Start link\n");
        __e1000_linkup(currentNIC);
        goto done;

    // 0x80 - Reveive.
    // INTERRUPT_RXT0
    } else if (InterruptCause & 0x80){
        __e1000_on_receive();
        goto done;

    // INTERRUPT_RXDMT0
    } else if (InterruptCause & 0x10){
        printk("DeviceInterface_e1000: Good threshold!\n");
        goto done;

    // ??
    // INTERRUPT_SRPD
    } else if (InterruptCause & 0x8000){
        printk ("DeviceInterface_e1000: status = 0x8000\n");
        goto done;

    } else {
        printk("DeviceInterface_e1000: Unknown interrupt cause {%x}\n",
            InterruptCause);
        goto fail;
    };

done:
// Clear all the bits.
// Write 1b, clear the bit.
    // __E1000WriteCommand( currentNIC, 0xC0, 0xffffffff );
    return;
fail:
    return;
}

/*
 * irq_E1000:
 *     Esse é o handler da interrupção para o NIC intel 8086:100E.
 *     Esse é o driver do controlador, ele não atua sobre protocolos 
 * de rede, então deve-se enviar uma mensagem para o servidor de rede 
 * para ele analizar o conteúdo do buffer, para assim decidir qual 
 * é o protocolo e redirecionar para a rotina de tratamento do 
 * protocolo específico.
 *     Esse é o driver do controlador, ele deve solicitar ao kernel
 * qual é o PID do processo que é o servidor de rede, e enviar
 * a mensagem para ele, contendo o endereço do buffer.
 */
// Isso é chamado pelo assembly.

__VOID_IRQ 
irq_E1000(void)
{
// >> The interrupt handler.

// Is the driver initialized?
    if (e1000_initialized != TRUE){
        return;
    }

// Time in ticks.
    gE1000InputTime = (unsigned long) get_systime_totalticks();

// Call the handler.
    DeviceInterface_e1000();
}

//
// $
// INITIALIZATION
//

// ---------------------------------------------
// DDINIT_e1000:
// Called by ...
// Initialize the driver.
// == NIC Intel. ===================
// #bugbug
// Ver em que hora que os buffers são configurados.
// precisam ser os mesmos encontrados na 
// infraestrutura de network e usados pelos aplicativos.
// #todo
// O driver funciona na virtualbox,
// se optarmos por PIIX3. Em ICH9 não funciona.
// Estamos suspendendo porque as interrupçoes
// geram muito ruido e a inicialização nem consegue
// terminar. Talvez tenha algo a ver com habilitar
// as interrupções antes do momento em que o
// init habilita as interrupções.

int 
DDINIT_e1000 ( 
    unsigned char bus, 
    unsigned char dev, 
    unsigned char fun, 
    struct pci_device_d *pci_device )
{
// #todo: Describe here the steps for the driver initialization.
// +
// +
// +

    register uint32_t i=0;  // loop
    uint32_t data=0;        // pci info 
    unsigned short Vendor=0;
    unsigned short Device=0;
    unsigned long phy_address=0;
    unsigned long virt_address=0;
    unsigned short tmp16=0;
    uint32_t Val=0;

    // #debug
    PROGRESS ("DDINIT_e1000:\n");
    printk   ("DDINIT_e1000:\n");
    //printk("b=%d d=%d f=%d \n", D->bus, D->dev, D->func );
    //printk("82540EM Gigabit Ethernet Controller found\n");

    e1000_initialized = FALSE;
    e1000_tx_counter=0;
    e1000_rx_counter=0;

// NIC Intel.
// #importante
// Devemos falhar antes de alocarmos memória para a estrutura.
// #todo
// Fazer uma lista de dispositivos Intel suportados por esse driver.
// +usar if else.

    data = (uint32_t) diskReadPCIConfigAddr( bus, dev, fun, 0 );
    Vendor = (unsigned short) (data       & 0xffff);
    Device = (unsigned short) (data >> 16 & 0xffff);


    /*
    //old
    if ( Vendor != 0x8086 || Device != 0x100E )
    {
        debug_print("DDINIT_e1000: Expected 82540EM\n");
        panic      ("DDINIT_e1000: Expected 82540EM\n");
    }
    */

    // #debug
    printk("Vendor=%x | Device=%x \n", Vendor, Device );

    int ValidIntelVendor = FALSE;
    int ValidIntelDevice = FALSE;

// ---------
// Valid vendor
    if (Vendor == 0x8086)
        ValidIntelVendor = TRUE;

// ---------
// Valid device

    // 82540EM
    if (Device == 0x100E)
        ValidIntelDevice = TRUE;
    if (Device == 0x1015)
        ValidIntelDevice = TRUE;

    // 82543GC
    if (Device == 0x1001)
        ValidIntelDevice = TRUE;
    if (Device == 0x1004)
        ValidIntelDevice = TRUE;

    // 82545EM
    if (Device == 0x100f)
        ValidIntelDevice = TRUE;
    if (Device == 0x1011)
        ValidIntelDevice = TRUE;

// ---------
    if (ValidIntelVendor != TRUE)
        panic("DDINIT_e1000: Invalid vendor\n");
    if (ValidIntelDevice != TRUE)
        panic("DDINIT_e1000: Invalid device\n");

// pci_device structure.
// pci device struct
// passado via argumento. 

    if ((void *) pci_device == NULL){
        panic("DDINIT_e1000: pci_device\n");
    }

    pci_device->used = TRUE;
    pci_device->magic = 1234;
    pci_device->bus  = (unsigned char) bus;
    pci_device->dev  = (unsigned char) dev;
    pci_device->func = (unsigned char) fun;
    pci_device->Vendor = (unsigned short) (data       & 0xffff);
    pci_device->Device = (unsigned short) (data >> 16 & 0xffff);

// #IMPORTANTE
// #bugbug:
// Esse driver é para placa Intel, vamos cancelar a inicialização 
// do driver se a placa não for Intel.
// 8086:100e | 82540EM Gigabit Ethernet Controller
// #todo
// Fazer uma lista de dispositivos Intel suportados por esse driver.
// +usar if else.
// já fizemos essa checagem antes.

/*
    if ( pci_device->Vendor != 0x8086 || pci_device->Device != 0x100E )
    {
        panic ("e1000_init_nic: 82540EM not found\n");
        // #bugbug: Maybe only return.
        return (int) (-1);
    }
*/

// BARs
    pci_device->BAR0 = 
        (unsigned long) diskReadPCIConfigAddr( bus, dev, fun, 0x10 );
    pci_device->BAR1 = 
        (unsigned long) diskReadPCIConfigAddr( bus, dev, fun, 0x14 ); 
    pci_device->BAR2 = 
        (unsigned long) diskReadPCIConfigAddr( bus, dev, fun, 0x18 );
    pci_device->BAR3 = 
        (unsigned long) diskReadPCIConfigAddr( bus, dev, fun, 0x1C );
    pci_device->BAR4 = 
        (unsigned long) diskReadPCIConfigAddr( bus, dev, fun, 0x20 );
    pci_device->BAR5 = 
        (unsigned long) diskReadPCIConfigAddr( bus, dev, fun, 0x24 );

// IRQ

// irq
    pci_device->irq_line = 
        (uint8_t) pciConfigReadByte( bus, dev, fun, 0x3C );
// Those letters.
    pci_device->irq_pin = 
        (uint8_t) pciConfigReadByte( bus, dev, fun, 0x3D ); 

// PCI-X Register Access Split?

// ---------------------
// The physical address!
// #importante:
// Grab the Base I/O Address of the device
// Aqui nós pegamos o endereço dos registadores na BAR0,
// Então, logo abaixo, mapearemos esse endereço físico 
// para termos um  endereço virtual, para manipularmos os registradores. 

// #bugbug: 
// size 32bit 64bit?

    phy_address = (unsigned long) (pci_device->BAR0 & 0xFFFFFFF0);
    if (phy_address == 0){
        panic("DDINIT_e1000: Invalid phy_address\n");
    }

    // ...

// ---------------------
// The virtual address!
// Base address
// #importante:
// Mapeando para obter o endereço virtual que 
// o kernel pode manipular.
// pages.c
// #bugbug: 
// >> Isso é um improviso. Ainda falta criar rotinas melhores.

// This is the desired virtual address.
// See: x64gva.h
    virt_address = (unsigned long) NIC_INTEL_E1000_VA;

    int map_status = -1;
    map_status = 
        (int) __mapping_nic1_device_address(
                (unsigned long) phy_address,     // pa
                (unsigned long) virt_address );  // va

    if (map_status != 0){
        panic("DDINIT_e1000: on __mapping_nic1_device_address()\n");
    }

// Endereço base.
// Preparando a mesma base de duas maneiras.
// char
    unsigned char *base_address = 
        (unsigned char *) virt_address;
// #bugbug 64bit address
// #todo: Do we need to change this name?
    unsigned long *base_address32 = 
        (unsigned long *) virt_address; 

//
// == NIC =========================
//

// #todo: 
// Checar essa estrutura.
// see: nicintel.h

    currentNIC = (void *) kmalloc( sizeof(struct intel_nic_info_d) );
    if ((void *) currentNIC ==  NULL){
        panic("DDINIT_e1000: currentNIC\n");
    }
    currentNIC->used = TRUE;
    currentNIC->magic = 1234;
    currentNIC->initialized = FALSE;

    currentNIC->interrupt_count = 0;
    currentNIC->pci = (struct pci_device_d *) pci_device;

// The base address for the registers.
    currentNIC->registers_base_address = 
        (unsigned long) &base_address[0];

    currentNIC->use_io = FALSE;
    //currentNIC->io_base = ?;  // i/o base port.

//
// Get info.
//

// EEPROM
// Como ainda não sabemos, vamos dizer que não.
    currentNIC->has_eeprom = FALSE; 
// Let's try to discover reading the status field!
    for ( i=0; 
          i < 1000 && !currentNIC->has_eeprom; 
          ++i ) 
    {
        Val = (uint32_t) __E1000ReadCommand( currentNIC, 0x14 );
        // We have? Yes!.
        if ( (Val & 0x10) == 0x10 ){
            currentNIC->has_eeprom = TRUE; 
        }
    };

// MAC
// Let's read the MAC Address!

    uint32_t tmp=0;
    
// We can use the EEPROM!
// Get info inside the eeprom memory.
    if (currentNIC->has_eeprom == TRUE) {
        tmp = __E1000ReadEEPROM ( currentNIC, 0 );
        currentNIC->mac_address[0] = (uint8_t)(tmp & 0xFF);
        currentNIC->mac_address[1] = (uint8_t)(tmp >> 8);
        tmp = __E1000ReadEEPROM ( currentNIC, 1);
        currentNIC->mac_address[2] = (uint8_t)(tmp & 0xFF);
        currentNIC->mac_address[3] = (uint8_t)(tmp >> 8);
        tmp = __E1000ReadEEPROM ( currentNIC, 2);
        currentNIC->mac_address[4] = (uint8_t)(tmp & 0xFF);
        currentNIC->mac_address[5] = (uint8_t)(tmp >> 8);
// We can't use the EEPROM :(
// Get info inside the registers.
// MAC - Get the mac address directly in the registers.
// One byte each per time.
    } else if (currentNIC->has_eeprom == FALSE){
        currentNIC->mac_address[0] = (uint8_t) base_address[ 0x5400 +0 ];
        currentNIC->mac_address[1] = (uint8_t) base_address[ 0x5400 +1 ];
        currentNIC->mac_address[2] = (uint8_t) base_address[ 0x5400 +2 ];
        currentNIC->mac_address[3] = (uint8_t) base_address[ 0x5400 +3 ];
        currentNIC->mac_address[4] = (uint8_t) base_address[ 0x5400 +4 ];
        currentNIC->mac_address[5] = (uint8_t) base_address[ 0x5400 +5 ];
    }

// BUS Mastering.
// Let's enable bus mastering!
// #define PCI_COMMAND  0x04
// We really need to do it?
// Yes, get the cmd, set the bus mastering bit and write back. 
// ( bus, slot, func, PCI_COMMAND )

    uint16_t cmd=0;
    cmd = 
        (uint16_t) pciConfigReadWord ( 
                       (unsigned char) bus, 
                       (unsigned char) dev, 
                       (unsigned char) fun, 
                       (unsigned char) 0x04 );

    // IN: (bus, slot, func, PCI_COMMAND, cmd);
    if ( (cmd & 0x04) != 0x04 )
    {
        cmd |= 0x04;
        diskWritePCIConfigAddr ( 
            (int) bus, (int) dev, (int) fun, 
            (int) 0x04, (int) cmd ); 
    }

// irq line:
    unsigned char irq_line = 
        (unsigned char) pciGetInterruptLine(bus,dev);


    // The value we get above.
    if (irq_line != pci_device->irq_line)
        panic("e1000: irq_line");

    //#debug
    //printk("Done irqline %d\n",irq_line);   
    //refresh_screen();

// irq
    __e1000_setup_irq(irq_line);

// Saving in the Intel NIC structure.
    currentNIC->irq_line = (unsigned char) pci_device->irq_line;
    currentNIC->irq_pin  = (unsigned char) pci_device->irq_pin;

// Reset the controller.
    __e1000_reset_controller(currentNIC);

// Flags
    currentNIC->initialized = TRUE;
    e1000_initialized = TRUE;

    //#debug
    //printk ("e1000_init_nic: Test #breakpoint\n");
    //refresh_screen();
    //while(1){ asm("hlt"); }

// 0 = no errors
    return 0;
}

