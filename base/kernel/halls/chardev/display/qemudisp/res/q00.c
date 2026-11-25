// qemudisp.c
// QEMU Standard VGA / Bochs Graphics Adapter (BGA) driver.
// Runtime resolution change using the Bochs DISPI interface.
// Compatible with QEMU -vga std / -device VGA.

#include <kernel.h>
#include "qemudisp.h"

// =========================================================
// Global driver instance
// =========================================================

struct qemudisp_d *qemudisp = NULL;

// =========================================================
// Low-level register access (I/O ports)
// =========================================================

void qemudisp_write_reg(unsigned short index, unsigned short value)
{
    out16(VBE_DISPI_IOPORT_INDEX, index);
    out16(VBE_DISPI_IOPORT_DATA,  value);
}

unsigned short qemudisp_read_reg(unsigned short index)
{
    out16(VBE_DISPI_IOPORT_INDEX, index);
    return (unsigned short) in16(VBE_DISPI_IOPORT_DATA);
}

// =========================================================
// Detection
// =========================================================

int qemudisp_is_present(void)
{
    unsigned short id = qemudisp_read_reg(VBE_DISPI_INDEX_ID);

    if (id >= VBE_DISPI_ID0 && id <= VBE_DISPI_ID5)
        return TRUE;

    return FALSE;
}

// =========================================================
// Initialization
// =========================================================

int qemudisp_initialize(void)
{
    PROGRESS("qemudisp_initialize:\n");

    //if (qemudisp != NULL && qemudisp->magic == 1234 && qemudisp->initialized)
        //return 0;   // already initialized

    // Allocate structure
    qemudisp = (struct qemudisp_d *) kmalloc(sizeof(struct qemudisp_d));
    if ((void *) qemudisp == NULL) {
        debug_print("qemudisp_initialize: kmalloc failed\n");
        return -1;
    }
    memset(qemudisp, 0, sizeof(struct qemudisp_d));

    qemudisp->used  = TRUE;
    qemudisp->magic = 1234;

    // Check if Bochs interface is present
    if (qemudisp_is_present() != TRUE) {
        debug_print("qemudisp_initialize: Bochs DISPI not present\n");
        qemudisp->initialized = FALSE;
        return -1;
    }

    // Read current mode (whatever the bootloader left us)
    qemudisp->width  = qemudisp_read_reg(VBE_DISPI_INDEX_XRES);
    qemudisp->height = qemudisp_read_reg(VBE_DISPI_INDEX_YRES);
    qemudisp->bpp    = qemudisp_read_reg(VBE_DISPI_INDEX_BPP);

    if (qemudisp->width == 0 || qemudisp->height == 0)
    {
        // Fallback to a safe default if somehow zero
        qemudisp->width  = 800;
        qemudisp->height = 600;
        qemudisp->bpp    = 32;
    }

    qemudisp->pitch = (qemudisp->bpp / 8) * qemudisp->width;
    qemudisp->size_in_bytes = qemudisp->height * qemudisp->pitch;
    qemudisp->size_in_kb    = qemudisp->size_in_bytes / 1024;

    // Try to get capabilities
    qemudisp_write_reg(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_GETCAPS);
    qemudisp->max_xres = qemudisp_read_reg(VBE_DISPI_INDEX_XRES);
    qemudisp->max_yres = qemudisp_read_reg(VBE_DISPI_INDEX_YRES);
    qemudisp->max_bpp  = qemudisp_read_reg(VBE_DISPI_INDEX_BPP);
    qemudisp_write_reg(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);

    // LFB address – we keep the one the bootloader already mapped
    // (most common case on Gramado)
    qemudisp->lfb_pa = gSavedLFB;          // from bootloader
    qemudisp->lfb_va = FRONTBUFFER_VA;     // your usual VA

    // Optional: try to find the PCI device (for future use)
    qemudisp->using_pci = FALSE;
    // You can enable this later if you want:
    /*
    struct pci_device_d *pci = pci_find_qemu_vga(); // you can implement this helper
    if (pci) {
        qemudisp->using_pci = TRUE;
        qemudisp->bus = pci->bus;
        qemudisp->dev = pci->dev;
        qemudisp->fun = pci->func;
    }
    */

    qemudisp->initialized = TRUE;

    debug_print("qemudisp_initialize: OK\n");
    return 0;
}

// =========================================================
// Set resolution (the main feature)
// =========================================================

