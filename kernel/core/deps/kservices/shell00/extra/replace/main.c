// Exemplo de replace em C.
// Por Luiz Felipe.
// https://github.com/Silva97

// rtl
#include <stdio.h>
#include <string.h>

static int replace(char *text, char *word, char *new);


int main (int argc, char *argv[])
{
    int status = -1;
    char text[] = "Apenas um teste.";

// Replace
    status = replace(text, "um", "dois");
    if (status<=0){
        printf ("fail\n");
        return -1;
    }
    printf ("%s \n",text);
    //puts(text);
    return 0;  
}

static int replace(char *text, char *word, char *new)
{
    char *start;
    int sizew = strlen(word); 
    int sizen = strlen(new);

// Onde está a primeira ocorrência.
    start = strstr(text, word);
    if (!start)
        return 0;

// #bugbug: Isso provavelmente falhou
// Rever essa função na libc.
    strcpy(
        start +sizen, 
        start +sizew);

    memmove(
        start, 
        new, 
        sizen );

    return 1;
}


