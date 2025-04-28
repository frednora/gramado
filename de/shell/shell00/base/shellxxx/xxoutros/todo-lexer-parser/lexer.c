/*
 * File: lexer.c
 */


#include <types.h>

#include "heap.h"
#include "api.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#include "table.h"
#include "lexer.h"
#include "parser.h"
#include "gramcc.h"



void check_newline (){

    register int c;
    //register int token;	

    while (1)
    {
	   //Entramos nessa função porque encontramos um '\n'.		
        lineno++;
		//printf(" [LF1] ");
		
		c = getc (finput);
        
		if ( c == '#' )
		{
            /* Skip whitespace after the #.  */
            while (1)
	        {
	            c = getc (finput);
	            if ( !(c == ' ' || c == '\t') )
	                break;
	        }
			
            /* 
		     If the # is the only nonwhite char on the line,
	         just ignore it.  Check the new newline.  
		    */
            if (c == '\n')
	            continue;


            //encontramos algum char válido após #.
			/* Something follows the #; read a token.  */
            ungetc (c, finput);
            //token = yylex ();		
		
		    //#bugbug:
			// ?? O que fizemos com esse token ??
			
		    /* skip the rest of this line.  */
            while ((c = getc (finput)) != '\n');		

		}else{
			
	        /* 
		     If no #, unread the character,
	         except don't bother if it is whitespace.  
		    */		
			
			//se não é #, retorna ao encontrar espaço e
			//devolve se encontrar outra coisa. depois retorne também.
			
	        if (c == ' ' || c == '\t'){
				return;
			}else{
			    ungetc (c, finput);	
			    return;
			};
		};
	};//while
	
};


int skip_white_space (){
    
	register int c;
    register int inside;

    c = getc(finput);

    for (;;)
    {
        switch (c)
	    {
			// ## comments ##
	        case '/':
			
			    c = getc(finput);
				
				//single line comments.
				if ( c == '/')
				{	
			        while (1)
					{
					    c = getc(finput);
						
						//quando alinha acabar,
						//apenas saímos do switch
				        if( c == '\n'){
					        break;;
				        }
				    };
				};
			
	            //multiple lines comments.
	            if (c != '*')
	            {
	                ungetc(c, finput);
	                return '/';
	            };

	            // é * ... para completar o /*
				c = getc(finput);

	            //dentro do comentário.
				inside = 1;
	            
				while (inside)
	            {
	                if (c == '*'){
						
						//sequencia de **************
		                while (c == '*')
		                    c = getc(finput);

		                if (c == '/')
		                {
							//fim do comentário  */
							//sai do while ... com alguma coisa em c.
		                    inside = 0;
		                    c = getc(finput);
		                }
		            }else if (c == '\n'){
						
						      //precisamos contar.
		                      lineno++;
							  //printf(" [LF2] ");
		                      c = getc(finput);
							  
					//}else if (c == EOF || c == '\0')  //isso também é uma opção de comentário não terminado.		  
		            
					}else if (c == EOF)
		                    printf("unterminated comment");
	                 else
		                 c = getc(finput);
	            };
	            break;

			// ## new lines ##	
	        case '\n':
	            check_newline(); //todo
				//break;

			// ## spaces ##	
            case ' ':
            case '\t':
            case '\f':
            case '\r':
            case '\b':
                c = getc(finput);
                break;

            //#test 
            // ## ignorando diretivas do preprocessdor '#' ##
            case '#':
                while(1)
				{
					c = getc(finput);
					
					//quando acabar a linha,
					//apenas saímos do switch
					if( c == '\n' ){
						//não precisa contar, pois sairemos do switch e 
						//entraremos no switch novamente agora com \n que será contado na hora apropriada.
						//lineno++;
						//printf(" [LF3] ");
					    break;
					}
				};
                break;

			// ## \ ##	
            case '\\':
	            c = getc(finput);
	            if (c == '\n'){
	                lineno++;
				    //printf(" [LF4] ");
				}else{
				    printf("stray '\\' in program"); 
				} 
	            c = getc(finput);    
	            break;
				
				

            default:
                return (c);

        };//switch

    }; // for
};


/*
 ***************************
 * yylex:
 *     Pega o próximo token.
 *     copiando do gcc 0.9
 */
