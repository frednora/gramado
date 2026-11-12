
// compiler.c 
// Aqui ficará o compilador, 
// que transformará a linguagem intermediária em assembly.
// Bom, como ainda não temos linguagem intermediária,
// vamos transformar C em Assembly
// lexer >> parser >> compiler
// 2022 - Fred Nora

#include "gramcnf.h"


// Called by main().
// Routine:
// + Initialize the lexer.
// + Parse the tokens.
// + Return a pointer to the output file.
// OUT: The output pointer.
FILE *compiler (int dump_output)
{
    int lexer_return = -1;
    int parser_return = -1;

// --------------
// Lexer:
// Initialize lexer and parser.
// Just initializing the lexer.
    lexer_return = (int) lexer_initialize();
    //analizar retorno

// --------------
// Parser:

// The parser will call the yylex() a lot of times.
    int status = (int) parser_initialize();

// IN: dump output file?
    parser_return = (int) parse(dump_output);


// Nesse momento ja temos um arquivo de output.
// more ...

// Returning the output.
// #bugbug
// Do not write things in stdout, because its gonna show it 
// into the screen.
// We gotta use another output file to simply save it into the disk.
    return (FILE *) stdout;
}

