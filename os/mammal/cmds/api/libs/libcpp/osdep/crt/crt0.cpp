
// crt0 implementation for Gramado OS libcpp.
// 2024 - Created by Fred Nora.

#include <cstddef>
#include "../gramado/syscalls.hpp"

// ==============================
// Import from the ring3 application.
extern int main(int argc, char *argv[]);

/*
// #bugbug: NULL
const char *argv[] = { 
    "-flag1", 
    "-flag2", 
    "-flag3", 
    "-flag4", 
    NULL 
};
*/

/*
	$USER:      Gives current user's name.
	$PATH:      Gives search path for commands.
	$PWD:       Gives the path of present working directory.
	$HOME:      Gives path of home directory.
	$HOSTNAME:  Gives name of the host.
	$LANG:      Gives the default system language.
	$EDITOR:    Gives default file editor.
	$UID:       Gives user ID of current user.
	$SHELL:     Gives location of current user's shell program.
*/

/*
// #bugbug: NULL
const char *my_environ[] = { 

    "DISPLAY=kgws-or-gwssrv",  //#todo
    "EDITOR=gramcode",
    "HOME=/HOME",
    "HOSTNAME=gramado",
    "LANG=en-us",
    "LANG2=pt-br",
    "LANGX=en-br",
    "OSTYPE=gramado",   //#todo
    "PATH=/:/PROGRAMS:/GRAMADO",
    "PS1=~$",
    "PS2=:>",
    "PWD=/",             //#bugbug: This is the working directory.
    "SHELL=gdeshell",
    "TERM=noraterm",     //#bugbug
    "TMP=/TMP",
    "UID=no-id",         //#todo
    "USER=anon",  
    NULL 
};
*/


void crt0(unsigned long rdi)
{

    asm volatile ("int $3");

// The IOPL defines the minimum CPL required to 
// directly access I/O ports and to 
// execute I/O Sensitive Instructions (IN, INS, OUT, OUTS, CLI, STI). 

    //asm volatile ("int $199");

//
// Call main().
//

    // IN: argc, argv.
    //main_ret = (int) main( token_count, tokenList );

//#debug
    while(1){
    };
}



