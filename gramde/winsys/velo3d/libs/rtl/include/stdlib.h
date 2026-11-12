
// stdlib.h
// The stdlib header for 64bit usermode in Gramado OS.
// 2016 - Created by Fred Nora.
 
/*
   O que deve conter:

    atof string para float
    atoi	string para integer
    atol	string para long integer
    strtod	string para double
    strtol	string para long int
    strtoul	string para unsigned long int

	Geração de seqüência pseudo-aleatória
    rand	gera um número inteiro pseudo aleatório
    srand	seleciona a semente do gerador pseudo aleatório
    
	Alocação e liberação de memória
    malloc
    calloc
    realloc	aloca memória do "heap"
    free	libera memória de volta para o "heap"

	Controle de processos
    abort	força o término da execução
    atexit	registra uma função "callback" para saída do programa
    exit	termina a execução do programa
    getenv	obtém uma variável de ambiente
    system	executa um comando externo

	Ordenamento e procura
    bsearch	procura binária em "array"
    qsort	ordena "array" segundo algoritmo Quick Sort

	Matemática
    abs
    labs	valor absoluto
    div
    ldiv	divisão inteira
*/

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
	int quot;		/* quotient */
	int rem;		/* remainder */
} div_t;

typedef struct {
	long quot;		/* quotient */
	long rem;		/* remainder */
} ldiv_t;


#if !defined(_ANSI_SOURCE) && \
    (defined(_ISOC99_SOURCE) || (__STDC_VERSION__ - 0) >= 199901L || \
     (__cplusplus - 0) >= 201103L || defined(_NETBSD_SOURCE))
typedef struct {
	/* LONGLONG */
	long long int quot;	/* quotient */
	/* LONGLONG */
	long long int rem;	/* remainder */
} lldiv_t;
#endif

#if defined(_NETBSD_SOURCE)
typedef struct {
	quad_t quot;		/* quotient */
	quad_t rem;		    /* remainder */
} qdiv_t;
#endif


#define EXIT_FAILURE  1
#define EXIT_SUCCESS  0

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
char *nvmatch ( char *s1, char *s2 );
char *v7_getenv ( char *name );


//
// environ
//

char *getenv (const char *name);

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
char *mktemp (char *template);

//
// alloc
//

void *malloc (size_t size);
void *xmalloc ( size_t size );
void *xmemdup (void const *p, size_t s);
char *xstrdup(char const *string);
void *calloc (size_t count, size_t size);
void *xcalloc (size_t count, size_t size);
void *xzalloc (size_t n);
void *zmalloc ( size_t size );
void *realloc ( void *start, size_t newsize );

void *rtl_malloc ( size_t size );
void *rtl_calloc (size_t count, size_t size);

void free (void *ptr);

int rand(void);
void srand(unsigned int seed);
int random(void);
void srandom(unsigned int seed);

// #todo
int system (const char *command);

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


//rt support
//pegando informações sobre o heap usado pela biblioteca C99 em user mode.
unsigned long rtGetHeapStart(void);
unsigned long rtGetHeapEnd(void);
unsigned long rtGetHeapPointer(void);
unsigned long rtGetAvailableHeap(void);
//...


/*
 * libcInitRT:
 *     Inicializa o gerenciamento em user mode de memória virtual
 * para a biblioteca libC99.
 * Obs: *IMPORTANTE: Essa rotina deve ser chamada entes que a biblioteca C 
 * seja usada.
 * Obs: Pode haver uma chamada à ela em crt0.s por exemplo.
 */
int libcInitRT(void);

#endif    



//
// End
//
