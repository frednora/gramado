// jslex.h
// This is the main header file for the lexer component 
// of the gramcnf project.
// Created by Fred Nora.

#ifndef __JSLEX_H
#define __JSLEX_H    1


// ------------------------------------------
// Lexer codes
// See: variable 'lexer_expression'.
typedef enum {
    LEXERCODE_NULL,
    PLUS_EXPR,       //  1
    MINUS_EXPR,      //  2 
    BIT_AND_EXPR,    //  3 
    BIT_IOR_EXPR,    //  4
    MULT_EXPR,       //  5
    TRUNC_DIV_EXPR,  //  6
    TRUNC_MOD_EXPR,  //  7
    BIT_XOR_EXPR,    //  8
    LSHIFT_EXPR,     //  9
    RSHIFT_EXPR,     // 10
    LT_EXPR,         // 11
    GT_EXPR,         // 12
    LE_EXPR,         // 13
    GE_EXPR,         // 14
    NE_EXPR,         // 15
    EQ_EXPR          // 16
}lexerexpr_t;  //lexercode_t;


// jslex info
struct jslex_info_d 
{
    int lexer_token_count;

//
// Line support
//

    int lexer_firstline;        // First line
    int lexer_lastline;
    int lexer_number_of_lines;  // Total numbe rof lines
    int current_line;           // Current line

    int lexer_expression;

    // ...

// --------------------------------------
// When some element was found
    int directive_found;
    int type_found;
    int modifier_found;
    int qualifier_found;
    int keyword_found;
    int constant_type_found;
    int constant_base_found;
    int return_found;
    int main_found;

/*
// #test
// ## current ##
// Usado pelo lexer pra saber 
// qual lugar na lista colocar o lexeme.
    int current_keyword;
    int current_identifier; 
    int current_constant;
    int current_string;
    int current_separator; 
    int current_special;
*/

    int eofno;

//()
    int parentheses_start;
    int parentheses_end;
    int parentheses_count;
//{}
    int brace_start;
    int brace_end;
    int brace_count;


};
extern struct jslex_info_d  JSLEX_Info;


//
// -- Prototypes --------
//

void jslex_error(char *msg);
//int check_newline ();
//Isso pega o código original, retira os espaços 
//e separa em palavras usando o espaço como delimitador.
//char *lexerCreateLexemes ( char *in );
//char **lexerCreateTokens ( char *s );
//int getElementID ( char *s );
//int getElementClass ( int number );
//...

// Get stuff
int js_yylex(void);

// Initialize
int jslex_initialize(void);

#endif    

