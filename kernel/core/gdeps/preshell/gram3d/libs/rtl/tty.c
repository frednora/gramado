
// tty.c

#include <sys/types.h>  
#include <errno.h>
#include <utmp.h>
#include <stdio.h>


/*
    The login_tty() function prepares for a login on the terminal fd
    (which may be a real terminal device, or the slave of a
    pseudoterminal as returned by openpty()) by creating a new session,
    making fd the controlling terminal for the calling process, setting
    fd to be the standard input, output, and error streams of the current
    process, and closing fd. 
 */


int login_tty (int fd)
{
    debug_print ("login_tty: [TODO]\n");

    if(fd<0)
    {
        errno=EBADF;
        return (int) -1;
    }

    return -1;
}


