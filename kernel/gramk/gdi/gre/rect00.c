Creating new workers.


=======================================================================


/**
 * __refresh_rectangle2
 *
 * Copies a rectangular area from one buffer to another, allowing the source
 * and destination positions to be specified independently.
 *
 * Parameters:
 *   dest_x      - X coordinate in the destination buffer where the rectangle begins.
 *   dest_y      - Y coordinate in the destination buffer where the rectangle begins.
 *   src_x       - X coordinate in the source buffer from which to copy.
 *   src_y       - Y coordinate in the source buffer from which to copy.
 *   width       - Width (in pixels) of the rectangle to copy.
 *   height      - Height (in pixels) of the rectangle to copy.
 *   buffer_dest - The starting address of the destination buffer.
 *   buffer_src  - The starting address of the source buffer.
 *
 * Notes:
 *   - Performs clipping against the device height.
 *   - Uses specialized memcpy routines when possible (8 or 4 bytes at a time)
 *     for strength reduction.
 *   - Relies on a global variable 'gSavedBPP' to determine bytes per pixel.
 */
static void __refresh_rectangle2(
    unsigned long dest_x,
    unsigned long dest_y,
    unsigned long src_x,
    unsigned long src_y,
    unsigned long width,
    unsigned long height,
    unsigned long buffer_dest,
    unsigned long buffer_src )
{
    // Declare pointers for destination and source.
    void *dest;
    const void *src;

    // Device (screen) dimensions.
    unsigned int deviceWidth, deviceHeight;
    
    // Strides, pitches, and offset calculations.
    unsigned int screen_pitch;
    unsigned int rectangle_pitch;
    unsigned int dest_offset, src_offset;

    // Other variables.
    int bytes_count;
    unsigned int lines;
    unsigned int i;
    int count;
    int UseClipping = 1;  // TRUE.
    int FirstLine = (int) dest_y;  // First line in the destination for clipping.

    // Retrieve device dimensions.
    deviceWidth = (unsigned int) screenGetWidth();
    deviceHeight = (unsigned int) screenGetHeight();
    if ( deviceWidth == 0 || deviceHeight == 0 ) {
        debug_print ("__refresh_rectangle2: invalid device dimensions\n");
        panic       ("__refresh_rectangle2: invalid device dimensions\n");
    }

    // Validate the rectangle dimensions.
    unsigned int hl_width = (unsigned int) width;
    lines = (unsigned int) height;
    if (hl_width == 0 || lines == 0)
        return;

    /*
     * Determine the bytes-per-pixel count.
     * gSavedBPP should be defined globally:
     *   - 32 bpp uses 4 bytes per pixel.
     *   - 24 bpp uses 3 bytes per pixel.
     */
    switch (gSavedBPP){
        case 32:
            bytes_count = 4;
            break;
        case 24:
            bytes_count = 3;
            break;
        default:
            panic("__refresh_rectangle2: gSavedBPP unsupported\n");
            break;
    };

    // Calculate the screen pitch (bytes per full line).
    screen_pitch = bytes_count * deviceWidth;
    // Calculate the pitch for our rectangle.
    rectangle_pitch = bytes_count * hl_width;

    // Calculate starting offsets in each buffer.
    // Destination offset is based on (dest_y, dest_x)
    dest_offset = (dest_y * screen_pitch) + (dest_x * bytes_count);
    // Source offset is based on (src_y, src_x)
    src_offset  = (src_y * screen_pitch) + (src_x * bytes_count);

    // Set the initial pointer positions.
    dest = (void *) ((unsigned char *) buffer_dest + dest_offset);
    src  = (const void *) ((unsigned char *) buffer_src  + src_offset);

    // If the source and destination are the same, nothing to do.
    if (dest == src)
        return;

    /*
     * Try to optimize by copying multiple bytes at a time.
     * If the rectangle (line) width is divisible by 8, use memcpy64.
     */
    if ((rectangle_pitch % 8) == 0) {
        count = rectangle_pitch >> 3; // divide by 8
        for (i = 0; i < lines; i++) {
            if (UseClipping) {
                if ((FirstLine + i) > deviceHeight)
                    break;
            }
            memcpy64(dest, src, count);
            // Move to the next line in each buffer.
            dest = (void *) ((unsigned char *) dest + screen_pitch);
            src  = (const void *) ((unsigned char *) src  + screen_pitch);
        }
        return;
    }

    // If not, check if the rectangle width is divisible by 4 and use memcpy32.
    if ((rectangle_pitch % 4) == 0) {
        count = rectangle_pitch >> 2; // divide by 4
        for (i = 0; i < lines; i++) {
            if (UseClipping) {
                if ((FirstLine + i) > deviceHeight)
                    break;
            }
            memcpy32(dest, src, count);
            dest = (void *) ((unsigned char *) dest + screen_pitch);
            src  = (const void *) ((unsigned char *) src  + screen_pitch);
        }
        return;
    }

    // Fallback: use standard memcpy, which copies byte-by-byte.
    for (i = 0; i < lines; i++) {
        if (UseClipping) {
            if ((FirstLine + i) > deviceHeight)
                break;
        }
        memcpy(dest, src, rectangle_pitch);
        dest = (void *) ((unsigned char *) dest + screen_pitch);
        src  = (const void *) ((unsigned char *) src  + screen_pitch);
    }
}

