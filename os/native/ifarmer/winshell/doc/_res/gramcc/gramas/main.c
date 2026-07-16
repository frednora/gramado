/*
 * File: main.c
 *     Main file for 32bit app.
 *
 * env: gramado 0.4
 */

#include  "as.h" 


#ifndef __COPYRIGHT__
char copyright[] = "Copyright (c) 2018 Fred Nora.\n\
 All rights reserved.\n";
#endif 

/* While POSIX defines isblank(), it's not ANSI C. */
//#define IS_BLANK(c) ((c) == ' ' || (c) == '\t')

//default name.
char program_name[] = "[Default program name]";

// Specification for gramcc.
char *standard_spec = "%{%{CC} -c %{I} -o %{O}}";


char *compiler_name;

//procurar os marcadores no arquivo intermediário 
//pegamos um arquivo intermediário e separamos cada um dos 
//elementos, colocando seus ponteiros em um vetor.
//A cada passo do offset comparamos uma palavra.
char **create_tokenlist( char *s );

// Imprime todas as strings de um vetor de ponteiros.
// Isso vai ser usado pelo echo.
void fncc_print_tokenList ( char *token_list[], char *separator );

//inicializando as variáveis e buffers.
int gramccInitialize();

//testando suporte à ctype.
//void mainTestingCTYPE();

//static int running = 1;
int running = 1;


//Para o caso de não precisarmos produzir 
//nenhum arquivo de output. 
int no_output;

//static char *dest_argv[] = { "-sujo0","-sujo1","-sujo2",NULL };
//static unsigned char *dest_envp[] = { "-sujo", NULL };
//static unsigned char dest_msg[512];

void editorClearScreen(); 

void debugShowStat();


/*
 ************************************************************
 * mainGetMessage:
 *     Função principaL chamada pelo crt0.asm.
 *     Testando o recebimento de mensagens enviadas pelo shell.
 *
 * #importante:
 *	Recebendo mensagens via memória compartilhada.
 *	Obs: Esse não é o melhor endereço para se usar,
 *	mas isso é um teste por enquanto.
 * Origem: Provavelmente está dentro do backbuffer na parte não visível.	 
 *
 */

#define LSH_TOK_DELIM " \t\r\n\a" 
#define SPACE " "
#define TOKENLIST_MAX_DEFAULT 80
 
int gramas_main (){
	
	//Lexemes suport.
	char *tokenList[TOKENLIST_MAX_DEFAULT];
	char *token;
	int token_count;
	int index;
    int Ret;	
	
	// #importante
	// Linha de comandos passada pelo shell.
	char *shared_memory = (char *) (0xC0800000 -0x100);
	
	
//#ifdef GRAMCC_VERBOSE
	printf("\n");
	printf("gramas_main: Initializing gramas ...\n");
	//printf("\n");
	printf ("# cmdline={%s} #\n", shared_memory );
//#endif
	
    
	// Criando o ambiente.
	// Transferindo os ponteiros do vetor para o ambiente.

	tokenList[0] = strtok ( &shared_memory[0], LSH_TOK_DELIM);
	
 	// Salva a primeira palavra digitada.
	token = (char *) tokenList[0];
 
	index = 0;                                  
    while ( token != NULL )
	{
        // Coloca na lista.
        // Salva a primeira palavra digitada.
		tokenList[index] = token;

		//#debug
        //printf("shellCompare: %s \n", tokenList[i] );
		
		token = strtok( NULL, LSH_TOK_DELIM );
		
		// Incrementa o índice da lista
        index++;
		
		// Salvando a contagem.
		token_count = index;
    }; 

	//Finalizando a lista.
    tokenList[index] = NULL;	
	
	
	// #debug 
	// Mostra argumentos.
#ifdef GRAMCC_VERBOSE	
	// Mostra a quantidade de argumentos. 	
	printf("\n");
	printf("token_count={%d}\n", token_count );
	
	//Mostra os primeiros argumentos.
	for ( index=0; index < token_count; index++ )
	{
		token = (char *) tokenList[index];
	    if( token == NULL )
		{
			printf ("gramas_main: for fail!\n");
			exit (1);
			//goto hang;
		};
	    printf ("# argv{%d}={%s} #\n", index, tokenList[index] );		
	};
#endif
 
	//
	// ## Main ##	
    //
	
	Ret = (int) gramas_main2 ( token_count, tokenList );	
	
	//#bugbug
	//precisamos trocar o cr0, ele não tem tratamento algum 
	//de retorno e exit.
	//então vamos sair aqui mesmo.
	
	switch (Ret)
	{
		case 0:
		    printf("gramas_main: exit(0)\n");
            exit(0);
		    break;
			
		case 1:
            printf("gramas_main: exit(1)\n");
			exit(1); 
		    break;
			
		default:
		    printf("gramas_main: default exit(%d)\n", Ret );
            exit(Ret);
		    break;
	};	
	
	//
	// End
	//
	
	printf("gramas_main: unexpected exit code %d\n", Ret );
	exit(1);
};