int yylex (){

    register int c;
    register char *p;
    register int value;	

	register int c1;
	register int number_length = 0;	

again:
	
	//#testando getc
	//c = (int) getc(finput);
	
	//if( c == 'm' )
	//	printf("\n@@@@@ m found @@@\n");
	
	// ## espaços ##
	//if( c == ' ' || c == '\n' )
	//    goto again;
	
    c = skip_white_space();

  //yylloc.first_line = lineno;
    //lexer_firstline = lineno;
	
	//#debug
	//if( c == 'm' )
	//	printf("\n@@@@@ m found @@@\n");
  
    switch (c)
    {
        //case EOF:
        //    value = ENDFILE; 
		//	break; 
		
		case EOF:
		    //    value = ENDFILE; 
		    printf("yylex EOF\n");
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

			p = token_buffer;
			
			while (1)
			{
				//@todo: limite tamanho do buffer
				
			    //coloca no buffer.
			    *p = c;
			    p++;				
				
				c = getc(finput);	
				
				//se não for
				if( ( isalnum(c) == 0 ) && (c != '_') )
				{
			        //finaliza
                    *p = 0;
					
			        //devolve o que não batia com a comparação do while.
                    ungetc(c, finput);
					//ungetc( 'X', finput);
                    goto id_ok;					
				}
			};

            id_ok:

			//temos um identificador.
			value = TOKENIDENTIFIER;
			
			//e se esse ideitificador fou uma palavra reservada ??
			
			//temos um tipo
			if( strncmp( real_token_buffer, "int", 3 ) == 0  )
			{
				value = TOKENTYPE;
				type_found = TINT;
			}
			
			//temos um tipo
			if( strncmp( real_token_buffer, "void", 4 ) == 0  )
			{
				value = TOKENTYPE;
				type_found = TVOID;
			}				
			
			//temos um tipo
			if( strncmp( real_token_buffer, "char", 4 ) == 0  )
			{
				value = TOKENTYPE;
				type_found = TCHAR;
			}
			
			
			//temos um tipo
			if( strncmp( real_token_buffer, "short", 5 ) == 0  )
			{
				value = TOKENTYPE;
				type_found = TSHORT;
			}	


			//temos um tipo
			if( strncmp( real_token_buffer, "long", 4 ) == 0  )
			{
				value = TOKENTYPE;
				type_found = TLONG;
			}			
			
			
			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "asm", 3 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWASM;
			}		
			
			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "goto", 4 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWGOTO;
			}			
			
			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "return", 6 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWRETURN;
			}
			
			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "continue", 8 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWCONTINUE;
			}
			
			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "default", 7 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWDEFAULT;
			}			
			
			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "case", 4 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWCASE;
			}
			
			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "switch", 6 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWSWITCH;
			}			
			
			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "for", 3 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWFOR;
			}

			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "do", 2 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWDO;
			}			
			
			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "while", 5 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWWHILE;
			}		

			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "else", 4 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWELSE;
			}	

			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "if", 2 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWIF;
			}		

			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "union", 5 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWUNION;
			}				
			
			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "struct", 6 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWSTRUCT;
			}				
			
			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "enum", 4 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWENUM;
			}			
			
			
			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "sizeof", 6 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWSIZEOF;
			}		

			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "volatile", 8 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWVOLATILE;
			}				
			
			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "inline", 6 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWINLINE;
			}				
			
			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "def", 3 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWDEF;
			}	

			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "static", 6 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWSTATIC;
			}	

			//temos uma palavra reservada.
			if( strncmp( real_token_buffer, "var", 3 ) == 0  ){
				value = TOKENKEYWORD;
				keyword_found = KWVAR;
			}

            //...

            break;

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
        //case '.':

            p = token_buffer;

			if( c == '0' )
			{
				//coloca no buffer.
			    *p = c;
			    p++;

				c = getc(finput);
				if( c == 'x' || c == 'X' )
				{
					//base = 16;
					//*p++ = c; //coloca o x.

					*p = c;
			         p++;

					while(1)
					{
						c = getc (finput);

						// Se o próximo não for um digito heaxadecimal. 
						if( isxdigit(c) == 0 )
						{
							*p = 0;

							ungetc(c, finput);
							
						   //fim.
						    value = TOKENCONSTANT;
                            goto constant_done;
						}

						//coloca se é heaxa.
						//*p++ = c;
					    *p = c;
			            p++;
					};
				}

				printf("yylex: expected x");
				exit(1);
			    
			}else{
				//base = 10.

				*p++ = c; 
				while(1)
				{
					c = getc (finput);

					//se não é digito.
					if( isdigit( c ) == 0 )
					{
						//fim.
						*p = 0;
							
						ungetc(c, finput);
							
						value = TOKENCONSTANT;
                        goto constant_done;
					}

					//coloca o digito.
					*p++ = c;
				}
			};
			
			constant_done:
            break;
			

        case '\"':
        {
	        c = getc(finput);
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
		        //          lineno++;
	            //      }

	            //if (p == token_buffer + maxtoken)
	               // p = extend_token_buffer(p);
	                *p++ = c;

	            //skipnewline:
	                c = getc (finput);
	        };//while

			//finaliza a string
	        *p++ = 0;

	        //yylval.ttype = build_string (p - token_buffer, token_buffer);
	        //TREE_TYPE (yylval.ttype) = char_array_type_node;

			//avisa que é uma string ... ela vai estar no token_buffer.
	        //value = STRING; 
			value = TOKENSTRING;
			
			break;
        };	
		
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
		    p = token_buffer;
			*p++ = c;
			*p++ = 0;
			value = TOKENSEPARATOR;
		    break;


		//usadas em expressões matemáticas, 
		//#todo: não mudar isso.
        //@todo: talvez se enviarmos esses chars para o buffer ajude no debug.		
        case '+':
        case '-':
        case '&':
        case '|':
        case '<':
        case '>':
        case '*':
        case '/':
        case '%':
        case '^':
        case '!':
        case '=':
        {

            combine:

	        switch (c)
	        {
                case '+':
	                lexer_code = PLUS_EXPR; 
					break;
					
                case '-':
	                lexer_code = MINUS_EXPR; 
					break;
					
                case '&':
	                lexer_code = BIT_AND_EXPR; 
					break;
					
                case '|':
	                lexer_code = BIT_IOR_EXPR; 
					break;

                case '*':
	                lexer_code = MULT_EXPR; 
					break;

                case '/':
	                lexer_code = TRUNC_DIV_EXPR; 
					break;

                case '%':
                    lexer_code = TRUNC_MOD_EXPR; 
					break;

                case '^':
	                lexer_code = BIT_XOR_EXPR; 
					break;

                case LSHIFT:
	                lexer_code = LSHIFT_EXPR; 
					break;

	            case RSHIFT:
	               lexer_code = RSHIFT_EXPR; 
					break;
					
                case '<':
	                lexer_code = LT_EXPR; 
					break;
					
                case '>':
	                lexer_code = GT_EXPR; 
					break;
	        }	

	        c1 = getc(finput);
			
	        if (c1 == '=')
	        {
	            switch (c)
	            {
                    case '<':
		                value = ARITHCOMPARE; 
						lexer_code = LE_EXPR; 
						goto done;
                    case '>':
		                value = ARITHCOMPARE; 
						lexer_code = GE_EXPR; 
						goto done;
                    case '!':
		                value = EQCOMPARE; 
						lexer_code = NE_EXPR; 
						goto done;
                    case '=':
		                value = EQCOMPARE; 
						lexer_code = EQ_EXPR; 
						goto done;
	            }
				
	            value = ASSIGN; 
				goto done;
	        }else if (c == c1){
	                switch (c)
	                {
	                    case '+':
	                        value = PLUSPLUS; 
						    goto done;
                        case '-':
	                        value = MINUSMINUS; 
							goto done;
                        case '&':
	                        value = ANDAND; 
							goto done;
                        case '|':
	                        value = OROR; 
							goto done;
                        case '<':
	                        c = LSHIFT;
	                        goto combine;
                        case '>':
	                        c = RSHIFT;
	                        goto combine;
	                }
            }else if ((c == '-') && (c1 == '>')){ 
		         value = POINTSAT; 
				 goto done; 
		    }
	        
			ungetc (c1, finput);

            if ((c == '<') || (c == '>'))
	            value = ARITHCOMPARE;
                else value = c;
                goto done;
        };


  
        default:
            value = c;
    
    }; //switch

done:
  //yylloc.last_line = lineno;

  return (value);
};


// Vamos inicializar o lexer.
int lexerInit (){

	//number_of_tokens = 0;
	//current_token = 0;
	//next_index = 0;
	
	// ## line support ##
	
	//arquivos de texto começa com a linha 1.
	lineno = 1;
	//lexer_lineno = 0;
	lexer_firstline = 1;
	//lexer_lastline = 0;


    lexer_code = 0;


	//#todo: size
	//token_buffer = malloc((unsigned)(maxtoken+1));
	//token_buffer = malloc( 128 );
	token_buffer = &real_token_buffer[0]; 

	sprintf(real_token_buffer,"suja");

	//...

    return 0;
};


void error ( char *msg ){

    printf("error: %s\n", msg);
};
