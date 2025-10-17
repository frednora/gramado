/* 
 * __drawrectangle0:
 *   Draws (fills) a rectangle on the backbuffer or frontbuffer.
 *   This function runs in Ring 0.
 *
 * Service 9.
 * #bugbug: Consider the limitation of 2MB mapped for both the LFB and backbuffer.
 *
 * IN:
 *   1 = backbuffer, 2 = frontbuffer.
 */
static void 
__drawrectangle0( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    unsigned long rop_flags,
    int back_or_front )
{
    // Local variables.
    unsigned long X      = x      & 0xFFFF;
    unsigned long Y      = y      & 0xFFFF;
    unsigned long Width  = width  & 0xFFFF; 
    unsigned long Height = height & 0xFFFF;
    unsigned int Color   = color;

    // Check for valid destination specifier.
    if (back_or_front != 1 && back_or_front != 2) {
        // Invalid argument: back_or_front must be 1 (backbuffer) or 2 (frontbuffer).
        return;
    }

    // Set the initial internal height.
    unsigned long internal_height = Height;

    // Prepare clipping rectangles.
    struct gws_rect_d Rect;
    struct gws_rect_d ClippingRect;
    int UseClipping = TRUE;

    unsigned long deviceWidth  = gws_get_device_width();
    unsigned long deviceHeight = gws_get_device_height();

    if (deviceWidth == 0 || deviceHeight == 0) {
        return;
    }

    // Create a clipping rectangle based on the device's dimensions.
    ClippingRect.left   = 0;
    ClippingRect.top    = 0;
    ClippingRect.width  = deviceWidth & 0xFFFF;
    ClippingRect.height = deviceHeight & 0xFFFF;
    ClippingRect.right  = ClippingRect.left + ClippingRect.width;
    ClippingRect.bottom = ClippingRect.top + ClippingRect.height;

    // Temporary hardcoded limits for debugging.
    if (ClippingRect.width > 800)  return;
    if (ClippingRect.height > 600) return;
    if (ClippingRect.right > 800)  return;
    if (ClippingRect.bottom > 600) return;

    // --- Set up the target rectangle ---
    Rect.bg_color = Color;
    Rect.x = 0;
    Rect.y = 0;
    Rect.width  = Width;
    Rect.height = Height;

    // Set rectangle position and compute its boundaries.
    Rect.left   = X;
    Rect.top    = Y;
    Rect.right  = Rect.left + Rect.width;
    Rect.bottom = Rect.top  + Rect.height; 

    // Adjust the rectangle using the clipping rectangle.
    if (Rect.left   < ClippingRect.left)   Rect.left   = ClippingRect.left;
    if (Rect.top    < ClippingRect.top)    Rect.top    = ClippingRect.top;
    if (Rect.right  > ClippingRect.right)  Rect.right  = ClippingRect.right;
    if (Rect.bottom > ClippingRect.bottom) Rect.bottom = ClippingRect.bottom;

    // Check internal height limits (for example, do not process if larger than expected).
    if (internal_height > 600) return;

    // --- Drawing Loop ---
    // Draw horizontal lines for each row in the rectangle.
    while (1) {
        // Draw on the appropriate surface.
        if (back_or_front == 1) {
            backbuffer_draw_horizontal_line(Rect.left, Y, Rect.right, Rect.bg_color, rop_flags);
        } else if (back_or_front == 2) {
            frontbuffer_draw_horizontal_line(Rect.left, Y, Rect.right, Rect.bg_color, rop_flags);
        }

        Y++;  // Move to the next line.

        // If use clipping, stop once the bottom of the clipping rectangle is reached.
        if (UseClipping == TRUE) {
            if (Y > ClippingRect.bottom) break;
        }

        // Decrement the internal height and exit if all lines have been processed.
        internal_height--;
        if (internal_height == 0) break;
    }

    // Optionally, mark the rectangle as dirty if needed.
    Rect.dirty = TRUE;
}
