/*
 # Lexical analysis #

 In computer science, lexical analysis, lexing or tokenization 
is the process of converting a sequence of characters 
(such as in a computer program or web page) into a 
sequence of tokens (strings with an assigned and thus identified meaning). 

A program that performs lexical analysis may be termed a lexer, 
tokenizer,[1] or scanner, though scanner is also a term for the 
first stage of a lexer. 

A lexer is generally combined with a parser, which together 
analyze the syntax of programming languages, 
web pages, and so forth.

Lexing can be divided into two stages: 
the scanning, which segments the input string into syntactic 
units called lexemes and categorizes these into token classes; 
and the evaluating, which converts lexemes into processed values.

From Wikipedia, the free encyclopedia
*/


#include "types.h"
#include "heap.h"
#include "api.h"
#include "stddef.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"



#include "table.h"
#include "lexer.h"
#include "parser.h"

#include "fncc.h"


//pega o próximo token de uma lista com todos. 
int next (){
	
	int i;
	
	// No caso de erro.
	register int value = TOKENNULL;
	
	char *token_string;
	
    char **list = (char **) program.h0;	
	
	
	//Limites
	if( next_index < 0 || next_index >= number_of_tokens )
	{
	    printf("next: Limits: next=%d max=%d  \n", next_index, number_of_tokens);
		value = -1;
        goto done;		

	}else{
	    i = next_index;
        current_index = next_index; 		
	};
	
	//#debug.
	printf("next: %d ",i);
	printf("%s\n", list[i]);
	
	//#importante:
	//Último token da lista.
	token_string = (char *) list[i];
	
	//??
	//filtro.
	// token inválido ou fim da lista.
	if( token_string == NULL )
	{
		printf("[null]\n");
		value = TOKENNULL;
		goto done;
	};
	
	//pegando caracteres que formarão a string.
	if( string_flag == 1 && *token_string != '\"' )
	{
		value = TOKENNULL;
		goto done;
	}
	
	
	switch (*token_string)
	{
		//eof
        //todo 		
		//case '\0':
		//    value = -1;
		//	goto done;
		//  break;
		
		
		//0x1234 ou número ou erro
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
        // case '.':		
		    
			token_string++;
		    
			if( *token_string == 'x' )
			{
				//antes foi encontrado a palavra resevada 'return'
				if( return_found == 1 )
				{
					//índice na lista para o retorno ...
				   //return_index = i;
				}
				constant_is_hexa = 1;
		        value = TOKENCONSTANT;
		        goto done;				
			}else{

			//antes foi encontrado a palavra resevada 'return'
				if( return_found == 1 )
				{
					//índice na lista para o retorno ...
				    //return_index = i;
				}
				
				constant_is_hexa = 0;
		        value = TOKENCONSTANT;
		        goto done;								
			};
			
		    break;
			
        case 'A':  
		case 'B':  
		case 'C':  
		case 'D':  
		case 'E':
        case 'F':  
		case 'G':  
		case 'H':  
		case 'I':  
		case 'J':
        case 'K':  
		case 'L':  
		case 'M':  
		case 'N':  
		case 'O':
        case 'P':  
		case 'Q':  
		case 'R':  
		case 'S':  
		case 'T':
        case 'U':  
		case 'V':  
		case 'W':  
		case 'X':  
		case 'Y':
        case 'Z':			
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
		case '_':  
		   
		    //int
		    if( strncmp( (char *) list[i], "int", 3 ) == 0  )
            {
				//printf (" ## GOT INT ## ");
			    type_found = 1;
			    current_type = TINT;
			
			    //palavra reservada.
			    value = TOKENKEYWORD;
			    goto done;
			}	

			//void
		    if( strncmp( (char *) list[i], "void", 4 ) == 0  )
            {
			    type_found = 1;
			    current_type = TVOID;
			
			    //palavra reservada.
			    value = TOKENKEYWORD;
			    goto done;
			}	
			
			
			//main
		    if( strncmp( (char *) list[i], "main", 4 ) == 0  )
            {
			    main_found = 1;
				
				if( type_found == 1 )
				{
					//o tipo atual vira o retorno de main.
					main_return_type = current_type;
				}else{
					
					//main está sem tipo definido,
					//vamos usar o tipo default 'int'
					//por enquanto
					//printf("error");
					main_return_type = TINT;
				};
			    value = TOKENIDENTIFIER;
			    goto done;
			}					
			
			
			//return
		    if( strncmp( (char *) list[i], "return", 6 ) == 0  )
            {
			    return_found = 1;
				
			    //palavra reservada.
			    value = TOKENKEYWORD;
			    goto done;
			}


			//return
		    if( strncmp( (char *) list[i], "asm", 3 ) == 0  )
            {	
			    //palavra reservada.
			    value = TOKENKEYWORD;
			    goto done;
			}				
			
			//...
			
			//identificador comum. ex: x1 x2 var1
			value = TOKENIDENTIFIER;
			goto done;   
		    
			break;
		
        //separators (){}[],.;:?		
	    case '(':
		case ')':
		case '{':
		case '}':
		case '[':	
		case ']':
		case ',':
		case '.':
		case ';':
		case ':':
		case '?':
		    value = TOKENSEPARATOR;
			goto done;
		    break;
			
		//operadores ??	
		case '+':
		case '-':
		case '*':
		case '/':
		case '=':			
		case '|':
        case '%':
        case '&':
        case '^':		
		    value = TOKENOPERATOR;
			goto done;
			break;

			
        case '\"': 
		    value = TOKENSPECIAL;
			goto done;
		    break;			

        case '\'': 
		    value = TOKENSPECIAL;
			goto done;
		    break;			
			
		//...	
		
		default:
		    value = -1;
			goto done;
		    break;
		
	};
	
	
	
	 
done:	
    next_index++;
	return (int) value;
}


