// bitblt.h
// Bit-block transfer.

#ifndef __RTL_BITBLT_H
#define __RTL_BITBLT_H    1

#define BITBLT_OP_ERASE  0
#define BITBLT_OP_COPY  1
//#define BITBLT_OP_COPY  2
// ...

int 
bitblt(
    struct gws_rect_d *dst_rect,
    struct gws_rect_d *src_rect,
    unsigned long new_rop,
    int op );

int 
backbuffer_bitblt(
    struct gws_rect_d *src_rect,
    unsigned long new_rop,
    int op,
    int show );

int 
frontbuffer_bitblt(
    struct gws_rect_d *src_rect,
    unsigned long new_rop,
    int op );


#endif    


