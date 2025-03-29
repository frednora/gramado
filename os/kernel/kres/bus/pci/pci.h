// pci.h
// pci bus support.
// Created by Fred Nora.

#ifndef __PCI_PCI_H
#define __PCI_PCI_H    1


//------------------------------------------------------

// Building a 32bit value given the elements.
// Used in atapci.c
#define __PCI_CONFIG_ADDR(bus,device,fn,offset)\
            (\
            (((uint32_t)(bus) &0xff) << 16)|\
            (((uint32_t)(device) &0x3f) << 11)|\
            (((uint32_t)(fn) &0x07) << 8)|\
            ((uint32_t)(offset) &0xfc)|0x80000000)

//------------------------------------------------------

// Ports
#define PCI_PORT_ADDR     0xCF8
#define PCI_PORT_DATA     0xCFC
#define PCI_ADDRESS_PORT  0x0CF8
#define PCI_DATA_PORT     0x0CFC

// Header Type
#define PCI_TYPE_GENERIC         0x00
#define PCI_TYPE_PCI_BRIDGE      0x01
#define PCI_TYPE_CARDBUS_BRIDGE  0x02
#define PCI_TYPE_MULTIFUNC       0x80

// BDF
#define PCI_MAX_BUSES      0xFF
#define PCI_MAX_DEVICES    32
#define PCI_MAX_FUNCTIONS  8

#define PCI_INVALID_VENDORID  0xFFFF

//
// PCI Offsets
//

// Deslocamento dentro da tabela de configuração, dado em hexa.
#define PCI_OFFSET_VENDORID       0       //short
#define PCI_OFFSET_DEVICEID       0x02    //short

#define PCI_OFFSET_COMMAND        0x04    //short CMD
#define PCI_OFFSET_STATUS         0x06    //short

#define PCI_OFFSET_REVISIONID     0x08    //char RID
#define PCI_OFFSET_PROGIF         0x09    //char  PI Programming Inteface
#define PCI_OFFSET_SUBCLASS       0x0A    //char Class Code CC /* Device class */ //*importante
#define PCI_OFFSET_CLASSCODE      0x0B    //char.

#define PCI_OFFSET_CACHELINESIZE  0x0C    //char CLS
#define PCI_OFFSET_LATENCYTIMER   0x0D    //char Master Latency Timer - MLT
#define PCI_OFFSET_HEADERTYPE     0x0E    //char HTYPE
#define PCI_OFFSET_BIST           0x0F    //char

// Base addresses specify locations in memory or I/O space.
// Decoded size can be determined by writing a value of
// 0xffffffff to the register, and reading it back.  
// Only 1 bits are decoded.

#define PCI_OFFSET_BASEADDRESS0   0x10  //unsigned long
#define PCI_OFFSET_BASEADDRESS1   0x14  //unsigned long [htype 0,1 only]
#define PCI_OFFSET_BASEADDRESS2   0x18  //unsigned long [htype 0 only]
#define PCI_OFFSET_BASEADDRESS3   0x1C  //unsigned long
#define PCI_OFFSET_BASEADDRESS4   0x20  //unsigned long
#define PCI_OFFSET_BASEADDRESS5   0x24  //unsigned long

/*
#define  PCI_BASE_ADDRESS_SPACE	0x01	// 0 = memory, 1 = I/O 
#define  PCI_BASE_ADDRESS_SPACE_IO 0x01
#define  PCI_BASE_ADDRESS_SPACE_MEMORY 0x00
#define  PCI_BASE_ADDRESS_MEM_TYPE_MASK 0x06
#define  PCI_BASE_ADDRESS_MEM_TYPE_32	0x00	// 32 bit address 
#define  PCI_BASE_ADDRESS_MEM_TYPE_1M	0x02	// Below 1M [obsolete]
#define  PCI_BASE_ADDRESS_MEM_TYPE_64	0x04	// 64 bit address 
#define  PCI_BASE_ADDRESS_MEM_PREFETCH	0x08	// prefetchable? 
#define  PCI_BASE_ADDRESS_MEM_MASK	(~(pciaddr_t)0x0f)
#define  PCI_BASE_ADDRESS_IO_MASK	(~(pciaddr_t)0x03)
// bit 1 is reserved if address_space = 1 
*/

