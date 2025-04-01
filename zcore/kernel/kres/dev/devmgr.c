// devmgr.c
// Created by Fred Nora.

#include <kernel.h>  

// List of devices.
unsigned long deviceList[DEVICE_LIST_MAX];
// #maybe
// A linked list of devices.
// struct device_d  *devices;


// Lista de placas de rede.
// #todo:
// O valor máximo precisa ser definido. 
// #todo:
// Uma lista de ponteiros para estruturas de 
// tipo generico, esse ponteiro contera um ponteiro
// para uma estrutura de dispositivo nic de marca especifica.
unsigned long nicList[8]; 

static int __devmgr_init_device_list(void);

// ------------------------

// Show the itens from the device list
// that matches with the given object type.
void devmgr_show_device_list(int object_type)
{
    register int i=0;  // Iterator
    struct device_d  *d;  // Device
    file *fp;  // Object

    printk("\n");
    printk("devmgr_show_device_list:\n");

// Get the structure pointer and
// show the info if it's a valid device structure.

    for (i=0; i<DEVICE_LIST_MAX; ++i)
    {
        d = (struct device_d *) deviceList[i];
        if ((void *) d != NULL)
        {
            if ( d->used == TRUE && d->magic == 1234 )
            {
                fp = (file *) d->_fp;
                if ((void*) fp != NULL)
                {
                    if (fp->____object == object_type)
                    {

                        //#debug
                        //if ( fp->____object == ObjectTypeTTY )
                        //    printk ("TTY DEVICE: ");
                        //if ( fp->____object == ObjectTypePciDevice )
                        //    printk ("PCI DEVICE: ");

                        //#todo: more ...
                        printk("id=%d class=%d type=%d name={%s} mount_point={%s}\n", 
                            d->index, 
                            d->__class,  // char | block | network
                            d->__type,   // pci  | legacy
                            d->name,
                            d->mount_point );  //#todo
                    }
                }
            }
            //printk (".");
        }
    };

    printk("Done\n");
}

// devmgr_search_in_dev_list:
//     Search a name into the device list.
// Used by sys_open();
// OUT:
// fp or NULL
// #todo
// Só podemos chamar isso se a lista ja estiver inicializada.
// precisamos de uma flag. Pois os valores podem estar sujos.
// #todo: Use 'const char*'
file *devmgr_search_in_dev_list(char *path)
{
    register int i=0;
    size_t PathSize=0;
    struct device_d *tmp_dev;
    void *p;

    if ((void*) path == NULL){
        return NULL;
    }

    PathSize = (size_t) strlen(path);
    if (PathSize >= 64){
        return NULL;
    }

    //if (*path == 0)
        //return NULL;

    for (i=0; i<DEVICE_LIST_MAX; i++)
    {
        // Get a pointer to a device structure.
        tmp_dev = (struct device_d *) deviceList[i];

        // Is it a valid pointer?
        if ((void*) tmp_dev != NULL)
        {
            // Is it a valid structure?
            if (tmp_dev->magic == 1234)
            {
                // Get a pointer to the 'mount point'.
                // Is this a valid mount point?
                p = (void*) tmp_dev->mount_point;
                if ((void*) p != NULL)
                {
                    // We already know the size of the given path.
                    // It can't be bigger than 64.
                    // see: devmgr.h.
                    if ( kstrncmp( p, path, PathSize ) == 0 )
                    {
                        // #debug
                        printk("Device found!\n");
                        //refresh_screen();
                        // OUT:
                        // Return the file pointer for 
                        // the file structure of this device.
                        return (file *) tmp_dev->_fp;
                    }
                }
            }
        }
    };

    return NULL;
}

// OUT: 
// A pointer to a void mounted device.
// Retorna um ponteiro de estrutura do tipo dispositivo.
struct device_d *devmgr_device_object(void)
{
    struct device_d *d;
    register int i=0;
    unsigned long __tmp=0;

// Procura um slot vazio
    for (i=0; i<DEVICE_LIST_MAX; i++)
    {
         // List of pointers.
        __tmp = (unsigned long) deviceList[i];
        if (__tmp == 0) 
        {
            // Device structure.
            // #bugbug
            // Maybe it will spend a lot of memory.
            d = (struct device_d *) kmalloc( sizeof(struct device_d) );
            if ((void *) d == NULL)
            {
                // #fatal
                panic("devmgr_device_object: [ERROR] d\n"); 
            }
            memset( d, 0, sizeof(struct device_d) );
            d->used = TRUE;
            d->magic = 1234;
            d->index = i;
            //#todo
            //d->name 
            d->name[0] = 'x';
            d->name[1] = 0;
            // ...
            // Save and return.
            deviceList[i] = (unsigned long) d;

            return (struct device_d *) d;
        }
    };

// fail
    panic("devmgr_device_object: Overflow\n");
    return NULL;
}

