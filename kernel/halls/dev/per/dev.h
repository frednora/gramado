// dev.h
// Created by Fred Nora.

#ifndef __DEV_DEVMGR_H
#define __DEV_DEVMGR_H    1

// #todo
// In order to use this list,
// we need a structure for generic NIC devices,
// not for a specific manufacturer.

extern unsigned long nicList[8]; 

/*
// #test
// #todo
// + When it is a built-in device driver.
// + When it is a loadable ring0 loadable device driver.
//   - dynlinked or not.
// + When it is a ring3 device driver.
#define DEVICE_DRIVER_UNDEFINED  0
#define DEVICE_DRIVER_BUILTIN    1000
#define DEVICE_DRIVER_LOADABLE_RING0  2000
#define DEVICE_DRIVER_LOADABLE_RING3  3000
*/

// unix-like
#define DEVICE_CLASS_CHAR     1
#define DEVICE_CLASS_BLOCK    2
#define DEVICE_CLASS_NETWORK  3

/*
//#test
#define DEVICE_TYPE_PCI  1
#define DEVICE_TYPE_LEGACY  2
*/

//
// Device types (subsystem identifiers)
// ------------------------------------
// These describe WHAT the device *is*.
// They are orthogonal to DEVICE_CLASS_* (char, block, network).
//

// -------------------------
// Hardware-backed devices
// -------------------------
#define DEVICE_TYPE_PCI             1   // PCI/PCIe devices
#define DEVICE_TYPE_LEGACY          2   // ISA, PS/2, PIT, PIC, RTC, etc.
#define DEVICE_TYPE_PLATFORM        3   // Non-PCI SoC / chipset devices
#define DEVICE_TYPE_USB             4   // USB devices (future)

// -------------------------
// TTY subsystem
// -------------------------
#define DEVICE_TYPE_TTY             10  // Virtual consoles, serial TTYs
#define DEVICE_TYPE_PTY             11  // Pseudo-terminals (master/slave)

// -------------------------
// Storage subsystem
// -------------------------
#define DEVICE_TYPE_ATA             20  // ATA/IDE controllers/devices
#define DEVICE_TYPE_SATA            21  // AHCI/SATA controllers/devices
#define DEVICE_TYPE_NVME            22  // NVMe controllers/devices
#define DEVICE_TYPE_RAMDISK         23  // RAM-backed block devices

// -------------------------
// Input subsystem
// -------------------------
#define DEVICE_TYPE_INPUT           30  // Generic input device
#define DEVICE_TYPE_KEYBOARD        31  // Keyboard device
#define DEVICE_TYPE_MOUSE           32  // Mouse device
#define DEVICE_TYPE_HID             33  // HID-compliant device

// -------------------------
// Display / GPU subsystem
// -------------------------
#define DEVICE_TYPE_FRAMEBUFFER     40  // Linear framebuffer device
#define DEVICE_TYPE_GPU             41  // GPU / video controller

// -------------------------
// Network subsystem
// -------------------------
#define DEVICE_TYPE_NET             50  // Network interface controller
#define DEVICE_TYPE_LOOPBACK        51  // Loopback network device

// -------------------------
// Miscellaneous
// -------------------------
#define DEVICE_TYPE_SOUND           60  // Audio devices
#define DEVICE_TYPE_TIMER           61  // PIT/HPET/APIC timers
#define DEVICE_TYPE_RTC             62  // Real-time clock
#define DEVICE_TYPE_RANDOM          63  // /dev/random, /dev/urandom


// struct more complete, with a lot of information.
struct device_class_d 
{
    //object_type_t objectType;
    //object_class_t objectClass;
    int device_class;
    int device_subclass;    
};

#define DEVICE_NAME_SIZE  64

// Device structure.
struct device_d 
{
    object_type_t objectType;
    object_class_t objectClass;
    int used;
    int magic;
    int index;

    //name for pci devices: "/DEV_8086_8086"  
    char name[DEVICE_NAME_SIZE];
    size_t Name_len;
    // #todo: merge.

// #todo: good for pci devices?
// struct more complete, with a lot of information.
    struct device_class_d *_class; 
    
//
// Device class
//

// class: char, block, network
    unsigned char __class;
// pci, legacy ...
    unsigned char __type;

//
// == (1) storage ========
//

// object? or buffer?
    file *_fp;
// #importante
// Isso deve ser um pathname 
// do mesmo tipo usado no sistema de arquivos.
// /dev/tty0
    char *mount_point;
// Se o tipo for pci.
    struct pci_device_d *pci_device;

// se o dispositivo for do tipo legado,
// como PIC, PIT, ps2, etc ...
// Qualquer coisa que não esteja na interface pci.
    //struct legacy_device_d *legacy_device;

//#todo: 
//estruturas para outros grupos de dispositivos.

//?? why light - suspenso.
//Se o dispositivo petence ao grupo dos prioritários.
    //int Light;

//Fila de dispositivos que está esperando
//por esse dispositivo.
//Na verdade podemos usar uma lista linkada ou outro recurso.
//de gerenciamento.
    //int pid;
    //unsigned long queue[8];
    //struct te_d *list;

// Driver.
// ?? Talvez pudesse ser 'device driver' e não 'tty driver'
// mas está bom assim.
    struct ttydrv_d *ttydrv;
    
//
// == (1) storage ========
//
    struct tty_d *tty;

//
// == (2) synchronization ========
//
    //int stopped;

//
// == (3) transmition ========
//

// Continua ...

    // maybe not.
    //struct device_d *next;
};

// #todo: 
// Parece uma lista muito grande para o número de dispositivos.
// mas se estamos falando de dispositivos PCI a lista é grande mesmo.
// #importante
// O número de dispositivos será o mesmo número de arquivos
// na lista Streams.
// Se o arquivo for um dispositivo então teremos
// um ponteiro na lista deviceList.

//
// The list
//

// NUMBER_OF_FILES
#define DEVICE_LIST_MAX    512

// see: devmgr.c
extern unsigned long deviceList[DEVICE_LIST_MAX];

//
// == Prototypes ==============
//

void devmgr_show_device_list(int object_type);
file *devmgr_search_in_dev_list(char *path);

struct device_d *devmgr_device_object(void);

/*
// Old Worker
int 
devmgr_register_device ( 
    file *f, 
    char *name,
    unsigned char dev_class, 
    unsigned char dev_type,
    struct pci_device_d *pci_device,
    struct tty_d *tty_device );
*/

int 
devmgr_register_tty_device(
    file *fp,
    const char *name,
    unsigned char dev_class,
    unsigned char dev_type,
    struct tty_d *tty );

int 
devmgr_register_pci_device(
    file *fp,
    const char *name,
    unsigned char dev_class,
    unsigned char dev_type,
    struct pci_device_d *pci );

int 
devmgr_register_legacy_device(
    file *fp,
    const char *name,
    unsigned char dev_class,
    unsigned char dev_type );

void devInitialize(void);

#endif    

