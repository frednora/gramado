
// lexer.h

#ifndef __LEXER_H
#define __LEXER_H    1


// ## current ##
// Usado pelo lexer pra saber qual lugar na lista 
// colocar o lexeme.
extern int current_keyword; 
extern int current_identifier; 
extern int current_constant; 
extern int current_string; 
extern int current_separator; 
extern int current_special;

//#bugbug: 
//olexer vai usar esse porque a rotina no ctype pode estar com problema #todo 
#define lexer_isalnum(char) ((char >= 'a' && char <= 'z') || (char >= 'A' && char <= 'Z') || (char >= '0' && char <= '9'))
//#define isdigit(char) (char >= '0' && char <= '9')
#define ENDFILE -1  /* token that represents end-of-file */

// Lexer codes
typedef enum {

    LEXERCODE_NULL,
    PLUS_EXPR,       //1
    MINUS_EXPR,      //2 
    BIT_AND_EXPR,    //3 
    BIT_IOR_EXPR,    //4
    MULT_EXPR,       //5
    TRUNC_DIV_EXPR,  //6
    TRUNC_MOD_EXPR,  //7
    BIT_XOR_EXPR,    //8
    LSHIFT_EXPR,     //9
    RSHIFT_EXPR,     //10
    LT_EXPR,         //11
    GT_EXPR,         //12
    LE_EXPR,  //13
    GE_EXPR,  //14
    NE_EXPR,  //15
    EQ_EXPR   //16

}lexercode_t;


extern int eofno;

// LIne support.
extern int lineno;
extern int lexer_lineno;   // Total numbe rof lines.
extern int lexer_firstline;
extern int lexer_lastline;
extern int lexer_token_count;

extern int number_of_tokens;

extern int lexer_code;
extern int current_token;  // The class of the curent token.

// When some element was found.
extern int directive_fould;
extern int type_found;
extern int modifier_found;
extern int qualifier_found;
extern int keyword_found;
extern int constant_type_found;
extern int constant_base_found;
extern int return_found;
extern int main_found;


//
// Return support
//

// Tipo de retorno da função.
extern int function_return_type;
// Tipo de retorno da função main.
extern int main_return_type;

// Índices na lista de tokens.
extern int return_index;  // Índice para a posição na lista onde está o retorno.
extern int next_index;    //índice do próximo token na lista de tokens.
extern int current_index;

// Flag para o tratamento da string dentro do asm inline.
// \" marcando início e fim de string.
extern int string_flag;

//tipo que foi encontrado.
extern int current_type;

//()
extern int parentheses_start;
extern int parentheses_end;
extern int parentheses_count;
//{}
extern int brace_start;
extern int brace_end;
extern int brace_count;

//
// -- Prototypes --------
//

int lexer_initialize(void);  // Initialize
int yylex(void);  // get stuff.
void error(char *msg);

//int check_newline ();
//Isso pega o código original, retira os espaços 
//e separa em palavras usando o espaço como delimitador.
//char *lexerCreateLexemes ( char *in );
//char **lexerCreateTokens ( char *s );
//int getElementID ( char *s );
//int getElementClass ( int number );
//...

#endif    

