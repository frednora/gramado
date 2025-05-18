// bitblt.h
// Bit-block transfer.

#ifndef __RTL_BITBLT_H
#define __RTL_BITBLT_H    1

/**
 * BITBLT_OP_ERASE
 *
 * Erases the destination rectangle by filling it with a predetermined
 * background color. This essentially "clears" the area.
 */
#define BITBLT_OP_ERASE         0x00000001

/**
 * BITBLT_OP_COPY
 *
 * Directly copies the pixel data from the source rectangle into the
 * destination rectangle without any modification.
 */
#define BITBLT_OP_COPY          0x00000002

/**
 * BITBLT_OP_INVERT
 *
 * Copies the source rectangle into the destination while inverting the
 * pixel values (e.g., each pixel becomes ~pixel). This produces a negative image.
 */
#define BITBLT_OP_INVERT        0x00000004

/**
 * BITBLT_OP_TRANSCOPY
 *
 * Copies the source rectangle with a transparency check. Pixels matching a
 * designated "transparent" color are skipped, preserving the destination's pixel.
 */
#define BITBLT_OP_TRANSCOPY     0x00000008

/**
 * BITBLT_OP_FLIP_HORIZONTAL
 *
 * Mirrors the source rectangle horizontally (left-to-right reversal)
 * before applying the primary operation.
 */
#define BITBLT_OP_FLIP_HORIZONTAL 0x00000010

/**
 * BITBLT_OP_FLIP_VERTICAL
 *
 * Mirrors the source rectangle vertically (top-to-bottom reversal)
 * before applying the primary operation.
 */
#define BITBLT_OP_FLIP_VERTICAL   0x00000020

/**
 * BITBLT_OP_ROTATE_90
 *
 * Rotates the source rectangle 90 degrees clockwise before copying.
 */
#define BITBLT_OP_ROTATE_90       0x00000040

/**
 * BITBLT_OP_ROTATE_180
 *
 * Rotates the source rectangle 180 degrees before copying, which can be achieved
 * by flipping both horizontally and vertically.
 */
#define BITBLT_OP_ROTATE_180      0x00000080

/**
 * BITBLT_OP_ROTATE_270
 *
 * Rotates the source rectangle 270 degrees clockwise (or 90 degrees counterclockwise)
 * before copying.
 */
#define BITBLT_OP_ROTATE_270      0x00000100

/**
 * BITBLT_OP_ALPHA_BLEND
 *
 * Performs an alpha-blended copy, meaning the source and destination pixels are
 * blended based on per-pixel or constant alpha values.
 */
#define BITBLT_OP_ALPHA_BLEND     0x00000200

/**
 * BITBLT_OP_STRETCH
 *
 * Scales or stretches the source rectangle so it fits the destination rectangle,
 * which can be useful when the source size differs from the destination.
 */
#define BITBLT_OP_STRETCH         0x00000400

/**
 * BITBLT_OP_PATTERN_FILL
 *
 * Fills the destination rectangle with a pattern (or brush) instead of
 * a direct pixel-for-pixel copy. This is useful for tiling small bitmap patterns.
 */
#define BITBLT_OP_PATTERN_FILL    0x00000800

/**
 * BITBLT_OP_MASK
 *
 * Uses a mask (a binary pattern) to control which pixels in the source are
 * copied to the destination. Only pixels where the mask is “on” are updated.
 */
#define BITBLT_OP_MASK            0x00001000


//
// ========================================================
//


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


