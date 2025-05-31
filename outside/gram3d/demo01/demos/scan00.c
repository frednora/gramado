// scan00.c
// Let's scan floating values.
// Let's scan floating values witha custom function created by Copilot.
// v 1.234 -5.678e-2 9.0123



#include "../gram3d.h"

// Local, for now.
// Declaration of our custom float parser.
double __pow0000(double __x, double __y);

// ==============================================================


double __pow0000(double __x, double __y)
{
    double RetValue = 0.0;
    asm volatile (
        "fyl2x;"
        "fld %%st;"
        "frndint;"
        "fsub %%st, %%st(1);"
        "fxch;"
        "fchs;"
        "f2xm1;"
        "fld1;"
        "faddp;"
        "fxch;"
        "fld1;"
        "fscale;"
        "fstp %%st(1);"
        "fmulp;" : "=t"(RetValue) : "0"(__x),"u"(__y) : "st(1)" );

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


int testing_model_scanner(void) 
{
    // Example vertex line from a 3D model (e.g., Wavefront OBJ format)

    // ok, this is working.
    //const char *line = "v 1.234 2.345 3.456";
    const char *line = "v 0.1 0.2 0.3";

    //const char *line = "v 1.234 -5.678e-2 9.0123";
    //const char *line = "v .123, v 123., v -0.0001e5";


    // Pointer to traverse the line.
    const char *ptr = line;
    
    // Skip the initial identifier ("v") and subsequent whitespace.
    if (*ptr == 'v') {
        ptr++;
    }
    while (*ptr && isspace((unsigned char)*ptr))
        ptr++;
    
    // Parse three floats for the vertex coordinates.
    float x = (float) scan00_custom_read_float(&ptr);
    float y = (float) scan00_custom_read_float(&ptr);
    float z = (float) scan00_custom_read_float(&ptr);

    // Output the parsed coordinates.
    printf("Parsed Vertex Coordinates:\n");
    printf("x = %f\n", (double)x);
    printf("y = %f\n", (double)y);
    printf("z = %f\n", (double)z);
    
    return 0;
}
