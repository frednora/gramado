// qemudisp.h
// Display controller support.
// Created by Fred Nora.

#ifndef __QEMUDISP_QEMUDISP_H
#define __QEMUDISP_QEMUDISP_H    1



extern struct pci_device_d *PCIDeviceQemuDisplay;

//
// ======================================================
//

int 
qemudisp_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg );

int DDINIT_qemudisp(void);

#endif  

