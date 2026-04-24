
// compiler.c 
// Aqui ficará o compilador, 
// que transformará a linguagem intermediária em assembly.
// Bom, como ainda não temos linguagem intermediária,
// vamos transformar C em Assembly
// lexer >> parser >> compiler
// 2022 - Fred Nora

#include "gramcnf.h"


// OUT: The output pointer.
FILE *compiler (int dump_output)
{
// Routine:
// + Initialize the lexer.
// + Parse the tokens.
// + Return a pointer to the output file.

    int lexer_return = -1;
    int parser_return = -1;


//
// Initialize lexer and parser.
//

// Just initializing the lexer.
    //printf ("compiler: calling lexer ...\n");
    lexer_return = (int) lexer_initialize();
    //analizar retorno

// The parser will call the yylex() a lot of times.
    //printf ("compiler: calling parser ...\n");
    int status = (int) parser_initialize();
    //analizar retorno

//
// Parse
//

    // IN: dump output file?
    parser_return = (int) parse(dump_output);

// Nesse momento ja temos um arquivo de output.
// more ...

    //printf ("compiler: Done\n");

// Returning the output.
    return (FILE *) stdout;
}

