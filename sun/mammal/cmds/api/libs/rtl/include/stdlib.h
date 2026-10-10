// stdlib.h
// The stdlib header for 64bit usermode in Gramado OS.
// 2016 - Created by Fred Nora.

// #todo
// It needs to have:
// atof, atoi, atol,
// strtod, strtol, strtoul,
// rand, srand,
// malloc, calloc, realloc, free,
// abort, atexit, exit, getenv, system,
// bsearch, qsort,
// abs, labs, div, ldiv

#ifndef __RTL_STDLIB_H
#define __RTL_STDLIB_H    1

//
// Includes
//

#include <stddef.h>


#ifdef _BSD_SIZE_T_
typedef _BSD_SIZE_T_  size_t;
#undef _BSD_SIZE_T_
#endif

#if defined(_BSD_WCHAR_T_) && !defined(__cplusplus)
typedef _BSD_WCHAR_T_  wchar_t;
#undef _BSD_WCHAR_T_
#endif

typedef struct {
	int quot;		// quotient
	int rem;		// remainder
} div_t;

typedef struct {
	long quot;		// quotient
	long rem;		// remainder
} ldiv_t;


#if !defined(_ANSI_SOURCE) && \
    (defined(_ISOC99_SOURCE) || (__STDC_VERSION__ - 0) >= 199901L || \
     (__cplusplus - 0) >= 201103L || defined(_NETBSD_SOURCE))

// #todo: Review these types
typedef struct {
	long long int quot;  // quotient
	long long int rem;   // remainder
} lldiv_t;

#endif

#if defined(_NETBSD_SOURCE)
typedef struct {
	quad_t quot;  // quotient
	quad_t rem;   // remainder
} qdiv_t;
#endif


#define EXIT_SUCCESS  0
#define EXIT_FAILURE  1

// bsd-like
// #todo rand max for 64 bit?
// #define	RAND_MAX	0x7fffffff
#define RAND_MAX  32767 


// bsd stuff
// extern size_t __mb_cur_max;
// #define	MB_CUR_MAX	__mb_cur_max

void *stdlib_system_call ( 
    unsigned long ax, 
    unsigned long bx, 
    unsigned long cx, 
    unsigned long dx );

const char *getprogname(void);
void setprogname(const char *progname);

// pseudo-terminal support.
int posix_openpt (int flags);
int grantpt(int fd);
int unlockpt(int fd);
int getpt(void);

// The ptsname() function returns the name 
// of the slave pseudoterminal device 
// corresponding to the master referred to by fd.
char *ptsname (int fd);

// The ptsname_r() function is the reentrant equivalent of ptsname().
// It returns the name of the slave pseudoterminal device 
// as a null-terminated string in the buffer pointed to by buf. 
// The buflen argument specifies the number of bytes available in buf.
int ptsname_r (int fd, char *buf, size_t buflen);

int mkostemps(char *template, int suffixlen, int flags);

int mkstemps(char *template, int suffixlen);
int mkostemp(char *template, int flags);
int mkstemp(char *template);
long labs(long j);

int atoi(const char *str);
void itoa(int n, char s[]);

// unix v7 - like.
char *nvmatch(char *s1, char *s2);
char *v7_getenv(char *name);

//
// environ
//

char *getenv(const char *name);

int 
setenv(
    const char *name, 
    const char *value, 
    int overwrite );

int unsetenv (const char *name);
int clearenv(void);


// mktemp - make a unique temporary filename
// 4.3BSD, POSIX.1-2001.  
// POSIX.1-2008 removes the specification of mktemp().
char *mktemp(char *template);

void _Exit(int status); 

//
// alloc
//

void *malloc (size_t size);
void *xmalloc (size_t size);
void *xmemdup (void const *p, size_t s);
char *xstrdup(char const *string);
void *calloc (size_t count, size_t size);
void *xcalloc (size_t count, size_t size);
void *xzalloc (size_t n);
void *zmalloc (size_t size);
void *realloc (void *start, size_t newsize);

void *rtl_malloc (size_t size);
void *rtl_calloc (size_t count, size_t size);

void free (void *ptr);

int rand(void);
void srand(unsigned int seed);
int random(void);
void srandom(unsigned int seed);

int system(const char *command);

//
// failure routines
//

void abort(void);
void stdlib_die (char *str);

//=================================

long strtol(const char *nptr, char **endptr, int base);
unsigned long strtoul( const char *nptr, char **endptr, int base);

double strtod(const char *nptr, char **endptr);
float strtof(const char *str, char **endptr);
double atof(const char *str);


double my_strtod(const char *s, char **endptr);
int my_strtoi(const char *s);

//=================================

void *bsearch ( 
    const void *key, 
    const void *base, 
    size_t nmemb,
    size_t size,
    int (*compar)(const void *, const void *)
    );

void 
qsort (
    void *base, 
    size_t nmemb, 
    size_t size,
    int (*compar)(const void *, const void *)
    );

void 
qsort_r (
    void *base, 
    size_t nmemb, 
    size_t size,
    int (*compar)(const void *, const void *, void *),
    void *arg 
    );

int putenv(char *string);

int abs( int j);

// Heap support
unsigned long rtGetHeapStart(void);
unsigned long rtGetHeapEnd(void);
unsigned long rtGetHeapPointer(void);
unsigned long rtGetAvailableHeap(void);
//...


/*
 * libcInitRT:
 * rt initialization.
 * + Initialize the memory management for the ring 3 part.
 * + #ps: It needs to be called at the initialization.
 */

int libcInitRT(void);

#endif    

