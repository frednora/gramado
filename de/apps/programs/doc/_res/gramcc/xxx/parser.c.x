// Suporte a status bar.

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


//#test: trabalahndo primeiramente na função main. 
//cria a main com base em uma lista de tokens ...
//deverá pegar um por um usando next();
int parserCreateMain ( char **list ){
	
	char *token;
	
	function_main = ( struct function_d * ) &function_main_buffer[0];
	
	function_main->used = 1; 
	function_main->magic = 1234;
	
	function_main->id = 0; 
	
	//estrutura pequena.
	function_main->h0 = (struct function_h0 *) malloc ( sizeof(struct function_h0) );
	
	if( (void *) function_main->h0 ==  NULL )
	{
		printf("parserCreateMain: struct\n");
		return 1;
	}else{
		
		function_main->h0->modifier0_token_index = 0;
		function_main->h0->modifier1_token_index = 0;
		function_main->h0->type_token_index = 0;
		function_main->h0->identifier_token_index = 0;
	};
	
	
	//#debug 
	//printf("\nparserCreateMain-LIST:\n\n");
	//fncc_print_tokenList( list, "|");
	
	
	sprintf(outfile,";Starting assembly file \n");
	strcat(outfile,"\n");
	strcat(outfile,"[bits 32]\n");
	strcat(outfile,"\n");
	
	int i=0;
	
	int running = 1;
	int state = 1;
	
	
    // um asm inline foi encontrado. asm("...");
    int asm_found;
    
	//tamanho de uma linha.
	char asm_string[80];


	//flags ... todo
	string_flag = 0;
	asm_found = 0;
	
	
	// states:
	//int main(){}
	//1 int 
	//2 main 
	//3 ( 
	//4 )
	//5 {
	//6 }
	
	while ( running == 1 )
	{
		
		//#debug
		printf(".");		

		// pega o token do próximo indice da lista.
		i = (int) next ();
		
		if ( i < 0 )
            goto done;

	    
		token = (char *) list[current_index];
		if ( token == NULL ){
		    printf("parserCreateMain: error token null\n");
			running = 0;
			goto done;
		}	
		
	    switch ( state )
        {
			
			// type: esperamos por um tipo, nada mais.
            case 1:
			    switch (i)
				{
				    case TOKENKEYWORD:
				        //se no momento encontramos um tipo, então checaremos qual foi.
			            if( type_found == 1 )
				        {
							strcat(outfile,";type found \n");
					        state = 2;
							
					        switch ( current_type )
					        {
						        case TNULL:
						            //todo
						            break;

						        //um tipo int foi encontrado, pode ser uma função ou uma variável..	
						        case TINT:
						    
						            break;
						
						        case TVOID:
						            //todo
						            break;
						
						        case TCHAR:
						            //todo
						            break;
						
						        case TSHORT:
						            //todo
						            break;
						
						        case TLONG:
						            //todo
						            break;
						
						
						        default:
						            printf("parserCreateMain: default type\n");
									running = 0;
						            break;
					        };//switch
					
					    }else{
							printf("parserCreateMain: type not found\n");
							running = 0;
						};
                        break;

                    default:
					    printf("parserCreateMain: default: type not found\n");
						running = 0;
                        break;					
				};//switch
			    break;
				
			//esperamos o identificador de funçao.
            //'main'			
		    case 2:
			    switch (i)
				{
			        case TOKENIDENTIFIER:
			            // se encontramos o main já podemos contruir a função
			            if( main_found == 1 )
					    {
							strcat(outfile,";identfier found \n");
							state = 3;
			                
							strcat(outfile,"\n");
			                strcat(outfile,"global _main \n");
							strcat(outfile,"_main: \n");
							strcat(outfile,"\n");
							
                            break;							
					    }else{
							printf("main not fould\n");
							running = 0;
						}
				    break;			
				};
			    break;
		
		    //esperamos um separador. (
		    case 3:
			    switch (i)
			    {
					case TOKENSEPARATOR:
			            //todo
						strcat(outfile,";separator ( found \n");
						state = 4;
				        break;
						
					default:
					    printf("separator not fould");
						running = 0;
					    break;
				};
				break;
				
		    //esperamos um separador. )
		    case 4:
			    switch (i)
			    {
					case TOKENSEPARATOR:
			            //todo
						strcat(outfile,";separator ) found \n");
						state = 5;
				        break;
						
					default:
					    printf("separator not fould");
						running = 0;
					    break;
				};
				break;
				

		    //esperamos um separador. {	 
		    //o propósito desse ciclo é somente entrar no corpo da função.
			case 5:
			    switch (i)
			    {
					case TOKENSEPARATOR:
			            //todo
						strcat(outfile,";separator { found \n");
						state = 6;
				        break;
						
					default:
					    printf("separator not fould");
						running = 0;
					    break;
				};
				break;
				
				
		    //esperamos um separador. }
			//estamos dentro do corpo.
			//muitos statement poderão ser encontrados aqui.
		    case 6:
			
			    //imprimir a string enquanto não encontrarmos o \".
				//se for igual a \", então entra no switch.
			    if( string_flag == 1 && list[current_index][0] != '\"'  ) 
				{
					if( i == TOKENNULL && list[current_index][0] == ' ' )
					{
						strcat(outfile," ");
						break;
					}
					//strcat(outfile," ");
				    strcat(outfile, list[current_index]);
                    //strcat(outfile," ");
					break;					
				};
				
			    switch (i)
			    {
					
                    //quando encontrarmos uma constante dentro do corpo da função.					
			        case TOKENCONSTANT:
			
			            //se encontramos uma constante e o retorno foi encontrado,
                        //então essa constante é o retorno da função.				
			            if( return_found == 1 )
				        {
							
							strcat(outfile,"\n");
							strcat(outfile,";return found \n");
							strcat(outfile,".__return__: \n");
							
			                //token = (char *) list[current_index];
			                //if ( token == NULL ){
			                //    printf("parserCreateMain: error token null\n");
					        //    running = 0;
							//	goto done;
					        //}
			                
							strcat(outfile,"\n");
						    strcat(outfile,"mov eax, ");
			                strcat(outfile, list[current_index]); //0x1234
			                strcat(outfile,"\n ret \n");
							
							return_found = 0;
							 							
				        };
						
						//todo: existem outros casos onde encontramos uma constante dentro do corpo da função.
						
						//...
						
			            break;
						
					//quando encontrarmos um separador dentro do corpo da função.		
					case TOKENSEPARATOR:
					
			                //token = (char *) list[current_index];
			                //if ( token == NULL ){
			                //    printf("parserCreateMain: error token null\n");
					        //    running = 0;
							//	goto done;
					        //}
							
					    //finalizando variáveis locais e return.
					    if( list[current_index][0] == ';' )
						{
						    //strcat(outfile,";separator ; found \n");	
						    break;
						}
						
				        
						//#importante:
						//Encontramos o separador que fecha o corpo da função, então vamos encerrar o ciclo.
						if( list[current_index][0] == '}' )
						{
						    strcat(outfile,";separator } found \n");	
						    state = 7;
							break;
						}
				        break;
						
						
					case TOKENSPECIAL:
					    //tratando strings. \"
						if( asm_found == 1 )
						{
							
							//se ainda não abrimos a string do asm, então abriremos agora caso 
							//o char seja '"'
							if( string_flag == 0 )
							{
							    if( list[current_index][0] == '\"' ){
									string_flag = 1;
									strcat(outfile,"\n;asm open \n\n");
									break;
								}	
							};							
							
							
							//se já abrimos a string do asm
							//então vamos tratar a string
							if( string_flag == 1 )
							{
								//se a string está aberta e encontramos o fechador,
								//então fecharemos a string.
								if( list[current_index][0] == '\"' ){
										
								    string_flag = 0;  //fechamos a string.
							        asm_found = 0;   //fechamos o asm.
									strcat(outfile,"\n;asm close \n\n");
                                    break;
                                    //o ciclo não fecha, ainda continuaremos dentro do corpo da função.										
								};
									
								//aqui estamos dentro da string mas ainda não encontramos o fechador,
								//então devemos apenas copiar a string.
								//#todo:
								//...
									
										
							};
								
						};
					
					    break;
						
					//se dentro do corpo da função encontrarmos uma palavra reservada.	
					case TOKENKEYWORD:
					
			               // token = (char *) list[current_index];
			               // if ( token == NULL ){
			               //     printf("parserCreateMain: error token null\n");
					       //     running = 0;
							//	goto done;
					       // }
                            
							// se a palavra reservada encontrada dentro do corpo da função for 'asm'
                            if( strncmp( (char *) list[current_index], "asm", 3 ) == 0  )
                            {
								asm_found = 1;
								//strcat(outfile,";asm found \n");
								break;								
							};
                             
							//outros iguais 
							//...
							 								
						break;  					 
						
					    default:
					        //printf("separator not fould");
						    //running = 0;
					        break;
				};
				break;

			//esperamos por um ';' para finalizar o corpo de main.	
            case 7:
			    switch (i)
				{
					case TOKENSEPARATOR:
					
			                //token = (char *) list[current_index];
			                //if ( token == NULL ){
			                //    printf("parserCreateMain: error token null\n");
					        //    running = 0;
							//	goto done;
					        //}					
					    
						if( list[current_index][0] == ';' )
						{
						    strcat(outfile,";End \n");
					        state = 8;							
						}	 
					    break;
				} 
                break;
            //done
            case 8:
			    strcat(outfile,"done\n");
			    goto done;
                break; 			
		};
	};
	
done:
    //#debug
    printf("state %d\n",state);
    return 0;	
};  
 