/*
 ********************************************
 * lexerCreateLexemes:
 *     Primeiro teste de análise léxica. 
 *     Isso pega o código original, retira os espaços 
 * e separa em palavras usando o espaço como delimitador.
 *
 * Podemos criar listas para todos os tipos de tokens.
 * 6 listas.
 *
 */ 
char *lexerCreateLexemes ( char *in ){
	
	//?? #teste
	//Registra o inteiro identificador de token.
	register int value;
	
	int comment_flag;
	
	// Colocaremos os elementos encontrados dentro de %{x}
	

	// Pularemos os espaços.
	// continente conteúdo.
	// statement
	//expression
	
    /* Character being scanned in main loop.  */
    register char c;
	register char last_char;

    /* Length of pending accumulated identifier.  */
    /* O primeiro caractere de um identificador precisa ser uma letra */
	register int ident_length = 0;	
	
	/*comprimento de um número pendente. Ex 0x1234 */
	/*O primeiro caractere de um número precisa ser um dígito.*/
	register int number_length = 0;	
	
	// ( expression ) { statement statement }  [ expression ]
	
	register statement_leght = 0;
	
	register expression_leght = 0;
	
	int i, ns = 0;
	int j = 0;
	 
	//in
	//register char *p = STRING_CFILE; 
	register char *p;
	p = in;
    
	//out
	char *out = (char *) &file_i[0];
	
	
	
	// base decimal.
    int base = 10;	
	
	
	//#debug
	//printf("lexer_test1:\n");
	//printf("p={%s}\n",p);
	//printf("out={%s}\n",out);
	
	
	//#while
	//Até o fim de uma string. '\0'
	
	while ( *p && *p != '\0' )
	{
        switch (*p)
	    {
			// Spaces
            // Ignora e o contador vai avançar.
			case ' ':
			case '\t':
                
				//se estamos pegando espaços dentro de uma string.
				//if( string_flag == 1)
				//{
					//tab
				//	if( *p == '\t' )
				//	{
						//todo: while ...
				//        out[j] = ' ';
				//        j++;
				//        out[j] = ' ';
				//        j++;
				//        out[j] = ' ';
				//        j++;
				//        out[j] = ' ';
				//        j++;						
                //        break; 											
				//	}
				//    out[j] = ' ';
				//    j++;
                //    break; 					
				//};			
		        break;
				
			//token ## (){};
            case '(':  //pode significar que o identificador é uma função.
			case ')':  
			case '{':  //pode significar que estamos construindo uma função.
			case '}':
			case ';':  //pode significar que estamos chamando a função.
			
				//delim
				out[j] = ' ';
				j++;					
			    out[j] = *p;
				j++;
				//delim
				out[j] = ' ';
				j++;				
                last_char = *p;				
              
                if( p[0] == '(' )
				{
					//incrementa o número do parênteses.
					parentheses_count++;
                    parentheses_start = 1;
                }
				
                if( p[0] == ')' )
				{
					//decrementa o número de parênteses.
					parentheses_count--;
                    parentheses_end = 1;
				}
				
                if( p[0] == '{' )
				{
					brace_count++;
                    brace_start = 1;
                }
                
				if( p[0] == '}' )
				{
					brace_count--;
                    brace_end = 1;
				};
				
				if( p[0] == ';' )
				{
					
				};
		        break;
				
				
			//identificador,(symbol)	
			 /* O primeiro caractere de um identificador precisa ser uma letra */		
			
            case 'A':  
		    case 'B':  
		    case 'C':  
		    case 'D':  
		    case 'E':
            case 'F':  
		    case 'G':  
		    case 'H':  
		    case 'I':  
		    case 'J':
            case 'K':  
		    case 'L':  
		    case 'M':  
		    case 'N':  
		    case 'O':
            case 'P':  
		    case 'Q':  
		    case 'R':  
		    case 'S':  
		    case 'T':
            case 'U':  
		    case 'V':  
		    case 'W':  
		    case 'X':  
		    case 'Y':
            case 'Z':			
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':
			case '_':  // * '_'
			
			    //se o contador já foi iniciado então apenas 
				//colocamos o char e incrementamos o contador.
				if( ident_length > 0 )
                {
			        out[j] = *p;
				    j++;
				    ident_length++;
					last_char = *p;	 
					
					// Se o próximo não for alphanumerico, e não for '_', 
					// então não será o identificador terminou.
					if( isalnum( p[1] ) == 0 && p[1] != '_' )
					{
						//delim
						//se o identificador terminou 
						//precisamos fechar.
			            out[j] = ' ';
				        j++;						
						ident_length = 0;
					}
					break;					
				}					
				//Abrimos se o contador for zero.
				if( ident_length == 0 )
				{
			        out[j] = ' ';
				    j++;
			        out[j] = *p;
				    j++;
				    ident_length++;
					last_char = *p;	
					break;
				}
				break;
				
			/*O primeiro caractere de um número precisa ser um dígito.*/
			//@todo: o caso do segundo char ser um 'x' quando o primeiro é um '0'.
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			//case '.':  //fp
			
			
				//estamos construindo um identificador que comtém números. ex 'var1'; 
				if( ident_length > 0 )
                {
			        out[j] = *p;
				    j++;
				    ident_length++;
					last_char = *p;	 
					
					// Se o próximo não for alphanumerico, e não for '_', 
					// então não será o identificador terminou.
					if( isalnum( p[1] ) == 0 && p[1] != '_' )
					{
						//delim
						//se o identificador terminou 
						//precisamos fechar.
			            out[j] = ' ';
				        j++;						
						ident_length = 0;
					}
					break;					
				};					
			    
				//se o contador já foi iniciado então apenas 
				//colocamos o char e incrementamos o contador.
				if( number_length > 0 )
                {
			        out[j] = *p;
				    j++;
				    number_length++;
					last_char = *p;	 
					//se o próximo não for alphanumerico, então não será 
					//o identificador terminou.
					if( isdigit( p[1] ) == 0 )
					{
						//se o identificador terminou 
						//precisamos fechar.
			            out[j] = ' ';
				        j++;						
						number_length = 0;
					}
					break;					
				}					
				//Abrimos se o contador for zero.
				if( number_length == 0 )
				{
			        out[j] = ' ';
				    j++;
			        out[j] = *p;
				    j++;
				    number_length++;
					last_char = *p;
					
					//ex: 0x1234
					if( p[0] == '0' )
					{
					    if( p[1] == 'x' || p[1] == 'X'  )
                        {
							//base hexadecimal.
							base = 16;
							
							//inclui o x no output
							out[j] = p[1];
				            j++;
							number_length++;
							last_char = p[1];
							//pulo o 0, no final vai pular o x.
							p++;
							
						}else{
							
							//base octal.
							base = 8;
						}
                        //... e se não for um x ??
					    break;	
					};
					break;
				};
							
			    break;
				
			//New line.	
			case '\n':
			    lexer_lineno++;
				break;

			case '#':
			    directive_fould = 1;
				break;
				
				
			case '=':
			    //não é '=='. É apenas =.
				if( p[1] != '=' )
				{
					out[j] = *p;
				    j++; 
			        
					//delim
					out[j] = ' ';
				    j++;						
				};			
				break;	
				
			case '+':
			    //não é incremento 
				//não é +=.
				//é adição.
				if( p[1] != '+' && p[1] != '=' )
				{
					//delim
					out[j] = ' ';
				    j++;
					
					out[j] = *p;
				    j++;
					
					//delim
					out[j] = ' ';
				    j++;											
				};			
				break;
				
				
			case '-':
			    //não é decremento 
				//também não é -=
				//é subtração.
				if( p[1] != '-' && p[1] != '=' )
				{
					//delim
					out[j] = ' ';
				    j++;
					
					out[j] = *p;
				    j++;
					
					//delim
					out[j] = ' ';
				    j++;											
				};			
				break;
				
			//módulo.	
			case '%':
			    //não é %= 
				//é módulo.
				if(  p[1] != '=' )
				{
					//delim
					out[j] = ' ';
				    j++;
					
					out[j] = *p;
				    j++;
					
					//delim
					out[j] = ' ';
				    j++;											
				};			
				break;
				
				
			//	
			case '|':
			    //não é || 
				//também não é |=
				//é |.
				if(  p[1] != '|' && p[1] != '=' )
				{
					//delim
					out[j] = ' ';
				    j++;
					
					out[j] = *p;
				    j++;
					
					//delim
					out[j] = ' ';
				    j++;											
				};			
				break;	

			//Greater than	
			case '>':
			    //não é >> 
				//é >.
				if(  p[1] != '>' )
				{
					//delim
					out[j] = ' ';
				    j++;
					
					out[j] = *p;
				    j++;
					
					//delim
					out[j] = ' ';
				    j++;											
				};			
				break;					
                				
            //Less than
			case '<':
			    //não é << 
				//é <.
				if(  p[1] != '<' )
				{
					//delim
					out[j] = ' ';
				    j++;
					
					out[j] = *p;
				    j++;
					
					//delim
					out[j] = ' ';
				    j++;											
				};			
				break;	


			//xor	
			case '^':
			    //não é ^= 
				//é ^.
				if(  p[1] != '=' )
				{
					//delim
					out[j] = ' ';
				    j++;
					
					out[j] = *p;
				    j++;
					
					//delim
					out[j] = ' ';
				    j++;											
				};			
				break;					

								
			//Comments
            // '/* */' and '//...'			
            //#bugbug: loops infinitos.
			//se for divisão temos que imprimir no output.
			case '/':
			
			    //todo
				//if ( p[0] == '/' && p[1] == '=' )
                
				if ( p[0] == '/' && p[1] == '/' )
				{
					while ( *p && *p != '\n' )
					    p++;
                    
                    break;					
				};

                if( p[0] == '/' && p[1] == '*' )
				{
					//pula abertura.
					p++;
					p++;
					
					while ( *p && *p != '*' )
						p++;
					
					if( p[0] == '*' && p[1] == '/' )
                    {
					    //pula fechamento.
						p++;
                        p++;						
					}
                    break; 					
				};

				//Não é comentá. É divisão.
				if( p[1] != '/' && p[1] == '*' )
				{
					out[j] = *p;
				    j++;    	
				};
			    break;
            
			//\"	
			case '\"':
			    //delim
				out[j] = ' ';
				j++;
					
				out[j] = *p;
				j++;
					
				//delim
				out[j] = ' ';
				j++;														
				break;	
				
			//\'	
			case '\'':
			    //delim
				out[j] = ' ';
				j++;
					
				out[j] = *p;
				j++;
					
				//delim
				out[j] = ' ';
				j++;														
				break;	

            //...				
 
			
			//outros.
			default:
			    break;
	    };

		//printf(".");
		p++;
	};
	
	
	//last line.
    lexer_lastline = lexer_lineno;	
	
	//printf("\n lexer_test1:o1={%s} \n",out);
	
    return (char *) out;	
};

 

