// qemudisp.c
// Display controller support.
// #todo: display device for qemu vm. (bochs)
// Created by Fred Nora.


#include <kernel.h>

struct pci_device_d *PCIDeviceQemuDisplay;
struct qemudisp_info_d  qemudisp_info;


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

int DDINIT_qemudisp(void)
{
    int Status = -1;

    PROGRESS("DDINIT_qemudisp:\n");

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

// #test
// Initialize structure
    qemudisp_info.used = TRUE;
    qemudisp_info.magic = 1234;
    qemudisp_info.initialized = TRUE;
    // ...

    return 0;

fail:
    return (int) -1;
}

