
// math.h 


#ifndef  _MATH_H
#define  _MATH_H  1

// hehe
#define GRAMADO_PI  3.14159265 

union __float_u {
	unsigned char __dummy[sizeof(float)];
	float __val;
};

union __double_u {
	unsigned char __dummy[sizeof(double)];
	double __val;
};

union __long_double_u {
	unsigned char __dummy[sizeof(long double)];
	long double __val;
};



/* The above constants are not adequate for computation using `long double's.
   Therefore we provide as an extension constants with similar names as a
   GNU extension.  Provide enough digits for the 128-bit IEEE quad.  */
#ifdef __USE_GNU
# define M_El		2.718281828459045235360287471352662498L /* e */
# define M_LOG2El	1.442695040888963407359924681001892137L /* log_2 e */
# define M_LOG10El	0.434294481903251827651128918916605082L /* log_10 e */
# define M_LN2l		0.693147180559945309417232121458176568L /* log_e 2 */
# define M_LN10l	2.302585092994045684017991454684364208L /* log_e 10 */
# define M_PIl		3.141592653589793238462643383279502884L /* pi */
# define M_PI_2l	1.570796326794896619231321691639751442L /* pi/2 */
# define M_PI_4l	0.785398163397448309615660845819875721L /* pi/4 */
# define M_1_PIl	0.318309886183790671537767526745028724L /* 1/pi */
# define M_2_PIl	0.636619772367581343075535053490057448L /* 2/pi */
# define M_2_SQRTPIl	1.128379167095512573896158903121545172L /* 2/sqrt(pi) */
# define M_SQRT2l	1.414213562373095048801688724209698079L /* sqrt(2) */
# define M_SQRT1_2l	0.707106781186547524400844362104849039L /* 1/sqrt(2) */
#endif

/*
 * XOPEN/SVID
 */
#if defined(_XOPEN_SOURCE) || defined(_NETBSD_SOURCE)
#define	M_E		2.7182818284590452354	/* e */
#define	M_LOG2E		1.4426950408889634074	/* log 2e */
#define	M_LOG10E	0.43429448190325182765	/* log 10e */
#define	M_LN2		0.69314718055994530942	/* log e2 */
#define	M_LN10		2.30258509299404568402	/* log e10 */
#define	M_PI		3.14159265358979323846	/* pi */
#define	M_PI_2		1.57079632679489661923	/* pi/2 */
#define	M_PI_4		0.78539816339744830962	/* pi/4 */
#define	M_1_PI		0.31830988618379067154	/* 1/pi */
#define	M_2_PI		0.63661977236758134308	/* 2/pi */
#define	M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
#define	M_SQRT2		1.41421356237309504880	/* sqrt(2) */
#define	M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */

#define	MAXFLOAT	((float)3.40282346638528860e+38)
//extern int signgam;
#endif /* _XOPEN_SOURCE || _NETBSD_SOURCE */


#ifndef __cplusplus
struct exception {
	int type;
	const char *name;
	double arg1;
	double arg2;
	double retval;
};
#endif


//v7unix style
//#define HUGE	1.701411733192644270e38
//#define LOGHUGE 39


/*
 * set X_TLOSS = pi*2**52, which is possibly defined in <values.h>
 * (one may replace the following line by "#include <values.h>")
 */

#define X_TLOSS    1.41484755040568800000e+16

#define DOMAIN     1
#define SING       2
#define OVERFLOW   3
#define UNDERFLOW  4
#define TLOSS      5
#define PLOSS      6

// sin, cos, tan
double sin(double __x);
double cos(double __x);
double tan(double __x);
// asin, acos, atan
double asin(double __x);
double acos(double __x);
double atan(double __x);
// floor, ceil
double floor(double __x);
double ceil(double __x);
// pow, sqrt
double pow(double __x, double __y);
double sqrt(double __x);

// -----------------------------
// extra
long power0(long x, unsigned int n);
long power1(long x, unsigned int n);
long power2(long x, unsigned int y);
float power3(float x, int y);
double power4(double x, int y);

//----------------
float sinf(float arg);
float cosf(float arg);
float tanf(float arg);



#endif    



