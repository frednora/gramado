// stdlib.c
// Created by Fred Nora.

#include "../bl.h"  

//
//
//

void *malloc(size_t size)
{
    void *h;
    unsigned long s = ( unsigned long) size;

// Parameter:
    if (s < 0){
        //#todo: Message
        return NULL;
    }
    if (s == 0){
        s=1;
    }

// Alocar memória no heap do kernel.
    h = (void *) heapAllocateMemory(s);
    if ((void *) h == NULL){
        return NULL;
    }

    return (void *) h; 
}

/*
 * free:
 * >> #importante:
 * >> Tradicionalmente essa função só libera o que foi alocado pela última 
 * chamada de malloc. Mas estamos tentando algo diferente.
 * The free() function frees the memory space pointed to by ptr, 
 * which must have been returned by a previous call 
 * to malloc(), calloc() or realloc(). 
 * Otherwise, or if free(ptr) has already been called before, 
 * undefined behavior occurs. 
 * >> If ptr is NULL, no operation is performed.
 * Importante:
 *     uma estratégia seria apenas sinalizarmos na estrutura que 
 * desejamos que o GC libere os recurso. Em seguida devemos sinalizar 
 * no mmblock que libere o bloco para outras alocações. 
 */
void free(void *ptr)
{
    if ((void *) ptr == NULL){
        return;
    }
// See: memory.c
    FreeHeap(ptr);
}

//
// End
//

