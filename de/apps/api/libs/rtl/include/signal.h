
//signal.h 
//signal - ANSI C signal handling
//credits: 
//http://man7.org/linux/man-pages/man2/signal.2.html
//http://man7.org/linux/man-pages/man2/sigaction.2.html
//...


#ifndef  __SIGNAL_H
#define  __SIGNAL_H    1


#include <sys/types.h>


// Definitions
typedef          int  sig_atomic_t;
typedef unsigned int  sigset_t;
// ...


/* Type of a signal handler.  */
typedef void (*sighandler_t)(int);

typedef sighandler_t __sighandler_t;
typedef sighandler_t sig_t;
//...

#define SIG_DFL ((__sighandler_t) 0)
#define SIG_ERR ((__sighandler_t)-1)
#define SIG_IGN ((__sighandler_t) 1)


//credits: serenity os
#define SIGINVAL   0
#define SIGHUP     1
#define SIGINT     2
#define SIGQUIT    3
#define SIGILL     4
#define SIGTRAP    5
#define SIGABRT    6
#define SIGBUS     7
#define SIGFPE     8
#define SIGKILL    9
#define SIGUSR1    10
#define SIGSEGV    11
#define SIGUSR2    12
#define SIGPIPE    13
#define SIGALRM    14
#define SIGTERM    15
#define SIGSTKFLT  16
#define SIGCHLD    17
#define SIGCONT    18
#define SIGSTOP    19
#define SIGTSTP    20
#define SIGTTIN    21
#define SIGTTOU    22
#define SIGURG     23
#define SIGXCPU    24
#define SIGXFSZ    25
#define SIGVTALRM  26
#define SIGPROF    27
#define SIGWINCH   28
#define SIGIO      29
#define SIGPWR     30
#define SIGSYS     31
#define NSIG       32


//============


/* Type for data associated with a signal.  */
/* Data passed with notification */
typedef union sigval {          
     int     sival_int;         /* Integer value */
     void   *sival_ptr;         /* Pointer value */
}sigval_t;

typedef sigval_t  __sigval_t;
//...

struct sigevent 
{
    int          sigev_notify; /* Notification method */
    int          sigev_signo;  /* Notification signal */

    union sigval sigev_value;  /* Data passed with
                                         notification */
    void       (*sigev_notify_function) (union sigval);
                            /* Function used for thread
                               notification (SIGEV_THREAD) */
    void        *sigev_notify_attributes;
                            /* Attributes for notification thread
                               (SIGEV_THREAD) */
    pid_t        sigev_notify_thread_id;
                           /* ID of thread to signal (SIGEV_THREAD_ID) */
};


typedef struct siginfo {
    int      si_signo;     /* Signal number */
    int      si_errno;     /* An errno value */
    int      si_code;      /* Signal code */
    int      si_trapno;    /* Trap number that caused hardware-generated signal (unused on most architectures) */
    pid_t    si_pid;       /* Sending process ID */
    uid_t    si_uid;       /* Real user ID of sending process */
    int      si_status;    /* Exit value or signal */
    clock_t  si_utime;     /* User time consumed */
    clock_t  si_stime;     /* System time consumed */
    sigval_t si_value;     /* Signal value */
    int      si_int;       /* POSIX.1b signal */
    void    *si_ptr;       /* POSIX.1b signal */
    int      si_overrun;   /* Timer overrun count;
                                         POSIX.1b timers */
    int      si_timerid;   /* Timer ID; POSIX.1b timers */
    void    *si_addr;      /* Memory location which caused fault */
    long     si_band;      /* Band event (was int in
                                         glibc 2.3.2 and earlier) */
    int      si_fd;        /* File descriptor */
    short    si_addr_lsb;  /* Least significant bit of address
                                         (since Linux 2.6.32) */
    void    *si_lower;     /* Lower bound when address violation
                                         occurred (since Linux 3.19) */
    void    *si_upper;     /* Upper bound when address violation
                                         occurred (since Linux 3.19) */
    int      si_pkey;      /* Protection key on PTE that caused
                                         fault (since Linux 4.6) */
    void    *si_call_addr; /* Address of system call instruction
                                         (since Linux 3.5) */
    int      si_syscall;   /* Number of attempted system call
                                         (since Linux 3.5) */
    unsigned int si_arch;  /* Architecture of attempted system call
                                         (since Linux 3.5) */
}siginfo_t ;


