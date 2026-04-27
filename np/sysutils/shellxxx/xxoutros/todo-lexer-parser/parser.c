/*
 * File: parser.c
 *
 * 2018 - Created by Fred Nora.
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


//interna 
int parse_def (){
	
	int c;
	
	//printf("parse_def:\n");
	
    c = yylex ();
	
	//se for um tipo, pois o modificador foi negligenciado.
	if ( c == TOKENTYPE )
	{
		printf("line %d TOKENTYPE={%s} \n", lineno, real_token_buffer );
		goto do_id;
	}else{
		
		//se não for um tipo precisa ser um modificador.
		//O modificador é uma palavra reservada.
		if ( c == TOKENKEYWORD ){
			printf("line %d (modifier) TOKENKEYWORD={%s} \n", lineno, 
			    real_token_buffer );
		}
		//goto do_type;	
	};
	
do_type:

    c = yylex ();
    
	//esperamso que seja um tipo, se não for algo deu errado,
	//ou ainda podemos usar o tipo padrão para fuções que será 'int'
	
	if( c == TOKENTYPE )
	{
		printf("line %d TOKENTYPE={%s} \n", lineno, real_token_buffer );
		
	}else{
	    //podemos usar o tipo padrão.
        printf("line %d TOKENTYPE={DEFAULT} \n", lineno );
         		
	}; 
	
do_id:	
    c = yylex ();
	if( c == TOKENIDENTIFIER)
	{
	    printf("line %d TOKENIDENTIFIER={%s} \n", lineno, real_token_buffer );	
	}else{
			//falhou.
			printf("parse_def: do_id fail");
			//exit(1);
			while(1){}		
	}
	
    //...
checkopen:
     c = yylex ();
    if( c == TOKENSEPARATOR )
    {
	    if( strncmp( (char *) real_token_buffer, "(", 1 ) == 0  )
        {
			//deu certo.
			// printf("line %d TOKENSEPARATOR={%s} \n", lineno, real_token_buffer );
		    goto done;
		}else{
			//falhou.
			printf("parse_def: checkopen fail");
			//exit(1);
			while(1){}
		};			
	}

done:	
    //printf("parse_def: done \n");
	return (int) c;
};



//interna 
int parse_var (){
	
	int c;
	
	//printf("parse_var:\n");
	
    c = yylex ();
	
	//se for um tipo, pois o modificador foi negligenciado.
	if( c == TOKENTYPE )
	{
		printf("line %d TOKENTYPE={%s} \n", lineno, real_token_buffer );
		goto do_id;
	}else{
		
		//se não for um tipo precisa ser um modificador.
		//O modificador é uma palavra reservada.
		if( c == TOKENKEYWORD )
	    {
			printf("line %d (modifier) TOKENKEYWORD={%s} \n", lineno, real_token_buffer );
             			
		}
		//goto do_type;	
	};	
	
do_type:
    c = yylex ();
    //esperamso que seja um tipo, se não for algo deu errado,
	//ou ainda podemos usar o tipo padrão para fuções que será 'int'
	if( c == TOKENTYPE )
	{
		printf("line %d TOKENTYPE={%s} \n", lineno, real_token_buffer );
		
	}else{
	    //podemos usar o tipo padrão.
        printf("line %d TOKENTYPE={DEFAULT} \n", lineno );
         		
	}; 
	
do_id:
	c = yylex ();
	//identifier para a variável.
	if( c == TOKENIDENTIFIER)
	{
	    printf("line %d TOKENIDENTIFIER={%s} \n", lineno, real_token_buffer );	
	}else{
			//falhou.
			printf("parse_var: do_id fail");
			//exit(1);
			while(1){}		
	};
	
	//## todo
	//aqui esperamos que a variável possa ter sido inicializada com uma expressão 
	// ou com uma constante.
	
//do_exp:
	//@todo
	
do_separator:
    c = yylex ();
    
	//separador ';'
	if( c == TOKENSEPARATOR )
    {
	    if( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
        {
			//deu certo.
			//printf("line %d TOKENSEPARATOR={%s} \n", lineno, real_token_buffer );
		    goto done;
		}else{
			//falhou.
			printf("parse_var: separador fail");
			//exit(1);
			while(1){}
		};	
		
	}else{
			//falhou.
			printf("parse_var: expected separator ;");
			//exit(1);
			while(1){}		
	};	
	
done:	
    //printf("parse_var: done \n");
    return c;	
};


//interna 
int parse_return (){
	
	int c;
	
	//printf("parse_var:\n");
	
	
	//pegaremos o tipo e a exp.
	
do_constant:
	c = yylex ();
	//identifier para a variável.
	if( c == TOKENCONSTANT)
	{
	    printf("line %d TOKENCONSTANT={%s} \n", lineno, real_token_buffer );	
	}else{
			//falhou.
			printf("parse_return: do_constant fail");
			//exit(1);
			while(1){}		
	};
	
	
	
do_separator:
    c = yylex ();
    
	//separador ';'
	if( c == TOKENSEPARATOR )
    {
	    if( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
        {
			//deu certo.
			//printf("line %d TOKENSEPARATOR={%s} \n", lineno, real_token_buffer );
		    goto done;
		}else{
			//falhou.
			printf("parse_return: do_separator: fail");
			//exit(1);
			while(1){}
		};	
		
	}else{
			//falhou.
			printf("parse_return: expected separator ;");
			//exit(1);
			while(1){}		
	};	
	
done:		
   return c;
};


/*
 * parse:
 *     Pegando tokens com o lexer e fazendo coisas ...
 */
