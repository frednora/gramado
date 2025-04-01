// qemudisp.h
// Display controller support.

#ifndef __QEMUDISP_H
#define __QEMUDISP_H    1

extern struct pci_device_d *PCIDeviceQemuDisplay;

//
// ======================================================
//

int 
qemudisp_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg );

int qemudisp_initialize(void);

#endif  

