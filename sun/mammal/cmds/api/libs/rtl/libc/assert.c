
// assert.c
// Credits: Serenity OS

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#ifdef DEBUG

void 
__assertion_failed ( 
    const char* msg, 
    const char* file, 
    unsigned line, 
    const char* func )
{
    printf ("USERSPACE(%d) ASSERTION FAILED: ", getpid() );
    printf (" %s \n %s:%u in %s \n",  msg, file, line, func );
    
    stdlib_die ( "__assertion_failed: *hang");
    // Suspended.
    // fprintf (stderr, "ASSERTION FAILED: %s\n%s:%u in %s\n", msg, file, line, func);
    //abort ();
    //for (;;)
}

#endif


/*
static int
fmtassert(char *buf, size_t len, const char *file, int line,
    const char *function, const char *failedexpr);
static int
fmtassert(char *buf, size_t len, const char *file, int line,
    const char *function, const char *failedexpr)
{
	return snprintf_ss(buf, len,
	    "assertion \"%s\" failed: file \"%s\", line %d%s%s%s\n",
	    failedexpr, file, line,
	    function ? ", function \"" : "",
	    function ? function : "",
	    function ? "\"" : "");
}
*/


/*
void
__assert13(const char *file, int line, const char *function,
    const char *failedexpr);
void
__assert13(const char *file, int line, const char *function,
    const char *failedexpr)
{
	char buf[1024];
	int l = fmtassert(buf, sizeof(buf), file, line, function, failedexpr);
	if (l < 0)
		abort();
	(void)write(STDERR_FILENO, buf, (size_t)l);
	abort();
	//NOTREACHED 
}
*/


/*
void
__assert(const char *file, int line, const char *failedexpr);
void
__assert(const char *file, int line, const char *failedexpr)
{

	__assert13(file, line, NULL, failedexpr);
	//NOTREACHED
}
*/


enum {
    DIAGASSERT_ABORT   =  1<<0,
    DIAGASSERT_STDERR  =  1<<1,
    DIAGASSERT_SYSLOG  =  1<<2 
};

static int diagassert_flags = -1;


/*
void
__diagassert13(const char *file, int line, const char *function,
    const char *failedexpr);
void
__diagassert13(const char *file, int line, const char *function,
    const char *failedexpr)
{
	//#todo
}
*/


/*
void
__diagassert(const char *file, int line, const char *failedexpr);
void
__diagassert(const char *file, int line, const char *failedexpr)
{
	__diagassert13(file, line, NULL, failedexpr);
}
*/


