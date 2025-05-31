
// math.c
// The system has the support for double in ring3.
// #todo
// Remember: double is double precision float.


double sin(double __x)
{
    return 0;
}
double cos(double __x)
{
    return 0;
}
double tan(double __x)
{
    return 0;
}


// Returns the arcsine of x.
double asin(double __x)
{
    return 0;
}
// Returns the arccosine of x.
double acos(double __x)
{
    return 0;
}
// Returns the arctangent of x.
double atan(double __x)
{
    return 0;
}


// It returns the 
// integer value which is 
// less than or equal to given number.
// (rounds down the given number.)
double floor(double __x)
{
    return 0;
}

// It returns the 
// smallest integer value 
// greater than or equal to x. 
// (rounds up the given number.)
double ceil(double __x)
{
    return 0;
}


#include <stdio.h>

#define PRECISION 20  // Number of terms in the Taylor series

/**
 * exp - Computes e^number using Taylor series expansion.
 *
 * @number: The exponent to which e is raised.
 *
 * Returns: Approximate value of e^number.
 */
/*
double exp00(double number);
double exp00(double number) 
{
    double result = 1.0;  // First term (e^0 = 1)
    double term = 1.0;    // Current term in the series
    int i=0;

    for (i = 1; i < PRECISION; i++) {
        term *= number / i;  // Compute next term
        result += term;      // Add to result
    }

    return (double) result;
}
*/

// #todo
// x to power of y
double pow(double __x, double __y)
{
    return 0.0;

/*
    // Edge case: x^0 should return 1
    if (__y == 0) return 1.0;

    // Edge case: If base is zero and exponent is positive, return 0
    if (__x == 0) return 0.0;

    // If exponent is an integer, use iterative multiplication
    if ((int)__y == __y)  // Checking if y is an integer
    {
        double result = 1.0;
        int exponent = (int)__y;
        
        // Handle negative exponent
        int positive_exp = (exponent < 0) ? -exponent : exponent;
        for (int i = 0; i < positive_exp; i++)
        {
            result *= __x;
        }
        
        // If exponent was negative, return the reciprocal
        return (exponent < 0) ? (1.0 / result) : result;
    }
    
    // If exponent is fractional, use exp(y * log(x)) approach
    return exp(__y * log(__x));
*/
}


// fsqrt:
// Computes the square root of the source 
// value in the ST(0) register and stores the result in ST(0).
// OUT: Square root of __x
// Credits: Sirius OS.

double sqrt(double __x)
{
    double Value=0;

    asm volatile (
        "finit;"
        "fldl %1;"  // st(0) => st(1), st(0) = x. FLDL means load double float
        "fsqrt;"    // st(0) = square root st(0)
        : "=t"(Value) 
        : "m"(__x) );
//OUT:
    return (double) Value;
}

 
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
    }else if ( (y%2) == 0){
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
        return (float) 1;
    }

    temp = (float) power3( (float) x, (float) (y>>1) );

    if ( (y%2) == 0){
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

    if ( (y%2) == 0){
        return (double) (temp * temp);
    }else{

        if (y > 0){
            return (double) (x * temp * temp);
        }else{
            return (double) ((temp * temp) / x);
        };
    };
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

