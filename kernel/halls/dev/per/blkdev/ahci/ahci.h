// ahci.h
// AHCI device driver.
// Environment: 64-bit long mode kernel-side.
// Creted by Fred Nora.

#ifndef __BLKDEV_AHCI_H
#define __BLKDEV_AHCI_H    1


#define NR_PORTS  32  // maximum number of ports
//#define NR_CMDS   32  // maximum number of queued commands

// AHCI Port Interrupt Status Register (PxIS) bits
#define HBA_PxIS_TFES    (1 << 30)   // Task File Error Status
#define HBA_PxIS_HBFS    (1 << 29)   // Host Bus Fatal Error
#define HBA_PxIS_IFS     (1 << 27)   // Interface Fatal Error
#define HBA_PxIS_INFS    (1 << 26)   // Interface Non-Fatal Error
#define HBA_PxIS_OFS     (1 << 24)   // Overflow Error
#define HBA_PxIS_IPMS    (1 << 23)   // Incorrect Port Multiplier Status
#define HBA_PxIS_PRCS    (1 << 22)   // PhyRdy Change Status
#define HBA_PxIS_DMPS    (1 <<  7)   // Device Mechanical Presence Status
#define HBA_PxIS_PCS     (1 <<  6)   // Port Connect Change Status
#define HBA_PxIS_DPS     (1 <<  5)   // Descriptor Processed
#define HBA_PxIS_UFS     (1 <<  4)   // Unknown FIS
#define HBA_PxIS_SDBS    (1 <<  3)   // Set Device Bits FIS
#define HBA_PxIS_DSS     (1 <<  2)   // DMA Setup FIS
#define HBA_PxIS_PSS     (1 <<  1)   // PIO Setup FIS
#define HBA_PxIS_DHRS    (1 <<  0)   // Device to Host Register FIS

// PxCMD - Port x Command and Status
#define HBA_PxCMD_ST    0x1     // Start (ST)
#define HBA_PxCMD_FRE   0x10    // FIS Receive Enable (FRE)
#define HBA_PxCMD_FR    0x4000  // FIS Receive Running (FR)
#define HBA_PxCMD_CR    0x8000  // Command List Running (CR)
//#define HBA_PxIS_TFES   0x40000000

//
// Structure based on osdev.org totorial.
//

// 1) FIS types
// Following code defines different kinds of 
// FIS specified in Serial ATA Revision 3.0.

typedef enum {

	FIS_TYPE_REG_H2D	= 0x27,	// Register FIS - host to device
	FIS_TYPE_REG_D2H	= 0x34,	// Register FIS - device to host
	FIS_TYPE_DMA_ACT	= 0x39,	// DMA activate FIS - device to host
	FIS_TYPE_DMA_SETUP	= 0x41,	// DMA setup FIS - bidirectional
	FIS_TYPE_DATA		= 0x46,	// Data FIS - bidirectional
	FIS_TYPE_BIST		= 0x58,	// BIST activate FIS - bidirectional
	FIS_TYPE_PIO_SETUP	= 0x5F,	// PIO setup FIS - device to host
	FIS_TYPE_DEV_BITS	= 0xA1,	// Set device bits FIS - device to host

} FIS_TYPE;

// 2) Register FIS – Host to Device
typedef struct tagFIS_REG_H2D
{
	// DWORD 0
	uint8_t  fis_type;	// FIS_TYPE_REG_H2D

	uint8_t  pmport:4;	// Port multiplier
	uint8_t  rsv0:3;		// Reserved
	uint8_t  c:1;		// 1: Command, 0: Control

	uint8_t  command;	// Command register
	uint8_t  featurel;	// Feature register, 7:0
	
	// DWORD 1
	uint8_t  lba0;		// LBA low register, 7:0
	uint8_t  lba1;		// LBA mid register, 15:8
	uint8_t  lba2;		// LBA high register, 23:16
	uint8_t  device;		// Device register

	// DWORD 2
	uint8_t  lba3;		// LBA register, 31:24
	uint8_t  lba4;		// LBA register, 39:32
	uint8_t  lba5;		// LBA register, 47:40
	uint8_t  featureh;	// Feature register, 15:8

	// DWORD 3
	uint8_t  countl;		// Count register, 7:0
	uint8_t  counth;		// Count register, 15:8
	uint8_t  icc;		// Isochronous command completion
	uint8_t  control;	// Control register

	// DWORD 4
	uint8_t  rsv1[4];	// Reserved

} __attribute__((packed)) FIS_REG_H2D;


