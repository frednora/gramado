// lexer.c
// The lexer for gramcnf interpreter.
// #ps: There is a lot o comments for academic purpose.
// 2018 - Created by Fred Nora.

// Credits:
// Inspired on gcc 0.9.

//
// Lexer?
//

// Through the lens of Formal Languages and Automata Theory. A lexer is 
// essentially a finite automaton that scans an input stream and 
// classifies substrings into tokens.

/*
Finite Automaton:  
The lexer behaves like a deterministic finite automaton (DFA). 
Each branch in the switch(c) inside yylex() represents a transition 
based on the current input symbol. For example, 
encountering 'a'..'z' or 'A'..'Z' moves the automaton 
into the identifier state.
*/

/*
Lexical Categories:  
Tokens are classified into categories: 
identifiers, keywords, constants, strings, separators, and operators. 
This corresponds to the alphabet partitioning in automata theory, 
where the input alphabet is divided into equivalence classes.
*/

/*
Regular Languages:  
Each token type (like identifiers or numbers) is defined by 
a regular expression. For instance:
+ Identifier:       [A-Za-z_][A-Za-z0-9_]*
+ Decimal constant: [0-9]+
+ Hex constant:     0[xX][0-9A-Fa-f]+  
The lexer implements these regexes procedurally, 
but conceptually they are regular languages recognized by DFAs.
*/

/*
Token Buffer:  
The token_buffer acts as the automaton’s memory, storing the lexeme 
until classification is complete. Once the DFA reaches an accepting state, 
the buffer is finalized.
*/

//
// Key Functions Explained
//

/*
__skip_white_space():
Implements transitions that ignore whitespace and comments. 
In automata terms, these are ε-transitions 
(moves that don’t produce tokens but advance the input).
*/

/*
yylex():
The main DFA driver. It:
+ Reads the next symbol.
+ Chooses a branch (state transition).
+ Accumulates characters in the buffer.
+ Stops when a non-matching symbol is found (accepting state).
+ Returns a token code.
*/

/*
Keyword Recognition:
After recognizing an identifier, the lexer checks if 
it matches reserved words (if, while, return, etc.). 
This is a post-processing step: the DFA accepts the identifier, then 
a lookup table refines its classification.
*/

/*
Operator Handling:
Operators like ==, !=, <=, >=, ++, -- are handled by lookahead. 
The lexer peeks at the next character (getc) and 
decides whether to combine symbols. This is equivalent to DFA transitions 
that require multiple input symbols before reaching an accepting state.
*/

// Identifier recognition:
// Matches the regular expression [A-Za-z_][A-Za-z0-9_]*.
// Initially classified as TK_IDENTIFIER, but refined
// through keyword lookup to TK_KEYWORD, TK_TYPE, or TK_MODIFIER.

// Constant recognition:
// Implements two regular languages:
//   Decimal: [0-9]+
//   Hexadecimal: 0[xX][0-9A-Fa-f]+
// These are disjoint token classes in the lexer’s alphabet.

//
// Extra academic insight
//

// Token buffer as memory: 
// In DFA terms, the buffer is not part of the automaton’s formal definition 
// (DFAs don’t have memory beyond state), but in implementation it’s necessary 
// to store the lexeme for later semantic use.

// Reserved word lookup: 
// This is a post-DFA refinement. The DFA recognizes the identifier language, 
// then a table lookup reclassifies certain strings. This shows how 
// lexical analysis combines automata with symbol tables.

// Error handling: 
// Unterminated comments or malformed constants are examples of 
// the lexer rejecting strings not in the defined regular languages.


#include "gramcnf.h"

// ## current ##
// Usado pelo lexer pra saber qual lugar na lista 
// colocar o lexeme.
int current_keyword=0;
int current_identifier=0; 
int current_constant=0;
int current_string=0;
int current_separator=0; 
int current_special=0;


// Token support
//int lexer_token_count=0;
int number_of_tokens=0;  // Total number of tokens.
int current_token=0;  // The class of the curent token.

