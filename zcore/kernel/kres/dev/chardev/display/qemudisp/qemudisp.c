// qemudisp.c
// Display controller support.
// #todo: display device for qemu vm. (bochs)
// Created by Fred Nora.


#include <kernel.h>

struct pci_device_d *PCIDeviceQemuDisplay;





// #todo ioctl
int 
qemudisp_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg )
{
    debug_print("qemudisp_ioctl: #todo\n");
    if(fd<0){
        return -1;
    }
    return -1;
}


//
// $
// INITIALIZATION
//


int qemudisp_initialize(void)
{
    int Status = -1;

// #test
// Sondando na lista de dispositivos encontrados 
// pra ver se tem algum controlador de display.
// #importante:
// Estamos sondando uma lista que contruimos quando fizemos
// uma sondagem no começo da inicializaçao do kernel.
// #todo: 
// Podemos salvar essa lista.
// #todo
// É uma estrutura para dispositivos pci. (pci_device_d)
// Vamos mudar de nome.


// pci device.
    PCIDeviceQemuDisplay = 
        (struct pci_device_d *) scan_pci_device_list2 ( 
                                    (unsigned char) PCI_CLASSCODE_DISPLAY, 
                                    (unsigned char) PCI_SUBCLASS_NONVGA );

    if ((void *) PCIDeviceQemuDisplay == NULL){
        printk("qemudisp_initialize: PCIDeviceQemuDisplay\n");
        Status = (int) -1;
        goto fail;
    }
    if ( PCIDeviceQemuDisplay->used != TRUE || PCIDeviceQemuDisplay->magic != 1234 ){
        printk ("qemudisp_initialize: PCIDeviceQemuDisplay validation\n");
        Status = (int) -1;
        goto fail;
    }


    // #todo
    // Initialize cntroller.
    // ...

    return 0;
fail:
    return (int) -1;
}