/*
 ********************************
 * gramc_main:
 *     Função principal do fncc.
 *     Essa função é chamada por mainGetMessage.
 */
int gramas_main2 ( int argc, char *argv[] ){
	
	FILE *fp;       //file pointer.
	register int i;
    char *filename;
	
	// Output string.
	char *o;	
	
	//switches
	int flagA = 0;
	int flagB = 0;
	int flagC = 0;
	int flagD = 0;
	
	int flagString1 = 0;	
	int flagString2 = 0;
	int flagString3 = 0;
	int flagString4 = 0;
	
    int flagX = 0;	
	int flagY = 0;
	int flagZ = 0;
	
	int flagR = 0;
	int flagS = 0;
	int flagT = 0;
	
	
	
	// Initializing.
//init:
    
#ifdef GRAMCC_VERBOSE		
    printf("\n");
	printf("gramc_main2: Initializing ..\n");
#endif 

    libcInitRT();
    stdioInitialize();
	
	gramccInitialize ();
	
	// O nome do programa é o primeiro comando da linha.
	// compiler_name = argv[0];
	
	
    // #debug 
    // Mostrando os argumentos. 
	
#ifdef GRAMCC_VERBOSE		
	printf("argc={%d} ...\n",argc);
	for( i=0; i < argc; i++ )
	    printf(" ## argv{%d}={%s} ##  \n", i, argv[i] );
#endif 

	
	//
	// ## Args ##
	//
	
//setupFlags:

	// Comparando os argumentos para acionar as flags.
	for ( i=0; i < argc; i++ )
	{
        if ( strcmp( argv[i], "-a") == 0 )
        {
		   printf("## %d flag a ##\n",i);	
		}					

        if ( strcmp( argv[i], "-b") == 0 )
        {
		   printf("## %d flag b ##\n",i);	
		}					

        if ( strcmp( argv[i], "-s") == 0 )
        {
		    printf("## %d flag -s ##\n",i);
            asm_flag = 1;		   
		}	
		
	    //...	
	};
	
    //
	// # arquivo de entrada #
	//
	
    fp = fopen ( (char *) argv[2], "rb" );	
	
	if ( fp == NULL )
    {
        printf ("fopen fail\n");
        exit (1);
		
		//while (1){
		//	asm ("pause");
		//}	
		
    }else{

		// Exibe o arquivo.
        //printf("%s",fp->_base);
		
		size_t size = (size_t) strlen( (const char *) fp->_base );

        int z;
        for ( z=0; z<size; z++ ){
			infile[z] = fp->_base[z];
		}

        infile[z] = 0; 		
	};	
	
	//#importante.
	stdin = fp;
	finput = fp;
	
	//
	// ## Parse ##
	//

	int parse_ret;
	//Testando funções do compilador.
	parse_ret = (int) parse ();

	
	printf ("parse returned %d\n");
	/*
	//#todo parse return
	switch (parse_ret)
	{
		case 0:
		    //return 0;
			//goto exit_success;
		    break;
			
		case 1:
		    //return 1;
			//goto exit_error;
		    break;
			
		default:
		    //goto exit_default;
		    break;
	};
	*/
	
//exit_default:	
	
	//#importante
	//mostra o asm
	//isso dependerá da flag -s
	

   
    // More ...   
	
out:
	if ( asm_flag == 1 )
	    printf (" \n OUTPUT: \n%s\n\n", outfile );
	
    //printf("\n");	
	//printf("*hang\n");
	
	//#debug suspensa por enquanto.
	//debugShowStat();
	
    //printf("\n");	
	printf("gramc_main2: DONE! *hang\n");	
    
	while (1){
		asm ("pause");
	}

exit_success:	

#ifdef GRAMCC_VERBOSE		
	printf("gramcc_main: exit success\n");
#endif 
    return 0;
	//exit (0);	
};


