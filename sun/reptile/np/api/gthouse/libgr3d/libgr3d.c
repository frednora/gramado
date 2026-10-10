

#include "include/libgr3d.h"

int libgr_dummy=0;


// Local, for now.
// Declaration of our custom float parser.
double __pow0000(double __x, double __y);


// Internal
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

// Skip any leading whitespace
    while ( isspace((unsigned char)*s) )
    {
        s++;
    };
    
// Process optional sign
    int sign = 1;
    if (*s == '-'){
        sign = -1;
        s++;
    } else if (*s == '+'){
        s++;
    };

    // Parse the integer part of the number.
    float result = 0.0f;
    //double result = 0.0;  // Use double for better accuracy

    while ( *s && isdigit((unsigned char)*s) )
    {
        result = result * 10.0f + (*s - '0');
        s++;
    };

// Parse the fractional part if a decimal point is present
    if (*s == '.')
    {
        s++;
        float fraction = 0.0f;
        float divisor = 10.0f;
        while (*s && isdigit((unsigned char)*s))
        {
            fraction += (*s - '0') / divisor;
            divisor *= 10.0f;
            s++;
        };
        result += fraction;
    }

// Parse exponential part if present
// 'e' or 'E'
    if (*s == 'e' || *s == 'E') 
    {
        s++;
        int expSign = 1;
        if (*s == '-') {
            expSign = -1;
            s++;
        } else if (*s == '+') {
            s++;
        }
        int exponent = 0;
        while (*s && isdigit((unsigned char)*s)) 
        {
            exponent = exponent * 10 + (*s - '0');
            s++;
        };
        //result *= pow(10, expSign * exponent);
        result *= __pow0000(10, expSign * exponent);
    }

// Update the pointer to reflect the new reading position
    *strPtr = s;
    
    return sign * result;
// #todo: return type.
    //return (float) (sign * result);
}

// Modified function: it now returns a pointer into the string after the newline.
const char *scan00_scanline(
    const char *line_ptr, 
    struct gr_vecF3D_d *return_v )
{
    // 'ptr' will traverse the string.
    const char *ptr = line_ptr;

// Skip any whitespace before the identifier
    while (*ptr && isspace((unsigned char)*ptr))
    {
        ptr++;
    };

// If we reached the end already, return NULL
    if (*ptr == '\0')
        return NULL;

// Skip the initial identifier ("v") if it's present
    if (*ptr == 'v') {
        ptr++;
    }

// Skip any whitespace after the identifier
    while (*ptr && isspace((unsigned char)*ptr))
    {
        ptr++;
    };

// Parse three floats and store them in the structure
    return_v->x = scan00_custom_read_float(&ptr);
    return_v->y = scan00_custom_read_float(&ptr);
    return_v->z = scan00_custom_read_float(&ptr);

    // Debug output: print the parsed coordinates.
    //printf("Parsed Vertex Coordinates:\n");
    //printf("x = %f\n", (double)return_v->x);
    //printf("y = %f\n", (double)return_v->y);
    //printf("z = %f\n", (double)return_v->z);

// Advance 'ptr' to the end of the current line
    while (*ptr && *ptr != '\n')
    {
        ptr++;
    };

// If a newline is found, move one character beyond it
    if (*ptr == '\n')
        ptr++;

// If we've reached the end of the string, return NULL
    if (*ptr == '\0')
        return NULL;

// Otherwise, return the pointer to the next line's start
    return ptr;
// #todo: Return type.
    //return (const char *) ptr;
}


void 
gr_MultiplyMatrixVector(
    struct gr_vecF3D_d *i, 
    struct gr_vecF3D_d *o, 
    struct gr_mat4x4_d *m )
{
    o->x = 
        (float) (
        i->x * m->m[0][0] + 
        i->y * m->m[1][0] + 
        i->z * m->m[2][0] + 
        m->m[3][0] );

    o->y = 
        (float) (
        i->x * m->m[0][1] + 
        i->y * m->m[1][1] + 
        i->z * m->m[2][1] + 
        m->m[3][1] );
    
    o->z = 
        (float) (
        i->x * m->m[0][2] + 
        i->y * m->m[1][2] + 
        i->z * m->m[2][2] + 
        m->m[3][2] );

    float w = 
        (float) (
        i->x * m->m[0][3] + 
        i->y * m->m[1][3] + 
        i->z * m->m[2][3] + 
        m->m[3][3] );

// Normalization.
    if (w != 0.0f)
    {
        o->x = (float) (o->x / w); 
        o->y = (float) (o->y / w); 
        o->z = (float) (o->z / w);
    }
}

struct gr_vecF3D_d *grVectorCrossProduct(
    struct gr_vecF3D_d *v1, 
    struct gr_vecF3D_d *v2 )
{
//#todo: Not tested yet.

    struct gr_vecF3D_d vRes;

    vRes.x = (float) (v1->y * v2->z - v1->z * v2->y);
    vRes.y = (float) (v1->z * v2->x - v1->x * v2->z);
    vRes.z = (float) (v1->x * v2->y - v1->y * v2->x);

    return (struct gr_vecF3D_d *) &vRes;
}

float dot_productF( struct gr_vecF3D_d *v1, struct gr_vecF3D_d *v2 )
{
// Dot product.
// The dot product describe the 
// relationship between two vectors.
// Positive: Same direction
// negative: Opposite direction
// 0:        Perpendicular.

// Fake perpendicular.
    if ( (void*) v1 == NULL ){ return (float) 0.0f; }
    if ( (void*) v2 == NULL ){ return (float) 0.0f; }
// (x*x + y*y + z*z)
    return (float) ( v1->x * v2->x + 
                     v1->y * v2->y + 
                     v1->z * v2->z );
}

// Get delta for bhaskara.
// d<0: (negative) "Raiz de número negativo em Baskara"
// d=0: (null)     duas raizes reais iguais.
// d>0: (positive) duas raizes reais diferentes. (Intersection)
float gr_discriminant(float a, float b, float c)
{
// Used to test for intesection in the ray tracing.
// Discriminant: Delta da função em bhaskara.
    float Discriminant = (float) ((b*b) - (4*a*c));
    return (float) Discriminant;
}


// ===================================
// Load a file.
// Function to read entire file into buffer using open()
// # Limited size. 512 bytes
char *libgr3dReadFileIntoBuffer(const char *filename) 
{
    size_t FakeFileSize=512;
    int fd;

    if ((void*) filename == NULL)
        return NULL;
    if (*filename == 0)
        return NULL;

// --------------------------
// Open
    fd = open(filename,0,"a+");
    if (fd < 0) {
        printf("File opening failed\n");
        return NULL;
    }

    // Get file size
    //off_t size = lseek(fd, 0, SEEK_END);
    //lseek(fd, 0, SEEK_SET); // Reset file position

    // Allocate buffer
    char *buffer = (char *) malloc(FakeFileSize);
    if (!buffer) 
    {
        printf("Memory allocation failed\n");
        //close(fd);
        return NULL;
    }

    // Read file content
    memset(buffer,0,FakeFileSize);

// --------------------------
// Read
    ssize_t bytesRead = (ssize_t) read(fd, buffer, FakeFileSize);
    buffer[bytesRead] = '\0'; // Null-terminate

    //close(fd); // Close file descriptor

// Return 
    return (char *) buffer;
}





