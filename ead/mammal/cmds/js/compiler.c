// compiler.c 
// High level part of the interpreter.
// lexer >> parser >> compiler
// 2022 - Fred Nora

// Aqui ficará o compilador, 
// que transformará a linguagem intermediária em assembly.
// Bom, como ainda não temos linguagem intermediária,
// vamos transformar C em Assembly

#include "gramcnf.h"


// Called by main().
// Routine:
// + Initialize the lexer.
// + Parse the tokens.
// + Return a pointer to the output file.
// OUT: The output pointer.
FILE *compiler(int dump_output)
{
    int Status = -1;

    //printf("compiler:\n");

//
// Initializations
//

    Status = (int) jslex_initialize();
    if (Status < 0){ 
        printf("compiler: on jslex_initialize()\n");
        goto fail; 
    }
    Status = (int) jspar_initialize();
    if (Status < 0){ 
        printf("compiler: on jspar_initialize()\n");
        goto fail; 
    }
    Status = (int) jsvm_initialize();
    if (Status < 0){ 
        printf("compiler: on jsvm_initialize()\n");
        goto fail; 
    }


//
// Parser loop
//

    Status = (int) parser_loop(dump_output);
    if (Status < 0){ 
        printf("compiler: on parser_loop()\n");
        goto fail; 
    }


//
// VM loop
//

    Status = (int) vm_loop();
    if (Status < 0){ 
        printf("compiler: on vm_loop()\n");
        goto fail; 
    }

    // ...

// Nesse momento ja temos um arquivo de output.
// more ...

// Returning the output.
// #bugbug
// Do not write things in stdout, because its gonna show it 
// into the screen.
// We gotta use another output file to simply save it into the disk.
    return (FILE *) stdout;

fail:
    printf("compiler: fail\n");
    return NULL;
}

