/*
 * File: builtins.c
 *     Buittin commands for Gramado Core Shell.
 * History:
 *     2018 - Created by Fred Nora.
 */

#include "shell.h"


void cd_buitins (void)
{	
	printf("cd_buitins:\n");
}

void cls_builtins (void)
{	
    shellClearScreen();
}

void copy_builtins(void)
{
}

void date_builtins(void)
{	
}

void del_builtins(void)
{	
}

void dir_builtins (void)
{
// @todo: 
// get set disk id e directory id.

    printf ("~dir \n");		        
	//#test 
	//173 Lista arquivos de um diretório, dado o número do disco,
    //o número do volume e o número do diretório,	
    system_call (173,0,0,0);		
	printf ("~done\n");	
}


//echo command.
//imprimir a lista de comandos de uma linha de comandos.

void echo_builtins (char *list[])
{	
    shell_print_tokenList( list, " ");
	//shell_print_tokenList( list, "/");
}

void exec_builtins (void)
{
    printf("@todo: ~exec\n");	
}

void exit_builtins (void)
{	
	printf ("Exiting shell ...\n");
	exit(0);	
}

int getpid_builtins (void)
{	
    shellShowPID ();	
	return 0; 
}

int getppid_builtins (void)
{	
	shellShowPPID ();	
	return 0;
}

int getgid_builtins (void)
{	
	shellShowGID ();	
	return 0;
}

int getuid_builtins (void)
{	
	shellShowUID ();
	return 0;
}

void help_builtins(int arg)
{	
	printf ("gdeshell:\n");
	
	switch (arg)
	{
		//all
		case 1:
		    printf ("Show all help topics\n");
			shellHelp ();
		    break;
		//min
		case 2:
		    printf ("cls, help, exit..\n");
			break;
		
		default:
			shellHelp ();
			break;
	};
}

void pwd_builtins (void)
{	
    //printf("~pwd - print working directory\n");
    system_call ( 170, 0, 0, 0 );		
}

//
// End
//