/*
 ********************************************
 * mainTextEditor:
 *     O editor de textos.
 * In this fuction:
 *     Initializes crt.
 *     Initializes stdio.
 *
 */
int mainTextEditor( int argc, char *argv[] )
{
	int ch;
	FILE *fp;
    int char_count = 0;	
	
	
#ifdef GRAMCC_VERBOSE			
	printf("\n");
	printf("Initializing Text Editor:\n");
	printf("mainTextEditor:  ## argv={%s} ##  \n",
	    &argv[0] );
#endif	
	
	//
	// ## vamos repetir o que dá certo ...
	//
	
	//vamos passar mais coisas via registrador.
	
	//ok
	//foi passado ao crt0 via registrador
	//printf("argc={%d}\n", argc ); 
	
	//foi passado ao crt0 via memória compartilhada.
	//printf("argvAddress={%x}\n", &argv[0] ); //endereço.
	
	
	//unsigned char* buf = (unsigned char*) (0x401000 - 0x100) ;
	//printf("argvString={%s}\n" ,  &argv[0] );
	//printf("argvString={%s}\n" , &buf[0] );
	
	//printf("argv={%s}\n", &argv[2] );
	
	
    //stdlib
	//inicializando o suporte a alocação dinâmica de memória.
	libcInitRT();

	//stdio
	//inicializando o suporte ao fluxo padrão.
    stdioInitialize();	
	

	//
	// ## screen ##
	//

	apiBeginPaint();
	editorClearScreen(); 
	apiEndPaint();
	
	//
	//  ## Testing file support. ##
	//
	
file:

#ifdef GRAMCC_VERBOSE	
	printf("\n");
	printf("\n");
    printf("Loading file ...\n");
#endif	
	
	//Page fault:
	//
    //fp = (FILE *) fopen( "init.txt", "r+");
    //fp = fopen("init.txt","rb");

    //fp = fopen("test1.txt","rb");	
	fp = fopen( (char*) &argv[0],"rb");	
	
	if( fp == NULL )
    {
        printf("fopen fail\n");
        goto fail;		
    }else{
		
#ifdef GRAMCC_VERBOSE			
        printf(".\n");		
        printf("..\n");		
        printf("...\n");
#endif 
		// Exibe o arquivo.
        printf("%s",fp->_base);	

#ifdef GRAMCC_VERBOSE	        
		printf("...\n");
        printf("..\n");		
        printf(".\n");		
#endif


#ifdef GRAMCC_VERBOSE	
		printf("\n");
		printf("Typing a text ...\n");
#endif

Mainloop:		
	    while(running)
	    {
			//enterCriticalSection(); 
	        ch = (int) getchar();
			//exitCriticalSection();
			
			if(ch == -1)
			{
			    asm("pause");
               // printf("EOF reached! ?? \n");  				
			};
			
	        if(ch != -1)
	        {
				
	            printf("%c",ch);
	    
	            //switch(ch)
                //{
			        //quit
			    //    case 'q':
			    //        goto hang;
				//        break;				 
		        //};		   
		    };
	    };		
		
		//saiu.
        printf(".\n");		
        printf(".\n");		
        printf(".\n");
		goto done;
	};
	
	
fail:	
    printf("fail.\n");
done:
    running = 0;
    printf("Exiting editor ...\n");
    printf("done.\n");
	while(1)
	{
		asm("pause");
		exit(0);
	};
    // Never reach this.	
	return (int) 0;
};


