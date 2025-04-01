
// kstring.h
// Ring 0, string operations.
// Created by Fred Nora.

#ifndef __KSTRING_H
#define __KSTRING_H    1

// #important
void *memcpy   ( void *v_dst, const void *v_src, unsigned long n );
void *memcpy32 ( void *v_dst, const void *v_src, unsigned long n );
void *memcpy64 ( void *v_dst, const void *v_src, unsigned long n );

int strcmp (char * s1, char * s2);
int kstrncmp ( char *s1, char *s2, int len );
int str_cmp(unsigned char *str1, unsigned char *str2);

char *strcpy ( char *to, const char *from );
char *strncpy (char *s1, const char *s2, size_t n);
char *strcat (char *to, const char *from);

void bcopy (char *from, char *to, int len);
void bzero (char *cp, int len);

size_t strlen (const char *s);

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
#define SPACE " "
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