#define PCI_OFFSET_CISP    0x28  //unsigned long (Cardbus CIS Pointer)

#define PCI_OFFSET_SUBSYSTEMVENDORID    0x2C //short sub system identifiers SS
#define PCI_OFFSET_SUBSYSTEMID          0x2E //short

#define PCI_OFFSET_EXPANSIONROMBASEADDRESS  0x30  //unsigned long

#define PCI_OFFSET_CAPABILITIES_POINTER  0x34    //char Capabilities Pointer. CAP
//#define PCI_OFFSET_RESERVEDZZ         0x35    //3 bytes.

//#define PCI_OFFSET_RESERVEDZZZ 0x38  //unsigned long  

#define PCI_OFFSET_INTERRUPTLINE    0x3C //char (Read and wright). Interrupt Information INTR
#define PCI_OFFSET_INTERRUPTPIN     0x3D //char (Read only).
#define PCI_OFFSET_MIN_GRANT        0x3E //char  Minimum Grant MGNT
#define PCI_OFFSET_MAX_LATENCY      0x3F //char  Maximum Latency MLAT

//Intel sata host controller support:
//40h 2 ?Primary Timing (PTIM)?Offset 40h? on page 153 0000h
//42h 2 ?Secondary Timing (STIM)?Offset 42h? on page 154 0000h
//44h 1 ?Device 1 IDE Timing (D1TIM)?Offset 44h? on page 155 00h
//48h 1 ?Synchronous DMA Control (Synchronous_DMA_Control)?Offset 48h? on page 155 00h
//4Ah 2 ?Synchronous_DMA_Timing?Offset 4Ah? on page 156 0000h
//54h 4 ?IDE I/O Configuration (IIOC)?Offset 54h? on page 156 00000000h
//70h 2 ?PCI Power Management Capability ID (PID)?Offset 70h? on page 157 A801h
//72h 2 ?Primary Command Block Base Address (PCMDBA)?Offset 10h? on page 147 4003h
//74h 2 ?PCI Power Management Control and Status (PMCS)?Offset 74h? on page 158 0008h
//80h 2 ?Message Signaled Interrupt Identifier (MID)?Offset 80h? on page 159 7005h
//82h 2 ?Message Signaled Interrupt Message Control (MC)?Offset 82h? on page 159 0000h
//84h 4 ?Message Signaled Interrupt Message Address (MA)?Offset 84h? on page 160 00000000h
//88h 2 ?Message Signaled Interrupt Message Data (MD)?Offset 88h? on page 160 0000h
//90h 2 ?Port Mapping Register (MAP)?Offset 90h? on page 161 0420h
//92h 2 ?Port Control and Status (PCS)?Offset 92h? on page 162 0000h
//94h 4 ?Test Mode Register (TM)?Offset 94h? on page 163 00000000h
//9Ch 4 ?SATA General Configuration (SATAGC)?Offset 9Ch? on page 163 00000000h
//A0h 1 ?SATA Initialization Register Index (SIRI)?Offset A0h? on page 165 00h
//A4h 4 ?SATA Initialization Register Data (SIRD)?Offset A4h? on page 165 00000000h
//A8h 4 ?Serial ATA Capability Register 0 (SATACR0)?Offset A8h? on page 166 00100012h
//ACh 4 ?Serial ATA Capability Register 1 (SATACR1)?Offset ACh? on page 167 00000048h
//B0h 2 ?FLR Capability ID (FLRCID)?Offset B0h? on page 167 0000h
//B4h 2 ?FLR Control (FLRCTL)?Offset B4h? on page 168 0000h
//D0h 4 ?Scratch Pad (SP)?Offset D0h? on page 168 00000000h
//E0h 4 ?BIST FIS Control/Status (BFCS)?Offset E0h? on page 169 00000000h
//E4h 4 ?BIST FIS Transmit Data 1 (BFTD1)?Offset E4h? on page 170 00000000h
//E8h 4 ?BIST FIS Transmit Data 2 (BFTD2)?Offset E8h? on page 170 00000000h
//F8h 4 ?Manufacturing ID (MFID)?Offset F8h? on page 171 08000FB1h


