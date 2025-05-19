// bitblt.c
// Bit-block Transfer.
// Created by Fred Nora.

#include "../gwsint.h"


// -----------------------------------------------------------------------
// BitBlt Function
// -----------------------------------------------------------------------
//
// Copies from one rectangle of a source surface to a rectangle of a destination
// surface. The source and destination rectangles are described by pointers
// to gws_rect_d structures.
//
// It uses the following important fields from the rectangles:
//   - left, top, width, and height.
//
// The rop and op parameters allow you to supply additional raster and
// extra operation flags; for now, this example simply performs a direct copy.
// In the future you can extend the behavior based on these flags.
//
// see: 
// rect.c for methods with rectangles.
// #important
// The copy area is the width and height of the destine.
int 
bitblt00(
    struct gws_rect_d *dst_rect,
    struct gws_rect_d *src_rect,
    unsigned long dst_surface_base,  // Destination buffer base address
    unsigned long src_surface_base,  // Source buffer base address
    unsigned long rop,               // Raster operation flags (e.g., SRCCOPY)
    int op )                        // Additional operation flags (e.g., flip, alpha blend)
{
    unsigned long __dst_surface_base = (unsigned long) dst_surface_base;
    unsigned long __src_surface_base = (unsigned long) src_surface_base;

    // Validate the input rectangles.
    if (dst_rect == NULL || src_rect == NULL)
       return -1;

    // Extract the four primary elements from each rectangle.
    unsigned long dst_left   = dst_rect->left   & 0xFFFF;
    unsigned long dst_top    = dst_rect->top    & 0xFFFF;
    unsigned long rect_width = dst_rect->width  & 0xFFFF;
    unsigned long rect_height= dst_rect->height & 0xFFFF;

    unsigned long src_left   = src_rect->left   & 0xFFFF;
    unsigned long src_top    = src_rect->top    & 0xFFFF;
    // You might also consider validating that the source rectangle’s width/height
    // match those of the destination. It depends on your application logic.

    //
    // Here you can optionally process the rop and op flags.
    // For example, if your raster operation is not a straight copy,
    // you may perform a different routine or adjust an intermediate buffer.
    //
    // For now, we assume a simple, direct copying (SRCCOPY-like).
    //

    // In our design, assume that the source data comes from the backbuffer,
    // and we copy it to the frontbuffer.
    __refresh_rectangle1( 
        rect_width, rect_height,
        dst_left, dst_top, __dst_surface_base,
        src_left, src_top, __src_surface_base );

    // Optionally, if you later wish to process any op flags (like flipping or alpha blending),
    // you can add that logic here—either before or after the copy.
    return 0;
}

int 
bitblt01(
    struct dc_d *dc_dst,   // Handle to the destination device context.
    unsigned long dst_l,   // X-coordinate of the upper-left corner of the destination rectangle.
    unsigned long dst_t,   // Y-coordinate of the upper-left corner of the destination rectangle.
    struct dc_d *dc_src,   // Handle to the source device context.
    unsigned long src_l,   // X-coordinate of the upper-left corner of the source rectangle.
    unsigned long src_t,   // Y-coordinate of the upper-left corner of the source rectangle.
    unsigned long width,   // Width of the source and destination rectangles.
    unsigned long height,  // Height of the source and destination rectangles.
    unsigned long rop )    // Raster operation code (defines how pixels are combined).
{
    return (int) -1;
}

