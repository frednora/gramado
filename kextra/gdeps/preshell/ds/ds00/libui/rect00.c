//

Below is an example of a new worker function written for the display server that supports independent offsets in the destination and source buffers. In other words, you can copy a rectangle from one part of the source surface to a different location on the destination surface. This code preserves much of your previous work (using our fast‐path memcpy routines when possible) and adapts it by introducing separate coordinate parameters for source and destination.

Feel free to adjust variable names or error handling to suit your style and requirements.


/**
 * __refresh_rectangle_with_offsets:
 *
 * Copies a rectangular region from one surface (source) to another (destination)
 * while supporting independent offsets for each. This worker is meant to be used
 * in the display server (Ring 3) and can serve as the low-level routine for the
 * higher level BitBlt function.
 *
 * Parameters:
 *   dest_x           - X coordinate in the destination surface where the rectangle begins.
 *   dest_y           - Y coordinate in the destination surface where the rectangle begins.
 *   src_x            - X coordinate in the source surface where the rectangle begins.
 *   src_y            - Y coordinate in the source surface where the rectangle begins.
 *   width            - Width (in pixels) of the rectangle to copy.
 *   height           - Height (in pixels) of the rectangle to copy.
 *   dst_surface_base - Base address of the destination surface (e.g., frontbuffer).
 *   src_surface_base - Base address of the source surface (e.g., backbuffer).
 *
 * Notes:
 *   - This routine expects that both surfaces use the same pitch and bytes-per-pixel.
 *   - Clipping is done against the device’s screen height.
 *   - For performance, if the number of bytes per row (rectangle_pitch) is divisible
 *     by 4, the __rect_memcpy32 routine is used; otherwise, a regular memcpy is done.
 */
static void __refresh_rectangle_with_offsets(
    unsigned long dest_x,
    unsigned long dest_y,
    unsigned long src_x,
    unsigned long src_y,
    unsigned long width,
    unsigned long height,
    unsigned long dst_surface_base,
    unsigned long src_surface_base )
{
    // Variables for pointers to the surfaces.
    void       *dest;
    const void *src;

    // Local loop and count variables.
    unsigned int i = 0;
    int count = 0;
    unsigned int lines = (unsigned int) (height & 0xFFFF);       // Number of lines (rows)
    unsigned int rectangle_pitch = 0;  // Number of bytes per row in the rectangle
    unsigned int dest_offset = 0;
    unsigned int src_offset  = 0;

    // Get display device dimensions.
    unsigned long screenWidth  = gws_get_device_width();
    unsigned long screenHeight = gws_get_device_height();
    if (screenWidth == 0 || screenHeight == 0) {
        debug_print("__refresh_rectangle_with_offsets: [ERROR] Invalid device dimensions\n");
        exit(1);
    }
    // Limit screens to 16 bits for safety.
    screenWidth  &= 0xFFFF;
    screenHeight &= 0xFFFF;

    // Validate basic input.
    if ((width  & 0xFFFF) == 0)  return;
    if (lines == 0)              return;
    if (dest_y >= screenHeight)  return;
    if (lines > (screenHeight - dest_y)) return; 

    // Determine bytes per pixel using the global SavedBPP.
    int bytes_count = 0;
    switch (SavedBPP) {
        case 32:
            bytes_count = 4;
            break;
        case 24:
            bytes_count = 3;
            break;
        default:
            debug_print("__refresh_rectangle_with_offsets: [ERROR] Unsupported SavedBPP\n");
            exit(1);
            break;
    };

    // Calculate the screen pitch:
    // screenPitch = (screen width in pixels) * (bytes per pixel)
    unsigned int screen_pitch = bytes_count * screenWidth;

    // The byte width of the rectangle.
    rectangle_pitch = bytes_count * (width & 0xFFFF);

    // Compute offsets in each surface independently.
    // Destination offset based on dest_x, dest_y.
    dest_offset = (dest_y * screen_pitch) + (dest_x * bytes_count);
    // Source offset based on src_x, src_y.
    src_offset = (src_y * screen_pitch) + (src_x * bytes_count);

    // Adjust the base pointers for both surfaces.
    dest = (void *) ((unsigned char *) dst_surface_base + dest_offset);
    src  = (const void *) ((unsigned char *) src_surface_base  + src_offset);

    // If the source and destination areas are the same in memory, nothing to do.
    if (dest == src) {
        return;
    }

    // --- Copy Optimization Routines ---

    // If the rectangle's width in bytes is divisible by 4, use __rect_memcpy32.
    if ((rectangle_pitch % 4) == 0) {
        count = rectangle_pitch / 4;
        for (i = 0; i < lines; i++) {
            __rect_memcpy32(dest, src, count);
            // Advance pointers by one screen pitch (in bytes) per row.
            dest = (void *) ((unsigned char *) dest + screen_pitch);
            src  = (const void *) ((unsigned char *) src  + screen_pitch);
        }
    } else {
        // Otherwise, use the regular memcpy per line.
        for (i = 0; i < lines; i++) {
            memcpy(dest, src, rectangle_pitch);
            dest = (void *) ((unsigned char *) dest + screen_pitch);
            src  = (const void *) ((unsigned char *) src  + screen_pitch);
        }
    }
}


===============================================


How This Worker Works

Separate Offsets: Two sets of coordinates are passed—for the destination (dest_x, dest_y) and the source (src_x, src_y). Two separate offsets are computed by multiplying the y-coordinate by the screen’s pitch (which is calculated using the screen width and bytes per pixel) and adding the product of the x-coordinate and the bytes-per-pixel.

Pointer Adjustment: The base pointers for both the destination and source surfaces are advanced by their respective calculated offsets. Each subsequent row is then processed by adding the entire screen pitch to the pointer.

Optimized Row Copy: If the number of bytes per row in the rectangle (called rectangle_pitch) is divisible by 4, the code uses a fast, specialized copy function (__rect_memcpy32). Otherwise, it falls back to the standard memcpy.

Clipping: Simple clipping checks ensure that the copy does not run past the screen’s dimensions. You might later expand these checks to validate the source surface boundaries as well.

Integration with BitBlt
When you integrate this worker into your higher-level BitBlt function, you can pass different source and destination offsets (extracted from your bitblt_rectangles or gws_rect_d structures). This worker now allows you to move image fragments from arbitrary locations in the source buffer to arbitrary locations in the destination buffer, which is exactly what BitBlt needs.




