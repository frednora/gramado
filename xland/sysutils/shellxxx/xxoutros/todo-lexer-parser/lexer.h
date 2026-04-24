
//gcc 0.9
//#define isalnum(char) ((char >= 'a' && char <= 'z') || (char >= 'A' && char <= 'Z') || (char >= '0' && char <= '9'))
//#define isdigit(char) (char >= '0' && char <= '9')
#define ENDFILE -1  /* token that represents end-of-file */


// ## next support ##

//número total de tokens encontrados.
int number_of_tokens;

//classe do token atual.
int current_token;

//índice do próximo token na lista de tokens.
int next_index;

int current_index;

//um tipo foi encontrado,
//esperamos por uma função ou uma variável.
int type_found;


//palavra reservada encontrada.
int keyword_found;

//tipo que foi encontrado.
int current_type;

//se a constant é hexa.
int constant_is_hexa;

//um retorno foi encontrado.
int return_found;

//flag para o tratamento da string dentro do asm inline.
// \" marcando início e fim de string.
int string_flag;

//índice para a posição na lista onde está o retorno.
int return_index;

//se main foi encontrada.
int main_found;

//tipo de retorno da função main.
int main_return_type;

//tipo de retorno da função.
int function_return_type;


//encontramos um #, significando que o segue é uma diretiva do preprocessador.
int directive_fould;




//  ### segue suporte a lexer code ###
int lexer_code;

typedef enum {
	
    LEXERCODE_NULL,
    PLUS_EXPR,       //1
	MINUS_EXPR,       //2 
	BIT_AND_EXPR,      //3 
	BIT_IOR_EXPR,      //4
	MULT_EXPR,         //5
	TRUNC_DIV_EXPR,    //6
	TRUNC_MOD_EXPR,    //7
	BIT_XOR_EXPR,      //8
	LSHIFT_EXPR,      //9
	RSHIFT_EXPR,     //10
	LT_EXPR,          //11
	GT_EXPR,         //12
	LE_EXPR, //13
	GE_EXPR, //14
	NE_EXPR,  //15
	EQ_EXPR   //16

}lexercode_t;






//()
int parentheses_start;
int parentheses_end;
int parentheses_count;

//{}
int brace_start;
int brace_end;
int brace_count;


// Deixa o lexer contar o número de linhas
int lexer_lineno;		
int lexer_firstline;		
int lexer_lastline;
int lexer_token_count;

//gcc 0.9
int lineno;

//gcc 0.9
static int maxtoken;		/* Current length of token buffer */
static char *token_buffer;	/* Pointer to token buffer */

char real_token_buffer[256];

//gcc 0.9
void check_newline ();
int skip_white_space ();
int yylex ();

 // Vamos inicializar o lexer.
int lexerInit ();


//...


//Isso pega o código original, retira os espaços 
//e separa em palavras usando o espaço como delimitador.
//char *lexerCreateLexemes ( char *in );


//char **lexerCreateTokens ( char *s );

//int getElementID ( char *s );

//int getElementClass ( int number );

void error ( char *msg );

