/*
 * File: parser.c
 * 2018 - Created by Fred Nora.
 */

#include "gramcnf.h"

// -------------------------------------
// Getting the string from "asm()" and saving here
// as a structured data.

int meta_index=0;
size_t string_size=0;
int meta_stage=0;
// -------------------------------------

// -----------------------
// Elementos que explicam o identificador.
// See: token.h
//int id[ID_MAX];
//#test: 
// Usaremos long porque estamos armazenando endereços de memoria também.
long id[ID_MAX];

// -----------------------
// Elementos que explicam a constante.
long constant[CONSTANT_MAX];


// Salvando a string das constantes,
// onde a constante será armazenada dependendo do tipo.
char constant_byte[2];    //0xFF
char constant_word[4];    //0xFFFF
char constant_dword[8];   //0xFFFFFFFF
char constant_qword[16];  //0xFFFFFFFFFFFFFFFF
// O que colocar antes dessa constante.
// Isso varia com a base. Ex: 0x, 0X
char constant_before[2];
// O que colocar depois dessa constante.
// Isso varia com a base. Ex: H, B.
char constant_after[2];

// -----------------------
long return_info[8];

unsigned long functionList[FUNCTION_COUNT_MAX];

// Espiada, olhadinha.
// #importante
// esquema
// Um símbolo vam seguido de um char.
// a ação depende dessa combinação.
// peekSymbol >>> peekChar
int peekSymbol=0;
int peekChar=0;

char function_main_buffer[512];
char save_symbol[32];


//
// -- Private: Prototypes --------
//

static int __parserInit(void);
// Functions
static int parse_function(int token);
// Statements

// name/content
static int parse_name(int token);
static int parse_content(int token);

static int parse_do(int token);
static int parse_for(int token);
static int parse_if(int token);
static int parse_return(int token);
static unsigned long parse_sizeof(int token);
static int parse_while(int token);
//static int parse_number(int olen);
// Expression
static unsigned long parse_expression(int token);
// Emit
static void emit_label(void);
static void emit_function(void);

//
// -------------------------------------
//

static void emit_label(void)
{
    strcat (TEXT,";[LABEL]\n");
    strcat (TEXT,"segment .text\n");
    strcat (TEXT,"_");
    strcat (TEXT,save_symbol);
    strcat (TEXT,":\n");
}

static void emit_function(void)
{
    strcat (TEXT,";[FUNCTION] (\n");
    strcat (TEXT,"segment .text \n");
    strcat (TEXT,"_");
    strcat (TEXT,save_symbol);
    strcat (TEXT,":\n");
}

// Parse function.
static int parse_function(int token)
{
    int c=0;
    int running = 1;
    int State = 1;

//#ifdef PARSER_FUNCTION_VERBOSE
	//debug
	//printf("parse_function: Initializing ...\n");
//#endif

// Se entramos errado.
    if (token != TK_IDENTIFIER){
        printf ("parse_function: Can't initialize function statement\n");
        exit(1);
    }

// #bugbug: We don't need this IF statemente here.
    if (token == TK_IDENTIFIER)
    {
        id[ID_TOKEN] = TK_IDENTIFIER;
        id[ID_STACK_OFFSET] = stack_index;

	    //	printf ("parse_function: TK_IDENTIFIER={%s} in line %d\n", 
	    //	    real_token_buffer, lexer_currentline );
    }

    while (running)
    {
        c = yylex();

        switch (State)
        {
            // State 1 
            // Esperamos um separador '('
            // pois isso é uma função e não uma definação de variável.
            case 1:
                switch (c)
                {
                    case TK_SEPARATOR:
                        if ( strncmp( (char *) real_token_buffer, "(", 1 ) == 0  )
                        {
                            // printf ("parse_function: TK_SEPARATOR={%s} in line %d\n", 
	                        //     real_token_buffer, lexer_currentline );
                            State=2;
                        }
                        break;

                    default:
                        printf("parse_function: State1 Missed '(' separator in line %d\n", 
                            lexer_currentline );
                        exit(1);
                        break;
                };
                break;

            // State 2
            // Esperamos aqui tipos, 
            // símbolos , separador ',' ou o separador ')'.
           case 2:
                switch (c)
                {
                    // ')'
                    case TK_SEPARATOR:
                        if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0 )
                        {
	                        // printf ("parse_function: TK_SEPARATOR={%s} in line %d\n", 
	                        //    real_token_buffer, lexer_currentline ); 
                            State=3;
                        }
                        break;

                    //case type 
                    //case symbol 
                    //case ','
                    //...

                    default:
                        printf("parse_function: State2 something wrong in line %d\n", 
                            lexer_currentline );
                        exit(1);
                        break;
                };
                break;

            // State 3
            // Esperamos aqui o separador final ';'
            // isso finaliza o statement 'function'
            case 3:
                switch (c)
                {
                    // ';'
                    // Terminamos o statement function.
                    case TK_SEPARATOR:
                        if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0 )
                        {
	                        // printf ("parse_function: TK_SEPARATOR={%s} in line %d\n", 
	                        //     real_token_buffer, lexer_currentline ); 
                            return (int) TK_SEPARATOR;
                        }
                        break;

                    default:
                        printf("parse_function: State3 Expected separator ';' in line %d\n",
                            lexer_currentline );
                        exit(1);
                        break;
                };
                break;

            // Default State.
            default:
                printf("parse_function: default statement\n");
                exit(1);
                break;
        };
    };

    return 0;
}

// Parse asm statement.
static int parse_name(int token)
{
// #todo:
// "asm" pode virar um visualizador de strings.

    int c=0;
    int running = 1;
    int State = 1;
    int inside = 0;

    //debug
    //printf("parse_asm: Initializing ...\n");

// Se entramos errado.
    if (token != TK_KEYWORD){
        printf ("parse_name: token error\n");  exit(1);
    }
    if (token == TK_KEYWORD){
        // printf("parse_name: TK_KEYWORD={%s} in line %d\n", 
        //     real_token_buffer, lexer_currentline );
    }

//
// (
//

    c = yylex ();
    if (c != TK_SEPARATOR){
        printf("parse_name: expected (\n");  exit(1);
    }
    if (c == TK_SEPARATOR){
        if ( strncmp( (char *) real_token_buffer, "(", 1 ) == 0  )
        {
            // printf("parse_name: TK_KEYWORD={%s} in line %d\n", 
            //     real_token_buffer, lexer_currentline ); 
            //ok
            inside = 1;
        }
    }

//
// String?
// " .... "
//

    c = yylex();
    if (c != TK_STRING){
        printf("parse_name: expected string in name("")\n");  exit(1);
    }
    if (c == TK_STRING)
    {
        //if ( strncmp( (char *) real_token_buffer, "\"", 1 ) == 0 ){
            //ok
            //inside = 1;
        //} 

        // #test
        // Visualizar a string, pois "asm" pode ser usado 
        // para manipular strings por outros motivos.
        // Isso pode ser algum tipo de dado vindo 
        // de um arquivo de configuração.
        //printf ("name-string: {%s}\n",
          //  real_token_buffer );
        
        
        //Stage 0: Get the name string
        if (meta_stage == 0)
        {
            string_size = (size_t) strlen(real_token_buffer);
            if (string_size <= 0 ){
                printf("string size min\n");
                goto error0;
            }
            if (string_size >= 64 ){
                printf("string size max\n");
                goto error0;
            }
            memset(metadata[meta_index].name, 0, 64);
            strncpy(
                metadata[meta_index].name,
                real_token_buffer,
                string_size );
            metadata[meta_index].name_size = string_size;
            
            // Next stage.
            meta_stage++;
        }

        // Coloca a string no arquivo de saída.
        strcat( outfile, real_token_buffer );
        // Ao fim da string vamos para a próxima linha do output file
        strcat( outfile,"\n");

        c = yylex();

        //)
        if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0 )
        {
            inside = 0;
            c = yylex();
            // ;
            if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0 )
            {
                // ok
                return (int) c;
            }
            printf("parse_name: expected ; in name string\n");
            exit(1);
        }

        printf("parse_name: expected ) in name string\n");
        exit(1);
    }

error0:
    printf ("parse_name: todo unexpected error in name string\n");
    exit(1);
    return -1;
}


