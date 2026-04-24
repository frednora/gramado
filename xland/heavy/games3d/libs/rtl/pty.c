/*
 * File: pty.c
 * 
 * 
 */


#include <sys/types.h>  

#include <stdio.h>

#include <pty.h>



//See: http://man7.org/linux/man-pages/man7/pty.7.html

/*
    A pseudoterminal (sometimes abbreviated "pty") is a pair of virtual
    character devices that provide a bidirectional communication channel.
    One end of the channel is called the master; the other end is called
    the slave. 

    The slave end of the pseudoterminal provides an interface
    that behaves exactly like a classical terminal.
*/


/*
The xterm program is a terminal emulator for the X Window System. 
It provides DEC VT102/VT220 (VTxxx) and Tektronix 4014 compatible 
terminals for programs that cannot use the window system directly.
*/


/*
       The openpty() function finds an available pseudoterminal and returns
       file descriptors for the master and slave in amaster and aslave.  If
       name is not NULL, the filename of the slave is returned in name.  If
       termp is not NULL, the terminal parameters of the slave will be set
       to the values in termp.  If winp is not NULL, the window size of the
       slave will be set to the values in winp.  
 */
//#todo

int 
openpty ( 
    int *amaster, 
    int *aslave, 
    char *name,
    const struct termios *termp,
    const struct winsize *winp)
{
    debug_print ("openpty: [TODO]\n");
    return -1;
}


/*
    The forkpty() function combines openpty(), fork(2), and login_tty()
    to create a new process operating in a pseudoterminal.  The file
    descriptor of the master side of the pseudoterminal is returned in
    amaster.  If name is not NULL, the buffer it points to is used to
    return the filename of the slave.  The termp and winp arguments, if
    not NULL, will determine the terminal attributes and window size of
    the slave side of the pseudoterminal. 
 */

pid_t 
forkpty (
    int *amaster, 
    char *name,
    const struct termios *termp,
    const struct winsize *winp)
{
    debug_print ("forkpty: [TODO]\n");
    return -1;
}

//
// End.
//

