// math.c
// It belongs to the library for the winu environment.

#include <math.h>

// See:
// https://man7.org/linux/man-pages/man3/log.3.html
double log(double x) 
{
    double ret;
    const double ln2 = 0.69314718055994530942; // ln(2)
    asm volatile (
        "fld1 \n"        // push 1.0
        "fldl %1 \n"     // push x
        "fyl2x \n"       // compute log2(x)
        "fldl %2 \n"     // push ln(2)
        "fmulp \n"       // multiply log2(x) * ln(2)
        : "=t"(ret)
        : "m"(x), "m"(ln2)
    );
    return ret;
}
float logf(float x){
    return (float)log((double)x);
}


double fabs(double x) 
{
    double ret;
    asm("fabs" : "=t"(ret) : "0"(x));

    return (double) ret;
}

float fabsf(float x){
    return (float) fabs((double)x);
}


// see:
// https://linux.die.net/man/3/fmin
double fmin(double x, double y) 
{
    return (double) (x < y ? x : y);
}
float fminf(float x, float y)
{
    return (float) (x < y ? x : y);
}

// see:
// https://linux.die.net/man/3/fmax
double fmax(double x, double y) 
{
    return (double) (x > y ? x : y);
}
float fmaxf(float x, float y)
{
    return (float) (x > y ? x : y);
}

// See:
// https://linux.die.net/man/3/modf
double modf(double x, double *iptr) 
{
    long long i = (long long)x;   // truncate
    *iptr = (double) i;

    return (double) (x - *iptr);
}

// See:
// https://linux.die.net/man/3/modf
float modff(float x, float *iptr) {
    int i = (int)x;   // truncate
    *iptr = (float)i;
    return x - *iptr;
}



//
// sin, cos, tan
//


double sin(double __x)
{
    double ret;
    asm("fsin" : "=t"(ret) : "0"(__x));
    return ret;
}
double cos(double __x)
{
    double ret;
    asm("fcos" : "=t"(ret) : "0"(__x));
    return ret;
}
double tan(double __x) {
    double ret;
    asm("fptan\n" : "=t"(ret) : "0"(__x) : "st(1)");
    return ret;
}

//------------------------------
//------------------------------

double tan00(double x)
{
	return (double) ( sin(x) / cos(x) );
}
double cot00(double x)
{
	return (double) ( cos(x) / sin(x) );
}
double cot01(double x)
{
	return (double)  ( 1 / tan00(x) );
}

//------------------------------

double sec00(double x)
{
    return (double) (1/cos(x));
}

double cossec00(double x)
{
    return (double) (1/sin(x));
}


//
// asin, acos, atan
//

// Returns the arcsine of x.
// Returns the arcsine of x using atan identity.
double asin(double __x)
{
    // asin(x) = atan(__x / sqrt(1 - __x*__x))
    return atan(__x / sqrt(1.0 - __x * __x));
}
// Returns the arccosine of x.
// Returns the arccosine of x using atan identity.
double acos(double __x)
{
    if (__x == 1.0) {
        return 0.0;  // arccos(1) = 0
    }
    if (__x == -1.0) {
        return GRAMADO_PI;  // arccos(-1) = π
    }

    return atan( sqrt(1.0 - __x * __x) / __x );
}
// Returns the arctangent of x.
double atan(double __x)
{
    double ret;
    asm("fld1\n" "fldl %1\n" "fpatan\n" : "=t"(ret) : "m"(__x));
    return ret;
}


// It returns the 
// integer value which is 
// less than or equal to given number.
// (rounds down the given number.)
double floor(double __x) 
{
    long long i = (long long)__x;
    return (__x < i) ? (double)(i - 1) : (double)i;
}

// It returns the 
// smallest integer value 
// greater than or equal to x. 
// (rounds up the given number.)
double ceil(double __x) 
{
    long long i = (long long) __x;
    return (__x > i) ? (double)(i + 1) : (double)i;
}

// #todo
// x to power of y
double pow(double __x, double __y)
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

// sqrt:
// Computes the square root of the source 
// value in the ST(0) register and stores the result in ST(0).
// OUT: Square root of __x
// Credits: Sirius OS.

double sqrt(double __x)
{
    double Value=0;

    asm volatile (
        "finit \n"
        "fldl %1 \n"  // st(0) => st(1), st(0) = x. FLDL means load double float
        "fsqrt \n"    // st(0) = square root st(0)
        : "=t"(Value) 
        : "m"(__x) );
//OUT:
    return (double) Value;
}

float sqrtf(float x)
{
// float x → promoted to double. No dirty values. hahaha
    return (float) sqrt((double)x);
}

 
// -----------------------------

// -----------------------------

// Credits:
// https://www.geeksforgeeks.org/write-a-c-program-to-calculate-powxn/
long power0(long x, unsigned int n)
{
// Iterator.
    register int i=0;
// Initialize result to 1
    long Pow=1;
// Multiply x for n times
    for (i=0; i<n; i++)
    {
        Pow = (long) (Pow * x);
    };
 
    return (long) Pow;
}