// Parse asm statement.
static int parse_content(int token)
{
// #todo:
// "asm" pode virar um visualizador de strings.

    int c=0;
    int running = 1;
    int State = 1;
    int inside = 0;

    //debug
    //printf("[CONTENT]\n");

// Se entramos errado.
    if (token != TK_KEYWORD){
        printf ("parse_content: token error\n");  exit(1);
    }
    if (token == TK_KEYWORD){
        // printf("parse_content: TK_KEYWORD={%s} in line %d\n", 
        //     real_token_buffer, lexer_currentline );
    }

//
// (
//

    c = yylex ();
    if (c != TK_SEPARATOR){
        printf("parse_content: expected (\n");  exit(1);
    }
    if (c == TK_SEPARATOR){
        if ( strncmp( (char *) real_token_buffer, "(", 1 ) == 0  )
        {
            // printf("parse_content: TK_KEYWORD={%s} in line %d\n", 
            //     real_token_buffer, lexer_currentline ); 
            //ok
            inside = 1;
        }
    }

//
// String?
// " .... "
//

    c = yylex();
    if (c != TK_STRING){
        printf("parse_content: expected string in content("")\n");  exit(1);
    }
    if (c == TK_STRING)
    {
        //if ( strncmp( (char *) real_token_buffer, "\"", 1 ) == 0 ){
            //ok
            //inside = 1;
        //} 

        // #test
        // Visualizar a string, pois "asm" pode ser usado 
        // para manipular strings por outros motivos.
        // Isso pode ser algum tipo de dado vindo 
        // de um arquivo de configuração.
        //printf ("content-string: {%s}\n", real_token_buffer );

        //Stage 1: Get the content string
        if (meta_stage == 1)
        {
            string_size = (size_t) strlen(real_token_buffer);
            if (string_size <= 0 ){
                printf("string size min\n");
                goto error0;
            }
            if (string_size >= 128){
                printf("string size max\n");
                goto error0;
            }
            memset(metadata[meta_index].content, 0, 128);
            strncpy(
                metadata[meta_index].content,
                real_token_buffer,
                string_size );
            metadata[meta_index].content_size = string_size;
            metadata[meta_index].initialized = TRUE;
            //printf("INITIALIZED\n");
            
            // Salva o index.
            metadata[meta_index].id = (int) meta_index;
            
            // Só muda de index depois de 2 strings.
            meta_index++;
            if (meta_index >= 32){
                printf("meta_index limits\n");
                goto error0;
            }

            // Come back to first stage.
            meta_stage=0;
        }

        // Coloca a string no arquivo de saída.
        strcat( outfile, real_token_buffer );
        // Ao fim da string vamos para a próxima linha do output file
        strcat( outfile,"\n");

        c = yylex();

        //)
        if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0 )
        {
            inside = 0;
            c = yylex();
            // ;
            if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0 )
            {
                // ok
                return (int) c;
            }
            printf("parse_content: expected ; in content string\n");
            exit(1);
        }

        printf("parse_content: expected ) in content string\n");
        exit(1);
    }

error0:
    printf ("parse_content: todo unexpected error in comtent string\n");
    exit(1);
    return -1;
}



// Parse do statement.
static int parse_do(int token)
{
    printf("todo: parse_do in line %d\n", lexer_currentline );
    exit(1);
    return -1;
}

// Parse for statement.
static int parse_for(int token)
{
    printf ("todo: parse_for in line %d\n", lexer_currentline );
    exit(1);
    return -1;
}

// Parse if statement.
static int parse_if(int token)
{
    int c=0;
    int If_Result = -1;
    unsigned long Exp_Result=0;

    printf("todo: parse_if in line %d\n", lexer_currentline );

// #todo
// Conferir se o token do argumento é um if 
// #todo 
// Temos que criar State para if.

// Pega o próximo que deve ser um (.

    c = yylex();
    if (c != TK_SEPARATOR){
        printf ("parse_if separator missed\n");  exit(1);
    }

// Expression.
// Testando chamar uma análise de expressão dentro do statement de if.

    Exp_Result = parse_expression(c);

// #importante 
// Retornamos 1 ou 0 da análise da expressão,
// quem chamou o if vai armazenar esse valor para chamar o else.

    If_Result = (int) Exp_Result;

    //#debug
    printf ("EXP={%d}\n",Exp_Result);

// '{'
    c = yylex();
    if (c != TK_SEPARATOR){
        printf ("parse_if separator { missed\n");  exit(1);
    }
// '}'
    c = yylex();
    if (c != TK_SEPARATOR){
        printf ("parse_if separator } missed\n");  exit(1);
    }
// ';'
    c = yylex();
    if (c != TK_SEPARATOR){
        printf ("parse_if separator ; missed\n");  exit(1);
    }

    //exit(1);
    return (int) If_Result;
}

// Parse number statement.
// Literal?
/*
static int parse_number(int olen)
{
    //pointer #todo	
    //register char *p = lexptr;
    register char *p = token_buffer;
    register long n = 0;
    register int c;
    register int base = 10;
    register len = olen;
    char *err_copy;

    //#todo função importada.
	extern double atof ();

    for (c = 0; c < len; c++)
	{
        //se encontrarmos um ponto no meio dos números.
		//pois não tem ponto no meio de uma expressões no  
		//#if (ainda não sei se gcc está falando de diretivas ou statement)
		if ( p[c] == '.' )
		{
            // It's a float since it contains a point.  
            //yyerror ("floating point numbers not allowed in #if expressions");
			printf ("floating point numbers not allowed in #if expressions");
            return ERROR;
      
            // ****************
	        //yylval.dval = atof (p);
	       // lexptr += len;
	       // return FLOAT;
		   // ****************  
        }
    }
  
//se o comprimeiro for mior que 3.
//se começar com '0x', ou '0X', indicando se um número heaxadecimal.
//então a base será 16.
	if ( len >= 3 && 
	     ( !strncmp (p, "0x", 2) || !strncmp (p, "0X", 2)) ) 
	{
		//baes hexa.
		base = 16;
        
		//pegaremos depois do x.
		p += 2;
		
		//atualiza o comprimeito.
		len -= 2;
    
	
	//Se o número começar com '0', então temos octal.
	
	}else if ( *p == '0' )
	      {
			base = 8;  
		  };

	while (len-- > 0) 
	{
        c = *p++;
        n *= base;
        
		//se o caractere for um dígito decimal.
		if (c >= '0' && c <= '9')
		{
            n += c - '0';
        
		}else{
			
            if (c >= 'A' && c <= 'Z') 
				c += 'a' - 'A';
            
			if (base == 16 && c >= 'a' && c <= 'f'){
				
	            n += c - 'a' + 10;
				
            }else if (len == 0 && c == 'l'){
				
	            ; //nothing

            }else{
	            //yyerror ("Invalid number in #if expression");
				printf ("Invalid number in #if expression");
	            return ERROR;
            }
        };
    };

    lexptr = p;
    yylval.lval = n;
    return INT;
};
*/

// Parse return statement.
// >> termina com  ';'
// return 0;
// return (int) 1;
// return 1+2;
// return (1+2);
// return (int) (1+2);
// return function();
// return (int) function();
static int parse_return(int token)
{
    int c=0;
    int running = 1;
    int State = 1;
    int open = 0;
    unsigned long eval_ret=0;
    char buffer[32];
    //char *buffer;

// #debug
    printf ("parse_return:\n");

// Se entramos errado.
    if ( token != TK_KEYWORD || 
         keyword_found != KWRETURN )
    {
        printf ("parse_return: Can't initialize return statement\n");
        exit(1);
    }

// #obs:
// Isso significa que o token atual é uma keyword 'return'.
// Se a próxima keyword for um ';' então não temos uma expressão.

// Eval

    eval_ret = (unsigned long) tree_eval();

// itoa
    itoa ( (int) eval_ret, buffer );
    // ??
    //buffer = (char *) itoa ( (int) eval_ret);

    // #debug
    printf("parse_return: value={%d}\n",eval_ret);
    printf("parse_return: value={%s}\n",buffer);

//
// Output
//

    // emit_return();
    //strcat ( TEXT,";[RETURN]\n");
    //strcat ( TEXT,"  mov eax, ");
    //strcat ( TEXT, buffer );
    //strcat ( TEXT,"\n  ret \n\n");

    //strcat ( outfile,"  mov eax, ");
    //strcat ( outfile, buffer );
    //strcat ( outfile,"\n  ret \n\n");

// O ultimo token em um return statement foi ';'
// vamos conferir
// #bugbug: The routine above is returning after find the ';'.
    if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0 )
    {
        printf("parse_return: ';' found\n");
        c = TK_SEPARATOR;
        return c;
    }