//Interrupt Pin (IPIN): This register tells which interrupt pin the device function uses. A
//value of 1 corresponds to INTA#. A value of 2 corresponds to INTB#. A value of 3
//corresponds to INTC#. A value of 4 corresponds to INTD#.

//Interrupt Line (ILINE): Software written value to indicate which interrupt line
//(vector) the interrupt is connected to. No hardware action is taken on this register.

/*
 OSDEV.ORG
Class Code	Description
0x00	Device was built prior definition of the class code field
0x01	Mass Storage Controller
0x02	Network Controller
0x03	Display Controller
0x04	Multimedia Controller
0x05	Memory Controller
0x06	Bridge Device
0x07	Simple Communication Controllers
0x08	Base System Peripherals
0x09	Input Devices
0x0A	Docking Stations
0x0B	Processors
0x0C	Serial Bus Controllers
0x0D	Wireless Controllers
0x0E	Intelligent I/O Controllers
0x0F	Satellite Communication Controllers
0x10	Encryption/Decryption Controllers
0x11	Data Acquisition and Signal Processing Controllers
0x12 - 0xFE	Reserved
0xFF	Device does not fit any defined class.
*/

//BAR for sata, intel host controller
//10h-?Primary Command Block Base Address (PCMDBA)?Offset 10h? 
//14h- ?Primary Control Block Base Address (PCTLBA)?Offset 14h? 
//18h -Secondary Command Block Base Address (SCMDBA)?Offset 18h? 
//1ch-?Secondary Control Block Base Address (SCTLBA)?Offset 1Ch? 
//20h-?Legacy IDE Base Address / AHCI Index Data Pair Base Address (LBAR)?Offset 20h? 
//24h-?AHCI Base Address/Serial ATA Index Data Pair Base Address (ABAR)?Offset 24h?

//vendor 0x8086 device 0x7000 
//Intel Corporation 82371SB PIIX3 ISA [Natoma/Triton II] 
//0x10de:0x0422:NVIDIA Corporation:G86 [GeForce 8400 GS]
//0x2668	82801FB (ICH6) High Definition Audio Controller	0x8086	Intel Corporation

// 
// Class Codes
//

#define PCI_CLASSCODE_LEGACY                    0
#define PCI_CLASSCODE_MASS                      0x01    //Mass storage controller.
#define PCI_CLASSCODE_NETWORK                   0x02
#define PCI_CLASSCODE_DISPLAY                   0x03
#define PCI_CLASSCODE_MULTIMEDIA                0x04
#define PCI_CLASSCODE_MEMORY                    0x05
#define PCI_CLASSCODE_BRIDGE                    0x06
#define PCI_CLASSCODE_COMMUNICATION             0x07
#define PCI_CLASSCODE_GENERICSYS                0x08    //Generic system peripheral
#define PCI_CLASSCODE_INPUT                     0x09
#define PCI_CLASSCODE_GENERICDOCKINGSTATION     0x0A
#define PCI_CLASSCODE_PROCESSOR                 0x0B
#define PCI_CLASSCODE_SERIALBUS                 0x0C
#define PCI_CLASSCODE_WIRELESS                  0x0D
#define PCI_CLASSCODE_INTELLIGENTIO             0x0E
#define PCI_CLASSCODE_ENCRYPTION_DECRYPTION     0x0F //Satellite communications controllers.
#define PCI_CLASSCODE_SATELLITECOMMUNICATION    0x10 //Encryption/Decryption controllers.
//11h - Data acquisition and signal processing controllers.
//12h~feh - Reserved.
//#define PCI_CLASSCODE_MISC   0xFF //Device does not fit any of the defined class codes.

// Class Code Register:
//Class Code          Sub-Class Code            Prog. I/F
//23..................16	15..................8	7..................0

//
// Device subclasses
//

//0
#define PCI_SUBCLASS_NONVGA  0  //Non-VGA unclassified device.
#define PCI_SUBCLASS_VGA     1  //VGA compatible unclassified device.

