// signal.c
// Ring 3 support for signals.
// Created by Fred Nora.

#include <types.h>
#include <sys/types.h>
#include <stddef.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <rtl/gramado.h> 


const char *sys_siglist[NSIG] = {

    "Invalid signal number",
    "Hangup",
    "Interrupt",
    "Quit",
    "Illegal instruction",
    "Trap",
    "Aborted",
    "Bus error",
    "Division by zero",
    "Killed",
    "User signal 1",
    "Segmentation violation",
    "User signal 2",
    "Broken pipe",
    "Alarm clock",
    "Terminated",
    "Stack fault",
    "Child exited",
    "Continued",
    "Stopped (signal)",
    "Stopped",
    "Stopped (tty input)",
    "Stopped (tty output)",
    "Urgent I/O condition)",
    "CPU limit exceeded",
    "File size limit exceeded",
    "Virtual timer expired",
    "Profiling timer expired",
    "Window changed",
    "I/O possible",
    "Power failure",
    "Bad system call",
};


/*
 * sigaction:
 * signum: 
 *     The signal number to be handled (e.g., SIGINT, SIGTERM).
 * act: 
 *     A pointer to a struct sigaction that specifies the new action for the signal.
 * oldact: 
 *     A pointer to a struct sigaction where the previous action for the signal is stored. 
 *     This can be NULL if you don't need the old action.
 */

// It is only used to set up (install) a signal handler.
 int 
 sigaction ( 
     int signum, 
     const struct sigaction *act,
     struct sigaction *oldact )
 {
    int r=0;

    debug_print ("sigaction: [TODO] \n");
 
    if (signum < 0)
        goto fail;
 
    r = 
    (int) sc80 ( 
        (unsigned long) 121, 
        (unsigned long) signum,
        (unsigned long) act, 
        (unsigned long) oldact ); 

    // Check return.
    if (r < 0){
        errno = (-r);
        return (int) (-1);
    }

 fail:
     return (int) -1;
 }


/*
 * signal:
 *     POSIX.1-2001, POSIX.1-2008, C89, C99.
 */

// It is only used to set up (install) a signal handler.
sighandler_t signal(int signum, sighandler_t handler)
{
    sighandler_t old_handler=0;
    unsigned long restorer = 0;   // #todo: We need to set a valid address here

    old_handler = 
        (sighandler_t) sc80 ( 
            (unsigned long) 122, 
            (unsigned long) signum,
            (unsigned long) handler, 
            (unsigned long) restorer ); 

    return (sighandler_t) old_handler;
}

// #test
// Ring 3 implementation of signal()
sighandler_t signal00(int signum, sighandler_t handler)
{
    return (sighandler_t) 0;
}


// #todo
// Sends a signal to a process or a process group.
int kill (pid_t pid, int sig)
{
    debug_print ("kill: [TODO] \n");

    if (pid<0)
    {
        errno=EINVAL;
        return (int) -1;
    }


	// #todo
	/*
	int ret;
	
	ret = (int) sc80 ( ?, ?, ?, ? );
	
    if (ret < 0)
	{
		errno = -ret;
		return (-1);
	}
	
	return (ret);
	*/

    return (int) (-1);
}


// ??
int raise (int sig)
{
    // FIXME: Support multi-threaded programs.
    return (int) kill( getpid(), sig);
}



int sigismember (const sigset_t* set, int signum)
{
    if (signum < 1 || signum > 32) 
    {
        errno = EINVAL;
        return -1;
    }

    if (*set & (1 << (signum - 1)))
    {
        return 1;
    }

    return 0;
}


int killpg (int pgrp, int sig)
{
    debug_print ("killpg: [TODO] \n");

    if( pgrp<0 )
    {
        errno=EINVAL;
        return -1;
    }

    return -1;
}


// ?? 
int sigemptyset (sigset_t *set)
{
    *set = 0;

    return 0;
}


// ??
int sigfillset (sigset_t *set)
{
    *set = 0xffffffff;

    return 0;
}



int sigaddset (sigset_t *set, int signum){

    if (signum < 1 || signum > 32) 
    {
        errno = EINVAL;
        return -1;
    }

    *set |= 1 << (signum - 1);

    return 0;
} 



int sigdelset (sigset_t *set, int signum)
{

    if (signum < 1 || signum > 32) 
    {
        errno = EINVAL;
        return -1;
    }
    
    *set &= ~(1 << (signum - 1));
    
    return 0;
}


/*
void psignal(int sig, const char *s);
void psignal(int sig, const char *s)
{}
*/



/*
void psiginfo (const siginfo_t *si, const char *s);
void psiginfo (const siginfo_t *si, const char *s)
{
    psignal (si->si_signo, s);
}
*/


//
// End.
//


