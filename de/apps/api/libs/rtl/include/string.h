/*
 * File: string.h
 * Descrição:
 *     Operações com strings. Padrão C.
 */

#ifndef __STRING_H__
#define __STRING_H__    1

#include <stddef.h>

#ifdef _BSD_SIZE_T_
typedef _BSD_SIZE_T_  size_t;
#undef _BSD_SIZE_T_
#endif

// #test
// #define streq(s1,s2)  (strcmp(s1,s2)==0)

int strupr(char *n); 
int strlwr(char *n); 

// ??
void *
check_bytes8 ( 
    const char *start, 
    char value, 
    unsigned int bytes );

int strcoll (const char *s1, const char *s2);

int memcmp (const void *s1, const void *s2, size_t n);

void *memmove(void *dest, const void *src, size_t count);
void *memscan (void *addr, int c, size_t size);
 
 
char *strsep (char **s, const char *ct);
char *strpbrk (const char * cs,const char * ct); 
char *strncat (char *dst, const char *src, size_t n);

char *strdup (const char *str);
char *strndup (const char *s, size_t n);

char *strchrnul (const char *s, int c); 

char *strnchr (const char *s, size_t count, int c); 
char *strrchr (const char *p, int ch);

int strcasecmp (const char *s1, const char*s2);

char *strreplace (char *s, char old, char new);

char *strncpy(char *s1, const char *s2, size_t n);

// See:
// http://man7.org/linux/man-pages/man3/strcmp.3.html
int strcmp(const char *s1, const char *s2);

// See:
// http://man7.org/linux/man-pages/man3/strcmp.3.html
int strncmp(const char *s1, const char *s2, size_t n);

//#bugbug deletando porque tá errado.
//int str_cmp(unsigned char *str1, unsigned char *str2);

void *memsetw (void *dst, int val, size_t count);
void *memset ( void *ptr, int value, int size );
void *memoryZeroMemory(void* ptr, size_t cnt);
void *memcpy (void *v_dst, const void *v_src, unsigned long c);
//@todo: void *memcpy(void *dst, const void *src, size_t c); 

void *memcpy32 ( void *v_dst, const void *v_src, unsigned long n );
void *memcpy64 ( void *v_dst, const void *v_src, unsigned long n );

size_t strlcpy (char *dst, const char *src, size_t size);
char *strcpy(char *to, const char *from);

size_t strlcat (char *dst, const char *src, size_t size);
char *strcat (char *to, const char *from);

void bcopy(char *from, char *to, int len);
void  bzero(char *cp, int len);
size_t strlen (const char *s);
size_t strnlen ( const char *s, size_t maxlen );

/* Copyright (c) 2011, 2012 Jonas 'Sortie' Termansen. */
size_t strcspn(const char* str, const char* reject);
size_t strspn(const char* str, const char* accept);
//char* strtok_r(char* str, const char* delim, char** saveptr);
//char* strtok(char* str, const char* delim);

/*apple style*/
char *strtok_r (char *s, const char *delim, char **last);
char *strtok (char *s, const char *delim);

char *strchr (const char *s, int c);

char *strstr (const char *s1, const char *s2);

size_t strxfrm (char *dst, const char *src, size_t n);
char *strerror(int errnum);

/* XSI-compliant */
int strerror_r (int errnum, char *buf, size_t buflen);

/* GNU-specific */
//char *strerror_r(int errnum, char *buf, size_t buflen);

char *strsignal(int sig);

void strtoupper(char *src);
void rtl_string_to_upper (char *src);

void rtl_reverse(char s[]);

#endif    

//
// End
//