int parse (){
	
	register int c;
	int i;
	
	size_t size = (size_t) strlen ( (const char *) stdin->_base );
	
	printf("\n");
	
	for ( i=0; i<size; i++ )
	{
	    //pega o char.
	    c = yylex ();
		
		//#debug
	    //printf("%c", c );
		
	    next:	
        
		switch (c)
		{
			case TOKENKEYWORD:
			    //printf("line %d TOKENKEYWORD={%s} \n", lineno, real_token_buffer );
			    
				//se a kw for 'def' chamamo parse_def() que retorna só quando 
				//encontrar um '(', onde começará os argumentos.
				if( keyword_found == KWDEF )
				{
					printf("line %d TOKENKEYWORD={%s} \n", lineno, real_token_buffer );
					// parse deve retornar o '(' quando encontrá-lo.
					c = parse_def();
					if ( c == TOKENSEPARATOR){
						goto next;
					}
					printf("case TOKENKEYWORD KWDEF fail;");
					while(1){}
					break;
				}
				
				//e a kw var foi encontrada, chamaremos parse_var(),
				//que retornará quando encontrar ';'.
				if( keyword_found == KWVAR )
				{
					printf("line %d TOKENKEYWORD={%s} \n", lineno, real_token_buffer );
					// parse deve retornar o ';' quando encontrá-lo.
					c = parse_var();
					if( c == TOKENSEPARATOR){
						goto next;
					}
					printf("case TOKENKEYWORD KWVAR fail;");
					while(1){}
					break;
				}	

                //return. Chamaremos o tratador do stmt parse_return() 
				if( keyword_found == KWRETURN )
				{
					printf("line %d TOKENKEYWORD={%s} \n", lineno, real_token_buffer );
					// parse deve retornar o '(' quando encontrá-lo.
					c = parse_return();
					if( c == TOKENSEPARATOR){
						goto next;
					}
					printf("case TOKENKEYWORD KWRETURN fail;");
					while(1){}
					break;
				}

                //default....
				
				break;
				
			case TOKENTYPE:
			    id[ID_TYPE] = type_found;
				printf("line %d TOKENTYPE={%s} \n", lineno, real_token_buffer );
			    break;
				
				
			case TOKENIDENTIFIER:
			    id[ID_TOKEN] = TOKENIDENTIFIER;
				id[ID_STACK_OFFSET] = stack_index;
				printf("line %d TOKENIDENTIFIER={%s} \n", lineno, real_token_buffer );
                break;

			case TOKENSTRING:
			    printf("line %d TOKENSTRING={%s} \n", lineno, real_token_buffer );
                break;

			case TOKENSEPARATOR:
			    printf("line %d TOKENSEPARATOR={%s} \n", lineno, real_token_buffer );
                break;

			case TOKENCONSTANT:
                printf("line %d TOKENCONSTANT={%s} \n", lineno, real_token_buffer );
				break;

			case ARITHCOMPARE:
                printf("line %d ARITHCOMPARE lexer code %d  \n", lineno, lexer_code );
				break;

			case EQCOMPARE:
                printf("line %d EQCOMPARE lexer code %d \n", lineno, lexer_code );
				break;
				
			case ENDFILE:
                printf("line %d EOF reached.\n", lineno);  	
				break;			
				
			//...
            
            default:
                break;			
		};
	
		//inicializa.
        c = 0;		
	};
	
	//#debug
	printf("\n");
	printf("stdin_base: %s\n",stdin->_base);
	printf("number of lines: %d \n",lineno);
	//...
	
hang:	
	printf("parse hang");
	    while (1){
			
			asm ("pause");
		}
		
	return 0;
} 


/*
 ***********************************
 * parserInit:
 *     Initializing parser.
 */
int parserInit (){
	
	register int i;
	
	//infile_size = 0;
	//outfile_size = 0;
	
    //stack support
	stack_flag = 0;
	stack_count = 0;
	stack_index = 0;
	
	//id support
	
	
	for ( i=0; i<512; i++ )
		stack[i] = 0;
	
    
	for ( i=0; i<8; i++ )
		id[i] = 0;

	
	//esses endereços vão depender do arquivo de configuração do 
	//linker ...
	//#test: default em 0.
    program_header_address =     0;
    program_text_address   =   100;
    program_data_address   = 2*100;
    program_bss_address    = 3*100;	
	
	//...
	
	return (int) 0;
};