// ==============
// devmgr_register_device:
// Registrando um dispositivo dado o ponteiro para o arquivo
// que representa seu objeto.
// #todo: 
// Tem vários argumentos.
// Possivelmente ampliaremos o número de argumentos no futuro.
// This is called by pciHandleDevice() in pci.c to register
// every found device and for the ps2 devices initialization.
// The pcidevice argument is null in this case.
// #todo
// #bugbug
// Essa rotina tem um parametro para ponteiro de estrutura
// de dispositivo pci.
// #todo: precisamos de uma rotina similar, para registrar
// qualquer tipo de dispositivo e associar um arquivo a ele.
// IN:
// fp, pathname, class, type, pcidevice?, ttydevice?

int 
devmgr_register_device ( 
    file *f, 
    char *name,
    unsigned char dev_class, 
    unsigned char dev_type,
    struct pci_device_d *pci_device,
    struct tty_d *tty_device )
{
    struct device_d *d;
    int id= -1;
// mount point
    size_t NameSize=0;
    int PathSize = 64;  // Pathname size.
    char buf[PathSize];
    char *new_mount_point;

    debug_print("devmgr_register_device:\n");

// Allocation memory for the string that is gonna have
// a pointer in the device structure. 'd->mount_point'.
    new_mount_point = (char *) kmalloc(PathSize);
    if ((void*) new_mount_point == NULL){
        panic("devmgr_register_device: new_mount_point\n");
    }
    memset( new_mount_point, 0, PathSize );

// =======================
// FILE. Device object.
    if ((void *) f == NULL){
        panic("devmgr_register_device: f\n");
    }
    if ( f->used != TRUE || f->magic != 1234 ){
        panic("devmgr_register_device: f validation\n");
    }
    if (f->isDevice != TRUE){
        panic("devmgr_register_device: This file is NOT a device\n");
    }

// =======================
// Device structure. 
// (It is NOT the pci device struct)
    d = (struct device_d *) devmgr_device_object();
    if ((void *) d == NULL){
        panic("devmgr_register_device: d\n");
    }
    if ( d->used != TRUE || d->magic != 1234 ){
        panic ("devmgr_register_device: d validation\n");
    }
// id
    id = d->index;
    if ( id < 0 || id >= DEVICE_LIST_MAX ){
        panic("devmgr_register_device: id\n");
    }

// Save parameters.
    d->__class = (unsigned char) dev_class;
    d->__type  = (unsigned char) dev_type;

// file
// The file pointer.
    //if( (void*) f == NULL ){
    //    panic ("devmgr_register_device: f\n");
    //}

//
// major/minor
//

// #todo
    f->dev_major = 0;
// Device index into the deviceList[].
    f->dev_minor = (short) (d->index & 0xFFFF);

// Device structure.
    f->device = (struct device_d *) d;
// Save the file pointer.
    d->_fp  = (file *) f;

//
// name
//

    d->name[0] = 0;
    d->Name_len = 0;

// Clear buffer
    memset( buf, 0, PathSize );

// ----------------------
// Se um nome não foi indicado.
    if ((void*) name == NULL)
    {
        ksprintf( buf, "/DEV/%d", id );
        strcpy( new_mount_point, buf );
    }
// ----------------------
// Se um nome foi indicado.
    if ((void*) name != NULL)
    {
        NameSize = (size_t) strlen(name);
        if (NameSize >= (PathSize-5) )
        {
            panic("devmgr_register_device: NameSize\n");
        }
        
        if (NameSize <= DEVICE_NAME_SIZE)
        {
            ksprintf( d->name, "%s", name );
            d->Name_len = NameSize;
        }

        //ksprintf( buf, name );
        // #todo: Copy n bytes using strncpy.
        //strcpy( new_mount_point, buf );
        ksprintf( buf, "/DEV/%s", name );
        strcpy( new_mount_point, buf );
    }

// /dev/tty0
    d->mount_point = (char *) new_mount_point;
    // DEV_8086_8086

// pci device
    d->pci_device = (struct pci_device_d *) pci_device;
// tty device
// Saving the given parameter into the device structure.
    d->tty = (struct tty_d *) tty_device;
    // ...
    return 0;
}


// Initialize the list.
static int __devmgr_init_device_list(void)
{
    register int i=0;

    for (i=0; i<DEVICE_LIST_MAX; i++){
        deviceList[i] = 0;
    };
    return 0;
}

//
// $
// INITIALIZATION
//

// devmgr_initialize:
// Called by I_initKernelComponents() in x64init.c.
// Inicializa o gerenciamento de dispositivos.
// Inicializa a lista de dispositivos.
// ===============================
// Initialize device manager.
// >>> We're gonna hace a list of devices,
// including the hal stuff.
// At this moment we didn't initialize any device,
// maybe only the 'serial port' used in the basic debug.

void devmgr_initialize(void)
{
    register int i=0;

    debug_print("devmgr_initialize:\n");

// Initialize the list of devices.
    __devmgr_init_device_list();

// Initialize the list of NIC devices.
    for (i=0; i<8; i++)
    {
        nicList[i] = 0;
    };
    // ...
}

//
// End
//