/*
 *  
 *     Limpar a tela deletar isso
 */

void editorClearScreen()
{
	int lin, col;    

	// @todo:
	//system( "cls" ); // calls the cls command.
	
	//cursor.
	apiSetCursor( 0, 0 );
	
	//
	// Tamanho da tela. 80x25
	//
	
	//linhas.
	for( lin=0; lin < ((600/8)-1); lin++)
	{
		col = 0;
		
		apiSetCursor(col,lin);
		
		//colunas.
		for( col=0; col < ((800/8)-1); col++)
		{
			printf("%c",' ');
	    }
	};
	
	apiSetCursor(0,2);
};




//procurar os marcadores no arquivo intermediário 
//pegamos um arquivo intermediário e separamos cada um dos 
//elementos, colocando seus ponteiros em um vetor.
//A cada passo do offset comparamos uma palavra.
//#define create_tokenlist_DELIM " \t\r\n\a" 
#define create_tokenlist_DELIM " " 
char **create_tokenlist ( char *s ) {
	
//criando o ambiente.
	//transferindo os ponteiros do vetor para o ambiente.
	
	char **Ret;
	char *tokenList[TOKENLIST_MAX_DEFAULT];
	char *token;
	int z;
	register int token_count;
	
	Ret = (char **) tokenList;
	
	tokenList[0] = strtok( s, create_tokenlist_DELIM);
	
 	//salva a primeira palavra digitada.
	token = (char *) tokenList[0];
 

	int index=0;                                  
    while( token != NULL )
	{
        // Coloca na lista.
        //salva a primeira palavra digitada.
		tokenList[index] = token;

		//#debug
		//Mostra
        //printf("shellCompare: %s \n", tokenList[i] );
        //refresh_screen();
		
		token = strtok( NULL, create_tokenlist_DELIM );
		
		// Incrementa o índice da lista
        index++;
		
		//salvando a contagem.
		token_count = index;
    }; 

	//Finalizando a lista.
    tokenList[index] = NULL;		
	
	
	//goto done;
		
done:	
	//fncc_print_tokenList(tokenList, "/");
	
	number_of_tokens = (int) token_count;
	lexer_token_count = (int) token_count;  
	
    return (char **) Ret;	
};


// Imprime todas as strings de um vetor de ponteiros.
// Isso vai ser usado pelo echo.
void fncc_print_tokenList ( char *token_list[], char *separator ){
	
	char *token;
	
	token = (char *) token_list[0];
	
	if( token == NULL )
	    goto fail;
	
	
	//token = (char *) tokenList[i];
		
	//	if( token == NULL ){
			
    int i;
	for( i=0; i,128; i++ )
    {
		token = (char *) token_list[i];

	    if( token == NULL )
	        goto done;
		
		//if( strncmp( (char*) token_list[i], "echo", 4 ) == 0 )
		//    continue;	
		
		printf("%s", token_list[i]);
		printf("%s", separator);
    }
	
fail:
done:
    return;	
};



/*
int is_letter(char c) ;
int is_letter(char c) 
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');

}
*/


/*
 **************************************
 * gramccInitialize:
 *     Função interna.
 */
