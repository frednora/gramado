// scan00.c
// Let's scan floating values.
// Let's scan floating values witha custom function created by Copilot.
// v 1.234 -5.678e-2 9.0123



#include "../gram3d.h"

// Local, for now.
// Declaration of our custom float parser.
double __pow0000(double __x, double __y);

// ==============================================================

// Internal
double __pow0000(double __x, double __y)
{
    double RetValue = 0.0;
    asm volatile (
        "fyl2x \n"
        "fld %%st \n"
        "frndint \n"
        "fsub %%st, %%st(1) \n"
        "fxch \n"
        "fchs \n"
        "f2xm1 \n"
        "fld1 \n"
        "faddp \n"
        "fxch \n"
        "fld1 \n"
        "fscale \n"
        "fstp %%st(1) \n"
        "fmulp \n" : "=t"(RetValue) : "0"(__x),"u"(__y) : "st(1)" );

    return (double) RetValue;
}

// =========================================

/**
 * scan00_custom_read_float - Parses a floating-point number from a string.
 *
 * @strPtr: A pointer to the string pointer that points to the number.
 *          The function will update this pointer to the position immediately
 *          following the parsed number.
 *
 * Returns: The floating-point number parsed from the string.
 *
 * This routine handles optional leading whitespace, an optional '+' or '-' sign,
 * the integral part followed by an optional fractional part (after a decimal point),
 * and an optional exponent prefixed with 'e' or 'E'.
 */

float scan00_custom_read_float(const char **strPtr) 
{
    const char *s = *strPtr;
    
    // Skip any leading whitespace.
    while (isspace((unsigned char)*s))
        s++;
    
    // Process optional sign.
    int sign = 1;
    if (*s == '-') {
        sign = -1;
        s++;
    } else if (*s == '+') {
        s++;
    }
    
    // Parse the integer part of the number.
    float result = 0.0f;
    //double result = 0.0;  // Use double for better accuracy

    while (*s && isdigit((unsigned char)*s)) {
        result = result * 10.0f + (*s - '0');
        s++;
    }
    
    // Parse the fractional part if a decimal point is present.
    if (*s == '.') {
        s++;
        float fraction = 0.0f;
        float divisor = 10.0f;
        while (*s && isdigit((unsigned char)*s)) {
            fraction += (*s - '0') / divisor;
            divisor *= 10.0f;
            s++;
        }
        result += fraction;
    }
    
    // Parse exponential part if present.
    if (*s == 'e' || *s == 'E') {
        s++;
        int expSign = 1;
        if (*s == '-') {
            expSign = -1;
            s++;
        } else if (*s == '+') {
            s++;
        }
        int exponent = 0;
        while (*s && isdigit((unsigned char)*s)) {
            exponent = exponent * 10 + (*s - '0');
            s++;
        }
        //result *= pow(10, expSign * exponent);
        result *= __pow0000(10, expSign * exponent);
    }

    // Update the pointer to reflect the new reading position.
    *strPtr = s;
    
    return sign * result;
}

// Modified function: it now returns a pointer into the string after the newline.
const char * scan00_read_vector_from_line(const char *line_ptr, struct gr_vecF3D_d *return_v)
{
    // 'ptr' will traverse the string.
    const char *ptr = line_ptr;

    // Skip any whitespace before the identifier.
    while (*ptr && isspace((unsigned char)*ptr))
        ptr++;

    // If we reached the end already, return NULL.
    if (*ptr == '\0')
        return NULL;

    // Skip the initial identifier ("v") if it's present.
    if (*ptr == 'v') {
        ptr++;
    }

    // Skip any whitespace after the identifier.
    while (*ptr && isspace((unsigned char)*ptr))
        ptr++;
    
    // Parse three floats and store them in the structure.
    return_v->x = scan00_custom_read_float(&ptr);
    return_v->y = scan00_custom_read_float(&ptr);
    return_v->z = scan00_custom_read_float(&ptr);

    // Debug output: print the parsed coordinates.
    //printf("Parsed Vertex Coordinates:\n");
    //printf("x = %f\n", (double)return_v->x);
    //printf("y = %f\n", (double)return_v->y);
    //printf("z = %f\n", (double)return_v->z);

    // Advance 'ptr' to the end of the current line.
    while (*ptr && *ptr != '\n')
        ptr++;

    // If a newline is found, move one character beyond it.
    if (*ptr == '\n')
        ptr++;

    // If we've reached the end of the string, return NULL.
    if (*ptr == '\0')
        return NULL;

    // Otherwise, return the pointer to the next line's start.
    return ptr;
}
