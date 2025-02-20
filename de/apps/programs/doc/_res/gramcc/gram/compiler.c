
//compiler.h 

// aqui ficará o compilador, que transformará a linguagem intermediária 
// em assembly.


// lexer >> parser >> compiler


#include "gramc.h"

//mover isso para o header.

FILE *compiler (){
	
	FILE *__OUTPUT;
	int lexer_return = -1;
	int parser_return = -1;
	
	__OUTPUT = (FILE *) outfile;
	
	printf ("compiler: Initializing ...\n");
	
	
	lexer_return = (int) lexer ();
	//analizar retorno
	
	parser_return = (int) parser ();
	//analizar retorno	


     //
     // Nesse momento ja temos um arquivo de output.
     //

      //more ...
	
//done:	
	printf ("compiler: Done \n\n");
	
	// Returning the output.
	return (FILE *) __OUTPUT;
}

