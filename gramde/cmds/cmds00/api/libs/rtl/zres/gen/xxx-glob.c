
// gen/glob.c
// Criando o arquivo baseado na libc do netbsd e do minix.

/*
#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <glob.h>
#include <pwd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
*/

#define GLOB_LIMIT_STRING   524288  /* number of readdirs */
#define GLOB_LIMIT_STAT     128     /* number of stat system calls */
#define GLOB_LIMIT_READDIR  65536   /* total buffer size of path strings */
#define GLOB_LIMIT_PATH     1024    /* number of path elements */
#define GLOB_LIMIT_BRACE    128     /* Number of brace calls */

struct glob_limit 
{
    size_t l_string;
    size_t l_stat;
    size_t l_readdir;
    size_t l_brace;
};

/*
 * XXX: For NetBSD 1.4.x compatibility. (kill me l8r)
 */
#ifndef _DIAGASSERT
#define _DIAGASSERT(a)
#endif


#define DOLLAR      '$'
#define DOT         '.'
#define EOS         '\0'
#define LBRACKET    '['
#define NOT         '!'
#define QUESTION    '?'
#define QUOTE       '\\'
#define RANGE       '-'
#define RBRACKET    ']'
#define SEP         '/'
#define STAR        '*'
#define TILDE       '~'
#define UNDERSCORE  '_'
#define LBRACE      '{'
#define RBRACE      '}'
#define SLASH       '/'
#define COMMA       ','


#ifndef USE_8BIT_CHARS
#define M_QUOTE    0x8000
#define M_PROTECT  0x4000
#define M_MASK     0xffff
#define M_ASCII    0x00ff
typedef unsigned short  Char;
#else
#define M_QUOTE    (Char)0x80
#define M_PROTECT  (Char)0x40
#define M_MASK     (Char)0xff
#define M_ASCII    (Char)0x7f
typedef char  Char;
#endif

#define CHAR(c)  ((Char)((c)&M_ASCII))
#define META(c)  ((Char)((c)|M_QUOTE))
#define M_ALL  META('*')
#define M_END  META(']')
#define M_NOT  META('!')
#define M_ONE  META('?')
#define M_RNG  META('-')
#define M_SET  META('[')
#define ismeta(c)  (((c)&M_QUOTE) != 0)


/*
int
glob(
    const char * __restrict pattern, 
    int flags, 
    int (*errfunc)(const char *, int), 
    glob_t * __restrict pglob )
{
    return -1;
}
*/

