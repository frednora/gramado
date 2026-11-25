/*
 * _split.c
 * History:
 *     2020 - Created by Fred Nora.
 */

#include <types.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>


#define ____SPLIT_STRING_DELIM  ","
#define ____SPLIT_STRING_TOKENLIST_MAX  80

typedef char ** split_t;

char *tokenList[____SPLIT_STRING_TOKENLIST_MAX];

// #todo: NOT TESTED YET.
// IN: Buffer address.
// OUT: The environmet where we have a lot of 
// string pointers.(its an array of pointers of strings)

split_t split_string (unsigned char *buffer);
split_t split_string (unsigned char *buffer)
{
    // Token support.
    char *token;
    int token_count=0;
    int index=0;

    unsigned char *address = (unsigned char *) buffer;


    if (buffer==0){
        return (split_t) 0;
    }

    size_t l = (size_t) strlen(buffer);
     
    if (l <= 0){
        return (split_t) 0;
    }

    if (l >= ____SPLIT_STRING_TOKENLIST_MAX){
        return (split_t) 0;
    }

// Criando o ambiente.
// Transferindo os ponteiros do vetor para o ambiente.
// quando encontrarmos o delimitador, mas nesse caso,
// como primeiro argumento não é NULL então começa
// do início do buffer.

    tokenList[0] = strtok ( &address[0], ____SPLIT_STRING_DELIM );

// Salva a primeira palavra digitada.
// O ponteiro para primeira string.
    token = (char *) tokenList[0];
    index=0; 

// Enquanto o ponteiro para a string atual
// for diferente de nulo.

    while ( token != NULL )
    {
        // Coloca na lista.
        // Salva a primeira palavra digitada.
        // O ponteiro da string atual.
        tokenList[index] = token;

        //#debug
        //printf("shellCompare: %s \n", tokenList[i] );

        // Quando encontrarmos o delimitador. 
        token = strtok ( NULL, ____SPLIT_STRING_DELIM );

        // Incrementa o índice da lista
        index++;

        // Salvando a contagem.
        token_count = index;
    }; 

// Finalizando a lista.
// A última string tem ponteiro NULL.

    tokenList[index] = NULL;
    
// #bugbug
// #todo: Conferir se o endereço está certo.
// Precisa ser o ambiente tokenList

    // char **tokenList; ou char *tokenList[?];
    // return (char **) address;
        
    return (split_t) tokenList;
}

