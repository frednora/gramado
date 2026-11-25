/*
 * Gramado Operating System - The main file for the shell.
 * (c) Copyright 2015~2019 - Fred Nora.
 *
 * File: apps/shell/crt0.c
 *
 * Environment: Gramado Core.
 * Usado para inicializar a rt na libc99
 */

// #bugbug: 
// No momento estamos usando uma versão persnalizada do crt0,
// onde os argumentos são criados à mão. Mas precisamos usar o crt0 padrão
// que pega os argumentos enviados pelo processo pai.


#include "shell.h" 


static char *argv[] = { 
    "-interactive",        //shell interativo
	"-login",              //login
	"Gramado Core Shell",  //nome do shell
	"test.sh",             //nome do arquivo de script.
	NULL 
};


// Fake environment.
static char *envp[] = { 
    "VFSROOT=root:/volume0",           //root dir do vfs
    "BOOTVOLUMEROOT=root:/volume1",    //root dir do volume de boot
    "SYSTEMVOLUMEROOT=root:/volume2",  //root dir do volume do sistema
	NULL 
};


extern int main ( int argc, char *argv[] );	


//
// Main function in C part.
// The entry point is in head.s
//


// #todo
// Nessa versão do shell talvez precisemos pegar os agumentos que foram passados via linha de comando.
// ?? avaliando nisso ??

void crt0 (){
	
	int Response;
	
	//char **empty_string_pool;
	

    // Inicializando o suporte a alocação dinâmica de memória.
	// Inicializando o suporte ao fluxo padrão.
    // Call main().	
	
	libcInitRT ();
	stdioInitialize ();	


	Response = (int) main ( 3, argv ); 
								
									
	//
	// Chama kill ou exit de acordo com o problema ocorrido em main.
	// O erro pode vir no retorno ou implementa-se uma forma de pegar a execessão 
	// ocorrida durante a execussão de main.
	//
	
	switch (Response)
	{
	    case 0:
		    exit (0);
            break;
 
        default:
		    //exit(app_response);
			//exit(1);
			die ("crt0: EXIT ERROR! \n");
            break;		
	};
	
	//
	// ## ERROR ##
	//
	
hang:	
    printf("crt0: EXIT ERROR! \n");
    printf("crt0: *Hang!\n");
	
	while (1){ asm("pause"); };
}