typedef struct sigaction {
    void  (*sa_handler)(int);
    void  (*sa_sigaction)(int, siginfo_t *, void *);
    sigset_t  sa_mask;
    int  sa_flags;
    void  (*sa_restorer)(void);
}sigaction_t;

/*
// Gramado version defined in ring0.
struct sigaction_d 
{
    //void (*sa_handler)(int);    // #suspended

    unsigned long sa_handler;  // ring3 address.
    unsigned long sa_msgcode;  // msg code
    unsigned long sa_long1;    // data1
    unsigned long sa_long2;    // data2

    sigset_t sa_mask;
    int sa_flags;
};
*/


#ifndef _POSIX_SOURCE
#define SA_ONSTACK  0x0001  /* take signal on signal stack */
#define SA_RESTART  0x0002  /* do not restart system on signal return */
#endif
#define SA_NOCLDSTOP  0x0004	/* do not generate SIGCHLD on child stop */



//se precisar!
/*
#define SA_NOCLDSTOP 1
#define SA_NOCLDWAIT 2
#define SA_SIGINFO 4
#define SA_ONSTACK 0x08000000
#define SA_RESTART 0x10000000
#define SA_NODEFER 0x40000000
#define SA_RESETHAND 0x80000000
#define SA_NOMASK SA_NODEFER
#define SA_ONESHOT SA_RESETHAND
*/



/*
 * Flags for sigprocmask:
 */
#define	SIG_BLOCK    1  /* block specified signal set */
#define	SIG_UNBLOCK  2  /* unblock specified signal set */
#define	SIG_SETMASK  3  /* set specified signal set */


/*
#define SIG_BLOCK 0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2
*/




#ifndef _POSIX_SOURCE
/*
 * 4.3 compatibility:
 * Signal vector "template" used in sigvec call.
 */
struct sigvec {
	
	//#bugbug: Cuidado!
    void  (*sv_handler)(void);  /* signal handler */
    
    int     sv_mask;        /* signal mask to apply */
    int     sv_flags;       /* see signal options below */
};


#define SV_ONSTACK	SA_ONSTACK
#define SV_INTERRUPT	SA_RESTART	/* same bit, opposite sense */
#define sv_onstack sv_flags	/* isn't compatibility wonderful! */

/*
 * Structure used in sigaltstack call.
 */
struct	sigaltstack {
	char  *ss_base;     /* signal stack base */
	int    ss_len;      /* signal stack length */
	int    ss_onstack;  /* current status */
};

/*
 * Structure used in sigstack call.
 */
struct	sigstack {
	char  *ss_sp;       /* signal stack pointer */
	int    ss_onstack;  /* current status */
};


/*
 * Information pushed on stack when a signal is delivered.
 * This is used by the kernel to restore state following
 * execution of the signal handler.  It is also made available
 * to the handler to allow it to restore state properly if
 * a non-standard exit is performed.
 */
struct	sigcontext {
	int	sc_onstack;  /* sigstack state to restore */
	int	sc_mask;     /* signal mask to restore */
	int	sc_sp;       /* sp to restore */
	int	sc_fp;       /* fp to restore */
	int	sc_ap;       /* ap to restore */
	int	sc_pc;       /* pc to restore */
	int	sc_ps;       /* psl to restore */
};


/*
 * Macro for converting signal number to a mask suitable for
 * sigblock().
 */
#define sigmask(m)	(1 << ((m)-1))

#define	BADSIG		(void (*)())-1
#endif	/* _POSIX_SOURCE */




//
// signal()
//


//POSIX.1-2001, POSIX.1-2008, C89, C99.
sighandler_t signal (int signum, sighandler_t handler);


int 
sigaction ( 
    int signum, 
    const struct sigaction *act,
    struct sigaction *oldact );
    

int kill(pid_t pid, int sig);                
int raise (int sig);
int sigismember(const sigset_t *set, int signum); 
int killpg(int pgrp, int sig);
int sigemptyset(sigset_t *set); 
int sigfillset(sigset_t *set); 

int sigaddset(sigset_t *set, int signum); 
int sigdelset(sigset_t *set, int signum); 

#endif


//
// End
//