// Credits:
// https://www.geeksforgeeks.org/write-a-c-program-to-calculate-powxn/
long power1(long x, unsigned int n)
{
// If x^0 return 1
    if (n == 0){ return (long) 1; }
// If we need to find of 0^y
    if (x == 0){ return 0; }
// For all other cases.
    long p = (long) power1( (long) x, (unsigned int) n-1 ); 

    return (long) (x * p);
}

// Credits:
// https://www.geeksforgeeks.org/write-a-c-program-to-calculate-powxn/
long power2(long x, unsigned int y)
{
    long v1=0;
    long v2=0;
    long v3=0;

    if (x==0){
        return 0;
    }else if(y == 0){
        return (long) 1;
    }else if ((y%2) == 0){
        v1 = (long) power2( (long) x,  (long) (y>>1) );
        v2 = (long) power2( (long) x,  (long) (y>>1) );
        return (long) (v1*v2);
    }else{
        v1 = (long) x;
        v2 = (long) power2( (long) x,  (long) (y>>1) );
        v3 = (long) power2( (long) x,  (long) (y>>1) );
        
        return (long) (v1 * v2 * v3);
    };
}

// float and negative y.
// Credits:
// https://www.geeksforgeeks.org/write-a-c-program-to-calculate-powxn/
float power3(float x, int y)
{
    float temp=0;

    if (y == 0){
        return (float) 1.0f;
    }

    temp = (float) power3( (float) x, (float) (y>>1) );

    if ((y%2) == 0){
        return (float) (temp * temp);
    }else{

        if (y > 0){
            return (float) (x * temp * temp);
        }else{
            return (float) ((temp * temp) / x);
        };
    };
}


// #test
// double and negative y.
double power4(double x, int y)
{
    double temp=0;

    if (y == 0){
        return (double) 1;
    }

    temp = (double) power3( (double) x, (double) (y>>1) );

    if ( (y%2) == 0 ){
        return (double) (temp * temp);
    }else{
        if (y > 0){
            return (double) (x * temp * temp);
        }else{
            return (double) ((temp * temp) / x);
        };
    };
}


double pow00(double x, double y)
{
    return (double) pow(x,y);
}

// -------------------------------

double exp(double x)
{
    return (double) pow(GRAMADO_E, x);
}

// -------------------------------

// IN: angle
float sinf(float arg)
{
    float ret = 0.0f;
    asm(
        "fsin"
        : "=t"(ret)
        : "0"(arg) );

    return (float) ret;
}

// IN: angle
float cosf(float arg)
{
    float ret= 0.0f;
    asm(
        "fcos"
        : "=t"(ret)
        : "0"(arg));
    return (float) ret;
}

// IN: angle
float tanf(float arg)
{
    return (float) __builtin_tan(arg);
}

//------------------------------

//------------------------------

float tanf00(float x)
{
	return (float)  (sinf(x) / cosf(x) );
}
float cotf00(float x)
{
	return (float)  ( cosf(x) / sinf(x) );
}
float cotf01(float x)
{
	return (float)  ( 1 / tanf00(x) );
}


//------------------------------

float secf00(float x)
{
    return (float) (1/cosf(x));
}

float cossecf00(float x)
{
    return (float) (1/sinf(x));
}

// #test
float neutral_element_of_add(void)
{
    float ret = 0.0f;
    return (float) ret;
}

// #test
float neutral_element_of_mul(void)
{
    float ret = 1.0f;
    return (float) ret;
}

/*
 * double_ne - Compare two double-precision floating point numbers.
 *
 * Parameters:
 *   n1 - first double value
 *   n2 - second double value
 *
 * Returns:
 *   1 if the two numbers are NOT equal
 *   0 if the two numbers are equal
 *
 * Notes:
 *   In C, the '!=' operator returns 1 (true) if the values differ,
 *   and 0 (false) if they are equal. We explicitly return that result
 *   as an int to make the function portable and predictable.
 */
int double_ne(double n1, double n2) 
{
    return (int) (n1 != n2);
}

/*
 * isnan - Check if a double-precision floating point value is NaN.
 *
 * Parameters:
 *   n - the double value to test
 *
 * Returns:
 *   1 if the value is NaN (Not a Number)
 *   0 if the value is a valid number (including +inf or -inf)
 *
 * Notes:
 *   According to the IEEE 754 floating-point standard:
 *     - NaN is the ONLY value that is not equal to itself.
 *     - For all other values (finite numbers, +inf, -inf),
 *       the comparison (n == n) is true.
 *
 *   Therefore, we can detect NaN by checking if (n != n).
 *   If true, then 'n' must be NaN.
 */
int isnan(double n)
{
    return (int) (n != n);
}


