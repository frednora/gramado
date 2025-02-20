
#include <types.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>


static char *ohboy_argv[] = { 
    "-flag1", 
    "-flag2", 
    "-flag3", 
    "-flag4", 
    NULL 
};

static char *ohboy_environ[] = { 

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


int main(void)
{
    environ = ohboy_environ;

    // See: stdlib/stdlib.c
    gramado_system_call(65,'@',0,0);
    libcInitRT();

    // See: stdio/stdio.c
    gramado_system_call(65,'x',0,0);
    stdioInitialize();

    printf ("\n");
    printf ("ohboy2.c: Hello world!\n");
    printf ("\n");

    //asm("int $3");
    return 0;
}