=======================================================================

#include <string.h> // For memcpy

/**
 * copy_rect_with_offset
 *
 * Copies a rectangular block of pixels from a source buffer with a specified offset
 * into a destination buffer with a different offset.
 *
 * Parameters:
 *   dest      - Pointer to the beginning of the destination buffer.
 *   destWidth - The total width (in pixels) of the destination buffer.
 *   destX     - The X coordinate in the destination buffer where the copy will start.
 *   destY     - The Y coordinate in the destination buffer where the copy will start.
 *
 *   src       - Pointer to the beginning of the source buffer.
 *   srcWidth  - The total width (in pixels) of the source buffer.
 *   srcX      - The X coordinate in the source buffer where the rectangle is located.
 *   srcY      - The Y coordinate in the source buffer where the rectangle is located.
 *
 *   rectWidth  - The width (in pixels) of the rectangle to copy.
 *   rectHeight - The height (in pixels) of the rectangle to copy.
 *
 * Note:
 *   - This function assumes that the source and destination buffers store pixels
 *     in row-major order.
 *   - The function uses memcpy on a per-scanline basis.
 */
void copy_rect_with_offset(
    unsigned char *dest, int destWidth, int destX, int destY,
    unsigned char *src,  int srcWidth,  int srcX,  int srcY,
    int rectWidth, int rectHeight)
{
    for (int y = 0; y < rectHeight; y++) {
        // Calculate the offset for the current row in the destination.
        unsigned char *destRow = dest + ((destY + y) * destWidth + destX);
        // Calculate the offset for the current row in the source.
        unsigned char *srcRow  = src + ((srcY + y) * srcWidth + srcX);

        // Copy the row from the source to the destination
        memcpy(destRow, srcRow, rectWidth);
    }
}


void copy_rect_with_offset_32bpp(
    unsigned char *dest, int destWidth, int destX, int destY,
    unsigned char *src,  int srcWidth,  int srcX,  int srcY,
    int rectWidth, int rectHeight)
{
    // Multiply widths and x coordinates by 4 since each pixel is 4 bytes.
    int destStride = destWidth * 4;
    int srcStride  = srcWidth * 4;
    int copyBytes = rectWidth * 4;

    for (int y = 0; y < rectHeight; y++) {
        unsigned char *destRow = dest + ((destY + y) * destStride + destX * 4);
        unsigned char *srcRow  = src + ((srcY + y) * srcStride + srcX * 4);
        memcpy(destRow, srcRow, copyBytes);
    }
}


