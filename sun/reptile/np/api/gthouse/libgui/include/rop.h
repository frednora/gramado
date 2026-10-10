#ifndef __ROP_H
#define __ROP_H 1

// Basic copy
#define ROP_COPY        0   // Direct copy of source into destination

// Bitwise operations
#define ROP_OR          1
#define ROP_AND         2
#define ROP_XOR         3
#define ROP_NAND        4

// Channel manipulation
#define ROP_LESS_RED    10
#define ROP_LESS_GREEN  11
#define ROP_LESS_BLUE   12

// Grayscale / masking
#define ROP_GRAY_MASK   20  // Force channels to 0x80 mask
#define ROP_REMOVE_RED  21  // Remove red channel
#define ROP_GRAY_LUMA   22  // Grayscale using luminosity weights

// Blending / effects
#define ROP_ALPHA       30  // Alpha blend (src over dst)
#define ROP_INVERT      31  // Invert destination channels
#define ROP_ADD         32  // Additive blend (dst + src, clamped)
#define ROP_MULTIPLY    33  // Multiply blend (darken/tint)

#define ROP_KEEP_DST   34  // Keep destination unchanged
#define ROP_HALF_SRC   35  // Source halved
#define ROP_HALF_DST   36  // Destination halved
#define ROP_MAX        37  // Max(src,dst)
#define ROP_MIN        38  // Min(src,dst)

#define ROP_NEGATE     39  // Negate (bitwise NOT of destination channels)
#define ROP_AVERAGE    40  // Average of src and dst
#define ROP_DIFF       41  // Absolute difference
#define ROP_BRIGHTEN   42  // Brighten destination



// Reserved ranges
// 40–49: brightness/contrast, gamma, etc.
// 50–59: special effects

#endif
