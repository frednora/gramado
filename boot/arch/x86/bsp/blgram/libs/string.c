// string.c
// 2015 - Created by Fred Nora.

#include "../bl.h"  

//
//
//


// Compare two strings.
int strcmp(char *s1, char *s2)
{
    register int i=0;

    for ( i=0; s1[i] == s2[i]; i++ )
    {
        if (s1[i] == '\0'){
            return 0;
        }
    };

    return ( (int) (s1[i] - s2[i]) );
}

// Compare n bytes of two strings.
int strncmp (char *s1, char *s2, int len)
{
    register int n = len;

    while (n > 0)
    {
        n--;
        
        if (*s1 != *s2){
            return (int) 1;
        }

        *s1++;
        *s2++;
    };

    if (*s1 != '\0' || *s2 != '\0')
    {
        return (int) 2;
    }

    return 0;
}

// Compare two strings.
int str_cmp (unsigned char *str1, unsigned char *str2)
{
    while ( *str1 == *str2 && 
            *str1 != 0 && 
            *str2 != 0 )
    { 
        str1++; 
        str2++; 
    };

// Mesma quantidade de chars iguais.
    if (*str1 == *str2) 
        return 0;

    if (*str1 > *str2) 
        return (int) 1;

    if (*str1 < *str2) 
        return (int) -1;

//Done
    return (int) -1;
}

// Copy n bytes.
// Simple, byte oriented memcpy.
void *memcpy( void *v_dst, const void *v_src, unsigned long c )
{
    const char *src = v_src;
    char *dst = v_dst;

    while (c--){
        *dst++ = *src++;
    };

//done:
    return v_dst;
}

// Get the size of a string.
size_t strlen (const char *s)
{
    register int i=0;

    for ( i=0; s[i] != '\0'; i++ )
    {
        // 
    };

    return (size_t) i;
}

// Copy a string.
char *strcpy(char *to, const char *from)
{
    register int i=0;
    while (to[i] = from[i]){
        i += 1;
    };
    return (to);
}

// Concatenate strings.
char *strcat(char *to, const char *from)
{
    char *ret = to;

    while (*to)
    {
        to += 1;
    };

    strcpy (to, from);
    return (ret);
}

// Copy n bytes.
void bcopy (char *from, char *to, int len)
{
    while (len--)
    {
        *to++ = *from++;
    };
}

// Fill with 0s.
void bzero(char *cp, int len)
{
    while (len--)
    {
        *(cp + len) = 0;
    };
}

//
// End
//