//1
#define PCI_SUBCLASS_SCSI        0     //SCSI storage controller.
#define PCI_SUBCLASS_IDE         0x01  //IDE interface.
#define PCI_SUBCLASS_FLOPPY      0x02  //Floppy disk controller.
#define PCI_SUBCLASS_IPI         0x03  //IPI bus controller.
#define PCI_SUBCLASS_RAID        0x04  //RAID bus controller.
#define PCI_SUBCLASS_ATA         0x05  //ATA controller.
#define PCI_SUBCLASS_SATA        0x06  //SATA controller.
#define PCI_SUBCLASS_SERIALSCSI  0x07  //Serial Attached SCSI controller.
#define PCI_SUBCLASS_NVMEMORY    0x08  //Non-Volatile memory controller. (NVME)
#define PCI_SUBCLASS_SAS         0x09  //SAS (Serial Attached SCSI)
#define PCI_SUBCLASS_MASS        0x80  //Mass storage controller.

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


//2
#define PCI_SUBCLASS_ETHERNET    0     //Ethernet controller
#define PCI_SUBCLASS_TRN         0x01  //Token ring network controller
#define PCI_SUBCLASS_FDDI        0x02  //FDDI network controller
#define PCI_SUBCLASS_ATM         0x03  //ATM network controller
#define PCI_SUBCLASS_ISDN        0x04  //ISDN controller
#define PCI_SUBCLASS_WF          0x05  //WorldFip controller
#define PCI_SUBCLASS_PICMG       0x06  //PICMG controller
#define PCI_SUBCLASS_INFINIBAND  0x07  //Infiniband controller
#define PCI_SUBCLASS_FABRIC      0x08  //Fabric controller
#define PCI_SUBCLASS_NETWORK     0x80  //Network controller

//3
#define PCI_SUBCLASS_VGACOMPATIBLE  0     //VGA compatible controller
#define PCI_SUBCLASS_XGACOMPATIBLE  0x01  //XGA compatible controller
#define PCI_SUBCLASS_3D             0x02  //3D controller
#define PCI_SUBCLASS_DISPLAY        0x80  //Display controller

//4
#define PCI_SUBCLASS_MVIDEO      0     //Multimedia video controller
#define PCI_SUBCLASS_MAUDIO      0x01  //Multimedia audio controller
#define PCI_SUBCLASS_TELEPHONY   0x02  //Computer telephony device
#define PCI_SUBCLASS_AUDIO       0x03  //Audio device
#define PCI_SUBCLASS_MULTIMEDIA  0x80  //Multimedia controller

//5
#define PCI_SUBCLASS_RAM     0     //RAM memory
#define PCI_SUBCLASS_FLASH   0x01  //FLASH memory
#define PCI_SUBCLASS_MEMORY  0x80  //Memory controller

//6
#define PCI_SUBCLASS_HOST             0     //Host bridge
#define PCI_SUBCLASS_ISA              0x01  //ISA bridge
#define PCI_SUBCLASS_EISA             0x02  //EISA bridge
#define PCI_SUBCLASS_MICROCHANNEL     0x03  //MicroChannel bridge
#define PCI_SUBCLASS_PCI              0x04  //PCI bridge
#define PCI_SUBCLASS_PCMCIA           0x05  //PCMCIA bridge
#define PCI_SUBCLASS_NUBUS            0x06  //NuBus bridge
#define PCI_SUBCLASS_CARDBUS          0x07  //CardBus bridge
#define PCI_SUBCLASS_RACEWAY          0x08  //RACEway bridge
#define PCI_SUBCLASS_PCITOPCI         0x09  //Semi-transparent PCI-to-PCI bridge
#define PCI_SUBCLASS_INFINIBANDTOPCI  0x0A  //InfiniBand to PCI host bridge
#define PCI_SUBCLASS_BRIDGE           0x80  //Bridge

//7
#define PCI_SUBCLASS_SERIAL         0     // Serial controller
#define PCI_SUBCLASS_PARALLEL       0x01  // Parallel controller
#define PCI_SUBCLASS_MULTIPORT      0x02  // Multiport serial controller
#define PCI_SUBCLASS_MODEM          0x03  // Modem
#define PCI_SUBCLASS_GPIB           0x04  // GPIB controller
#define PCI_SUBCLASS_SMARDCARD      0x05  // Smard Card controller
#define PCI_SUBCLASS_COMMUNICATION  0x80  // Communication controller