//
// Debug hang
//

// #debug
    printf ("parse_return: #breakpoint: ';' not found!\n");
    while(1){}

//#obs 
//supendemos todo o resto abaixo por enquanto.

/*
	while (running)
	{
		c = yylex ();
		
		switch ( State )
		{
			
			//#state1
		    //esperamos constante, expressão, tipo ou função.
			case 1:
			    switch(c)
				{
					//se encontramos a constante, o que segue é o separador ';' ou o separador ')'
					case TK_CONSTANT:
#ifdef PARSER_VERBOSE
						//ok;
						printf ("parse_return: State1 TK_CONSTANT={%s} line %d\n", 
						    real_token_buffer, lexer_currentline );
#endif

					    constant[CONSTANT_TOKEN] = TK_CONSTANT;
						constant[CONSTANT_TYPE] = constant_type_found;
						constant[CONSTANT_BASE] = constant_base_found;

						//#todo: fazer um switch para tipos válidos.
						strcat( outfile,"  mov eax, ");
						strcat( outfile, real_token_buffer );
						strcat( outfile,"\n  ret \n\n");
                        //strcat( outfile,"");							
						
						// se parênteses aberto.
						//if ( open == 1 )
						//{
						//    struct tree_node *n = (struct tree_node *) create_tree_node ( ?, NULL, NULL );	
						//}
						
						//Se não temos um parêntese aberto então vamos para o próximo
						//que deverá ser um ';'
						
						if ( open == 0 )
						{
						    State = 2;	
						}
						
						//mas se ainda temos um parentese aberto então
						//encontramos uma constante dentro do parêntese,
						//indicando que estamos um uma expressão.
						break;

					case TK_SEPARATOR:

						//iniciamos uma expressão ou condicional.
						//uma árvore.
						if ( strncmp( (char *) real_token_buffer, "(", 1 ) == 0  )
                        {
						    open = 1;
						}
						
						//se fecharmos, o que segue pode ser um separador ';',
						//uma função, uma constante ou uma expressão.
	                    if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0  )
						{
						    open = 0;							
						}
						
						//se o separador for ';' é porque estamos num retorno do tipo void.
						//se o retorno não for do tipo void então foi erro de sintaxe..
						if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
						{
							
#ifdef PARSER_RETURN_VERBOSE							
							printf("parse_return: separator ';' in line %d\n", 
							    lexer_currentline );
#endif							
							
						    if (open == 1)
                            {
								printf("parse_return: State1 wrong separator ';' in line %d\n", lexer_currentline);
								exit(1);
							}
							
                            //retorno do tipo void.
							if (open == 0)
                            {
								strcat( outfile,"\n  ret \n\n");
								return (int) TK_SEPARATOR;
                            }
						}
						break;

				    //case identificador. (símbolo)
					//significa que o return foi seguido de uma função.
					case TK_IDENTIFIER:
					    //#todo Nesse momento podemos chamar a rotian qu trata uma função 
						//function statement. function_parser
						
						// ';' foi encontrado.
						//finalizado o statement de função então vamos sair do statemente de return.
						//pois o ';' da função é o mesmo do return.
						//se estivermos após a keyword 'return' ou após o tipo '(int)'.
						if ( open == 0 )
						{
                            //vamos analizar uma chamada de função dentro de um statement de return.							
						    return (int) parse_function ( TK_IDENTIFIER );
						}
						
						//#importante
						//temos um identificador dentro do parênteses.
						//Se uma função foi chamada dentro do parênteses não devemos esperar 
						//por um separador ';'
						if ( open == 1 )
						{
						    //todo	
						}
						break;		
						
					default:
				        printf("parse_return: State1 default\n");
				        exit(1);
					    break;
				}
                break;

			
			//#state2
			// separador ';'
			case 2:
			    switch(c)
				{
					case TK_SEPARATOR:
	                    if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
                        {
#ifdef PARSER_RETURN_VERBOSE
			                //deu certo.
			                printf ("parse_return: State2 do_separator TK_SEPARATOR={%s} line %d \n", 
							    real_token_buffer, lexer_currentline );
#endif		                    
							//Se encontramos o separador que finaliza o statement
							//então podemos retornar.
							return (int) TK_SEPARATOR;
		                }	
					    break;
						
					default:
				        printf("parse_return: State2 default\n");
				        exit(1);
					    break;
				}
			    break;
				
			//Statemente não enumerado.
			default:
				printf("parse_return: default statement\n");
				exit(1);			
			    break;
		};
	};
	
	//printf("parse_var:\n");
	
	//pegaremos o tipo e a exp.
	
do_constant:

	c = yylex ();
	

	if (c == TK_CONSTANT)
	{
		
#ifdef PARSER_RETURN_VERBOSE		
	    printf ("parse_return: do_constant TK_CONSTANT={%s} line %d\n", 
		    real_token_buffer, lexer_currentline );	
#endif
		constant[CONSTANT_TOKEN] = TK_CONSTANT;
		constant[CONSTANT_TYPE] = constant_type_found;
		constant[CONSTANT_BASE] = constant_base_found;
	
	} else {
			//falhou.
			printf("parse_return: do_constant Expacted constant on line %d", lexer_currentline);
			exit(1);
			//while(1){}		
	};
	
	
	
do_separator:

    c = yylex ();
    
	//separador ';'
	//isso finaliza o statement.
	if (c == TK_SEPARATOR)
    {
	    if( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
        {

#ifdef PARSER_RETURN_VERBOSE
			//deu certo.
			printf("parse_return: do_separator TK_SEPARATOR={%s} line %d \n", 
			    real_token_buffer, lexer_currentline );
#endif		  
		  
			goto done;
		}else{
			//falhou.
			printf("parse_return: do_separator: fail");
			exit(1);
			//while(1){}
		};	
		
	}else{
			//falhou.
			printf("parse_return: do_separator Expected ';' on line %d",lexer_currentline);
			exit(1);
			//while(1){}		
	};

*/

done:
   return c;
}

// Parse 'sizeof' statement.
static unsigned long parse_sizeof(int token)
{
    unsigned long Result=0;
    int c = token;
    int State = 1;
    int running = 1;

    if (c != TK_KEYWORD){
        printf("parse_sizeof: fail");  exit(1);
    }
 
    while (running)
    {
        c = yylex(); 
        again:
        switch (State)
        {
            // State 1
            // #todo
            // (
            case 1:
                printf("parse_sizeof: State 1\n");
                if (c == TK_SEPARATOR){
                    State=2;  break;
                }
                printf("parse_sizeof: State 1 fail");
                exit(1);
                break;

            // State 2
            // #todo
            // tipo, símbolo, etc ...
            case 2:
				printf("parse_sizeof: State 2\n");
				//#todo modifier 
				//if ...
			    if (c == TK_TYPE)
				{
					switch (type_found)
					{
						case TNULL:  Result = sizeof(0);      break;
                        case TVOID:  Result = sizeof(void);   break;
						case TCHAR:  Result = sizeof(char);   break;  //1
						case TSHORT: Result = sizeof(short);  break;  //2
                        case TINT:   Result = sizeof(int);    break;  //4
						case TLONG:  Result = sizeof(long);   break;  //8
                        default:
                            printf("parse_sizeof: State 2 unexpected type found in line %d\n", 
                                lexer_currentline);
                            break;
                    }
                    State=3;
                    break;
                }

                printf("parse_sizeof: #TODO State 2 unexpected element on sizeof in line %d\n", 
                    lexer_currentline);
                exit(1);
                break;

            // State 3
            // #todo
            // )
            case 3:
                printf("parse_sizeof: State 3\n");
                if (c == TK_SEPARATOR)
                {
					goto done;
					//State = 2;
                    break;
                }
                printf("parse_sizeof: State 3 fail\n");
                exit(1);     
                goto done;
                break;

            // Default State.
            default:
                printf("parse_sizeof: default State\n");
                exit(1);                
                break;
        };
    };
done:
    return (unsigned long) Result;
}

