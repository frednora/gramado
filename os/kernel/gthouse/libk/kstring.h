// kstring.h
// Ring 0, string operations.
// Created by Fred Nora.

#ifndef __LIBK_KSTRING_H
#define __LIBK_KSTRING_H    1

// #important
void *memcpy   ( void *v_dst, const void *v_src, unsigned long n );
void *memcpy32 ( void *v_dst, const void *v_src, unsigned long n );
void *memcpy64 ( void *v_dst, const void *v_src, unsigned long n );

// strcmp: 
// Compare two strings
int strcmp (char * s1, char * s2);

/*
 * gramado_strncmp:
 * Custom string comparison function used in Gramado OS.
 *
 * Behavior:
 * - Compares up to 'n' characters.
 * - Returns 1 if a mismatch is found during comparison.
 * - Returns 2 if one string ends before the other after 'n' characters.
 * - Returns 0 only if both strings are identical and end together.
 *
 * Compatibility:
 * - NOT POSIX/glibc compatible.
 * - In POSIX/glibc, strncmp("meta1","meta",4) would return 0 (prefix match),
 *   but here it returns 2 because one string continues after the other.
 *
 * Usage:
 * - Safe for strict equality checks inside Gramado OS.
 * - Not portable: use strcmp for keyword matching in the lexer.
 */

int gramado_strncmp ( char *s1, char *s2, int len );

/*
 * strncmp:
 * Standard POSIX/glibc-compatible implementation.
 *
 * Behavior:
 * - Compares up to 'n' characters of two strings.
 * - Returns 0 if the first 'n' characters are equal.
 * - Returns a negative value if s1 < s2.
 * - Returns a positive value if s1 > s2.
 *
 * Compatibility:
 * - Fully compliant with C standard and POSIX/glibc.
 * - Ensures portable behavior across Linux, BSD, musl, newlib, etc.
 */

// int strncmp(const char *s1, const char *s2, size_t n);



char *safe_strcpy(char *to, const char *from, size_t maxlen);
char *strcpy ( char *to, const char *from );
char *strncpy (char *s1, const char *s2, size_t n);
char *strcat (char *to, const char *from);

void bcopy (char *from, char *to, int len);
void bzero (char *cp, int len);

size_t strlen (const char *s);
size_t k_strnlen(const char *s, size_t maxlen);

void *memset ( void *ptr, int value, int size );

size_t strcspn(const char *str, const char *reject);
size_t strspn (const char *str, const char *accept);

unsigned long 
string_compute_checksum ( 
    unsigned char *buffer, 
    unsigned long lenght );

//
// == strtok ============
//

// #test
// Tokenizer.
// We're gonna need these routines to split the pathnames 
// in the fs module.

#define LSH_TOK_DELIM   " \t\r\n\a" 
#define LSH_TOK_DELIM2  " \t\r\n\a+!:=/.<>;|&" 
#define SPACE  " "
#define TOKENLIST_MAX_DEFAULT  80

char *k_strtok_r (
    char *s, 
    const char *delim, 
    char **last );

char *k_strtok(char *s, const char *delim);

char *strdup(const char *str);

#endif    

//
// End
//

