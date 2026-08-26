// kmemory.c
// Memory support.
// Created by Fred Nora.

#include <kernel.h>


void *memset(void *ptr, int value, int size)
{
    register int i=0;

    if ( ptr != NULL && size > 0 )
    {
        //#bugbug.
        unsigned char *temp = ptr;
        for (i=0; i<size; i++){
            *temp++ = (unsigned char) value;
        };
    }
    return (void *) ptr;
}

