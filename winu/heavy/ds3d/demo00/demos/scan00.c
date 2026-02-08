// scan00.c
// Let's scan floating values.
// Let's scan floating values witha custom function created by Copilot.
// v 1.234 -5.678e-2 9.0123
// Created by Fred Nora.


#include "../gram3d.h"

// ==============================================================

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
/*
int scan00_custom_read_int(const char **strPtr)
{
    const char *s = *strPtr;

    // Skip whitespace
    while (*s && isspace((unsigned char)*s))
        s++;

    // Optional sign
    int sign = 1;
    if (*s == '-') { 
        sign = -1; 
        s++; }
    else if (*s == '+') { 
        s++; 
    }

    // Build integer
    long result = 0;
    while (*s && isdigit((unsigned char)*s)) 
    {
        result = result * 10 + (*s - '0');
        s++;
    }

    *strPtr = s;

    return (int)(sign * result);
}
*/

int scan00_custom_read_int(const char **strPtr)
{
    const char *s = *strPtr;

    // Skip whitespace
    while (*s && isspace((unsigned char)*s))
        s++;

    // Optional sign
    int sign = 1;
    if (*s == '-') { sign = -1; s++; }
    else if (*s == '+') { s++; }

    // Copy digits into a small buffer
    char buf[32];
    int len = 0;
    while (*s && isdigit((unsigned char)*s) && len < (int)(sizeof(buf)-1)) {
        buf[len++] = *s;
        s++;
    }
    buf[len] = '\0';

    // Convert with atoi
    int value = atoi(buf) * sign;

    // Advance pointer
    *strPtr = s;
    return value;
}


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

// Its real purpose is to parse a line containing a vertex definition (v x y z) and 
// return a 3D vector.
// Modified function: it now returns a pointer into the string after the newline.
const char *scan00_read_element_from_line(
    const char *line_ptr, 
    struct obj_element_d *elem )
{

// #bugbug
// The parser cant handle comments. It fails.
// Do not use comments for now!

// But there is still a small problem: 
// this code only skips the comment if the line 
// starts with # right after whitespace.


    // 'ptr' will traverse the string.
    const char *ptr = line_ptr;

    if ((void*) elem == NULL)
        return NULL;
    elem->initialized = FALSE;


    // Skip any whitespace before the identifier.
    while (*ptr && isspace((unsigned char)*ptr))
        ptr++;

    // If we reached the end already, return NULL.
    if (*ptr == '\0')
        return NULL;


// =======================================
// Skip comments and empty lines

/*
    while (*ptr && (isspace((unsigned char)*ptr) || *ptr == '#'))
    {
        if (*ptr == '#')
        {
            // Skip until end of line
            while (*ptr && *ptr != '\n')
                ptr++;
        }
        else
        {
            // Skip whitespace
            ptr++;
        }
    }

    if (*ptr == '\0' || *ptr == '\n')
    {
        // End of string or just newline → move to next line if possible
        if (*ptr == '\n') ptr++;
        return (*ptr != '\0') ? ptr : NULL;
    }
*/


// =======================================
// Handle comment lines ("#")
// When the line starts with comment
/*
    // #wrong
    if (*ptr == '#')
    {
        // Skip until end of line
        while (*ptr && *ptr != '\n')
            ptr++;
        if (*ptr == '\n')
            ptr++;
        if (*ptr == '\0')
            return NULL;

        // Comments are ignored, return pointer to next line
        return ptr;
    }
*/

// =======================================
// Handle vertex lines ("v")
    if (*ptr == 'v') 
    {
        // Skip the initial identifier ("v") if it's present.
        ptr++;

        // Skip any whitespace after the identifier.
        while (*ptr && isspace((unsigned char)*ptr))
            ptr++;

        // Fill the new structure instead of an external pointer
        elem->type = OBJ_ELEMENT_TYPE_VECTOR; 

        // Parse three floats and store them in the structure.
        elem->vertex.x = scan00_custom_read_float(&ptr);
        elem->vertex.y = scan00_custom_read_float(&ptr);
        elem->vertex.z = scan00_custom_read_float(&ptr);

        // Debug output: print the parsed coordinates.
        //printf("Parsed Vertex Coordinates:\n");
        //printf("x = %f\n", (double)return_v->x);
        //printf("y = %f\n", (double)return_v->y);
        //printf("z = %f\n", (double)return_v->z);

        // #test: default color
        elem->vertex.color = COLOR_WHITE;

        elem->initialized = TRUE;

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
    };

// =======================================
// Handle face lines ("f")
    if (*ptr == 'f') 
    {
        // Skip the initial identifier ("f") if it's present.
        ptr++;

        // Skip any whitespace after the identifier.
        while (*ptr && isspace((unsigned char)*ptr))
            ptr++;

        elem->type = OBJ_ELEMENT_TYPE_FACE;

        // Parse three integers
        elem->face.vi[0] = scan00_custom_read_int(&ptr);
        elem->face.vi[1] = scan00_custom_read_int(&ptr);
        elem->face.vi[2] = scan00_custom_read_int(&ptr);

        //printf ("f: %d %d %d \n",
            //elem->face.vi[0], elem->face.vi[1], elem->face.vi[2] );

        //while(1){}

        elem->initialized = TRUE;

        // Advance to end of line
        while (*ptr && *ptr != '\n') 
            ptr++;
        if (*ptr == '\n') 
            ptr++;
        if (*ptr == '\0') 
            return NULL;

        return ptr;
    }


    return NULL;
}