//8
#define PCI_SUBCLASS_PIC          0x00
#define PCI_SUBCLASS_DMA          0x01
#define PCI_SUBCLASS_TIMER        0x02
#define PCI_SUBCLASS_RTC          0x03
#define PCI_SUBCLASS_PCIHOTPLUG   0x04
#define PCI_SUBCLASS_SDHOST       0x05
#define PCI_SUBCLASS_IOMMU        0x06

//9
#define PCI_SUBCLASS_KEYBOARD  0     // Keyboard controller
#define PCI_SUBCLASS_DIGPEN    0x01  // Digitizer Pen
#define PCI_SUBCLASS_MOUSE     0x02  // Mouse controller
#define PCI_SUBCLASS_SCANNER   0x03  // Scanner controller
#define PCI_SUBCLASS_GAMEPORT  0x04  // Gameport controller
#define PCI_SUBCLASS_INPUT     0x80  // Input device controller

//A
#define PCI_SUBCLASS_GDS  0     // Generic Docking Station
#define PCI_SUBCLASS_DS   0x80  // Docking Station

//B
#define PCI_SUBCLASS_386          0     // 386
#define PCI_SUBCLASS_486          0x01  // 486
#define PCI_SUBCLASS_PENTIUM      0x02  // Pentium
#define PCI_SUBCLASS_ALPHA        0x10  // Alpha
#define PCI_SUBCLASS_POWERPC      0x20  // Power PC
#define PCI_SUBCLASS_MIPS         0x30  // MIPS
#define PCI_SUBCLASS_COPROCESSOR  0x40  // Co-processor

//C
#define PCI_SUBCLASS_FW           0     // FireWire (IEEE 1394)
#define PCI_SUBCLASS_ACCESS       0x01  // ACCESS Bus
#define PCI_SUBCLASS_SSA          0x02  // SSA
#define PCI_SUBCLASS_USB          0x03  // USB controller
#define PCI_SUBCLASS_FIBRE        0x04  // Fibre Channel
#define PCI_SUBCLASS_SMBUS        0x05  // SMBus
#define PCI_SUBCLASS_INFINIBAND2  0x06  // InfiniBand
#define PCI_SUBCLASS_IPMI         0x07  // IPMI SMIC interface
#define PCI_SUBCLASS_SERCOS       0x08  // SERCOS interface
#define PCI_SUBCLASS_CANBUS       0x09  // CANBUS

//D
#define PCI_SUBCLASS_IRDA       0     // IRDA controller
#define PCI_SUBCLASS_CIR        0x01  // Consumer IR controller
#define PCI_SUBCLASS_RF         0x10  // RF controller
#define PCI_SUBCLASS_BLUETOOTH  0x11  // Bluetooth
#define PCI_SUBCLASS_BROADBAND  0x12  // Broadband
#define PCI_SUBCLASS_8021A      0x20  // 802.1a controller
#define PCI_SUBCLASS_8021B      0x21  // 802.1b controller
#define PCI_SUBCLASS_WIRELESS   0x80  // Wireless controller

//E
#define PCI_SUBCLASS_I2O  0  //I2O.

//F
#define PCI_SUBCLASS_STV     0x01  // Satellite TV controller.
#define PCI_SUBCLASS_SAUDIO  0x02  // Satellite audio communication controller.	
#define PCI_SUBCLASS_SVOICE  0x03  // Satellite voice communication controller.	
#define PCI_SUBCLASS_SDATACOMMUNICATION  0x04  //Satellite data communication controller.

//0x10
#define PCI_SUBCLASS_NCENCRYPTION  0     // Network and computing encryption device.	
#define PCI_SUBCLASS_EENCRYPTION   0x10  // Entertainment encryption device.	
#define PCI_SUBCLASS_ENCRYPTION    0x80  // Encryption controller.

