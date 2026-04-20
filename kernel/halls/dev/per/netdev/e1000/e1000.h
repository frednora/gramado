// e1000.h
// see: e1000.c
// Credits: Chicago OS, Italo Matias.
// Created by Fred Nora.

#ifndef ____E1000_H
#define ____E1000_H    1


extern int e1000_irq_count;


// Interrupt Masks
// The handler uses this.

#define INTERRUPT_TXDW    (1 << 0)  // 0x01
#define INTERRUPT_TXQE    (1 << 1)  // 0x02
#define INTERRUPT_LSC     (1 << 2)  // 0x04
#define INTERRUPT_RXSEQ   (1 << 3)  // 0x08

#define INTERRUPT_RXDMT0  (1 << 4)  // 0x10
                                    // 0x20
#define INTERRUPT_RXO     (1 << 6)  // 0x40
#define INTERRUPT_RXT0    (1 << 7)  // 0x80

#define INTERRUPT_MDAC    (1 <<  9)  // 0x100
#define INTERRUPT_RXCFG   (1 << 10)  // 0x200
                                     // 0x400
#define INTERRUPT_PHYINT  (1 << 12)  // 0x800

                                     // 0x1000
                                     // 0x2000
#define INTERRUPT_TXD_LOW (1 << 15)  // 0x4000
#define INTERRUPT_SRPD    (1 << 16)  // 0x8000


#define TDESC_STA_DD    0x01  // Indicates hardware done with descriptor
#define TDESC_CMD_EOP   0x01  // Indicates end of packet
#define TDESC_CMD_IFCS  0x02  // Insert frame checksum (FCS)
#define TDESC_CMD_RS    0x08  // Requests status report

// Registers offsets:
#define REG_CTRL  0x0000
#define REG_STATUS  0x0008
#define REG_EEPROM  0x0014
#define REG_CTRL_EXT  0x0018
#define REG_INTERRUPT_CAUSE_READ  0x00C0
#define REG_INTERRUPT_RATE  0x00C4
#define REG_INTERRUPT_MASK_SET  0x00D0
#define REG_INTERRUPT_MASK_CLEAR  0x00D8

#define REG_RCTRL  0x0100
#define REG_RXDESCLO    0x2800
#define REG_RXDESCHI    0x2804
#define REG_RXDESCLEN   0x2808
#define REG_RXDESCHEAD  0x2810
#define REG_RXDESCTAIL  0x2818

#define REG_TCTRL  0x0400
#define REG_TXDESCLO    0x3800
#define REG_TXDESCHI    0x3804
#define REG_TXDESCLEN   0x3808
#define REG_TXDESCHEAD  0x3810
#define REG_TXDESCTAIL  0x3818

#define REG_RDTR    0x2820    // RX Delay Timer Register
#define REG_RXDCTL  0x3828    // RX Descriptor Control
#define REG_RADV    0x282C    // RX Int. Absolute Delay Timer
#define REG_RSRPD   0x2C00    // RX Small Packet Detect Interrupt

#define REG_TIPG  0x0410    // Transmit Inter Packet Gap

// Aliases
#define REG_TDH    0x3810  // Transmit Descriptor Head
#define REG_TDT    0x3818  // Transmit Descriptor Tail


// How many buffers.
#define SEND_BUFFER_MAX       8
#define RECEIVE_BUFFER_MAX   32
// #define E1000_NUM_TX_DESC 8
// #define E1000_NUM_RX_DESC 32

//  Frame size?
// ( 14 + 1500 + 4 ) = 1518.
// + The standard Ethernet (IEEE 802.3) frame size is 1,518 bytes.
// + Ethernet header (14 bytes).
// + The payload (IP packet, usually 1,500 bytes).
// + Frame Check Sequence (FCS) field (4 bytes).

// #todo
// The buffer size limit depends on the configuration
// 16*512
#define E1000_DEFAULT_BUFFER_SIZE  8192
#define E1000_DEFAULT_RECEIVE_BUFFER_SIZE  E1000_DEFAULT_BUFFER_SIZE
//#define E1000_DEFAULT_SEND_BUFFER_SIZE     E1000_DEFAULT_BUFFER_SIZE

/*
// Buffer Sizes
// case 0
#define RCTL_BSIZE_256  (3 << 16)
#define RCTL_BSIZE_512  (2 << 16)
#define RCTL_BSIZE_1024 (1 << 16)
#define RCTL_BSIZE_2048 (0 << 16)
// case 1
#define RCTL_BSIZE_4096  ((3 << 16) | (1 << 25))
#define RCTL_BSIZE_8192  ((2 << 16) | (1 << 25))
#define RCTL_BSIZE_16384 ((1 << 16) | (1 << 25))
*/