// Parse while statement.
static int parse_while(int token)
{
    int c=0;
    int While_Result = -1;
    unsigned long Exp_Result=0;

    printf("todo: parse_while in line %d\n", lexer_currentline );

// #todo
// conferir se o token do argumento é um while

// Pega o próximo que deve ser um (
    c = yylex();
    if (c != TK_SEPARATOR){
        printf ("parse_while separator missed\n");
        exit(1);
    }

// Testando chamar uma análise de expressão 
// dentro do statement de while.
    Exp_Result = parse_expression ( c );
    While_Result = (int) Exp_Result;

// '{'
    c = yylex();
    if (c != TK_SEPARATOR){
        printf ("parse_while separator { missed\n");  exit(1);
    }
// '}'
    c = yylex();
    if (c != TK_SEPARATOR){
        printf ("parse_while separator } missed\n");  exit(1);
    }
// ';'
    c = yylex();
    if (c != TK_SEPARATOR){
        printf ("parse_while separator ; missed\n");  exit(1);
    }

    printf ("EXP={%d}\n",Exp_Result);

    //exit(1);
    return (int) While_Result;
}

// Parse expression.
// Analizando uma expressão.
// Temos a questão de predecessores para os operadores.
static unsigned long parse_expression(int token)
{

    //#todo tree
    
    int c=0;
    int eCount=0;
// Operator
    int Op=0;
    unsigned long sizeof_constant=0;
    int sizeof_flag = 0;
// e1=x  e2=y
    unsigned long e[2];
    unsigned long Result=0;

    c = token;

// Primeiro analizamos o que veio via argumento 
// assim checaremos a validade da expressão e
// abortaremos logo cedo.
// uma expressão pode começar com (, operador, número ...
// então vamos ver qual é o primeiro token da expressão.

    switch (c)
    {
        // (
        case TK_SEPARATOR:
            if ( strncmp( (char *) real_token_buffer, "(", 1 ) == 0  )
            {
                // printf("parse_expression: TK_SEPARATOR={%s} in line %d\n", 
                //     real_token_buffer, lexer_currentline ); 
                //State = 2;
            }
            break;
        default:
            // printf ("parse_expression: State 1 Missed '(' separator in line %d \n", 
            //     lexer_currentline );
            exit(1);
            break;
    }

// #todo:
// #importante:
// estamos suspendendo a parte de baixo e chamado uma rotina 
// em tree para calcular a expressão.
    return (unsigned long) tree_eval();

// ===========================================================

	/*
	int running = 1;
	int State = 1;
	
//agora pegaremos os outros elementos da expressão.

    while (running == 1)
    {
	    c = yylex ();
		
		//voltando sem pegar.
		again:
		
		if (c == TK_EOF)
		{
            printf("parse_expression: #error EOF \n");
            exit(1);			
		}		
	    
		switch (State)   
	    {
			
			//===================
			//State 1
			//Aceitamos constantes ou identificadores.
			//não vamos começar com operadores e sim com operandos,
			//que pdoem ser números ou identificadores.
			case 1:
			    switch(c)
				{
					
					case TK_SEPARATOR:
	                    if ( strncmp( (char *) real_token_buffer, "(", 1 ) == 0  )
						{
#ifdef PARSER_EXPRESSION_VERBOSE
						    printf("parse_expression: TK_SEPARATOR={%s} in line %d\n", 
							    real_token_buffer, lexer_currentline ); 
#endif							
							//State = 1;	
						    break;
						}
	                    if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0  )
						{
							
#ifdef PARSER_EXPRESSION_VERBOSE
						    printf("parse_expression: TK_SEPARATOR={%s} in line %d\n", 
							    real_token_buffer, lexer_currentline );
#endif								
							//State = 1;
                            //provisório
							//retornaremos, mas falta o corpo {...};
							goto expression_exit;
							break;
						}
					    break;

					case TK_CONSTANT:
						//ok;
#ifdef PARSER_EXPRESSION_VERBOSE
						printf ("parse_expression: State1 TK_CONSTANT={%s} line %d\n", 
						    real_token_buffer, lexer_currentline );
#endif

						switch(eCount)
						{
							//primeiro elemento
							//esperamos uma constante
							case 0:

#ifdef PARSER_EXPRESSION_VERBOSE
							    printf("eCount-case-0:\n");
#endif								
								//se estamos lidando com a contante gerada pelo sizeof.
								if( sizeof_flag == 1 )
								{
									e[0] = sizeof_constant;
									sizeof_flag = 0;
									
								}else{
									e[0] = (unsigned long) atoi (real_token_buffer);
								};

								eCount++;
								//vamos para o tratador de operadores.
								State = 2;
								break;
								
							//segundo elemento	
							//error: o operador é tratado em outro case.
							case 1:
#ifdef PARSER_EXPRESSION_VERBOSE
						        printf("eCount-case-1:\n");
								printf ("parse_expression: State1 error unexpected operator element TK_CONSTANT={%s} line %d\n", 
						            real_token_buffer, lexer_currentline );
#endif                                
								exit(1); 
							    break;

							//terceito elemento	
							//esperamos uma constante
							case 2:
#ifdef PARSER_EXPRESSION_VERBOSE
							    printf("eCount-case-2:\n");
#endif							    
								//se estamos lidando com a contante gerada pelo sizeof.
								if( sizeof_flag == 1 )
								{
									e[1] = sizeof_constant;
									sizeof_flag = 0;
									
								}else{
									e[1] = (unsigned long) atoi (real_token_buffer);
								};
								eCount = 0;
								switch (Op)
								{
								    case PLUS_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] + (unsigned long) e[1] ); 									
								        break;

								    case MINUS_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] - (unsigned long) e[1] ); 									
								        break;

								    case BIT_AND_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] & (unsigned long) e[1] ); 									
								        break;

								    case BIT_IOR_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] | (unsigned long) e[1] ); 									
								        break;	

								    case MULT_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] * (unsigned long) e[1] ); 									
								        break;
										
								    case TRUNC_DIV_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] / (unsigned long) e[1] ); 									
								        break;
										
								    case TRUNC_MOD_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] % (unsigned long) e[1] ); 									
								        break;
										
								    case BIT_XOR_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] ^ (unsigned long) e[1] ); 									
								        break;
										
								    case LSHIFT_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] << (unsigned long) e[1] ); 									
								        break;
										
										
								    case RSHIFT_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] >> (unsigned long) e[1] ); 									
								        break;
										
								    case LE_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] <= (unsigned long) e[1] ); 									
								        break;
										
								    case GE_EXPR:
                                        Result = (unsigned long) ( (unsigned long) e[0] >= (unsigned long) e[1] ); 									
								        break;
										
									//...	
									
									default:
                                        printf("parse_expression: State1 default Op={%d}",Op);
										exit(1);
										break;
								};
								
								// ( 1 + 2 ')'
								//vamos ver se o que vem depois da segunda constante é um esperado separador 
								//ou outracoisa.
								c = yylex ();
								
								//Ok temos um sepador
								//vamos tratá-lo
								if (c == TK_SEPARATOR)
								{
									State = 1;
								    goto again;
								}else{
									printf("expected separator in line %d", lexer_currentline);
									exit(1);
								}

								//vamos ver se tem um separador ou não
								//que finalize a expressão.
								//então continuaremos no State 1.
								//State = 1;
							    break;
								
							default:
						        printf ("parse_expression: State1 error #default TK_CONSTANT={%s} line %d\n", 
						            real_token_buffer, lexer_currentline );
								exit(1);
							    break;
						};
						break;
						
					case TK_IDENTIFIER:
						printf ("parse_expression: State1 #todo TK_IDENTIFIER={%s} line %d\n", 
						    real_token_buffer, lexer_currentline );
					    //State = 2;
						exit(1);
						break;
						
					//sizeof	
					case TK_KEYWORD:
					    if (keyword_found == KWSIZEOF)
						{
							
#ifdef PARSER_EXPRESSION_VERBOSE
						   printf("parse_expression: State1 sizeof found in line %d\n", lexer_currentline);
#endif						   
						    //#bugbug
							//não sabemos se é a primeira ou a segunda constante.
							sizeof_constant = (unsigned long) parse_sizeof (TK_KEYWORD);
							sizeof_flag = 1;
							
#ifdef PARSER_EXPRESSION_VERBOSE
							printf("SIZEOF={%d}\n", sizeof_constant );
#endif 
							
							c = TK_CONSTANT; //transformamos a keyword em constant.
							State = 1;
							goto again;
							//eCount++;
							
							//c = yylex();
							//if (c == TK_SEPARATOR)
							//{
							//	State = 1;
							//    goto again;
							//}
							
							//se o próximo não for um separador então temos mais operadores e 
							//constantes.
							//depois de um sizeof tem operadores ou separador.
							//vamos para o 2 porque esperamos por operador.
							State = 2;
						    break;
						    //exit(1);
						};
					    break;
					//...

					default:
					    //printf("parse_expression: default case in State 1");
						//exit(1); //die();
					    //vamos permitir operadores, indo para o state2, 
						//ele voltará par o state 1 se for um número ou identificador.
						State = 2;
						goto again;
						break;
				}
			   break;

			//=================
			//State 2
            //operadores
			case 2:
			    switch (c)
			    {
					//#importante
					//o lexer lida com alguns tokens usados por expressões 
					//usar esse tokens. 
					//lexer_expression = o operador encontrado, que pode ser simples ou duplo. 
					
                    case '+':
                    case '-':
                    case '*':
					case '/':
					case '&':
                    case '|':
                    case '<':
                    case '>':
                    case '%':
                    case '^':
                    case '!':
                    case '=':

#ifdef PARSER_EXPRESSION_VERBOSE
					    printf("parse_expression: State 2 simple operator{%s} lexer_expression=%d \n", 
						    real_token_buffer, lexer_expression );
#endif					    
						
						if ( eCount != 1 )
						{
							printf ("parse_expression: State 2 eCount error %d", eCount);
							exit(1);
						}
						//avançamos para pegarmos constantes.
						eCount++; 
						
						switch (lexer_expression)
						{
                            case PLUS_EXPR:
							    Op = PLUS_EXPR;
                                break;
								
							case MINUS_EXPR:
							    Op = MINUS_EXPR;
								break;
								
                            case BIT_AND_EXPR:
							    Op = BIT_AND_EXPR;
								break;
								
                            case BIT_IOR_EXPR:
							    Op = BIT_IOR_EXPR;
								break;
								
                            case MULT_EXPR:
							    Op = MULT_EXPR;
								break;
								
                            case TRUNC_DIV_EXPR:
                                Op = TRUNC_DIV_EXPR;
								break;
								
							case TRUNC_MOD_EXPR:
                                Op = TRUNC_MOD_EXPR;
								break;
								
							case BIT_XOR_EXPR:
                                Op = BIT_XOR_EXPR;
								break;
								
							case LSHIFT_EXPR:
                                Op = LSHIFT_EXPR;
								break;

							case RSHIFT_EXPR:
							    Op = RSHIFT_EXPR;
								break;

							//...

							default:
                                printf ("parse_expression: State 2 default lexer code %d", lexer_expression );
								exit (1); 
								break;
							//...
						}
						//voltamos para pegarmos constantes.
						State = 1;
						break;

					case TK_ARITHCOMPARE:
					
#ifdef PARSER_EXPRESSION_VERBOSE	
					    printf("parse_expression: State 2 TK_ARITHCOMPARE{%s} lexer_expression=%d \n", 
						    real_token_buffer, lexer_expression );
#endif					    
						if ( eCount != 1 )
						{
							printf ("parse_expression: State 2 eCount error %d", eCount);
							exit(1);
						}
						//avançamos para pegarmos constantes.
						eCount++; 						
						
						switch (lexer_expression)
						{
							case LE_EXPR:
							    Op = LE_EXPR;

#ifdef PARSER_EXPRESSION_VERBOSE									
								printf("parse_expression: LE_EXPR lexer_expression=%d \n", lexer_expression );
#endif							    
								//State = 1;
								break;
								
							case GE_EXPR:
							    Op = GE_EXPR;
#ifdef PARSER_EXPRESSION_VERBOSE									
								printf("parse_expression: GE_EXPR lexer_expression=%d \n", lexer_expression );
#endif							    
								//State = 1;
								break;
	
								
							///...	
								
							default:
							    printf("parse_expression: State 2 error TK_ARITHCOMPARE default lexer_expression=%d in line %d \n", 
								    lexer_expression, lexer_currentline );
							    exit(1); //die();
								//State = 1;
								break;
						};
						State = 1;
						break;
					   
				    
					case TK_EQCOMPARE:
#ifdef PARSER_EXPRESSION_VERBOSE
					    printf("parse_expression: State 2 TK_EQCOMPARE{%s} lexer_expression=%d \n", 
						    real_token_buffer, lexer_expression );
#endif						
						switch (lexer_expression)
						{
							
							case NE_EXPR:
#ifdef PARSER_EXPRESSION_VERBOSE
							    printf("parse_expression: NE_EXPR lexer_expression=%d \n", lexer_expression );
#endif							    
								State = 1;
								break;
								
							case EQ_EXPR:
#ifdef PARSER_EXPRESSION_VERBOSE
							    printf("parse_expression: EQ_EXPR lexer_expression=%d \n", lexer_expression );
#endif							    
								State = 1;
								break;							
							
							default:
							    printf("parse_expression: State 2 error TK_EQCOMPARE default lexer_expression=%d in line %d \n", 
								    lexer_expression, lexer_currentline );
							    //exit(1); //die();
								State = 1;
								exit(1);
								break;
                        };						
					    break;
					   
					case TK_ASSIGN:
#ifdef PARSER_EXPRESSION_VERBOSE						
                        printf("parse_expression: State 2 TK_ASSIGN{%s} lexer_expression=%d \n",  
						    real_token_buffer, lexer_expression );
#endif						
						State = 1;
						break;					
					   
					case TK_PLUSPLUS:
#ifdef PARSER_EXPRESSION_VERBOSE						
					   printf("parse_expression: State 2 TK_PLUSPLUS{%s} lexer_expression=%d \n",  
					       real_token_buffer, lexer_expression );
#endif					   
					   State = 1;
					   break;
					   
					case TK_MINUSMINUS:
#ifdef PARSER_EXPRESSION_VERBOSE						
					   printf("parse_expression: State 2 TK_MINUSMINUS{%s} lexer_expression=%d \n", 
					       real_token_buffer, lexer_expression );
#endif					   
					   State = 1;
					   break;

					case TK_ANDAND:
#ifdef PARSER_EXPRESSION_VERBOSE						
					   printf("parse_expression: State 2 TK_ANDAND{%s} lexer_expression=%d \n",  
					       real_token_buffer, lexer_expression );
#endif					   
					   State = 1;
					   break;

					case TK_OROR:
#ifdef PARSER_EXPRESSION_VERBOSE						
					   printf("parse_expression: State 2 TK_OROR{%s} lexer_expression=%d \n",  
					       real_token_buffer, lexer_expression );
#endif					   
					   State = 1;
					   break;

					case TK_POINTSAT:
#ifdef PARSER_EXPRESSION_VERBOSE
					   printf("parse_expression: State 2 TK_POINTSAT{%s} lexer_expression=%d \n", 
					       real_token_buffer, lexer_expression );
#endif
					   State = 1;
					   break;
					   
					case TK_LSHIFT:
#ifdef PARSER_EXPRESSION_VERBOSE						
					   printf("parse_expression: State 2 TK_LSHIFT{%s} lexer_expression=%d \n", 
					       real_token_buffer, lexer_expression );
#endif
					   State = 1;
					   break;

					case TK_RSHIFT:
#ifdef PARSER_EXPRESSION_VERBOSE						
					   printf ("parse_expression: State 2 TK_RSHIFT{%s} lexer_expression=%d \n", 
					       real_token_buffer, lexer_expression );
#endif					   
					   State = 1;
					   break;
					   
					//...
					
					default:
					    printf("parse_expression: default case in State 2 {%s} lexer_expression=%d \n",real_token_buffer, lexer_expression );
						State = 1;
						goto again;
					    break;
				};
           	    break;		 
            
            //...
			
			//State default
            default:	
                printf("parse_expression: default State {%s} lexer_expression=%d \n",real_token_buffer, lexer_expression );
                exit(1); //die();				
		        break;
	    };
	};
 */

expression_exit:
    return (unsigned long) Result;
}