int qemudisp_set_resolution(
    unsigned long width,
    unsigned long height,
    unsigned long bpp )
{
    if (qemudisp == NULL || qemudisp->magic != 1234 || !qemudisp->initialized)
        return -1;

    // Basic validation
    if (width == 0 || height == 0)
        return -1;

    // QEMU prefers width multiple of 8
    if ((width % 8) != 0)
        return -1;

    if (bpp != 8 && bpp != 15 && bpp != 16 && bpp != 24 && bpp != 32)
        return -1;

    // Optional safety against the reported maximums
    if (qemudisp->max_xres && width  > qemudisp->max_xres) return -1;
    if (qemudisp->max_yres && height > qemudisp->max_yres) return -1;
    if (qemudisp->max_bpp  && bpp    > qemudisp->max_bpp)  return -1;

    // 1. Disable
    qemudisp_write_reg(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);

    // 2. Program new mode
    qemudisp_write_reg(VBE_DISPI_INDEX_XRES,   (unsigned short) width);
    qemudisp_write_reg(VBE_DISPI_INDEX_YRES,   (unsigned short) height);
    qemudisp_write_reg(VBE_DISPI_INDEX_BPP,    (unsigned short) bpp);

    qemudisp_write_reg(VBE_DISPI_INDEX_VIRT_WIDTH,  (unsigned short) width);
    qemudisp_write_reg(VBE_DISPI_INDEX_VIRT_HEIGHT, (unsigned short) height);
    qemudisp_write_reg(VBE_DISPI_INDEX_X_OFFSET, 0);
    qemudisp_write_reg(VBE_DISPI_INDEX_Y_OFFSET, 0);

    // 3. Enable with Linear Framebuffer
    qemudisp_write_reg(VBE_DISPI_INDEX_ENABLE,
                       VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
    // You can also add | VBE_DISPI_NOCLEARMEM if you want to keep old content

    // -------------------------------------------------
    // Update driver state
    // -------------------------------------------------
    qemudisp->width  = width;
    qemudisp->height = height;
    qemudisp->bpp    = bpp;
    qemudisp->pitch  = (bpp / 8) * width;
    qemudisp->size_in_bytes = height * qemudisp->pitch;
    qemudisp->size_in_kb    = qemudisp->size_in_bytes / 1024;

    // -------------------------------------------------
    // Also update the classic global variables used by the rest of the kernel
    // (so bldisp / kgws / etc. stay consistent)
    // -------------------------------------------------
    gSavedX   = width;
    gSavedY   = height;
    gSavedBPP = bpp;

    // If you still use the old screen_* variables
    screenSetSize(width, height);
    // screen_bpp and screen_pitch are usually updated inside screenSetSize
    // or you can set them explicitly if needed.

    // Update the bootloader display device structure if it exists
    if (bl_display_device && bl_display_device->magic == 1234) {
        bl_display_device->framebuffer_width  = width;
        bl_display_device->framebuffer_height = height;
        bl_display_device->framebuffer_bpp    = bpp;
        bl_display_device->framebuffer_pitch  = qemudisp->pitch;
        bl_display_device->framebuffer_size_in_bytes = qemudisp->size_in_bytes;
        bl_display_device->framebuffer_size_in_kb    = qemudisp->size_in_kb;
    }

    // Tell the rest of the system the screen is dirty
    // invalidate_screen();

    return 0;
}

// =========================================================
// Query helpers
// =========================================================

void qemudisp_get_resolution(
    unsigned long *width,
    unsigned long *height,
    unsigned long *bpp )
{
    if (qemudisp == NULL || qemudisp->magic != 1234)
        return;

    if (width)  *width  = qemudisp->width;
    if (height) *height = qemudisp->height;
    if (bpp)    *bpp    = qemudisp->bpp;
}

unsigned long qemudisp_get_pitch(void)
{
    if (qemudisp == NULL || qemudisp->magic != 1234)
        return 0;
    return qemudisp->pitch;
}

unsigned long qemudisp_get_lfb_pa(void)
{
    if (qemudisp == NULL || qemudisp->magic != 1234)
        return 0;
    return qemudisp->lfb_pa;
}

unsigned long qemudisp_get_lfb_va(void)
{
    if (qemudisp == NULL || qemudisp->magic != 1234)
        return 0;
    return qemudisp->lfb_va;
}

// =========================================================
// Debug / info
// =========================================================

void qemudisp_show_info(void)
{
    if (qemudisp == NULL || qemudisp->magic != 1234 || !qemudisp->initialized) {
        printk("qemudisp: not initialized\n");
        return;
    }

    printk("QEMU Display (Bochs DISPI)\n");
    printk("  Current : %dx%d %dbpp\n",
           qemudisp->width, qemudisp->height, qemudisp->bpp);
    printk("  Pitch   : %d bytes\n", qemudisp->pitch);
    printk("  Size    : %d KB\n", qemudisp->size_in_kb);
    printk("  Max     : %dx%d %dbpp\n",
           qemudisp->max_xres, qemudisp->max_yres, qemudisp->max_bpp);
    printk("  LFB PA  : %x\n", qemudisp->lfb_pa);
    printk("  LFB VA  : %x\n", qemudisp->lfb_va);
}

// =========================================================
// Optional: simple vsync (classic VGA style)
// =========================================================
void qemudisp_vsync(void)
{
    // Wait for vertical retrace
    while ( (in8(0x3DA) & 0x08) );
    while (!(in8(0x3DA) & 0x08) );
}

// =========================================================
// Optional: clear the entire framebuffer
// =========================================================
void qemudisp_clear(unsigned int color)
{
    if (qemudisp == NULL || qemudisp->magic != 1234)
        return;

    unsigned int *fb = (unsigned int *) qemudisp->lfb_va;
    unsigned long total_pixels = qemudisp->width * qemudisp->height;

    // Only works well for 32 bpp
    if (qemudisp->bpp == 32) {
        for (unsigned long i = 0; i < total_pixels; i++)
            fb[i] = color;
    }
}

// =========================================================
// Optional: find the PCI device (if you want to use it later)
// =========================================================
struct pci_device_d *qemudisp_find_pci_device(void)
{
    int i;
    struct pci_device_d *dev;

    for (i = 0; i < PCI_DEVICE_LIST_SIZE; i++) {
        dev = (struct pci_device_d *) pcideviceList[i];
        if (dev == NULL)
            continue;
        if (dev->magic != 1234)
            continue;

        if (dev->Vendor == QEMU_VGA_VENDOR_ID &&
            dev->Device == QEMU_VGA_DEVICE_ID)
        {
            return dev;
        }
    }
    return NULL;
}

