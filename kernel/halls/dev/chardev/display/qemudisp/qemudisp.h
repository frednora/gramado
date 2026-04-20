// qemudisp.h
// Display controller support.
// Created by Fred Nora.

#ifndef __QEMUDISP_QEMUDISP_H
#define __QEMUDISP_QEMUDISP_H    1

// The info about the display device driver.
struct qemudisp_info_d
{
    int used;
	int magic;
	int initialized;

	//#todo
	// Version, revision, etc.
	// name, etc ...
};
extern struct qemudisp_info_d  qemudisp_info;

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

