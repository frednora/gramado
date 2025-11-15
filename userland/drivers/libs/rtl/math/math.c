// math.c
// The system has the support for double in ring3.
// Created by Fred Nora.

// #todo
// Remember: double is double precision float.

#include <math.h>


// See:
// https://man7.org/linux/man-pages/man3/log.3.html
double log(double x)
{
    return (double) 0.0;
}
float logf(float x)
{
    return (float) 0.0f;
}


double fabs(double x)
{
    return (double) 0.0;
}
float fabsf(float x)
{
    return (float) 0.0f;
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
    return (double) 0.0;
}

// See:
// https://linux.die.net/man/3/modf
float modff(float x, float *iptr)
{
    return (double) 0.0f;
}

//
// sin, cos, tan
//


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
	return (double) ( sin(__x) / cos(__x) );
}

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


// #todo
// x to power of y
double pow(double __x, double __y)
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

// sqrt:
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