// When some element was found.
int directive_fould=0;
int type_found=0;
int modifier_found=0;
int qualifier_found=0;
int keyword_found=0;
int constant_type_found=0;
int constant_base_found=0;
int return_found=0;
int main_found=0;

//
// Return support
//

// Tipo de retorno da função.
int function_return_type=0;
// Tipo de retorno da função main.
int main_return_type=0;

// Índices na lista de tokens.
int return_index=0;   // Índice para a posição na lista onde está o retorno.
int next_index=0;     //índice do próximo token na lista de tokens.
int current_index=0;

// Flag para o tratamento da string dentro do asm inline.
// \" marcando início e fim de string.
int string_flag=0;

//tipo que foi encontrado.
int current_type=0;

//()
int parentheses_start=0;
int parentheses_end=0;
int parentheses_count=0;
//{}
int brace_start=0;
int brace_end=0;
int brace_count=0;

int eofno=0;

//
// Lexer information
//

struct lexer_info_d  LexerInfo;

//
// -- Prototypes --------
//

static int __lexerInit(void);
static int __skip_white_space(void);

//---------------------------------------------------


//#### supensa ###
/*
int check_newline ()
{
    register int c;
    //register int token;	
	
	//obs
	//entramos aqui porque encontramos um '\n'

    while (1)
    {
	   //Entramos nessa função porque encontramos um '\n'.		
        LexerInfo.current_line++;
		//printf(" [LF1] ");
		
		//pega mais um depois do '\n'
		c = getc (finput);
		
		//se o que segue o '\n' for um espaço, deixaremos o skip_white_space tratar o espaço.
		if (c == ' ' || c == '\t')
		{
			return (int) c;
		}
		
		//se for quanquer outra coisa também deixaremos o __skip_white_space tratar
		return (int) c;
        
		if ( c == '#' )
		{
            //Skip whitespace after the #.  
            while (1)
	        {
	            c = getc (finput);
	            if ( !(c == ' ' || c == '\t') )
	                break;
	        }
			
             
		     //If the # is the only nonwhite char on the line,
	         //just ignore it.  Check the new newline.  
		    
            if (c == '\n')
	            continue;


            //encontramos algum char válido após #.
			//Something follows the #; read a token.  
            ungetc (c, finput);
            //token = yylex ();		
		
		    //#bugbug:
			// ?? O que fizemos com esse token ??
			
		    //skip the rest of this line.  
            while ((c = getc (finput)) != '\n');		

		}else{
			 
		    // If no #, unread the character,
	        // except don't bother if it is whitespace.  		
			
			//se não é #, retorna ao encontrar espaço e
			//devolve se encontrar outra coisa. depois retorne também.
			
			//return (int) c;
			
	        //if (c == ' ' || c == '\t')
			//{
			//	return (int) c;
			//	
			//} else {
			//	
			//    ungetc ( c, finput );	
			//    return (int) -1;
			//};
		};
		
	};//while
};
*/

void error(char *msg)
{
    printf ("error: %s\n", msg );
}


// __skip_white_space:
// Models ε-transitions in automata theory.
// Whitespace and comments do not produce tokens,
// but advance the input pointer.
// - Single-line comments: //.*\n
// - Multi-line comments:  /\*([^*]|\*+[^/])*\*+/
// These are regular languages that the lexer discards,
// ensuring only meaningful tokens are passed to the parser.