void dump_output_file(void)
{
// Incluindo no arquivo de output os segmentos.
    strcat ( outfile, TEXT );
    strcat ( outfile, DATA );
    strcat ( outfile, BSS );
// Exibimos o arquivo de output.
    printf ("\n");
    printf ("--------------------------------\n");    
    printf ("OUTPUT FILE:\n");
    printf ("%s\n", outfile);
    printf ("\n");
    printf ("--------------------------------\n");
    printf ("number of lines: %d \n", lexer_currentline );
}

// -------------------------
// parse:
// Função principal.
// Pegando tokens com o lexer e fazendo coisas ...
int parse(int dump_output)
{
// Stages:
// 1: modifier, type, metatag, separator
// 2: identifier.
// 3: keyword.
// 4: separator. Only ';'.

    int running = 1;
    register int token=0;
    int i=0;

// Se estamos esperando um identificador para um tipo.
    int waiting_identifier_after_type = FALSE;

// Se entramos em um desses corpos.
    int braces_inside = 0;
    int parentheses_inside = 0;
    int square_brackets_inside = 0;
    int If_Result = -1;
    int While_Result = -1;
    //...
// Steps
    int State=1;
// size?
    size_t size=0;

    meta_index=0;

// #bugbug
// Tentando encontrar o tamanho do arquivo via fseek/ftell.
// #bugbug
// Esse tratamento de size está suspenso,
// pois não conseguimos calcular o tamanho do arquivo.

    //++
    //===============================
    // seek to end of file
    //fseek ( stdin, 0, SEEK_END);
    
    // get current file pointer 
    //size = ftell (stdin); 
    
    // seek back to beginning of file
    //fseek (stdin, 0, SEEK_SET); 
    
    // #debug
    //printf ("Size=%d\n", size);
    //====================
    //--

// Initial message.
    printf ("parse:\n");

// Vamos usar um while até que se encontre o fim do arquivo.

    while (running == 1){
        // Get a token from lexer.
        token = yylex();
        if (token == TK_EOF){ running=0; break; }

        again:

        // #importante
        // Estamos começando um arquivo. 
        // No começo do arquivo apenas alguns tokens são válidos, 
        // são eles:
        // >> '#' do preprocessador
        // >> modificadores 
        // >> qualificadores 
        // >> tipos
        // ...

        // The 4 States:
        // >> 1: keyword, modifier, type, metatag, separator
        // >> 2: identifier
        // >> 3: keyword
        // >> 4: separator. Only ';'
        switch (State)
        {
            // ---------------------------------------
            // # State 1: 
            // Modifier, type or separator,
            // [TODO]= tipo e enum
            case 1:
                switch (token)
                {
                    // 
                    case TK_MODIFIER:
                        //#ifdef PARSER_VERBOSE
                        //continua pois precisamos pegar um tipo.
                        //#bugbug ??mas e se o modificar vir seguido de um simbolo ???
                        //printf("State1: TK_MODIFIER={%s} line %d\n", 
                        //   real_token_buffer, lexer_currentline );
                        //#endif
                        State = 1;
                        //goto again;
                        break;

                    // TYPE
                    // >>> peekChar=) significa marcação de tipagem.
                    // >>> peekSymbol=symbol  significa declaração de variável ou função.
                    case TK_TYPE:
                        id[ID_TYPE] = type_found;
                        if (type_found == TBOX)
                        {
                            printf ("box: Line %d\n",lexer_currentline);
                            waiting_identifier_after_type = TRUE;
                        }
                        if (type_found == TMETA)
                        {
                            printf ("meta: Line %d\n",lexer_currentline);
                            waiting_identifier_after_type = TRUE;
                        }
                        // Depois de um type vem um identificador.
                        State=2;
                        break;

                    // #bugbug
                    // e se o arquivo começar com um separador, 
                    // então teremos problema.
                    case TK_SEPARATOR:
                        //printf("State1: TK_SEPARATOR={%s} line %d\n", real_token_buffer, lexer_currentline );
                        // ( função
                        if ( strncmp( (char *) real_token_buffer, "(", 1 ) == 0  )
                        {
                            printf("[PAR] line %d\n", lexer_currentline);
                            parentheses_inside++;
                            //#test
                            //peekChar = c;
                            //tentando mandar alguma coisa para o arquivo de output 
                            //pra ter o que salvar, pra construir o assembly file;	
                            // strcat( outfile,"\n segment .text \n");
                            strcat( outfile,"_");
                            strcat( outfile,save_symbol);
                            strcat( outfile,":\n");
                            //recomeçar a lista. 
                            //#bugbug desconsiderando o modificador.
                            State=1;
                            break;
                        }

                        // '{'
                        // Entramos no corpo da função.
                        // Curly bracket
                        if ( strncmp( (char *) real_token_buffer, "{", 1 ) == 0  )
                        {
                            //printf ("[BRACE] line %d\n", lexer_currentline); 
                            braces_inside++;
                            // Isso vai para o 1 onde procura-se por modificadores e tipos,
                            // mas se estivermos com o corpo da função aberto ele avançará para o próximo state.
                            State = 1;
                            break;
                        }

                        // ']'
                        // fechando um box.
                        // por enquanto so temos um box,
                        // entao fechamos o programa.
                        if ( strncmp( (char *) real_token_buffer, "]", 1 ) == 0  )
                        {
                            if ( square_brackets_inside > 0)
                            {
                                printf("box terminate: in line %d\n", lexer_currentline);
                                square_brackets_inside--;
                                State = 1;
                                if (square_brackets_inside == 0)
                                {
                                    running=0; 
                                    break;
                                }
                                break;
                            }
                            // #bugbug
                            // ')' found without entering with a '('.
                        }

                        // ')'
                        // Fechando UM parênteses, provavelmente sem nada dentro.
                        if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0  )
                        {
                            if (parentheses_inside > 0)
                            {
                                //printf("[/PAR] line %d\n", lexer_currentline);
                                parentheses_inside--;
                                State = 1;
                                break;
                            }
                            // #bugbug
                            // ')' found without entering with a '('.
                        }

                        // '}'
                        // Fechando UM corpo de função. 
                        // Curly bracket
                        if ( strncmp( (char *) real_token_buffer, "}", 1 ) == 0  )
                        {
                            //printf ("[/BRACE] line %d\n", lexer_currentline);
                            if (braces_inside > 0)
                            {
                                braces_inside--;
                                State = 1;
                                break;
                            }
                            // #bugbug
                            // '}' found without entering with a '{'.
                        }

                        if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
                        {
                            //printf(" ; separator found!\n");
                            State=1;
                            // Break here?
                            // break;
                        }
                        break;

                    //#bugbug
					//Não devemos aceitar diretivas do preprocessador nesse momento,
					//pois antes de tudo devemos rodar um preprocessador para expandir as 
					//diretivas.
					
                    //Se o corpo da função estiver aberto podemos ter identificadores sem tipo,
					//que são label: ou chamada de função (call). ou ainda keywords.
					
					//significa que dentro do um corpo de função não encontramos um tipo,
					//então vamos ver do que se trata o token e configurar a variável c.
					//loga após vamos reiniciar o running mas sem pegarmos o próximo token.
					//poderíamos realizar um ungetchar, mas não testamos isso direito.
					
					//Pode ser que não encontramos um token válido no início do documento.

                    default:

					    //se estamos dentro do parênteses e não encontramos nenhum case acima.
					    if (parentheses_inside > 0){
						    State++;
						    goto again;
					    }
					    //se estamos dentro de uma chave e não encontramos nenhum case acima.
				        if (braces_inside > 0){
						    State++;
						    goto again;
					    }
					    //se os braces acabaram então foram consumidos os braces de 
					    //dentro do corpo da função e brace que fecha o corpo da função;
					    //temos que dar a função por encerrada e temos que ir
					    //para um state que tenha definição de função ou terminar.
					    if (braces_inside == 0){
							goto debug_output;
						}
                        // EOF 
                        if (token == TK_EOF){
                            printf ("State1: eof\n");
                            goto debug_output; 
                        }
					    //printf("State1: default error\n");
						printf ("State1: default. MODIFIER,  TYPE or SEPARATOR expected on line %d \n", 
						    lexer_currentline);
                        printf (">>>token={%d}\n",token);
                        exit(1);
                        break;
                }
                break;

            // ---------------------------------------
            // # State 2: Identifier.
            // Esperamos um identificador, 
            // pois estamos logo após um tipo.
            // Pode ser um nome de função ou uma declaração de variável, 
            // isso depende do peekChar.
            case 2:
                switch (token)
                {
					// identificador. (símbolo)
					// peekChar = : //significa que o identificador é uma label. acontece no case.
					// peekChar = ( //estamos chamando uma função
					// peekChar = ; //estamos finalizando um goto ou um return.
					// peekChar = , //estamos listando variáveis.
                    
                    case TK_IDENTIFIER:
                        
                        printf("State2: TK_IDENTIFIER={%s} line %d\n", 
                            real_token_buffer, lexer_currentline );    
                        
                        id[ID_TOKEN] = TK_IDENTIFIER;
                        id[ID_STACK_OFFSET] = stack_index;
                        // Salva o símbolo. #isso funciona.
                        sprintf ( save_symbol, real_token_buffer );
                        
                        
                        //----------------------------------------------------
                        // Let's put the string into the current structure,
                        // even if its not initialized yet.
                        // If we are waiting an id for a type.
                        // We are waiting the tag name.
                        // waiting_tagname
                        if (waiting_identifier_after_type == TRUE)
                        {
                            waiting_identifier_after_type = FALSE;
                            
                            if (type_found == TMETA)
                            {
                                memset(metadata[meta_index].meta_tag, 0,64);
                                string_size = (size_t) strlen(real_token_buffer);
                                if (string_size <= 0){
                                    printf("ERROR: tag size min\n");
                                    exit(1);
                                }
                                if (string_size >= 64){
                                    printf("ERROR: tag size max\n");
                                    exit(1);
                                }
                                strncpy(
                                    metadata[meta_index].meta_tag,
                                    real_token_buffer,
                                    string_size );
                                metadata[meta_index].tag_size = (size_t) string_size;
                            }
                            if (type_found == TBOX)
                            {
                                // #todo
                                // Save the box symbol in some place.
                            }
                        }
                        //----------------------------------------------------

                        // emit_symbol();
                        //strcat (TEXT,";[SYMBOL]\n");
                        //strcat (TEXT,"segment .text\n");
                        //strcat (TEXT,"_");
                        //strcat (TEXT,save_symbol);
                        //strcat (TEXT,":\n");

                        // ?? 
                        // Não sabemos se real_token_buffer é 
                        // código ou dados ?? 

                        //#debug
                        //printf("[token]\n");

                        // O que vem depois do symbol?
                        // apos meta symbol vem (.
                        // apos box symbol vem [.
                        token = yylex();

                       // printf("test={%s} line %d\n", real_token_buffer, lexer_currentline ); 

                        // : para label 
                        // ( para função 
                        // ; para declaração de variável.
                        // , para sequência de variável.
                        // = para atribuição de valor à uma variável.
                        // vários operadores se estivermos em uma expressão == <= ...
                        // valor numérico.
                        // ... 
 
                        if (token == TK_SEPARATOR)
                        {
                            printf("Separator after a symbol\n");

                            // ':' O separador é uma label.
                            if ( strncmp ( (char *) real_token_buffer, ":", 1 ) == 0 )
                            {
                                emit_label();     
                                State=1;
                                break;
                            }

                            // '[' O separador indica que entramos no box.
                            if ( strncmp( (char *) real_token_buffer, "[", 1 ) == 0  )
                            {
                                printf ("box start:\n");
                                //while(1){}
                                square_brackets_inside++;
                                //emit_function();
                                State=1;
                                break;
                            }

                            // '(' O separador indica que iniciamos a pilha de parâmetro.
                            // incrementamos
                            // pois podemos estar no primeiro, no segundo etc ...
                            if ( strncmp( (char *) real_token_buffer, "(", 1 ) == 0  )
                            {
                                parentheses_inside++;
                                emit_function();
                                State=1;
                                break;
                            }
                            // ) Se encontramos um separador ')' 
                            // entao esperaremos um separador '{'.
                            if ( strncmp( (char *) real_token_buffer, ")", 1 ) == 0  )
                            {
								//se não tem parênteses aberto.
								if ( parentheses_inside < 1 )
								{
									printf("state2: Error trying to close a not opened parentheses in line %d \n", 
									  lexer_currentline );
									exit(1);
								}
								//fechamos um dos parênteses.
								parentheses_inside--;
                                //printf ("[/PAR] line %d\n", lexer_currentline);
                                // É bss porque não foi inicializada.
                                //strcat( BSS,"\n segment .bss \n");
                                strcat( BSS,"_");
                                strcat( BSS, save_symbol);
                                strcat( BSS,": dd 0 \n");

                                //?? depois de ) podemos ter o corpo da função.
                                // ou outra coisa caso estivermos parentese aberto.

                                token = yylex();
                                // { = entramos no corpo da função
                                // logo após o ()
                                if ( strncmp( (char *) real_token_buffer, "{", 1 ) == 0  )
                                {
                                    //printf ("State2: separator={%s} line %d\n", 
                                    //    real_token_buffer, lexer_currentline );  
                                    braces_inside++;
                                    //printf ("\n[BRACE] line %d\n", lexer_currentline);
									// Vai para o 1, 
									// onde procura-se por modificadores e tipos,
									// mas se estivermos com o corpo da 
									// função aberto ele avançará para o próximo state.
                                    State = 1;
                                    break;
                                }
                                break;
                            }

                            // ; = O identificador é uma variável.
                            // Ou finalizamos uma chamada de função.
                            // ou finalizamos um corpo.
                            if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  )
                            {
//#ifdef PARSER_VERBOSE
	//					        printf ("State2: SEP={%s} line %d\n", 
	//							    real_token_buffer, lexer_currentline );
//#endif

								//tentando mandar alguma coisa para o arquivo de output 
								//pra ter o que salvar, pra construir o assembly file;	

                                // É bss porque não foi inicializada.
                                //strcat( BSS,"\n segment .bss \n");
						        strcat( BSS,"_");
						        strcat( BSS,save_symbol);
						        strcat( BSS,": dd 0 \n");

								//recomeçar, vai que tem mais variável...
								State = 1;
								break;
							}

                            // , = Separador quando estamos listando 
                            // argumentos ou listando identificadores.
                            if ( strncmp( (char *) real_token_buffer, ",", 1 ) == 0  )
                            {
//#ifdef PARSER_VERBOSE 
	//					        printf ("State2: SEP={%s} line %d\n", 
	//							    real_token_buffer, lexer_currentline );
//#endif
								//tentando mandar alguma coisa para o arquivo de output 
						        //pra ter o que salvar, pra construir o assembly file;	

								//é bss porque não foi inicializada.
								//strcat( BSS,"\n segment .bss \n");
						        strcat( BSS,"_");
						        strcat( BSS,save_symbol);
						        strcat( BSS,": dd 0 \n");
								
								//recomeçar, vai que tem mais variável...
								State = 1;
                                break;
                            }

							//printf("...");
							//goto debug_output;
							
                        } // Fim o IF token==separator.

                        printf ("state2: TK_IDENTIFIER fail\n");
                        exit(1);

						//tentando mandar alguma coisa para o arquivo de output 
						//pra ter o que salvar, pra construir o assembly file;	
						//strcat( outfile,"\n");
						//strcat( outfile,"_");
						//strcat( outfile,real_token_buffer);
						//strcat( outfile,":\n");
						
						//depois do identificador deve vir um '(' ou um '=';
						//State = 3;
						break;

					//Se o corpo da função estiver aberto podemos ter identificadores sem tipo,
					//que são label: ou chamada de função (call). ou ainda keywords.
					//significa que dentro do um corpo de função não encontramos um tipo,
					//então vamos ver do que se trata o token e configurar a variável c.
					//loga após vamos reiniciar o running mas sem pegarmos o próximo token.
					//poderíamos realizar um ungetchar, mas não testamos isso direito.

                    default:
					    //se estamos dentro do parênteses e não encontramos nenhum case acima.
					    if (parentheses_inside > 0){
						    State++;  goto again;
					    }
					    //se estamos dentro de uma chave e não encontramos nenhum case acima.
				        if (braces_inside > 0){
						    State++;  goto again;
					    }
					    //printf("State2: default Error.\n");
						printf ("State2: default. expected identifier on line %d.\n", 
						    lexer_currentline );
                        exit(1);
                        break;

                }; // End of State 2.
                break;

            // ---------------------------------------
            // # State 3: Keyword.
            // Uma keyword é o prenúncio de um statement.
            case 3:
                switch (token)
                {
                    //KEYWORD.
                    //peekChar=; depois do break, ou continue.(obrigatório)
                    //peekChar=: depois do default.(obrigatório)
                    //peekChar=( depois do switch, if, while ...

                    case TK_KEYWORD:
                    
                        // Hub para qualquer stmt.
                        //parse_stmt ();
                        //State = 1;
                        //break;

                        //-------------------------
                        // STMT: 'return'.
                        if (keyword_found == KWRETURN)
                        {
                            // #debug
                            printf("State3: keyword return found in line %d\n",lexer_currentline);
                            //printf ("State3: TK_KEYWORD={%s} KWRETURN, line %d \n", 
                                //real_token_buffer, lexer_currentline );

                            token = parse_return(TK_KEYWORD);
                            // Expected: ';'.
                            if (token != TK_SEPARATOR){
                                printf ("State3: TK_KEYWORD TK_SEPARATOR fail\n");
                                exit (1);
                            }
                            State = 1;
                            break;
                        }

                        //-------------------------
                        // STMT: 'goto'.
                        // #todo: 
                        // + Pegamos o pŕoximo token.
                        // + Emitimos um jmp e o symbol.
                        if (keyword_found == KWGOTO)
                        {
                            printf("State3: goto stmt not supported in line %d \n", 
                               lexer_currentline );
                            //State = ?;
                            break;
                        }

                        //-------------------------
                        // STMT: 'if'
                        if (keyword_found == KWIF)
                        {
                            //printf ("State3: TK_KEYWORD={%s} KWIF in line %d \n", 
                                //real_token_buffer, lexer_currentline );
                                
                            If_Result = (int) parse_if(TK_KEYWORD);
                         
                            // printf("IF-RESULT={%d}\n",If_Result);
                            State = 1;
                            break;
                        }

                        //-------------------------
                        // STMT: 'while' 
                        if (keyword_found == KWWHILE)
                        {
                            While_Result = (int) parse_while(TK_KEYWORD);
                            //  printf("WHILE-RESULT={%d}\n",While_Result);
                            State = 1;
                            break;
                        }

                        //-------------------------
                        // STMT: 'name'
                        if (keyword_found == KWNAME)
                        {
                            //printf ("State3: TK_KEYWORD={%s} KWNAME in line %d \n", 
                            //    real_token_buffer, lexer_currentline );
                            parse_name(TK_KEYWORD);
                            //recomeçamos
                            State = 1;
                            break;
                        }

                        //-------------------------
                        // STMT: 'content'
                        if (keyword_found == KWCONTENT)
                        {
                            //printf ("State3: TK_KEYWORD={%s} KWCONTENT in line %d \n", 
                            //    real_token_buffer, lexer_currentline );
                            parse_content(TK_KEYWORD);
                            //recomeçamos
                            State = 1;
                            break;
                        }

                        //...

                        break;

                    // Estamos dentro do corpo da função e 
                    // não encontramos uma keyword.
                    // Mas não tem problema caso não exista keywords 
                    // dentro dos parenteses
                    // ou dentro das chaves.
                    default:
                        if (parentheses_inside > 0){
                            printf ("State3: Searching for keyword inside parentheses \n");
                            State=1;
                            exit(1);
                        }
						//#obs: Não é errado procurar keywords dentro das chaves.
						//Já que não encontramos então vamos fechar a chave se possível.
					    if ( braces_inside > 0 ){
						    printf("State3: Warning: searching for keyword inside braces \n");
						    State = 1;
						    //exit(1);
						    break;
					    }
						//??em que momento espera-se por uma keyword e não encontra ??
					    printf("State3: default. keyword expected in line %d\n",
					        lexer_currentline);
						State = 1;
						exit (1);
						break;

                };
                break;

            // ---------------------------------------
            // # State 4: Sepatator. Only ';'
            case 4:
                switch (token)
                {
                    // Separator.
                    case TK_SEPARATOR:
                        
                        // Expected: ';'
                        if ( strncmp( (char *) real_token_buffer, ";", 1 ) == 0  ){
                            //ok #todo
                        }else{
                            printf ("State4: [ERROR] Expected ';' in line %d\n", 
                                lexer_currentline );
                            exit(1);
                        };
                        break;

                    // #todo
                    // Maybe we can expect for other type of separator here.

                    // Not a separator.
                    default:
                        printf("State4: [default] Expected ';' separator in line %d\n", 
                            lexer_currentline );
                        exit(1);
                        break;
                };
                break;

            // ---------------------------------------
            // Default state
            default:
                //printf("<default>State default: Error.\n");

                if (parentheses_inside > 0){
                    printf("default: expected ) in line %d\n", 
                        lexer_currentline );
                }
                if (braces_inside > 0){
                    printf("default: expected } in line %d\n", 
                        lexer_currentline );
                }
                if (braces_inside == 0){
                    //
                }
                goto debug_output;
                //exit(1);
                break;
 
        };//switch State

        // size ?? 
        // sai do while
        // Esse tratamento de size está suspenso,
        // pois não conseguimos calcular o tamanho do
        // arquivo na rotina logo acima.
        
        //size--;
        //if (size <= 0){
            //printf ("parser: End of size cont \n");
            //running = 0;
        //}
    };