//0x11
#define PCI_SUBCLASS_DPIO       0     // DPIO module.
#define PCI_SUBCLASS_PCOUNTERS  0x01  // Performance counters.
#define PCI_SUBCLASS_CSYNC  0x10      // Communication synchronizer.
#define PCI_SUBCLASS_SPM    0x20      // Signal processing management.
#define PCI_SUBCLASS_SPC    0x80      // Signal processing controller.

//0x12
#define PCI_SUBCLASS_PACCEL  0  // Processing accelerators.

//...

//char pcideviceString[] = "PCI Device";
//char pcidString[] = "PCI"; 
 
//
// Vendor list
//

//asus 0x0AC8
//asustek 0x1043
//0x2646	Kingston Technology Co.
//0x15B7	Sandisk Corp.
//0x1022	Advanced Micro Devices amd
//0x1011	Digital Equipment Corporation dec
//0x101E	American Megatrends Inc.
//Oracle Corporation - InnoTek Systemberatung GmbH 0x80EE

//Em ordem alfabética.
#define PCI_VENDOR_ID_AMD                 0x1022
#define PCI_VENDOR_ID_APPLE               0x106B  //0x05ac
#define PCI_VENDOR_ID_ATI                 0x1002
#define PCI_VENDOR_ID_ATMEL_CORPORATION   0x1114  //Atmel Corp. 0x1114
#define PCI_VENDOR_ID_INTEL               0x8086
#define PCI_VENDOR_ID_NVIDIA              0x10DE
#define PCI_VENDOR_ID_ORACLE_CORPORATION  0x80EE 
#define PCI_VENDOR_ID_REALTEK             0x10EC
#define PCI_VENDOR_ID_TEXAS_INSTRUMENTS   0x104C
#define PCI_VENDOR_ID_VIA                 0x1106
//...


//------------------

//
// Variáveis internas.
//

//unsigned long pci_handler_address;

//
// PCI class struct
//
/*
struct pci_class_d
{
    unsigned long pci_class;
    unsigned long pci_subclass;
    char *name;
};
//
*/

//
// PCI device structure.
//
 
struct pci_device_d 
{
    object_type_t  objectType;
    object_class_t objectClass;
    int used;
    int magic;
    int id;

    char *name;

    //unsigned long flag;   
    //unsigned long error;  

    // Status do dispositivo.
    //int device_status;
    //char *device_status_string;

// #todo: 
// Completar com os elementos que faltam.
// Seguindo a ordem padrão.

// Salvar também qual bus o device pertence.
// Localização do dispositivo.

    unsigned char bus;
    unsigned char dev;
    unsigned char func;

    unsigned short Device;
    unsigned short Vendor;

    unsigned short Status;
    unsigned short Command;

    unsigned char classCode;
    unsigned char subclass;
    unsigned char progif;
    unsigned char revisionId;

    unsigned char bist;
    unsigned char header_type;
    unsigned char latency_timer;
    unsigned char cache_line_size;

// BARs:
// 32bit addresses.

    unsigned int BAR0;
    unsigned int BAR1;
    unsigned int BAR2;
    unsigned int BAR3;
    unsigned int BAR4;
    unsigned int BAR5;

// Sub-system vendor id.
// Sub-system device id.

    unsigned short subsystem_Vendor;
    unsigned short subsystem_Device;

    unsigned char max_latency;
    unsigned char min_grant;
    unsigned char irq_pin;     //??
    unsigned char irq_line;    //Qual IRQ será usada pelo PIC.

// Driver
// Estrutura para o driver do dispositivo.
    struct pci_driver_d *driver;

// ...

// A list of threads waiting on this device.
    struct thread_d *waiting_list;

    struct pci_device_d  *next;
};
extern struct pci_device_d  *pci_device;
extern struct pci_device_d  *current_pci_device;


// PCI information.
// Estrutura para gerenciar a interface pci.
// apenas uma instância dessa estrutura deve existir.
struct pci_d
{
    int used;
    int magic;
    int initialized;

// Number of found devices.
    unsigned int found_devices;
// Max
    unsigned int max;

    // ...

// List?
    //struct pci_device_d  *device_list;
    unsigned long device_list_base_address;
};
extern struct pci_d  PCI;