//@todo: rotina que compila todas as funções
//void compile()

//parse só uma função.
void parseFunction( char *s )
{
	char **Ret;
	char *tokenList[80];
	char *token;
	//int z;
	int token_count;
	
	int current_type;
	int return_reached;
	
	Ret = (char **) tokenList;	
	
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
	printf("compile:\n");
	printf("i-size={%d}\n", size);	
	printf("i-string={%s}\n", s);
	//...
	
	infile_size = (unsigned long) size;
	
	//limits
	if( size > 128 )
	{
		printf("parseFunction: fail, string size limits\n");
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
	    in[z] = InFilePointer[z];
	}


    //  # CreateTokenList #	

	char **list = (char **) create_tokenlist( (char *) s );
		
		
	// # mostra a lista de tokens #
	
	printf("\nLIST:\n\n");
	fncc_print_tokenList( list, "|");

    
	//
	// comparar NÃO funcionou para apenas parte do token ..
	// tem que comparar o token inteiro
	//
	
	
	
	//
	// ##############
	//
	
	printf("\n>>Scaning function\n");
	
	int eID;
	int eTmp;
	int i=0;
	while(1)
	{
		//#importante:
		//Último token da lista.
	    token = (char *) list[i];
	    if( token == NULL )
	        goto done;	
		
		//#importante:
		//last for now.
        if( strncmp( (char*) list[i], "}", 1) == 0  ){		
            goto done;
		};	
		
	
		i++;
		//nothing for now.
	};
	

	/*
	
	//int
	token = (char *) list[0];
	if( token == NULL )
	    goto done;	
	
	
	//## test ##
	
	eID = getElementID( list[0] );
	
	printf(" ## eID={%d} ##\n",eID);
		
		//#Entry void
		//se a palavra encontrada for void.
		//pode ser uma função ou uma variável.
		//printf(">{%s}\n",list[0]);
	if( strncmp( (char*) list[0], "int", 3) == 0  )
    {
		//a função atual terá esse tipo. (1=int)
		current_type = 1;
			//sprintf(outfile,".globl _main \n");
			//strcat(outfile,"_main: \n");
            //printf("void found\n");
			//printf("={%s}\n",list[0]);		
	};

	//main
	token = (char *) list[1];
	if( token == NULL )
	    goto done;	
		
		
		//printf(">{%s}\n",list[1]);
        if( strncmp( (char*) list[1], "main", 4) == 0  )
        {
			sprintf(outfile,".globl _main \n");
			strcat(outfile,"_main: \n");
            //printf("( found\n");
			//printf("={%s}\n",list[1]);		
        };	
		
		
	//(	
	token = (char *) list[2];
	if( token == NULL )
	    goto done;	
		
		//printf(">{%s}\n",list[2]);
        if( strncmp( (char*) list[2], "(", 1) == 0  )
        {
           // printf(") found\n");
			//printf("={%s}\n",list[2]);		
        };	
	
    //) 	
	token = (char *) list[3];
	if( token == NULL )
	    goto done;	
		
		
		//printf(">{%s}\n",list[3]);	
        if( strncmp( (char*) list[3], ")", 1) == 0  )
        {
			//strcat(outfile,"ret\n");
            //printf("{ found\n");
			//printf("={%s}\n",list[3]);		
        };	
	
    //{	
	token = (char *) list[4];
	if( token == NULL )
	    goto done;	
		
		
		//printf(">{%s}\n",list[5]);
        if( strncmp( (char*) list[4], "{", 1) == 0  )
        {
		    //printf("} found\n");
			//printf("={%s}\n",list[4]);		
            //goto done;
		};	
		
		

	//return	
	token = (char *) list[5];
	if( token == NULL )
	    goto done;	
		
		
		//printf(">{%s}\n",list[5]);
        if( strncmp( (char*) list[5], "return", 6) == 0  )
        {
			return_reached = 1;
			token = (char *) list[6];
			if( token == NULL )
			    {goto done;};
			strcat(outfile,"movl $");
			strcat(outfile, list[6]); //0x1234
			strcat(outfile,", %eax\n");
			strcat(outfile,"ret\n");
		};
		
	//0x1234
	//token = (char *) list[6];
	//if( token == NULL )
	//    goto done;	
		
		
		//printf(">{%s}\n",list[6]);
    //    if( strncmp( (char*) list[6], "0x1234", 6) == 0  )
    //    {
		    //printf("} found\n");
			//printf("={%s}\n",list[4]);		
            //goto done;
	//	};		


	//;	
	token = (char *) list[7];
	if( token == NULL )
	    goto done;	
		
		
		//printf(">{%s}\n",list[7]);
        if( strncmp( (char*) list[7], ";", 1) == 0  )
        {
		    //printf("} found\n");
			//printf("={%s}\n",list[4]);		
            //goto done;
		};

    //}	
	token = (char *) list[8];
	if( token == NULL )
	    goto done;	
		
		
		//printf(">{%s}\n",list[8]);
        if( strncmp( (char*) list[8], "}", 1) == 0  )
        {
		    //printf("} found\n");
			//printf("={%s}\n",list[4]);		
            goto done;
		};	

    */		
	
	
done:
    //#debug 
    printf("\nINPUT:\n%s\n\n",infile);	
    printf("\nOUTPUT:\n%s\n\n",outfile);
    return;		
};


/*
struct token_d * getToken( char *s );
struct token_d * getToken( char *s )
{
    struct token_d *t;	
    
	t = (struct token_d *) malloc( sizeof( struct token_d ) );
	if( (void*) ==  NULL 0)
	{
		goto fail;
	}
	
	
	switch (*s) {

    case '(' :
        t.type = 1;
		break;

    case ')' :
        t.type = 2;
		break;

    case '{' :
        t.type = 3;
		break;

    case '}' :
        t.type = 4;
		break;

    };		

	default:
	    goto fail;
		break;
		
done:
    return (struct token_d *) t;
fail:
    return NULL;    
};
*/

// Vamos inicializar o parser.
int parserInit()
{
	infile_size = 0;
	outfile_size = 0;
	

	
	//...
	
	return 0;
};