// ----------------------
// Stop running.
// Saimos do while.
debug_output:

// --------------------------------
// Dump input file?
    //printf("\n INPUT: \n");
    //printf("%s\n",stdin->_base);
    //printf("number of lines: %d \n",lexer_currentline);
    //...

// --------------------------------
// Dump output file?
    if (dump_output){
        dump_output_file();
    }

/*
// Incluindo no arquivo de output os segmentos.
    strcat ( outfile, TEXT );
    strcat ( outfile, DATA );
    strcat ( outfile, BSS );
// Exibimos o arquivo de output.
    printf ("\n");
    printf ("--------------------------------\n");    
    printf ("OUTPUT FILE:\n");
    printf ("%s\n", outfile);
    printf ("\n");
    printf ("--------------------------------\n");
    printf ("number of lines: %d \n", lexer_currentline );
*/

    // goto parse_exit;

// OK, done!
parse_exit:
    printf("parse: Done\n");
    return 0;
syntax:
    printf("parse: Systax error in line %d\n", 
        lexer_currentline );
    exit(1);
hang:
    printf("parse: *hang\n");
    while (1){
        asm ("pause");
    };
}

// parserInit:
// Initializing parser.
static int __parserInit(void)
{
    register int i=0;

    infile_size=0;
    outfile_size=0;

// Stack support
    stack_flag = 0;
    stack_count = 0;
    stack_index = 0;

    meta_index=0;
    string_size=0;
    meta_stage=0;

    for ( i=0; i<ID_MAX; i++ )       { id[i] = 0;          };
    for ( i=0; i<CONSTANT_MAX; i++ ) { constant[i] = 0;    };
    for ( i=0; i<8; i++ )            { return_info[i] = 0; };
    for ( i=0; i<512; i++ )          { stack[i] = 0;       };
    //...

// Esses endereços vão depender do arquivo 
// de configuração do linker ...
// #test: default em 0.
    program_header_address = 0;
    program_text_address = 1*100;
    program_data_address = 2*100;
    program_bss_address  = 3*100;
    //...

    return 0;
}

// Called by compiler().
int parser_initialize(void)
{
    //printf ("parser_initialize:\n");
    return (int) __parserInit();
}

//
// End
//