static int __skip_white_space(void)
{
// Skip whitespace and comments. Return the next non-whitespace character.

    register int c=0;
    register int inside=0;
begin:
    c = getc (finput);
    // #debug
    //printf("%c ",c); 
    for (;;)
    {
        switch (c)
        {
            // ## spaces ##
            // Se encontramos um espaço, 
            // pegamos o próximo e saímos do switch 
            // para reentrarmos no for.
            case ' ':
            case '\t':
            case '\f':
            case '\r':
            case '\b':
                c = getc(finput);
                break;

            // ## new lines ##
            case '\n':
                LexerInfo.current_line++;
                //próximo
                c = getc(finput);
                break;

            // ## comments ##
            // '/' 
            // #importante: Isso pode ser a 
            // primeira barra do comentário ou uma divisão.
            case '/':
                c = getc(finput);
                
                //#### inicia um comentário de uma linha ####
                //Aqui encontramos a segunda barra de dias consecutivas.
                //single line comments.
                if (c == '/')
                {
                    while (1)
                    {
                        c = getc(finput);

                        //quando alinha acabar,
                        //apenas saímos do switch
                        //sairemos com '\n'
                        //??? e se chegarmos ao fim do arquivo ??? #todo
                        // Acho que isso só sai do while.
                        if (c == '\n'){
                            break;
                        }
                        // ?
                    };
                    //isso sai do switch
                    break;
                }

				//#### inicia um comentário de múltiplas linhas ####
				//#importante 
				//excluindo os casos acima, então significa que nossa barra não tinha nada a ver com comentário 
				//lembrando que a barra aparecei depois de um espaço em branco.
				//por enquanto vamos dizer que algo está errado com essa barra,
				//printf("__skip_white_space: todo: depois da barra / .");
				//exit(1);

                if (c == '*')
                {
                    c = getc(finput);
                    inside = 1; 

                    while (inside)
                    {
                        if (c == '*'){

                            //sequência
                            while (c == '*')
                                c = getc (finput);

                            // se logo em seguida da sequencia
                            // de astetiscos tiver uma barra.
                            if (c == '/')
                            {
                                //fim do comentário  
                                //sai do while ... com alguma coisa em c.
		                        inside = 0;

		                        //c = getc(finput);
							    //break; //sai do while.
								
								//begin: ??
								//Ao fim de um comentário /* ... */
								//podemos ter espaços tabs e talvez outros comentários.
		                        
                                goto begin;
                            }

						// se vamos pular mudar de linha dentro do comentário.
                        } else if (c == '\n'){

						    // precisamos contar
                            LexerInfo.current_line++;
							//printf(" [LF2] ");
                            c = getc (finput);
							  
					        //?? para onde vamos??
							//precisamos continuar no while até encontrarmos a barra /. ou o *.
								
                        }else if (c == EOF || c == '\0'){  

                            eofno++;
                            printf ("__skip_white_space: Unterminated comment in line %d\n", 
                                LexerInfo.current_line );
                            exit(1);

						//default
                        }else{
							//isso são letras do comentário.
							//continuaremos dentro do while(inside)
							//??#bugbug: mas até quando ??
                            //temos que contar ou confiar no EOF.
                            c = getc(finput);
                        };
                    };
                }

				// aqui depois da barra não emcontramos nem o '*' nem o '/'
                // isso significa que estamos eliminando espaços dentro de uma expressão.
				// então vamos retornar a barra para que a rotina continue 
				// tratando a expressão.
                ungetc ( c, finput );
				return (int) '/';
                break;

            //#test 
            // ## ignorando diretivas do preprocessdor '#' ##
            /*
			case '#':
                while(1)
				{
					c = getc(finput);
					
					//quando acabar a linha,
					//apenas saímos do switch
					if( c == '\n' ){
						//não precisa contar, pois sairemos do switch e 
						//entraremos no switch novamente agora com \n que será contado na hora apropriada.
						//LexerInfo.current_line++;
						//printf(" [LF3] ");
					    break;
					}
				};
                break;
            */

            default:
                return (int) c;
        };//switch
    }; // for
}

// -----------------------------------------
// yylex:
// Implements a deterministic finite automaton (DFA).
// Each case in the switch corresponds to a transition
// based on the current input symbol. Accepting states
// produce tokens, which are elements of the regular language
// defined by the grammar of the source language.
//
// Academic note:
// - The lexer is equivalent to a DFA where states represent
//   partial recognition of a token (e.g., "inside identifier").
// - Transitions are triggered by input symbols.
// - Once an accepting state is reached, the token is emitted.
// - This bridges regular languages (lexical analysis) with
//   context-free grammars (parsing).