// 4) Data FIS – Bidirectional
// This FIS is used by the host or device to send data payload. 
// The data size can be varied.

typedef struct tagFIS_DATA
{
	// DWORD 0
	uint8_t  fis_type;	// FIS_TYPE_DATA

	uint8_t  pmport:4;	// Port multiplier
	uint8_t  rsv0:4;		// Reserved

	uint8_t  rsv1[2];	// Reserved

	// DWORD 1 ~ N
	// #bugbug
	// Maybe it needs to be bigger
	uint32_t data[1];	// Payload

}__attribute__((packed)) FIS_DATA;


//
// AHCI Registers and Memory Structures (osdev.org tutorial)
//

// ------------------------------------------------------------
// 1) HBA memory registers

typedef volatile struct tagHBA_PORT
{

// CLB: It points to a Command List. That points to Command Tables.
	uint32_t clb;		// 0x00, command list base address, 1K-byte aligned
	uint32_t clbu;		// 0x04, command list base address upper 32 bits

// FB: It points to a FIS receive struture.
	uint32_t fb;		// 0x08, FIS base address, 256-byte aligned
	uint32_t fbu;		// 0x0C, FIS base address upper 32 bits

	uint32_t is;		// 0x10, interrupt status
	uint32_t ie;		// 0x14, interrupt enable
	uint32_t cmd;		// 0x18, command and status
	uint32_t rsv0;		// 0x1C, Reserved
	uint32_t tfd;		// 0x20, task file data
	uint32_t sig;		// 0x24, signature
	uint32_t ssts;		// 0x28, SATA status (SCR0:SStatus)
	uint32_t sctl;		// 0x2C, SATA control (SCR2:SControl)
	uint32_t serr;		// 0x30, SATA error (SCR1:SError)
	uint32_t sact;		// 0x34, SATA active (SCR3:SActive)
	uint32_t ci;		// 0x38, command issue
	uint32_t sntf;		// 0x3C, SATA notification (SCR4:SNotification)
	uint32_t fbs;		// 0x40, FIS-based switch control
	uint32_t rsv1[11];	// 0x44 ~ 0x6F, Reserved
	uint32_t vendor[4];	// 0x70 ~ 0x7F, vendor specific

} HBA_PORT;

// This is the main structure
typedef volatile struct tagHBA_MEM
{

// 0x00 - 0x2B, Generic Host Control
	uint32_t cap;		// 0x00, Host capability
	uint32_t ghc;		// 0x04, Global host control
	uint32_t is;		// 0x08, Interrupt status
	uint32_t pi;		// 0x0C, Port implemented
	uint32_t vs;		// 0x10, Version
	uint32_t ccc_ctl;	// 0x14, Command completion coalescing control
	uint32_t ccc_pts;	// 0x18, Command completion coalescing ports
	uint32_t em_loc;	// 0x1C, Enclosure management location
	uint32_t em_ctl;	// 0x20, Enclosure management control
	uint32_t cap2;		// 0x24, Host capabilities extended
	uint32_t bohc;		// 0x28, BIOS/OS handoff control and status

// 0x2C - 0x9F, Reserved
	uint8_t  rsv[0xA0-0x2C];

// 0xA0 - 0xFF, Vendor specific registers
	uint8_t  vendor[0x100-0xA0];

// 0x100 - 0x10FF, Port control registers
	HBA_PORT ports[NR_PORTS];	// 1 ~ 32

} HBA_MEM;


// ------------------------------------------------------------
// 2) Port Received FIS and Command List Memory
// Nothing here
// (The structures are bellow)


// 6) DMA Setup – Device to Host
typedef struct tagFIS_DMA_SETUP
{
	// DWORD 0
	uint8_t  fis_type;	// FIS_TYPE_DMA_SETUP

	uint8_t  pmport:4;	// Port multiplier
	uint8_t  rsv0:1;		// Reserved
	uint8_t  d:1;		// Data transfer direction, 1 - device to host
	uint8_t  i:1;		// Interrupt bit
	uint8_t  a:1;            // Auto-activate. Specifies if DMA Activate FIS is needed

    uint8_t  rsved[2];       // Reserved

	//DWORD 1&2

    uint64_t DMAbufferID;   // DMA Buffer Identifier. Used to Identify DMA buffer in host memory.
                            // SATA Spec says host specific and not in Spec. Trying AHCI spec might work.

    //DWORD 3
    uint32_t rsvd;           //More reserved

    //DWORD 4
    uint32_t DMAbufOffset;   //Byte offset into buffer. First 2 bits must be 0

    //DWORD 5
    uint32_t TransferCount;  //Number of bytes to transfer. Bit 0 must be 0

    //DWORD 6
    uint32_t resvd;          //Reserved
        
} __attribute__((packed)) FIS_DMA_SETUP;

