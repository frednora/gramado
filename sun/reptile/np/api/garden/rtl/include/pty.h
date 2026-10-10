/*
 * File: pty.h
 * 
 */


#include <termios.h>


int 
openpty ( 
    int *amaster, 
    int *aslave, 
    char *name,
    const struct termios *termp,
    const struct winsize *winp );


pid_t 
forkpty ( 
    int *amaster, 
    char *name,
    const struct termios *termp,
    const struct winsize *winp ); 



//
// End.
//