//pega uma string com lexemes separados por um delimitador e cria uma lista de tokens.
//Essa lista de tokens poderá ser usada pelo parser para pegar um por um.
char **lexerCreateTokens( char *s )
{
	char *tokenList[80];
	char *token;
	//int z;
	//register int token_count;
	
	int current_type;
	int return_reached;
	
	//Ret = (char **) tokenList;	
	
	//char tmpin[32];
	//char in[512];
    //char out[512];	
	
	//out
	char *InFilePointer = (char *) &s[0];
	//char *out = (char *) &buffer[0];
	
	//buffer[0] = 's';
	//buffer[1] = 'u';
	//buffer[2] = 'j';
	//buffer[3] = 'o';
	//sprintf(out,";;dirt "); //initialize
	//strcat(out,"buffer ...\n");
	
	size_t size = (size_t) strlen( (const char *) s );
	
	//#debug
	printf("i-size={%d}\n", size);	
	printf("i-string={%s}\n", s);
	//...
	
	infile_size = (unsigned long) size;
	
	
	//limits
	if( size > 128 ){
		printf("lexerCreateTokens: fail, string size limits\n");
		goto done;
	}
		
	
    //int z;
	//for( z=0; z<size; z++)
	//{
	//	in[z] = *s;
	//	s++;
	//}
	
	//while( *s && *s == ' ' )
	//{
	//	s++;
	//}
		
	char in[512];	
	
	
    int z;
	for( z=0; z< size; z++)
	{
		//in[z] = infile[z];
	    in[z] = InFilePointer[z];
	}


    //  # CreateTokenList #	

	char **list = (char **) create_tokenlist( (char *) s );
		
		
	// # mostra a lista de lexemes #
	
	printf("\nlexerCreateTokens list of lexemes:\n\n");
	fncc_print_tokenList( list, "|");

    
	//
	// comparar NÃO funcionou para apenas parte do token ..
	// tem que comparar o token inteiro
	//
	
	
	
	//
	// #Createing tokens.
	// Temos lexames na lista.
	// Agora vamos checar qual tipo pertence cada lexeme,
	// Colocar um token na lista de tokens (outra lista) 
	// Empilhar em pilhas doferentes os diferentes tipos de tokens.
    //	
	
	//##bugbug ainda não implementada..
	//
	
	printf("\n>>Creating tokens ...\n");
	
	int eID;
	int eTmp;
	int i=0;
	
/*	
	while(1)
	{
		//#importante:
		//Último token da lista.
	    token = (char *) list[i];
	    if( token == NULL )
		{
			printf("[null]\n");
			goto done;
		};
	        	
			
		
		//0x1234
		if( ( list[i][0] ) == '0' )
		{
			if( ( list[i][1] ) == 'x'   )
			{
				TOKENLIST[i] = list[i];
				CONSTANTLIST[constant_count] = list[i];
				constant_count++;
			}
		}
		
		//se é alfabético (letra)
		//se for letra esperamos que seja um simbolo, ou uma palavra reservada.
		if( isalpha( list[i][0] ) == 0 )
		{
			// #importante
			//testamos todas as pelavras reservadas e depois 
			//caso não seja nenhuma então é um símbolo, mas não 
			//sabemos se é uma função ou uma variável ... uma flag 
			//deve ser acionada durante o token anterior.
			
		    
			//int
			if( strncmp( (char *) list[i], "int", 3 ) == 0  )	
			{
				TOKENLIST[i] = list[i];
				//KEYWORDLIST[keyword_count] = list[i];
				KEYWORDLIST[keyword_count] = TOKENKEYWORD_STRING;
                keyword_count++; 				
			}
		    
			//void
			if( strncmp( (char *) list[i], "void", 4 ) == 0  )	
			{
				TOKENLIST[i] = list[i];
				//KEYWORDLIST[keyword_count] = list[i];
				KEYWORDLIST[keyword_count] = TOKENKEYWORD_STRING;
                keyword_count++; 				
			}
			
			// continua ...
		
		    //então é um símbolo. será uma função ou uma variável, dependendo 
			//da flag ... mas com certeza é um símbolo.
		}
		
		
		
		//#importante:
		//last for now.
        //só pra sair do while por enquanto.
		if( strncmp( (char *) list[i], "}", 1) == 0  )
		{
		    printf("[}]\n");			
            goto done;
		};	
		
		//... continuar com outros tipos de tokens ...
		
	
		i++;
		//nothing for now.
	};
	
*/
	
	
	// # mostra a lista de tokens #
	
	printf("\nlexerCreateTokens list of TOKENS:\n\n");
	fncc_print_tokenList( TOKENLIST, "|");	
	
    //...
	
	
done:
	
   

    //#debug 
    //printf("\nINPUT:\n%s\n\n",infile);	
    //printf("\nOUTPUT:\n%s\n\n",outfile);
    

    return (char **) list;	
};


 
 