// 5) PIO Setup – Device to Host
// This FIS is used by the device to tell the host that 
// it’s about to send or ready to receive a PIO data payload.
typedef struct tagFIS_PIO_SETUP
{
	// DWORD 0
	uint8_t  fis_type;	// FIS_TYPE_PIO_SETUP

	uint8_t  pmport:4;	// Port multiplier
	uint8_t  rsv0:1;		// Reserved
	uint8_t  d:1;		// Data transfer direction, 1 - device to host
	uint8_t  i:1;		// Interrupt bit
	uint8_t  rsv1:1;

	uint8_t  status;		// Status register
	uint8_t  error;		// Error register

	// DWORD 1
	uint8_t  lba0;		// LBA low register, 7:0
	uint8_t  lba1;		// LBA mid register, 15:8
	uint8_t  lba2;		// LBA high register, 23:16
	uint8_t  device;		// Device register

	// DWORD 2
	uint8_t  lba3;		// LBA register, 31:24
	uint8_t  lba4;		// LBA register, 39:32
	uint8_t  lba5;		// LBA register, 47:40
	uint8_t  rsv2;		// Reserved

	// DWORD 3
	uint8_t  countl;		// Count register, 7:0
	uint8_t  counth;		// Count register, 15:8
	uint8_t  rsv3;		// Reserved
	uint8_t  e_status;	// New value of status register

	// DWORD 4
	uint16_t tc;		// Transfer count
	uint8_t  rsv4[2];	// Reserved

} __attribute__((packed)) FIS_PIO_SETUP;

// 3) Register FIS – Device to Host
typedef struct tagFIS_REG_D2H
{
	// DWORD 0
	uint8_t  fis_type;    // FIS_TYPE_REG_D2H

	uint8_t  pmport:4;    // Port multiplier
	uint8_t  rsv0:2;      // Reserved
	uint8_t  i:1;         // Interrupt bit
	uint8_t  rsv1:1;      // Reserved

	uint8_t  status;      // Status register
	uint8_t  error;       // Error register
	
	// DWORD 1
	uint8_t  lba0;        // LBA low register, 7:0
	uint8_t  lba1;        // LBA mid register, 15:8
	uint8_t  lba2;        // LBA high register, 23:16
	uint8_t  device;      // Device register

	// DWORD 2
	uint8_t  lba3;        // LBA register, 31:24
	uint8_t  lba4;        // LBA register, 39:32
	uint8_t  lba5;        // LBA register, 47:40
	uint8_t  rsv2;        // Reserved

	// DWORD 3
	uint8_t  countl;      // Count register, 7:0
	uint8_t  counth;      // Count register, 15:8
	uint8_t  rsv3[2];     // Reserved

	// DWORD 4
	uint8_t  rsv4[4];     // Reserved

} __attribute__((packed)) FIS_REG_D2H;

// Set Device Bits FIS – Device to Host
typedef struct tagFIS_DEV_BITS
{
    // DWORD 0
    uint8_t  fis_type;    // FIS_TYPE_DEV_BITS

    uint8_t  pmport:4;    // Port multiplier
    uint8_t  rsv0:2;      // Reserved
    uint8_t  i:1;         // Interrupt bit
    uint8_t  rsv1:1;      // Reserved

    uint8_t  status;      // Status register (bits 7:0)
    uint8_t  error;       // Error register

    // DWORD 1
    uint32_t sact;        // Shadow Active (SActive) register - 32 bits

    // DWORD 2
    uint32_t rsv2;        // Reserved

} __attribute__((packed)) FIS_DEV_BITS;