// Transmit Command

/*
#define CMD_EOP  (1 << 0)  // End of Packet
#define CMD_IFCS (1 << 1)  // Insert FCS
#define CMD_IC   (1 << 2)  // Insert Checksum
#define CMD_RS   (1 << 3)  // Report Status
#define CMD_RPS  (1 << 4)  // Report Packet Sent
#define CMD_VLE  (1 << 6)  // VLAN Packet Enable
#define CMD_IDE  (1 << 7)  // Interrupt Delay Enable
*/

// ====================================================

// Transmit Descriptor
struct legacy_tx_desc 
{
    //uint64_t addr;  // The physical address of the buffer.
    uint32_t addr;
    uint32_t addr2;

    uint16_t length;
    uint8_t cso;      // Checksum offset
    uint8_t cmd;

    uint8_t status;   // status and reserved
    uint8_t css;      // checksum start
    uint16_t special;
};

// Receive Descriptor
struct legacy_rx_desc 
{
    //uint64_t buffer_addr;  // The physical address of the buffer. 
    uint32_t addr;
    uint32_t addr2;

    uint16_t length;     // Length of data DMAed into data buffer 
    uint16_t csum;       // Packet checksum 
    uint8_t status;      // Descriptor status 
    uint8_t errors;      // Descriptor Errors 
    uint16_t special;
};

// ====================================================

// ARP cache item
// #bugbug:
// Why do we have this for dedicated to the nic device
// and not one for the whole net interface?
struct e1000_arp_cache_item_d
{
    int used;
    int magic;
    int id;  // Index.

// Pinned during the whole session?
//    int pinned;

// ?

    uint8_t mac_address[6];
    //uint8_t ipv6_address[6];
    uint8_t ipv4_address[4];
};

// ============================================================
// Device Info
// #bugbug
// Me parece que isso deve ser usado apenas para dispositivos  
// Intel. Pois cada marca terá suas características.
struct intel_nic_info_d
{
    object_type_t objectType;
    object_class_t objectClass;
    int used;
    int magic;

    int initialized;

// The base address for the registers.
    unsigned long registers_base_address;

    uint8_t mac_address[6];
    //uint8_t ipv6_address[6];
    uint8_t ip_address[4];

    uint16_t rx_cur;
    uint16_t tx_cur;

    int has_eeprom;

// i/o ports support.
    int use_io;
    // uint16_t io_base;

// Estrutura de descritores 
// Virtual address for the first descriptor.
    struct legacy_rx_desc *legacy_rx_descs;  // rx ring virtual address
    struct legacy_tx_desc *legacy_tx_descs;  // tx ring virtual address

// Physical address of the first descriptor.
    unsigned long rx_descs_phys;  // rx ring physical address
    unsigned long tx_descs_phys;  // tx ring physical address

// Arrays de ponteiros de buffers.
// Ponteiros virtuais de 64bit.
    unsigned long rx_buffers_virt[32];  // Receive
    unsigned long tx_buffers_virt[8];   // Send

// ARP cache
    struct e1000_arp_cache_item_d  arp_cache[32];
    //int arpcache_max;

// PCI device.
// The structure for this device.
    struct pci_device_d *pci;

// Counting the interrupts.
    int interrupt_count;

    unsigned char irq_line;    // Qual IRQ será usada pelo PIC.
    unsigned char irq_pin;     // ??

    // rede.
    //struct network_info_d *network;
    //struct device_d     *device;
    //struct intel_nic_info_d *next;
};

// #bugbug
// Esse nome genério não deveria ser usado para o caso 
// específico da intel.
extern struct intel_nic_info_d  *currentNIC;
// ...

// Lista de placas de rede.
// #todo:
// O valor máximo precisa ser definido. 
//extern unsigned long nicList[8]; 



//
// == Prototypes ========
//

void e1000_show_info(void);

// See: e1000.c
int 
e1000_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg );

int
e1000hw_send(
    struct intel_nic_info_d *dev, 
    size_t len, 
    const char *data );

int 
e1000_send(
    struct intel_nic_info_d *dev, 
    size_t len, 
    const char *data );


// Driver initialization
int 
DDINIT_e1000 ( 
    unsigned char bus, 
    unsigned char dev, 
    unsigned char fun, 
    struct pci_device_d *pci_device );

#endif    