int gramccInitialize (){
	
    int Status = 0;	
	
	int i;
	
	
#ifdef GRAMCC_VERBOSE	
    printf("gramccInitialize: Initializing ...\n");
#endif	
	
	
	// Clear buffers
	
	for ( i=0; i<INFILE_SIZE; i++ )
	{
		infile[i] = '\0';
	}

	for ( i=0; i<OUTFILE_SIZE; i++ )
	{
		outfile[i] = '\0';
	}
	
	
	// Inicializamos o buffer do arquivo sujando ele.
    // Mas depois carregaremos um arquivo.	
	
    //sprintf ( infile, STRING_CFILE );
	sprintf ( infile, "initializing infile " );
    strcat(infile,"buffer");	
	
    sprintf(outfile, "\n;; initializing outfile buffer \n" );
    strcat(outfile,";; its an assembly file \n\n");	
    strcat( outfile,"\n segment .text \n");
	
    sprintf(DATA, "\n;; initializing DATA buffer \n" );
    strcat(DATA,";; its an assembly DATA \n\n");	
    strcat( DATA,"\n segment .data \n");
	
    sprintf(BSS, "\n;; initializing BSS buffer \n" );
    strcat(BSS,";; its an assembly BSS \n\n");	
	strcat( BSS,"\n segment .bss \n");
	
	//table.
//contador para não estourar a lista. 
    keyword_count = 0;  
	identifier_count = 0; 
    keyword_count = 0; 
    constant_count = 0; 
    string_count = 0; 
    separator_count = 0; 
    special_count = 0;	
	//...
	
	
//usado pelo lexar pra saber qual lugar na lista 
//colocar o lexeme.
    current_keyword = 0; 
    current_identifier = 0; 
    current_keyword = 0; 
    current_constant = 0; 
    current_string = 0; 
    current_separator = 0; 
    current_special = 0;	
	
	//#imporante
	//Inicializando lexer e parser.
	
	Status = (int) lexerInit();
	Status = (int) parserInit();
	
	//
	// ## program ##
	//
	
	program.name = program_name;
	program.function_count;
	program.function_list = NULL;
	
	
	//...
	
#ifdef GRAMCC_VERBOSE	
    printf("gramccInitialize: done\n");
#endif		

    return (int) Status;	
};


//testing ctype support
/*
void mainTestingCTYPE()
{
    int var1 = 'h';
    int var2 = '2';
    
    printf("Testing ctype.h\n");
   
    if( isdigit(var1) ) 
    {
        printf("var1 = |%c| is a digit\n", var1 );
    }else{
        printf("var1 = |%c| is not a digit\n", var1 );
    };
   
    if( isdigit(var2) ) 
	{
        printf("var2 = |%c| is a digit\n", var2 );
    }else{
        printf("var2 = |%c| is not a digit\n", var2 );
    };

    // More ...
};
*/



//mostra as estatísticas para o desenvolvedor.
void debugShowStat (){
	
	printf("debugShowStat:\n\n");
	
	printf("name: %s\n", program.name );
	printf("function count : %d\n", program.function_count );
	printf("list handle : %x\n", program.function_list );
	
	
	if( (void *) function_main == NULL )
	{
		printf("debugShowStat: function_main struct \n");
		return;
	
	} else {
		
        if ( function_main->used != 1 || function_main->magic != 1234 )
        {
		    printf("function_main: validation \n");
		    return;
			
		} else {
			
			printf("main id: %d \n", function_main->id );
		    
			//...
			
			if( (void *) function_main->h0 ==  NULL )
			{
				printf("function_main: h0 \n");
			}else{
				
				printf("h0 type %d \n",function_main->h0->type_token_index);
				printf("h0 identifier %d \n",function_main->h0->identifier_token_index);
			};
			//nothing
		};			
        //nothing		
	};

	
#ifdef LEXER_VERBOSE	
	printf("number of liner: %d \n",lexer_lineno);
	printf("first line: %d \n",lexer_firstline);
	printf("last line: %d \n",lexer_lastline);
	printf("token count: %d \n",lexer_token_count);	
#endif	
	
#ifdef PARSER_VERBOSE	
	printf("infile_size: %d bytes \n",infile_size);
    printf("outfile_size: %d bytes \n",outfile_size);	
#endif	
	
};






