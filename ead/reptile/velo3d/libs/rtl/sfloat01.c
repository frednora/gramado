// Let's scan floating values witha custom function created by Copilot.

// v 1.234 -5.678e-2 9.0123


#include <ctype.h>
#include <math.h>

#include <stdio.h>
#include <ctype.h>


// Declaration of our custom float parser.
float custom_read_float(const char **strPtr);


// =========================================

/**
 * custom_read_float - Parses a floating-point number from a string.
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
float custom_read_float(const char **strPtr) {
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
        result *= pow(10, expSign * exponent);
    }
    
    // Update the pointer to reflect the new reading position.
    *strPtr = s;
    
    return sign * result;
}


int main(void) {
    // Example vertex line from a 3D model (e.g., Wavefront OBJ format)
    const char *line = "v 1.234 -5.678e-2 9.0123";
    
    // Pointer to traverse the line.
    const char *ptr = line;
    
    // Skip the initial identifier ("v") and subsequent whitespace.
    if (*ptr == 'v') {
        ptr++;
    }
    while (*ptr && isspace((unsigned char)*ptr))
        ptr++;
    
    // Parse three floats for the vertex coordinates.
    float x = custom_read_float(&ptr);
    float y = custom_read_float(&ptr);
    float z = custom_read_float(&ptr);
    
    // Output the parsed coordinates.
    printf("Parsed Vertex Coordinates:\n");
    printf("x = %f\n", x);
    printf("y = %f\n", y);
    printf("z = %f\n", z);
    
    return 0;
}
