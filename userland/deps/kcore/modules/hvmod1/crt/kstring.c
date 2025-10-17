// kstring.c
// String support.
// Created by Fred Nora.


#include <kernel.h>

//strlen
// strlen:
//    Give the string lenght in bytes.

int module_strlen(const char *s)
{
    register int i=0;

//#todo
    //if ( (void *) s == NULL ){ return -1; }

    for ( i=0; s[i] != '\0'; ++i )
    {
    };
    return (int) i;
}

// strcpy:  
//     Copy a string
char *strcpy(char *to, const char *from)
{
    register int i=0;
    while ( to[i] = from[i] )
    {
        i += 1;
    };
    return (to);
}

