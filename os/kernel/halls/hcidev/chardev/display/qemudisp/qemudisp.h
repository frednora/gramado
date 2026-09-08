// qemudisp.h
// Display controller support.
// Created by Fred Nora.

#ifndef __QEMUDISP_QEMUDISP_H
#define __QEMUDISP_QEMUDISP_H    1

#define QEMU_VGA_VENDOR_ID      0x1234
#define QEMU_VGA_DEVICE_ID      0x1111

// Classic I/O ports (always work with -vga std)
#define VBE_DISPI_IOPORT_INDEX  0x01CE
#define VBE_DISPI_IOPORT_DATA   0x01CF

// Register indexes
#define VBE_DISPI_INDEX_ID          0x0
#define VBE_DISPI_INDEX_XRES        0x1
#define VBE_DISPI_INDEX_YRES        0x2
#define VBE_DISPI_INDEX_BPP         0x3
#define VBE_DISPI_INDEX_ENABLE      0x4
#define VBE_DISPI_INDEX_BANK        0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH  0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT 0x7
#define VBE_DISPI_INDEX_X_OFFSET    0x8
#define VBE_DISPI_INDEX_Y_OFFSET    0x9
#define VBE_DISPI_INDEX_VIDEO_MEMORY_64K 0xA

// Enable flags
#define VBE_DISPI_DISABLED      0x00
#define VBE_DISPI_ENABLED       0x01
#define VBE_DISPI_GETCAPS       0x02
#define VBE_DISPI_LFB_ENABLED   0x40
#define VBE_DISPI_NOCLEARMEM     0x80

// Version IDs
#define VBE_DISPI_ID0   0xB0C0
#define VBE_DISPI_ID1   0xB0C1
#define VBE_DISPI_ID2   0xB0C2
#define VBE_DISPI_ID3   0xB0C3
#define VBE_DISPI_ID4   0xB0C4
#define VBE_DISPI_ID5   0xB0C5


//
// Driver state
//

struct qemudisp_d
{
    int used;
    int magic;
    int initialized;

    // From PCI (optional)
    int using_pci;
    unsigned char bus;
    unsigned char dev;
    unsigned char fun;

    // Current mode
    unsigned long width;
    unsigned long height;
    unsigned long bpp;
    unsigned long pitch;          // bytes per scanline
    unsigned long size_in_bytes;
    unsigned long size_in_kb;

    // Framebuffer
    unsigned long lfb_pa;
    unsigned long lfb_va;

    unsigned long mmio_pa;    // ← add this
    unsigned long mmio_va;    // later, when you map it

    // Max capabilities (from GETCAPS)
    unsigned long max_xres;
    unsigned long max_yres;
    unsigned long max_bpp;
};

extern struct qemudisp_d *qemudisp;


//
// PCI device
//

extern struct pci_device_d *PCIDeviceQemuDisplay;

//
// ======================================================
//

void qemudisp_write_reg(unsigned short index, unsigned short value);
unsigned short qemudisp_read_reg(unsigned short index);
int qemudisp_is_present(void);
int qemudisp_initialize(void);

int 
qemudisp_set_resolution(
    unsigned long width,
    unsigned long height,
    unsigned long bpp );

void 
qemudisp_get_resolution(
    unsigned long *width,
    unsigned long *height,
    unsigned long *bpp );

unsigned long qemudisp_get_pitch(void);
unsigned long qemudisp_get_lfb_pa(void);
unsigned long qemudisp_get_lfb_va(void);

void qemudisp_show_info(void);

void qemudisp_vsync(void);


int 
qemudisp_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg );

int DDINIT_qemudisp(void);

#endif  

