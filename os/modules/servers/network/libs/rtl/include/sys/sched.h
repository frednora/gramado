
//  sys/sched.h


#ifndef	_SYS_SCHED_H_
#define	_SYS_SCHED_H_



struct sched_param {
	int	sched_priority;
};


/*
 * Scheduling policies required by IEEE Std 1003.1-2001
 */
#define	SCHED_NONE	-1
#define	SCHED_OTHER	0
#define	SCHED_FIFO	1
#define	SCHED_RR	2

/*
 * Interface of CPU-sets.
 */

//#todo
//typedef struct _cpuset cpuset_t;


/*
 * CPU states.
 * XXX Not really scheduler state, but no other good place to put
 * it right now, and it really is per-CPU.
 */
#define	CP_USER		0
#define	CP_NICE		1
#define	CP_SYS		2
#define	CP_INTR		3
#define	CP_IDLE		4
#define	CPUSTATES	5


/*
 * Flags passed to the Linux-compatible __clone(2) system call.
 */
#define	CLONE_CSIGNAL		0x000000ff	/* signal to be sent at exit */
#define	CLONE_VM		0x00000100	/* share address space */
#define	CLONE_FS		0x00000200	/* share "file system" info */
#define	CLONE_FILES		0x00000400	/* share file descriptors */
#define	CLONE_SIGHAND		0x00000800	/* share signal actions */
#define	CLONE_PTRACE		0x00002000	/* ptrace(2) continues on
						   child */
#define	CLONE_VFORK		0x00004000	/* parent blocks until child
						   exits */
						   
						   
						   


#endif    //_SYS_SCHED_H_    