//Dada a string, deve-se retornar 
//o núme que ideitifica o elemento.
int getElementID( char *s )
{
/*	
 
    if( strcmp( TOKEN_ADD, &s[0] ) == 0 ){
		return (int) TOKEN_ADD_ID;
	}
    if( strcmp( TOKEN_SUB, &s[0] ) == 0 ){
		return (int) TOKEN_SUB_ID;
	}
    if( strcmp( TOKEN_MUL, &s[0] ) == 0 ){
		return (int) TOKEN_MUL_ID;
	}
    if( strcmp( TOKEN_DIV, &s[0] ) == 0 ){
		return (int) TOKEN_DIV_ID;
	}

    //char short long
    if( strcmp( TOKEN_CHAR, &s[0] ) == 0 ){
		return (int) TOKEN_CHAR_ID;
	}
    if( strcmp( TOKEN_SHORT, &s[0] ) == 0 ){
		return (int) TOKEN_SHORT_ID;
	}
    if( strcmp( TOKEN_LONG, &s[0] ) == 0 ){
		return (int) TOKEN_LONG_ID;
	}


    //signed unsigned
	if( strcmp( TOKEN_SIGNED, &s[0] ) == 0 ){
		return (int) TOKEN_SIGNED_ID;
	}
    if( strcmp( TOKEN_UNSIGNED, &s[0] ) == 0 ){
		return (int) TOKEN_UNSIGNED_ID;
	}
	
	
	//...
	
	
	
	//...
    if( strcmp( TOKEN_INT, &s[0] ) == 0 ){
		return (int) TOKEN_INT_ID;
	}

    if( strcmp( TOKEN_VOID, &s[0] ) == 0 ){
		return (int) TOKEN_VOID_ID;
	}
	
	//...
	*/
	
fail:
    return (int) 0;
};




int getElementClass( int number )
{
/*	
	switch(number)
	{
		case TOKEN_ADD_ID:
		    return (int) TOKEN_ADD_CLASS;
			break;

		case TOKEN_SUB_ID:
		    return (int) TOKEN_SUB_CLASS;
			break;

		case TOKEN_MUL_ID:
		    return (int) TOKEN_MUL_CLASS;
			break;

		case TOKEN_DIV_ID:
		    return (int) TOKEN_DIV_CLASS;
			break;
        //...
			
	    default:
		    break;
	}
	
*/	
    return 0;	
}

// Vamos inicializar o lexer.
int lexerInit (){
	
	number_of_tokens = 0;
	current_token = 0;
	next_index = 0;
	
	lexer_lineno = 0;
	lexer_firstline = 0;
	lexer_lastline = 0;
	
	//...
	
	return 0;
};


void error ( char *msg ){
	
    printf("error: %s\n", msg);
};



