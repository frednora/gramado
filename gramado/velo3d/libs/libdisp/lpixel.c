
// lpixel.c
// long pixel
// 2023 - Created by Fred Nora

void 
put_data(
    unsigned long x,
    unsigned long y, 
    unsigned long bpp,    // Bytes per pixel.
    unsigned long pitch,  // Bytes per line.
    unsigned long address,
    unsigned long data )
{
// Just for fun. Not for profit.
// This is a work in progress.

    unsigned long  *a64 = (unsigned long *)  address;
    unsigned int   *a32 = (unsigned int *)   address;
    unsigned short *a16 = (unsigned short *) address;
    unsigned char  *a8  = (unsigned char *)  address;

    uint8_t   data8 = (uint8_t)  (data & 0xFF);
    uint16_t data16 = (uint16_t) (data & 0xFFFF);
    uint32_t data32 = (uint32_t) (data & 0xFFFFFF);
    uint64_t data64 = (uint64_t) data;

    if (address == 0)
        return;

// Offset
    address = (unsigned long) (address + (y*pitch));
    address = (unsigned long) (address + (x*bpp));

// Record
    if (bpp == 1){ *a8  = (uint8_t)  data8;  return; }
    if (bpp == 2){ *a16 = (uint16_t) data16; return; }
    if (bpp == 3){ *a32 = (uint32_t) data32; return; }
    if (bpp == 4){ *a64 = (uint64_t) data64; return; }
}

