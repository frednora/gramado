// bitblt.c
// Bit-block Transfer.
// Created by Fred Nora.

#include "../gwsint.h"


// see: 
// rect.c for methods with rectangles.
int 
bitblt(
    struct gws_rect_d *dst_rect,
    struct gws_rect_d *src_rect,
    unsigned long rop,
    int op )
{
    if ( (void*) dst_rect == NULL ){
        return -1;
    }
    if ( (void*) src_rect == NULL ){
        return -1;
    }

// Copy from src to dst.
    if (op == BITBLT_OP_COPY)
    {
        return TRUE;
    }

// ...

    return FALSE;
}

int 
backbuffer_bitblt(
    struct gws_rect_d *src_rect,
    unsigned long new_rop,
    int op,
    int show )
{
    struct gws_rect_d *r;
    r = src_rect;
    if ((void*) r == NULL )
        return -1;

    r->rop = new_rop;


// 0
    if (op == BITBLT_OP_ERASE)
    {
        r->bg_color = COLOR_BLACK;
        backbuffer_draw_rectangle( 
           r->left,
           r->top,
           r->width,
           r->height,
           r->bg_color,
           r->rop );
    }

// 1
    if (op == BITBLT_OP_COPY)
    {
        backbuffer_draw_rectangle( 
           r->left,
           r->top,
           r->width,
           r->height,
           r->bg_color,
           r->rop );
    }

    if (show)
        flush_rectangle(r);

    return 0;
}


int 
frontbuffer_bitblt(
    struct gws_rect_d *src_rect,
    unsigned long new_rop,
    int op )
{
    struct gws_rect_d *r;
    r = src_rect;
    if ((void*) r == NULL )
        return -1;

    r->rop = new_rop;

    if (op == BITBLT_OP_COPY)
    {
        frontbuffer_draw_rectangle( 
           r->left,
           r->top,
           r->width,
           r->height,
           r->bg_color,
           r->rop );
    }

    return 0;
}