// PCI device list.

#define PCI_DEVICE_LIST_SIZE  128
// #todo: 
// Put this in pci.c and create an external refrence here.
unsigned long pcideviceList[PCI_DEVICE_LIST_SIZE];   

//
// DRIVER
//

/*
 * pci_driver_d:
 *     Estrutura para drivers de dispositivos pci.     
 */
struct pci_driver_d
{
    object_type_t  objectType;
    object_class_t objectClass;
    int used;
    int magic;
// Driver ID.
    int id;

//status do driver de dispositivo.
//Initialized, ... @todo: Criar enum. pci_device_status_t;
	//int status;

//Nome do driver do dispositivo PCI.
    const char *name;
//Pathname para a imagem do driver.
    const char *pathname;
	
	//unsigned long driver_version;
	
	//Endereço da rotina de tratamento de interrupção.
	//unsigned long irq_handler;
	
	//
	// Buffers!
	//

	// Os endereços de memória encontrados na configuração
	// de PCI poderão ser salvos aqui.

	//unsigned long io_buffer_address1;
	//unsigned long io_buffer_address2;
	//unsigned long io_buffer_address3;
	//unsigned long io_buffer_address4;

// Estrutura do dispositivo pci gerenciado pelo driver.
    struct pci_device_d  *pci_device;

    // ...

    struct pci_driver_d  *next;
};

// Lists?
//struct pci_driver_d  *pci_drivers_list;
//...

//Lista de drivers de dispositivos pci.
//unsigned long pcidriversList[32];

/*
 * The PCI interface treats multi-function devices as independent
 * devices.  The slot/function address of each device is encoded
 * in a single byte as follows:
 *
 *	7:3 = slot
 *	2:0 = function
 *
 * PCI_DEVFN(), PCI_SLOT(), and PCI_FUNC() are defined in uapi/linux/pci.h.
 * In the interest of not exposing interfaces to user-space unnecessarily,
 * the following kernel-only defines are being added here.
 */

//#define PCI_DEVID(bus, devfn)  ((((u16)(bus)) << 8) | (devfn))

/* return bus from PCI devid = ((u16)bus_number) << 8) | devfn */

//#define PCI_BUS_NUM(x)  (((x) >> 8) & 0xff)


//
// == prototypes ===========================
//

// read 8 bits.
unsigned char 
pciConfigReadByte ( 
    unsigned char bus, 
    unsigned char slot, 
    unsigned char func, 
    unsigned char offset );

// read 16 bits.
unsigned short 
pciConfigReadWord ( 
    unsigned char bus, 
    unsigned char slot, 
    unsigned char func, 
    unsigned char offset );

// read 32 bits.
unsigned int 
pciConfigReadDWord ( 
    unsigned char bus, 
    unsigned char slot, 
    unsigned char func, 
    unsigned char offset );

// ==========

unsigned int 
pciGetBAR ( 
    unsigned char bus, 
    unsigned char slot, 
    int number );

unsigned char 
pciGetClassCode (unsigned char bus, unsigned char slot);

unsigned char 
pciGetHeaderType (unsigned char bus, unsigned char slot);

unsigned char 
pciGetInterruptLine ( 
    unsigned char bus, 
    unsigned char slot );

unsigned char 
pciGetInterruptPin ( 
    unsigned char bus, 
    unsigned char slot );

unsigned char 
pciGetSubClass (unsigned char bus, unsigned char slot);

unsigned short 
pciCheckDevice (unsigned char bus, unsigned char slot);

unsigned short 
pciCheckVendor (unsigned char bus, unsigned char slot);

int pciscan_probe_devices (void);

struct pci_device_d *scan_pci_device_list ( 
    unsigned short vendor, 
    unsigned short device );

struct pci_device_d *scan_pci_device_list2 ( 
    unsigned char class, 
    unsigned char subclass );

int pciInfo (void);
int pciShowDeviceInfo (int number);

int 
pciHandleDevice ( 
    unsigned char bus, 
    unsigned char dev, 
    unsigned char fun );

//
// $
// INITIALIZATION
//

int init_pci (void);

#endif    