typedef volatile struct tagHBA_FIS
{
    // 0x00 - 0x1F
    FIS_DMA_SETUP   dsfis;      // DMA Setup FIS
    uint8_t         pad0[4];

    // 0x20 - 0x3F
    FIS_PIO_SETUP   psfis;      // PIO Setup FIS
    uint8_t         pad1[12];

    // 0x40 - 0x57
    FIS_REG_D2H     rfis;       // Register – Device to Host FIS
    uint8_t         pad2[4];

    // 0x58 - 0x5F
    FIS_DEV_BITS    sdbfis;     // Set Device Bits FIS

    // 0x60 - 0x9F
    uint8_t         ufis[64];   // Unknown FIS

    // 0xA0 - 0xFF (padding to 256 bytes)
    uint8_t         rsv[0x100 - 0xA0];

} HBA_FIS;

// -----------------------------------------------------------
// 4) Command List struture
typedef struct tagHBA_CMD_HEADER
{
	// DW0
	uint8_t  cfl:5;		// Command FIS length in DWORDS, 2 ~ 16
	uint8_t  a:1;		// ATAPI
	uint8_t  w:1;		// Write, 1: H2D, 0: D2H
	uint8_t  p:1;		// Prefetchable

	uint8_t  r:1;		// Reset
	uint8_t  b:1;		// BIST
	uint8_t  c:1;		// Clear busy upon R_OK
	uint8_t  rsv0:1;		// Reserved
	uint8_t  pmp:4;		// Port multiplier port

	uint16_t prdtl;		// Physical region descriptor table length in entries

	// DW1
	volatile
	uint32_t prdbc;		// Physical region descriptor byte count transferred

	// DW2, 3
	uint32_t ctba;		// Command table descriptor base address
	uint32_t ctbau;		// Command table descriptor base address upper 32 bits

	// DW4 - 7
	uint32_t rsv1[4];	// Reserved

} __attribute__((packed)) HBA_CMD_HEADER;

// -----------------------------------------------------------
// 5) Command table structure 
// Command Table and Physical Region Descriptor Table

typedef struct tagHBA_PRDT_ENTRY
{
	uint32_t dba;		// Data base address
	uint32_t dbau;		// Data base address upper 32 bits
	uint32_t rsv0;		// Reserved

	// DW3
	uint32_t dbc:22;		// Byte count, 4M max
	uint32_t rsv1:9;		// Reserved
	uint32_t i:1;		// Interrupt on completion

}__attribute__((packed)) HBA_PRDT_ENTRY;

// Command Table - better to support up to 8 PRDTs for larger transfers
typedef struct tagHBA_CMD_TBL
{
    uint8_t             cfis[64];           // Command FIS
    uint8_t             acmd[16];           // ATAPI command (12 or 16 bytes)
    uint8_t             rsv[48];            // Reserved
    HBA_PRDT_ENTRY      prdt_entry[8];      // Up to 8 entries (4MB each) - very safe for now
} __attribute__((packed)) HBA_CMD_TBL;

// ------------------------------------------------
// One contiguous aligned block per port (best practice)
typedef struct tagAHCI_PORT_MEMORY
{

	HBA_CMD_HEADER cmd_list[32] __attribute__((aligned(1024)));  // CLB — must be first, 1K aligned
    HBA_FIS        fis          __attribute__((aligned(256)));   // FB — right after, 256 aligned
    HBA_CMD_TBL    cmd_tbl[32]  __attribute__((aligned(128)));   // cmd tables after

} __attribute__((packed)) AHCI_PORT_MEMORY;

// AHCI port information
// Top level port structure
struct ahci_port_d 
{
    int port_num;
    int initialized;

// A big block of information
// cmd list, fis and cmd table 
    AHCI_PORT_MEMORY *mem;        // Virtual address of the whole block
    unsigned long     mem_pa;     // Physical address

    // Array of virtual pointers to each command table
	// Easy access for cmd tables?
    HBA_CMD_TBL      *cmd_tbl_va[32];

    // Later: device info, queue state, etc.
};
extern struct ahci_port_d  ahci_port[NR_PORTS];

//
// == Prototypes ===========================
//

// IN: port, lba. buffer, sector_count
int 
ahci_read_sector(
    int port, uint64_t lba, 
	void *buffer_va, 
	uint32_t sector_count );

int 
ahci_write_sector(
    int port, 
	uint64_t lba, 
	void *buffer_va, 
	uint32_t sector_count );

void ahci_test_read(void);
void ahci_test_rw(void);

int 
DDINIT_ahci(
    struct pci_device_d *pci_ahci,
    uint8_t controller_type );

#endif  