int yylex(void)
{
// Get the next token

    register int value=0;  // The return value

    register int c=0;
    register char *p;
    register int c1=0;
    register int number_length=0;

again:

    // Get a char from the input stream, skipping whitespace and comments.
    c = (int) __skip_white_space();

    switch (c)
    {
        // 0 or EOF. (-1).
        case EOF:
        case 0:
            //printf ("yylex: 0 or EOF\n");
            eofno++; 
            LexerInfo.lexer_lastline = LexerInfo.current_line;  // Last line?
            LexerInfo.lexer_number_of_lines = LexerInfo.lexer_lastline;
            value = (int) TK_EOF;
            goto done;
            break;


        // Identifier recognition:
        // Matches the regular expression [A-Za-z_][A-Za-z0-9_]*.
        // - The first character must be a letter or underscore.
        // - Subsequent characters may include digits.
        // This ensures identifiers belong to a regular language
        // where the alphabet is partitioned into {letters, digits, underscore}.
        // After recognition, identifiers are checked against a
        // reserved-word table, refining classification into
        // TK_KEYWORD, TK_TYPE, or TK_MODIFIER.

        case 'A': case 'B': case 'C': case 'D':
        case 'E': case 'F': case 'G': case 'H':
        case 'I': case 'J': case 'K': case 'L':
        case 'M': case 'N': case 'O': case 'P':
        case 'Q': case 'R': case 'S': case 'T':
        case 'U': case 'V': case 'W': case 'X':
        case 'Y': case 'Z':
        case 'a': case 'b': case 'c': case 'd':
        case 'e': case 'f': case 'g': case 'h':
        case 'i': case 'j': case 'k': case 'l':
        case 'm': case 'n': case 'o': case 'p':
        case 'q': case 'r': case 's': case 't':
        case 'u': case 'v': case 'w': case 'x':
        case 'y': case 'z':
        case '_':

            // Address
            p = token_buffer;
            // clean
            memset( real_token_buffer, 0, TOKEN_BUFFER_MAX );
            // #todo: 
            // Limite tamanho do buffer
            
            while (1)
            {
                // Put into the buffer and increment the buffer address.
                *p = c;
                p++;

                // Get next token from the file.
                c = getc(finput);

                // Not Alpha-numeric and not '_'.
                // Finalize the buffer if it's not an identifier.
                // The loop continues as long as the next character 
                // is alphanumeric (isalnum) or underscore.

                if ( ( isalnum(c) == 0 ) &&  (c != '_') )
                {
                    *p = 0;  // Finalize
                    ungetc( c, finput );
                    goto we_have_an_identifier;
                }
            };

            // Temos um identificador
            we_have_an_identifier:
            
            //#debug
            //printf ("real_token_buffer={%s}\n",real_token_buffer);

            // Vamos começar dizendo que somos um identificador.
            // Porem ...
            // O identificador pode ser um nome de função ou uma variável.
            // Mas vamos comparar com palavras reservadas.
            // Caso seja uma das palavras reservadas, então deixamos de ser um identificador.
            value = TK_IDENTIFIER;

            // Reserved?
            // Determinamos que era um identificador,
            // mas vamos ver se ele é uma palavra reservada.
            // As palavras reservadas podem ser modificadores, tipos
            // ou palavras chave.

            // Modifiers

            if ( gramado_strncmp( real_token_buffer, "signed", 6 ) == 0 )
            {
                keyword_found  = KWSIGNED;
                modifier_found = MSIGNED;
                //return (int) TK_MODIFIER;
                value = (int) TK_MODIFIER;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "unsigned", 8 ) == 0 )
            {
                keyword_found  = KWUNSIGNED;
                modifier_found = MUNSIGNED;
                //return (int) TK_MODIFIER;
                value = (int) TK_MODIFIER;
                goto done; 
            }
            if ( gramado_strncmp( real_token_buffer, "inline", 6 ) == 0 )
            {
                keyword_found  = KWINLINE;
                modifier_found = MINLINE;
                //return (int) TK_MODIFIER;
                value = (int) TK_MODIFIER;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "static", 6 ) == 0 )
            {
                keyword_found  = KWSTATIC;
                modifier_found = MSTATIC;
                //return (int) TK_MODIFIER;
                value = (int) TK_MODIFIER;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "volatile", 8 ) == 0  )
            {
                keyword_found  = KWVOLATILE;
                modifier_found = MVOLATILE;
                //return (int) TK_MODIFIER;
                value = (int) TK_MODIFIER;
                goto done;
            }

            // types

            if ( gramado_strncmp( real_token_buffer, "void", 4 ) == 0 )
            {
                keyword_found = KWVOID;
                type_found    = TVOID;
                //return (int) TK_TYPE;
                value = (int) TK_TYPE;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "char", 4 ) == 0 )
            {
                keyword_found = KWCHAR;
                type_found    = TCHAR;
                //return (int) TK_TYPE;
                value = (int) TK_TYPE;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "short", 5 ) == 0 )
            {
                keyword_found = KWSHORT;
                type_found    = TSHORT;
                //return (int) TK_TYPE;
                value = (int) TK_TYPE;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "int", 3 ) == 0 )
            {
                keyword_found = KWINT;
                type_found    = TINT;
                //return (int) TK_TYPE;
                value = (int) TK_TYPE;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "long", 4 ) == 0 )
            {
                keyword_found = KWLONG;
                type_found    = TLONG;
                //return (int) TK_TYPE;
                value = (int) TK_TYPE;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "box", 3 ) == 0 )
            {
                keyword_found = KWBOX;
                type_found    = TBOX;
                //return (int) TK_TYPE;
                value = (int) TK_TYPE;
                goto done;
            }
            // #in order to avoid mistach with meta1234
            //if ( gramado_strncmp( real_token_buffer, "meta", 4 ) == 0 )
            if ( strcmp( real_token_buffer, "meta" ) == 0 )
            {
                keyword_found = KWMETA;
                type_found    = TMETA;
                //return (int) TK_TYPE;
                value = (int) TK_TYPE;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "def", 3 ) == 0 )
            {
                keyword_found = KWDEF;
                type_found    = TDEF;
                //return (int) TK_TYPE;
                value = (int) TK_TYPE;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "var", 3 ) == 0  )
            {
                keyword_found = KWVAR;
                type_found    = TVAR;
                //return (int) TK_TYPE;
                value = (int) TK_TYPE;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "let", 3 ) == 0  )
            {
                keyword_found = KWLET;
                type_found    = TLET;
                //return (int) TK_TYPE;
                value = (int) TK_TYPE;
                goto done;
            }

            // keywords

            if ( gramado_strncmp( real_token_buffer, "name", 4 ) == 0 )
            {
                keyword_found = KWNAME;
                //return (int) TK_KEYWORD;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "content", 7 ) == 0 )
            {
                keyword_found = KWCONTENT;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "print", 5 ) == 0 )
            {
                keyword_found = KWPRINT;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "goto", 4 ) == 0 )
            {
                keyword_found = KWGOTO;
                //return (int) TK_KEYWORD;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "return", 6 ) == 0 )
            {
                keyword_found = KWRETURN;
                //return (int) TK_KEYWORD;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "exit", 4 ) == 0 )
            {
                keyword_found = KWEXIT;
                //return (int) TK_KEYWORD;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "switch", 6 ) == 0 )
            {
                keyword_found = KWSWITCH;
                //return (int) TK_KEYWORD;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "case", 4 ) == 0 )
            {
                keyword_found = KWCASE;
                //return (int) TK_KEYWORD;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "break", 5 ) == 0 )
            {
                keyword_found = KWBREAK;
                //return (int) TK_KEYWORD;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "default", 7 ) == 0 )
            {
                keyword_found = KWDEFAULT;
                //return (int) TK_KEYWORD;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "for", 3 ) == 0 )
            {
                keyword_found = KWFOR;
                //return (int) TK_KEYWORD;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "continue", 8 ) == 0 )
            {
                keyword_found = KWCONTINUE;
                //return (int) TK_KEYWORD;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "do", 2 ) == 0 )
            {
                keyword_found = KWDO;
                //return (int) TK_KEYWORD;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "while", 5 ) == 0 )
            {
                keyword_found = KWWHILE;
                //return (int) TK_KEYWORD;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "if", 2 ) == 0 )
            {
                keyword_found = KWIF;
                //return (int) TK_KEYWORD;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "else", 4 ) == 0 )
            {
                keyword_found = KWELSE;
                //return (int) TK_KEYWORD;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "union", 5 ) == 0 )
            {
                keyword_found = KWUNION;
                //return (int) TK_KEYWORD;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "struct", 6 ) == 0 )
            {
                keyword_found = KWSTRUCT;
                //return (int) TK_KEYWORD;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "enum", 4 ) == 0 )
            {
                keyword_found = KWENUM;
                //return (int) TK_KEYWORD;
                value = (int) TK_KEYWORD;
                goto done;
            }
            if ( gramado_strncmp( real_token_buffer, "sizeof", 6 ) == 0 )
            {
                keyword_found = KWSIZEOF;
                // return (int) TK_KEYWORD;
                value = (int) TK_KEYWORD;
                goto done;
            }

            //...

            //return (int) value;
            goto done;
            break;

        // Constant recognition:
        // Two disjoint regular languages are implemented:
        //   Decimal:     [0-9]+
        //   Hexadecimal: 0[xX][0-9A-Fa-f]+
        // Each is a regular expression recognized by a DFA.
        // The lexer distinguishes them by prefix inspection (0x/0X).
        // This illustrates how token classes are defined as
        // separate languages over the same input alphabet.

        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
        case '8': case '9':
        //case '.':

            p = token_buffer;  // Address of the buffer

            // #ps: In the case of a hexadecimal constant, 
            // we will have a prefix '0x' or '0X'.
            if (c == '0'){

                *p = c;  // Save it into the buffer
                p++;     // Increment the buffer pointer

                c = getc(finput);

                if ( c == 'x' || c == 'X' )
                {
                    //base = 16;  // Set the base to hexadecimal

                    *p = c;  // Save it into the buffer
                    p++;     // Increment the buffer pointer

                    while (1)
                    {
                        c = getc(finput);

                        // If the next character is not a hexadecimal digit, 
                        // we finalize the token.
                        if ( isxdigit(c) == 0 )
                        {
                            *p = 0;  // Finalize the buffer

                            ungetc( c, finput );

                            value = TK_CONSTANT;  // We have a constant token

                            //constant_type_found = //#todo tem que contar. 
                            constant_base_found = CONSTANT_BASE_HEX;
                            goto constant_done;
                        }

                        // Yes it is a hexadecimal digit
                        *p = c;  // Save it into the buffer
                        p++;     // Increment the buffer pointer
                    };
                }

                // #bugbug
                printf ("yylex: FAIL expected x in constant in line %d", 
                    LexerInfo.current_line );
                exit (1);

            } else {

                //base=10

                *p++ = c; 

                while (1)
                {
                    c = getc(finput);

                    // If the next character is not a digit, we finalize the token.
                    if ( isdigit( c ) == 0 )
                    {
						*p = 0;  // Finalize the buffer

						ungetc(c, finput);

						value = TK_CONSTANT;  // We have a constant token

						//constant_type_found = //#todo tem que contar. 
						constant_base_found = CONSTANT_BASE_DEC;
                        goto constant_done;
                    }

                    // Save it into the buffer and increment the buffer pointer.
                    *p++ = c;
                };
            };

            constant_done:
            //return (int) value;
            break;

        // String recognition:
        // Matches the regular expression "([^"])*"
        // - Begins and ends with double quotes.
        // - Accepts any character except unescaped quotes.
        // Strings are stored in the token buffer until the closing quote.
        // This is another regular language, though in practice
        // escape sequences complicate the DFA slightly.

        case '\"':
        {
            c = getc(finput);
            // Address
            p = token_buffer;
    
            //coloca no token_buffer.
            while (c != '\"')
            {
	            //if (c == '\\')
			    //{
		        //    c = readescape ();
		        //    if (c < 0)
		        //        goto skipnewline;
	        
			    //}else if (c == '\n')
			    //      {
		        //          LexerInfo.current_line++;
	            //      }

	            //if (p == token_buffer + maxtoken)
	               // p = extend_token_buffer(p);
	                *p++ = c;

	            //skipnewline:
	                c = getc(finput);
	        };//while

            *p++ = 0;  // Finalize the buffer for the string token

            //yylval.ttype = build_string (p - token_buffer, token_buffer);
            //TREE_TYPE (yylval.ttype) = char_array_type_node;

            value = TK_STRING;  // We have a string token

            //return (int) TK_STRING;
            break;
        };


        // Separator recognition:
        // ----------------------------------------------------
        // Grammar symbols like parentheses (), braces {}, brackets [],
        // commas, semicolons, colons, and question marks are recognized here.
        // These are not part of the "lexical content" (like identifiers or constants),
        // but rather delimiters that define the structure of the language.
        //
        // Academic perspective:
        // - Separators correspond to terminals in the context-free grammar (CFG).
        //   For example, productions like:
        //       function → identifier "(" ")" ";"
        //   require separators to enforce syntactic boundaries.
        // - In automata terms, separators are single-symbol tokens
        //   that move the parser into new states (e.g., entering/exiting a block).
        // - Recognizing separators ensures the parser can later
        //   validate nested constructs (balanced parentheses/braces).
        // - This illustrates the bridge between lexical analysis (DFA)
        //   and syntax analysis (PDA): separators are simple regular tokens,
        //   but their meaning emerges only in the parser when matched
        //   against grammar rules.
        // ----------------------------------------------------
        // Separators: (){}[],.;:?

        // Safe to extend: 
        // We can add more characters (like @, ~, $) 
        // to this list without breaking the lexer.

        case '(':  case ')':
        case '{':  case '}':
        case '[':  case ']':
        case ',':
        case '.':
        case ';':
        case ':':
        case '?':

            p = token_buffer;  // Address of the buffer

            *p++ = c;  // Save the separator character into the buffer
            *p++ = 0;  // Finalize the buffer for the separator token

            value = TK_SEPARATOR;  // We have a separator token

            //return (int) TK_SEPARATOR;
            break;

        // Operator recognition:
        // Handles single and compound operators (+, -, ==, !=, <=, >=, ++, --).
        // - Compound operators require lookahead (peek at next char).
        // - This simulates DFA transitions that consume multiple symbols
        //   before reaching an accepting state.
        // Academic note: lookahead is a practical extension beyond
        // pure DFA theory, but conceptually it is equivalent to
        // longer regular expressions (e.g., "==" vs "=").

        case '+':  case '-':  case '*':  case '/':
        case '<':  case '>':
        case '&':
        case '|':
        case '%':
        case '^':
        case '!':
        case '=':
        {
            combine:

            switch (c)
            {
                // '+-*/'
                case '+':  LexerInfo.lexer_expression = PLUS_EXPR;       break;
                case '-':  LexerInfo.lexer_expression = MINUS_EXPR;      break;
                case '*':  LexerInfo.lexer_expression = MULT_EXPR;       break;
                case '/':  LexerInfo.lexer_expression = TRUNC_DIV_EXPR;  break;

                case '&':  LexerInfo.lexer_expression = BIT_AND_EXPR;     break;
                case '|':  LexerInfo.lexer_expression = BIT_IOR_EXPR;     break;
                case '%':  LexerInfo.lexer_expression = TRUNC_MOD_EXPR;   break;
                case '^':  LexerInfo.lexer_expression = BIT_XOR_EXPR;     break;

                // ?
                case TK_LSHIFT:  LexerInfo.lexer_expression = LSHIFT_EXPR;  break;
                case TK_RSHIFT:  LexerInfo.lexer_expression = RSHIFT_EXPR;  break;

                case '<':  LexerInfo.lexer_expression = LT_EXPR;  break;
                case '>':  LexerInfo.lexer_expression = GT_EXPR;  break;
            }

            c1 = getc (finput);

            if (c1 == '=')
            {
                switch (c)
                {
                    case '<':
                        LexerInfo.lexer_expression = LE_EXPR; 
                        value = TK_ARITHCOMPARE;  //?
                        goto done;

                    case '>':
                        LexerInfo.lexer_expression = GE_EXPR; 
                        value = TK_ARITHCOMPARE;  //?
                        goto done;

                    case '!':
                        LexerInfo.lexer_expression = NE_EXPR; 
                        value = TK_EQCOMPARE;  //?
                        goto done;

                    case '=':
                        LexerInfo.lexer_expression = EQ_EXPR; 
                        value = TK_EQCOMPARE;  //?
                        goto done;
                };

                // ?
                value = TK_ASSIGN; 
                goto done;

            }else if (c == c1){

                switch (c)
                {
                    case '+':  value = TK_PLUSPLUS;    goto done;
                    case '-':  value = TK_MINUSMINUS;  goto done;
                    case '&':  value = TK_ANDAND;      goto done;
                    case '|':  value = TK_OROR;        goto done;
                    
                    case '<':  c = TK_LSHIFT;  goto combine;
                    case '>':  c = TK_RSHIFT;  goto combine;
                };

            } else if ((c == '-') && (c1 == '>')) {
                value = TK_POINTSAT; 
                goto done; 
            };

            ungetc (c1, finput);

            if ((c == '<') || (c == '>'))
                value = TK_ARITHCOMPARE;
            else value = c;
                goto done;
        };

        // Return the char itself
        default:
            value = (int) c;

    };  // switch end

done:
    // Increment counter here, once per token
    LexerInfo.lexer_token_count++;
    return (int) value;
}


/*
 * __lexerInit:
 *     The routine initializes the lexer.
 *     This is a worker, called by lexer_initialize().
 */
static int __lexerInit(void)
{
    register int i=0;

// Line support
// Arquivo de texto começa com a linha 1
    LexerInfo.current_line = 1;  // Current line
    LexerInfo.lexer_firstline=1;
    LexerInfo.lexer_lastline=1;
    LexerInfo.lexer_number_of_lines=1;

    LexerInfo.lexer_expression = 0;

    // Token support
    LexerInfo.lexer_token_count = 0;
    number_of_tokens=0;  // Total number of tokens.
    current_token=0;  // The class of the curent token.
    maxtoken = TOKEN_BUFFER_MAX;

    //()
    parentheses_start=0;
    parentheses_end=0;
    parentheses_count=0;
    //{}
    brace_start=0;
    brace_end=0;
    brace_count=0;

    eofno = 0;  // eof++

//
// Toke buffer.
//

// Clear buffer
    for ( i=0; i<TOKEN_BUFFER_MAX; i++ )
    {
        //real_token_buffer[i] = (char) '\0';
        real_token_buffer[i] = 0;
    };

    token_buffer = real_token_buffer;
    //token_buffer = &real_token_buffer[0]; 
    //sprintf ( real_token_buffer, "uninitialized-token-string" );

    //...

    return 0;
}

int lexer_initialize(void)
{
    //printf ("parser_initialize:\n");
    return (int) __lexerInit();
}

/*
//check subsequent
int check_subseq ( int c, int a, int b )
{
	//extern getchar, peekc;

	if (!peekc)
		peekc = getchar();
	
	//se for diferente de c, retorna a.
	//se for igual a c, retorn b.
	
	if (peekc != c)
		return (a);
	
	peekc = 0;
	
	return (b);
};
*/

//
// End
//

