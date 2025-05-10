
// #todo
// Esse será a inicialização para a arquitetura x86.

#include <types.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>


// #todo
// Criar x86start.h ??
int x86start ( int argc, char *argv[], char *envp[] );
int x86start ( int argc, char *argv[], char *envp[] )
{

    char **ev = &envp[0];

    //See: 
    // Algum arquivo define char **environ
    // Outro faz isso: #define __environ environ 
    __environ = ev;


    /* Save the command-line arguments.  */
    //__libc_argc = argc;
    //__libc_argv = argv;
    //__environ = envp;

    

    // Inicializa a biblioteca.
    libcInitRT ();
    stdioInitialize ();

    // #todo 
    // init  cpu features ?
    // threads ? thread pool ?
    // jmp buffers?
    // Initialize ctype data.
    
    //
    // retorna pois crt0 chamará a função main();
    //
    
    return 0
}


