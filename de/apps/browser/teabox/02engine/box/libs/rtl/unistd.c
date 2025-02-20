/*
 * File: unistd.c
 *     Unix standard.
 *     ... and maybe posix stuff.
 * History:
 *     2019 - Created by Fred Nora.
 */

#include <sys/types.h>  
#include <errno.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>   
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pty.h>
#include <utmp.h>
#include <sys/ioctl.h>
#include <sys/ioctls.h>
#include <termios.h>
#include <sys/utsname.h>
#include <stdio.h>
#include <string.h>
#include <spawn.h>
#include <rtl/gramado.h> 

#define UNISTD_SYSTEMCALL_FORK     71  
#define UNISTD_SYSTEMCALL_EXIT     70
#define UNISTD_SYSTEMCALL_GETPID   85
#define UNISTD_SYSTEMCALL_GETPPID  81
//#define UNISTD_SYSTEMCALL_GETGID ??
// ...

//
// Error
//

// Global thing. Don't use 'static'.
// Number of last error.
// Maybe errno.c is a better place for this. 
int errno=0;
int sys_nerr=0;

/*
//#todo: fix the order
const char *const sys_errlist[] = {

	"Operation succeeded",        //0 
	"Invalid argument",           //EINVAL
	"Bad memory address",         //EFAULT 
	"String too long",            //ENAMETOOLONG 
	"Out of memory",              //ENOMEM 
	"Input/output error",         //EIO 
	"No such file or directory",  //ENOENT 
	"Not a directory",            //ENOTDIR 
	"Is a directory",             //EISDIR 
	"File or object exists",      //EEXIST 
	"Cross-device link",          //EXDEV 
	"Try again later",            //EAGAIN 
	"Illegal seek",               //ESPIPE 
	"Unimplemented feature",      //EUNIMP 
	"Device not available",       //ENXIO 
	"No such device",             //ENODEV 
	"Device or resource busy",    //EBUSY 
	"Invalid/inappropriate ioctl", //EIOCTL (ENOTTY in Unix) 
	"Directory not empty",         // ENOTEMPTY 
	"Result too large",            // ERANGE 
	"No space left on device",     // ENOSPC 
	"Too many open files",         // EMFILE 
	"Too many open files in system", // ENFILE 
	"No such system call",        // ENOSYS 
	"File is not executable",     //ENOEXEC 
	"Argument list too long",     //E2BIG 
	"Bad file number",            //EBADF 
};
*/


// The <unistd.h> header shall declare 
// the following external variables:
char *optarg;
int opterr=0;
int optind=0; 
int optopt=0;


// unistd?
/* 
pid_t tcgetpgrp(int fd)
{
    return ioctl(fd, TIOCGPGRP);
}
*/


// unistd?
/*
int tcsetpgrp(int fd, pid_t pgid)
{
    return ioctl(fd, TIOCSPGRP, pgid);
} 
*/ 


// execv:
// #todo
// Testar essa rotina usando a variável **environ. 

char *__execv_environ[] = { 
    NULL, 
    NULL, 
    NULL 
};

int execv(const char *path, char *const argv[])
{
    if ( (void*) path == NULL ){
        errno = EINVAL;
        return -1;
    }

// #bugbug: 
// Falta a tipagem do último argumento?
    return (int) execve( path, (char **) argv, __execv_environ );
    //return (int) execve ( path, (char **) argv, (char **) __execv_environ );
    //return (int) execve ( path, (char **) argv, environ ); //#todo: use this one.
}

/*
 * execve:
 * 
 */

int 
execve ( 
    const char *path, 
    char *const argv[], 
    char *const envp[] )
{
    int value = -1;

    if( (void*) path == NULL ){
        errno = EINVAL;
        return -1;
    }

    value = 
        (int) gramado_system_call ( 
                  248, 
                  (unsigned long) path, 
                  (unsigned long) argv,   
                  (unsigned long) envp );  

    if (value < 0)
    {
        errno = (-value);
        return (int) (-1);
    } 

    return (int) value;
}

ssize_t read_tty (int fd, const void *buf, size_t count)
{
    if (fd<0)
    {
        errno=EBADF;
        return -1;
    }
    
    return (ssize_t) gramado_system_call ( 
                         272, 
                         (unsigned long) fd,      // dispositivo.
                         (unsigned long) buf, 
                         (unsigned long) count ); 
}


// o descritor seleciona uma tty em ttyList[]
ssize_t write_tty (int fd, const void *buf, size_t count)
{
    if (fd<0)
    {
        errno=EBADF;
        return -1;
    }

    return (ssize_t) gramado_system_call ( 
                         273, 
                         (unsigned long) fd,      // dispositivo.
                         (unsigned long) buf, 
                         (unsigned long) count ); 
}

// read on virtual console!
// range: 0 ~ 3
ssize_t read_VC (int fd, const void *buf, size_t count)
{
    if (fd<0)
    {
        errno=EBADF;
        return -1;
    }

    return (ssize_t) gramado_system_call ( 
                         262, 
                         (unsigned long) fd,      // dispositivo.
                         (unsigned long) buf, 
                         (unsigned long) count ); 

}

// write on virtual console!
// range: 0 ~ 3
ssize_t write_VC (int fd, const void *buf, size_t count)
{
    if (fd<0)
    {
        errno=EBADF;
        return -1;
    }

    return (ssize_t) gramado_system_call ( 
                         263, 
                         (unsigned long) fd,      // dispositivo.
                         (unsigned long) buf, 
                         (unsigned long) count ); 
}

ssize_t read(int fd, const void *buf, size_t count)
{
// (Input port)

    ssize_t value = (-1);

    if (fd<0){
        errno = EBADF;
        return (ssize_t) (-1);
    }
    if ((void *) buf == NULL){
        errno = EINVAL;
        return (ssize_t) (-1);
    }
    if (count <= 0){
        errno = EINVAL;
        return (ssize_t) (-1);
    }

// Syscall 18.
// (Input adapter. System dependent)
    value = 
        (ssize_t) sc82 ( 
            (unsigned long) 18, 
            (unsigned long) fd,
            (unsigned long) buf, 
            (unsigned long) count );

    if (value < 0){
        errno = (-value);
        return (ssize_t) (-1);
    }

    return (ssize_t) value;
}

ssize_t write(int fd, const void *buf, size_t count)
{
// (Output port)

    ssize_t value = (-1);

    if (fd<0){
        errno = EBADF;
        return (ssize_t) (-1);
    }
    if ((void*) buf == NULL){
        errno = EINVAL;
        return (ssize_t) (-1);
    }
    if (count <= 0){
        errno = EINVAL;
        return (ssize_t) (-1);
    }

// Syscall 19.
// (Output adapter. System dependent)
    value = 
        (ssize_t) sc82 ( 
            (unsigned long) 19, 
            (unsigned long) fd,
            (unsigned long) buf, 
            (unsigned long) count ); 

    if (value<0){
        errno = (-value);
        return (ssize_t) (-1);
    }

    return (ssize_t) value;
}


ssize_t 
pread(
    int fd, 
    void *buf, 
    size_t count, 
    off_t offset )
{
    debug_print ("pread: [TODO]\n");

    if (fd<0)
    {
        debug_print ("pread: fd\n");
        errno = EBADF;
        return -1;
    }

    if ( (void*) buf == NULL )
    {
        errno = EINVAL;
        return -1;
    }

    if ( count <= 0 )
    {
        errno = EINVAL;
        return -1;
    }

    return -1;

    /*   
    // Credits: Serenity OS. 
    // FIXME: This is not thread safe and should be implemented in the kernel instead.
    off_t old_offset = lseek(fd, 0, SEEK_CUR);
    lseek(fd, offset, SEEK_SET);
    ssize_t nread = read(fd, buf, count);
    lseek(fd, old_offset, SEEK_SET);
    return nread;
    */
}


// #todo: not implemented yet.
ssize_t 
pwrite (
    int fd, 
    const void *buf, 
    size_t count, 
    off_t offset )
{
    debug_print ("pwrite: [TODO]\n");

    if (fd<0){
        errno = EBADF;
        return -1;
    }

    if ( (void*) buf == NULL )
    {
        errno = EINVAL;
        return -1;
    }

    return -1;
}


// #todo
int truncate(const char *path, off_t length)
{ 
    debug_print ("truncate: [TODO]\n");

    if ( (void*) path == NULL ){
        errno=EINVAL;
        return -1;
    }
    if (*path == 0){
        errno=EINVAL;
        return -1;
    }

//#todo

    return -1;
}


int ftruncate (int fd, off_t length)
{ 
    debug_print ("ftrucate: [TODO]\n");

    if (fd<0){
        errno=EBADF;
        return -1;
    }

    return -1;
}


/*
 * exit:
 *     Torna zombie a thread atual.
 *     Mas o propósito é terminar sair do 
 *     programa, terminando o processo e
 *     liberar os recursos que o processo estava usando.
 *     #importante:
 *     @todo: se o status for (1) devemos imprimir o conteúdo 
 * de stderr na tela.
 * ?? Essa função também pertence à unistd
 */
// cause normal process termination
// Remember: The parent process called wait().
// The exit() function causes normal process termination and the value
// of status & 0xFF is returned to the parent.

void exit(int status)
{
    int value=-1;

    debug_print ("exit:\n");

    // #todo
    // Chamar rotina para finalizar a biblioteca.
    // Liberando os recursos usados.

    // #importante:
    // #todo: 
    
    // ??
    // Se o status for (1) devemos imprimir o conteúdo 
    // de stderr na tela.

    //if(status==1)
        //printf(stderr);

// Syscall 70.
    value = 
        (int) gramado_system_call( 
                  UNISTD_SYSTEMCALL_EXIT, 
                  (unsigned long) status, 
                  (unsigned long) status, 
                  (unsigned long) status );

    //if (value ...

// Wait forever.
    while (1){ asm ("pause"); };
}


pid_t fork(void)
{
    pid_t value = -1;

    debug_print("fork: \n");

// Syscall 71.
    value = 
        (pid_t) gramado_system_call ( 
                    UNISTD_SYSTEMCALL_FORK, 
                    0, 
                    0, 
                    0 );

    if (value<0)
    {
        debug_print("fork: [FAIL]\n");
        errno = (-value);
        return (pid_t) (-1);
    }

    debug_print("fork: Done\n");
    return (pid_t) value;
}


// #todo
pid_t vfork(void)
{
    return (pid_t) fork();
}


// Set user identity.
// Sets the effective user ID of the calling process.
// https://man7.org/linux/man-pages/man2/setuid.2.html
int setuid(uid_t uid)
{
    int value = -1;

    if (uid<0){
        errno = EINVAL;
        return (int) -1;
    }

// #todo: 
// Ainda não temos a suystem call.
// SYSTEMCALL_SETCURRENTUSERID

    value = (uid_t) gramado_system_call ( 151, 0, 0, 0 );
    if (value<0){
        errno = (-value);
        return (int) -1; 
    }

    return (int) value;
}

// Get user identity.
// Returns the real user ID of the calling process.
// https://man7.org/linux/man-pages/man2/getuid.2.html
uid_t getuid(void)
{
    uid_t value=-1;

// Syscall 152
    value = (uid_t) gramado_system_call(152,0,0,0);
    if (value<0){
        errno = (-value);
        return (int) -1; 
    }

    return (int) value;
}


// ===============================
// gid
int setgid(gid_t gid)
{
    if(gid<0)
    {
        errno=EINVAL;
        return -1;
    }
    return -1;
}
gid_t getgid (void)
{
    //SYSTEMCALL_GETCURRENTGROUPID
    return (gid_t) gramado_system_call ( 154, 0, 0, 0 );
}

// ===============================
// euid
int seteuid(uid_t euid)
{
    if (euid<0)
    {
        errno=EINVAL;
        return -1;
    }
    return -1;
}
uid_t geteuid(void)
{
    debug_print ("geteuid: [TODO]\n");
    //return (uid_t) gramado_system_call ( ?, 0, 0, 0 );
    return -1;
} 


// ===============================
// egid
int setegid(gid_t egid)
{
    if (egid<0)
    {
        errno=EINVAL;
        return -1;
    }
    return -1;
}
gid_t getegid(void)
{
    return -1;
}


// ===============================
// pid
pid_t getpid(void)
{
    unsigned long ul_value=0;
    ul_value = 
        (unsigned long) gramado_system_call ( 
            UNISTD_SYSTEMCALL_GETPID, 0, 0, 0 );
// 32bit value.
    return (pid_t) (ul_value & 0xFFFFFFFF);
}


// ===============================
// ppid
pid_t getppid(void)
{
    unsigned long ul_value=0;
    ul_value = 
        (unsigned long) gramado_system_call ( 
            UNISTD_SYSTEMCALL_GETPPID, 0, 0, 0 );
// 32bit value.
    return (pid_t) (ul_value & 0xFFFFFFFF);
}


/*
Get terminal foreground process group.
Will return the foreground process group ID of that terminal.
The function tcgetpgrp() returns the process group ID of the 
foreground process group on the terminal associated to fd, 
which must be the controlling terminal of the calling process. 
See: https://linux.die.net/man/3/tcsetpgrp
*/
// #??
// Why the return type is pid_t?
// gid_t?
pid_t tcgetpgrp(int fd)
{
    pid_t value = -1;
    int s=0;

    if (fd<0)
    {
        errno = EBADF;
        return (pid_t) -1;
    }

// #todo: 
// work in ring0 to implement this.

    value = (pid_t) ioctl(fd, TIOCGPGRP, &s);
    if (value<0){
        errno = (-value);
        return (pid_t) (-1);
    }

    return (pid_t) s;
}


/*
The function tcsetpgrp() makes the process group with process 
group ID pgrp the foreground process group on the terminal 
associated to fd, which must be the controlling terminal 
of the calling process, and still be associated with its session. 
Moreover, pgrp must be a (nonempty) process group belonging 
to the same session as the calling process.   
See: https://linux.die.net/man/3/tcsetpgrp 
 */

int tcsetpgrp(int fd, pid_t pgrp)
{
    int value = -1;
    int s=0;

    if (fd<0){
        errno = EBADF;
        return -1;
    }
    if (pgrp<0){
       errno = EINVAL;
       return -1;
    }

    s = pgrp;

// #todo: 
// work in ring0 to implement this.
    
    value = (int) ioctl(fd, TIOCSPGRP, &s);
    if (value<0){
        errno = (-value);
        return (int) -1;
    }

    return (int) value;
}


// #todo: Not implemented yet.
int setpgid(pid_t pid, pid_t pgid)
{
    debug_print ("setpgid: [TODO]\n");

    if (pid<0){
        errno=EINVAL;
        return -1;
    }
    if (pgid<0){
        errno=EINVAL;
        return -1;
    }

// #todo
        
    return -1;
}


pid_t getpgid(pid_t pid)
{
    debug_print ("getpgid: [TODO]\n");

    if (pid<0){
        debug_print ("getpgid: pid\n");
        errno=EINVAL;
        return -1;
    }

    return (pid_t) (-1);
}


/*
 There is differente versions of these routines.
 See:
 https://man7.org/linux/man-pages/man2/getpgrp.2.html 
 */


/* System V version */
int setpgrp(void)
{
    debug_print ("setpgrp: [TODO]\n");
    return -1;
}


/* POSIX.1 version */
pid_t getpgrp(void)
{
    debug_print ("getpgrp: [TODO]\n");
    return -1;
}



/* BSD version */
int bsd_setpgrp(pid_t pid, pid_t pgid)
{
    debug_print ("bsd_setpgrp: [TODO]\n");

    if (pid<0){
        errno=EINVAL;
        return -1;
    }
    if (pgid<0){
        errno=EINVAL;
        return -1;
    }

//#todo

    return -1;
}


/* BSD version */
pid_t bsd_getpgrp(pid_t pid)
{
    debug_print ("bsd_getpgrp: [TODO]\n");

    if (pid<0){
        debug_print ("bsd_getpgrp: pid\n");
        errno=EINVAL;
        return -1;
    }

//#todo

    return -1;
}


char *getcwd (char *buf, size_t size)
{
    debug_print ("getcwd: [TODO]\n");

    if ( (void*) buf == NULL )
    {
        errno=EINVAL;
        return NULL;
    }

    if (size<0)
    {
        errno=EINVAL;
        return NULL;
    }


    /*
    if (!buffer) {
        size = size ? size : PATH_MAX;
        buffer = (char*)malloc(size);
    }
    */
        
    return NULL;
}


char *getwd(char *buf)
{
    debug_print ("getwd: [TESTING]\n");

    if ( (void*) buf == NULL )
    {
        errno=EINVAL;
        return NULL;
    }

    char *p = getcwd(buf,PATH_MAX);

    return (char *) p;
}


char *get_current_dir_name(void)
{
    debug_print ("get_current_dir_name: [TODO]\n");
    return NULL;
}


/*
int gettid()
{
    //todo: use syscall!!
}
*/


/*
void sysbeep()
{
    //todo: use syscall!!
}
*/



/*
//#todo:
//Duplicates an opened file descriptor.
void fcntl_dup(int fd);
void fcntl_dup(int fd)
{
    if(fd<0)
        return;

    fcntl(fd, F_DUPFD, 0)
}
*/


// dup()
// https://man7.org/linux/man-pages/man2/dup.2.html

int dup(int oldfd)
{
    int value = -1;

    if (oldfd < 0){
        errno = EBADF;
        return (int) -1;
    }

// Syscall 600.
    value = 
        (int) gramado_system_call( 
                  600,
                  (unsigned long) oldfd,
                  (unsigned long) oldfd,
                  (unsigned long) oldfd );

    if (value<0){
        errno = (-value);
        return (int) -1;
    }

    return (int) value;
}


// dup2()

int dup2(int oldfd, int newfd)
{
    int value = -1;

    if (oldfd < 0){
        errno = EBADF;
        return (int) -1;
    }
    if (newfd < 0){
        errno = EBADF;
        return (int) -1;
    }

// Syscall 601.
    value = 
        (int) gramado_system_call ( 
                  601,
                  (unsigned long) oldfd, 
                  (unsigned long) newfd, 
                  0 );

    if (value<0){
        errno = (-value);
        return (int) -1;
    }

    return (int) value;
}


// dup3()

int dup3(int oldfd, int newfd, int flags)
{
    int value = -1;

    if (oldfd < 0){
        errno = EBADF;
        return (int) -1;
    }
    if (newfd < 0){
        errno = EBADF;
        return (int) -1;
    }

// Syscall 602
    value = 
        (int) gramado_system_call ( 
                  602,
                  (unsigned long) oldfd, 
                  (unsigned long) newfd, 
                  (unsigned long) flags );

    if (value<0){
        errno = (-value);
        return (int) -1;
    }

    return (int) value;
}


// See: sys/resource.h
int getpriority(int which, id_t who)
{
    debug_print ("getpriority: [TODO]\n");
    return -1;
}


// See: sys/resource.h
int setpriority (int which, id_t who, int prio)
{
    debug_print ("setpriority: [TODO]\n");
    return -1;
}


// nice:
// Change process priority.
// #todo:
// It looks very wasy to implement.

int nice(int inc)
{
    debug_print ("nice: [TODO]\n");
    return -1;    //#todo

//#todo

/*
    int prio;
    errno = 0;
    prio = getpriority(PRIO_PROCESS, 0);
    if (prio == -1 && errno)
        return -1;
    if (setpriority(PRIO_PROCESS, 0, prio + incr) == -1) {
        if (errno == EACCES)
            errno = EPERM;
        return -1;
    }
    return getpriority(PRIO_PROCESS, 0);
*/

}


int pause(void)
{
    debug_print ("pause: [TODO]\n");
    return -1; //#todo
}

int mkdir(const char *pathname, mode_t mode)
{
    int value = -1;

    debug_print ("mkdir: [TODO]\n");

    if ( (void*) pathname == NULL )
    {
        errno=EINVAL;
        return -1;
    }

    if ( *pathname == 0 )
    {
        errno=EINVAL;
        return -1;
    }

// #todo
// 'mode' parameter.
    value = 
        (int) gramado_system_call ( 
                  44, 
                  (unsigned long) pathname, 
                  0, 
                  0 );

    if (value<0)
    {
        errno= (-value);
        return -1;
    }

    return (int) value;
}

int rmdir(const char *pathname)
{
    debug_print ("rmdir: [TODO]\n");

    if ( (void*) pathname == NULL )
    {
        errno=EINVAL;
        return -1;
    }

    if ( *pathname == 0 )
    {
        errno=EINVAL;
        return -1;
    }

    return -1; //#todo
}


// Links a name to a file.
// #todo: Not implemented yet.
int link(const char *oldpath, const char *newpath)
{
    debug_print ("link: [TODO]\n");

    if ( (void*) oldpath == NULL )
    {
        errno = EINVAL;
        return -1;
    }

    if ( (void*) newpath == NULL )
    {
        errno = EINVAL;
        return -1;
    }

// #todo

    return -1; //#todo
}


//#todo.
int unlink (const char *pathname)
{
    debug_print ("unlink: [TODO]\n");

    if ( (void*) pathname == NULL )
    {
        errno = EINVAL;
        return -1;
    }

    if ( *pathname == 0 )
    {
        errno = EINVAL;
        return -1;
    }

    // ...
    
    return (int) (-1);
}


int mlock (const void *addr, size_t len)
{
    debug_print ("mlock: [TODO]\n");

    if ( (void*) addr == NULL )
    {
        errno = EINVAL;
        return -1;
    }

    if (len<0)
    {
        errno = EINVAL;
        return -1;
    }

    // ...
    
    return -1; //#todo
}


int munlock (const void *addr, size_t len)
{
    debug_print ("munlock: [TODO]\n");

    if ( (void*) addr == NULL )
    {
        errno = EINVAL;
        return -1;
    }

    if (len<0){
        errno = EINVAL;
        return -1;
    }

// #todo

    return -1; //#todo
}


int mlockall(int flags)
{
    debug_print ("mlockall: [TODO]\n");
    return -1; //#todo
}

int munlockall(void)
{
    debug_print ("munlockall: [TODO]\n");
    return -1; //#todo
}

// #??  name?
long sysconf(int name)
{
    debug_print ("sysconf: [TODO]\n");
    return -1; //#todo
}

// ==========================
// sync - salva no disco.
// commit buffer cache to disk 
// sync, syncfs - commit buffer cache to disk.
// sync() causes all buffered modifications to file metadata and 
// data to be written to the underlying file systems.
// fflush - flush a stream
// See also: fsync and syncfs.
// sync() won't write buffers from your process 
// (or any other process) to the kernel buffer pool; it is wholly unrelated to fflush().
// sync() affects all data written by all processes 
// on the system — you can become unpopular if your application 
// uses it very often; it subverts the good work the kernel does caching data.
// daemon
// Unix systems typically run some kind of flush or update daemon, 
// which calls the sync function on a regular basis.
// See:
// https://en.wikipedia.org/wiki/Sync_(Unix)
// ...

void sync(void)
{
    debug_print ("sync: [TODO]\n");
    
    // #todo: 
    // use syscall!!
}


// commit buffer cache to disk 
int syncfs(int fd)
{
    debug_print ("syncfs: [TODO]\n");

    if (fd<0){
        debug_print ("syncfs: fd\n");
        errno = EBADF;
        return -1;
    }    

    // ...
    
    return -1;
}

// fsync:
// Isso salva no disco o buffer que esta em ring0 .
// fsync: synchronize a file's in-core state with storage device
// fflsush: flush a stream
// fsync works on a lower level, 
// it tells the OS to flush its (ring0) buffers to the physical media.
// OSs heavily cache data you write to a file. 
// If the OS enforced every write to hit the drive, 
// things would be very slow. fsync (among other things) 
// allows you to control when the data should hit the drive.
// fsync() will synchronize all of the given file's data and 
// metadata with the permanent storage device. 
// It should be called just before the corresponding file has been closed.
// sync() will commit all modified files to disk.
// The related system call fsync() commits just the 
// buffered data relating to a specified file descriptor.[1] 

int fsync(int fd)
{
    debug_print ("fsync: [TODO]\n");
    
    if (fd<0){
        errno=EBADF;
        return -1;
    }    

    // ...
    
    return -1;    //#todo
}

// fdatasync:
// fdatasync() is also available to write out just the changes made 
// to the data in the file, and not necessarily the file's related metadata.

int fdatasync(int fd)
{
    debug_print ("fdatasync: [TODO]\n");

    if (fd<0){
        errno = EBADF;
        return -1;
    }    

    // ...

    return -1; //#todo
}

// close()
// Isso deve fechar o arquivo na lista de arquivo abertos.
// SVr4, 4.3BSD, POSIX.1-2001.
// https://man7.org/linux/man-pages/man2/close.2.html
/*
A successful close does not guarantee that the data has been
successfully saved to disk, as the kernel uses the buffer cache
to defer writes.  Typically, filesystems do not flush buffers
when a file is closed.  If you need to be sure that the data is
physically stored on the underlying disk, use fsync(2).  (It will
depend on the disk hardware at this point.)
*/

int close(int fd)
{
    int value = -1;

    if (fd<0){
        errno = EBADF;
        return (int) (-1);
    }

// Syscall 17.
// Is it for sys_close() in kernel?
    value = 
        (int) gramado_system_call ( 
                  17, 
                  (unsigned long) fd, 
                  (unsigned long) fd, 
                  (unsigned long) fd );

    if (value<0){
        errno = (-value);
        return (int) (-1);
    }

    return (int) value;
}

int pipe2 ( int pipefd[2], int flags )
{
    return (int) gramado_system_call ( 
                     247, 
                     (unsigned long) pipefd, 
                     (unsigned long) flags, 
                     (unsigned long) 0 );
}

int pipe(int pipefd[2])
{
    int value=-1;
    value = (int) pipe2(pipefd,0);
    if(value<0)
    {
        errno=(-value);
        return -1;
    }
    return (int) value;
}

long fpathconf (int fildes, int name)
{
    debug_print ("fpathconf: [TODO]\n");
    return -1;
}

long pathconf (const char *pathname, int name)
{
    debug_print ("pathconf: [TODO]\n");

    if ( (void*) pathname == NULL )
    {
        errno=EINVAL;
        return -1;
    }
    
    if (*pathname == 0)
    {
        errno=EINVAL;
        return -1;
    }


    return -1;
} 


// =======================
// __gethostname:

char __Hostname_buffer[64];

char *__gethostname(void)
{
// Adapter. System dependent.
    gramado_system_call ( 
        801, 
       (unsigned long) &__Hostname_buffer[0],
       (unsigned long) &__Hostname_buffer[0],
       (unsigned long) &__Hostname_buffer[0] );

    return __Hostname_buffer;
}


// gethostname:
// Passando para o kernel o buffer que está no app.
// See: 
// http://man7.org/linux/man-pages/man2/sethostname.2.html

int gethostname (char *name, size_t len)
{
    int value = -1;

    if ( (void*) name == NULL )
    {
        printf ("gethostname: buffer fail\n");
        errno=EINVAL;
        return -1;
    }

    if (len<0){
        printf ("gethostname: len fail\n");
        errno=EINVAL;
        return -1;
    }

    value = 
        (int) gramado_system_call ( 
                  38, 
                  (unsigned long) name,
                  (unsigned long) name,
                  (unsigned long) name );

    if (value<0){
        errno = (-value);
        return -1;
    }

    return (int) value;
}


// sethostname:
// See:
// http://man7.org/linux/man-pages/man2/sethostname.2.html
int sethostname (const char *name, size_t len)
{
    int value=-1;

    if ( (void*) name == NULL )
    {
        errno=EINVAL;
        return -1;
    }

    if (*name == 0)
    {
        errno=EINVAL;
        return -1;
    }

    if (len<0){
        errno=EINVAL;
        return -1;
    }

    value = 
        (int) gramado_system_call ( 
                  39, 
                  (unsigned long) name,
                  (unsigned long) name,
                  (unsigned long) name );
    if (value<0){
        errno = (-value);
        return -1;
    }

    return (int) value;
}


/*
// minix 3
int __getlogin(char *logname, size_t sz);
int __getlogin(char *logname, size_t sz)
{
  struct passwd *pw_entry;

  pw_entry = getpwuid(getuid());

  if (pw_entry == (struct passwd *)NULL)
    return 0; 
    
  strncpy(logname, pw_entry->pw_name, sz);
  return sz;
}
*/




/*
 * getlogin: 
 * 
 */

char __Login_buffer[64];
char *getlogin (void)
{
    gramado_system_call ( 
        803, 
        (unsigned long) &__Login_buffer[0],
        (unsigned long) &__Login_buffer[0],
        (unsigned long) &__Login_buffer[0] );

    return __Login_buffer;
}


int setlogin (const char *name)
{
    int value=-1;

    if ( (void*) name == NULL )
    {
        errno=EINVAL;
        return -1;
    }

    if (*name == 0)
    {
        errno=EINVAL;
        return -1;
    }

    value = 
        (int) gramado_system_call ( 
                  804, 
                  (unsigned long) name,
                  (unsigned long) name,
                  (unsigned long) name );
    if (value<0){
        errno=(-value);
        return -1;
    }

    return (int) value;
}


// getusername 
// #todo
// usar  setlogin 
int getusername (char *name, size_t len)
{
    int value=0;

    if ( (void*) name == NULL )
    {
        printf ("getusername: buffer fail\n");
        errno=EINVAL;
        return -1;
    }

    if ( len < 0 || len > HOST_NAME_MAX )
    {
        printf ("getusername: len\n");
        errno=EINVAL;
        return -1;
    }

// Coloca no buffer interno.

    value = 
        (int) gramado_system_call ( 
                  40, 
                  (unsigned long) name,
                  (unsigned long) name,
                  (unsigned long) name );

    if (value<0){
        errno = (-value);
        return -1;
    }

    if (value > HOST_NAME_MAX){
        return -1;
    }

    if (value > len){
        value = len;
    }

    return (int) value;
}

// setusername:
// Um pequeno buffer foi passado pelo aplicativo.
// O limite precisa ser respeitado. 
// #todo
// usar setlogin

int setusername (const char *name, size_t len)
{
    size_t __name_len = 0;
    int value = -1;

    if ( (void*) name == NULL )
    {
        errno=EINVAL;
        return -1;
    }

    if (*name == 0)
    {
        errno=EINVAL;
        return -1;
    }

    if (len<0){
        errno=EINVAL;
        return -1;
    }

// size.
    __name_len = strlen(name) + 1;

// Limite dado pelo sistema.
    if (len < 0 || len >= HOST_NAME_MAX )
    {
        printf ("setusername: *len\n");
        errno = EINVAL;
        return -1;
    }

// Tamanho indicado pelo aplicativo.
    if (__name_len > len){
        printf ("setusername: len\n");
        errno = EINVAL;
        return -1;     
    }

    value = 
        (int) gramado_system_call( 
                  41, 
                  (unsigned long) name,
                  (unsigned long) name,
                  (unsigned long) name );

    if (value<0){
        errno = (-value);
        return -1;
    }

    return (int) value;
}


// ttyname:
// POSIX.1-2001, POSIX.1-2008, 4.2BSD.
// ttyname, ttyname_r - return name of a terminal
// If the file descriptor filedes is associated with a terminal device, 
// the ttyname function returns a pointer to a statically-allocated, 
// null-terminated string containing the file name of the terminal file.
// The value is a null pointer if the file descriptor isn't associated 
// with a terminal, or the file name cannot be determined. 
// ok
// então o buffer para esse nome deve ficar aqui na libc em ring3
// Onde o app pode ler.
// Chamremos o kernel e diremos, coloque o nome aqui nesse buffer.
// char __ttyname_buffer[64];

char *ttyname (int fd)
{
    static char buf[PATH_MAX];
    int rv=0;
    
    if (fd<0){
        errno=EBADF;
        return NULL;
    }
    rv = (int) ttyname_r (fd, buf, sizeof(buf));
    if (rv < 0){
        errno = (-rv);
        return NULL;
    }

    return (char *) buf;
}


// POSIX.1-2001, POSIX.1-2008, 4.2BSD.
// ttyname, ttyname_r - return name of a terminal
// #todo: Not implemente yet.

int ttyname_r(int fd, char *buf, size_t buflen)
{ 
    debug_print ("ttyname_r: [TODO]\n");
    
    if (fd<0){
        errno=EBADF;
        return -1;
    }

    if ( (void*) buf == NULL )
    {
        errno=EINVAL;
        return -1;
    }

    if (buflen<0){
        errno=EINVAL;
        return -1;
    }

    if (buflen >= PATH_MAX){
        errno=EINVAL;
        return -1;
    }

    //#todo

    // ...
    
    return -1; 
}


// #todo
// ??
/*
int ttyslot (void);
int ttyslot (void)
{
   return -1;
}
*/


/*
// Credits: Fred Nora
// Coloca a tty do processo no seguinte slot de arquivos abertos.
int ttyto( int fd);
int ttyto( int fd)
{
    return (int) gramado_system_call ( ?, 
                    (unsigned long) fd,
                    (unsigned long) fd,
                    (unsigned long) fd );
}
*/


// POSIX.1-2001, POSIX.1-2008, SVr4, 4.3BSD.
// isatty - test whether a file descriptor refers to a terminal
// This function returns 1 if filedes is a file descriptor associated 
// with an open terminal device, and 0 otherwise. 

int isatty (int fd)
{
    int Ret=-1;
    struct termios  t;

    if (fd<0){
        errno=EBADF;
        return -1;
    }

    Ret = (int) tcgetattr(fd,&t);

    //#todo: Simplify this thing.
    //return ( tcgetattr(fd, &t) != -1 ); 
    //return ( tcgetattr(fd, &t) == 0);

// Redundant, but fun.
    // YES
    if (Ret == TRUE) { return TRUE;  }
    // NO
    if (Ret == FALSE){ return FALSE; }
// Crazy fail
    return -1;
}


/*
 //alternative
int isatty(int fd);
int isatty(int fd)
{
    struct termios tmp;

    if (ioctl(fd,TCGETS,&tmp)<0)
        return (0);
    return 1;
}
*/

int 
getopt (
    int argc, 
    char *const argv[], 
    const char *optstring )
{
    debug_print ("getopt: [TODO]\n");

    if (argc<0){
        errno=EINVAL;
        return -1;
    }

    if ( (void*) optstring == NULL ){
        errno=EINVAL;
        return -1;
    }

    return -1;
}


// fstat:
// sys/stat.h
int fstat(int fd, struct stat *buf)
{
    int value = -1;

    debug_print ("fstat: [TODO]\n");

    if (fd<0){
        errno = EBADF;
        return -1;
    }

    value = 
        (int) gramado_system_call ( 
                  4005, 
                  (unsigned long) fd,
                  (unsigned long) buf,
                  (unsigned long) buf );

    if (value<0){
        errno = (-value);
        return (int) -1;
    }

    return (int) value;
}


// stat:
// sys/stat.h
int stat(const char *path, struct stat *buf)
{
    int value = -1;
    int _fd = -1;

    debug_print ("stat: [TODO]\n");

    if ( (void*) path == NULL )
    {
        errno = EINVAL;
        return -1;
    }
    
    if ( *path == 0 )
    {
        errno = EINVAL;
        return -1;
    }

// Open
    _fd = (int) open (path, 0, 0);
    if (_fd<0){
        errno = EBADF;
        return -1;
    }

// fstat
    value = (int) fstat(_fd,buf);
    close(_fd);
    if (value<0){
        errno = (-value);
        return (int) -1;
    }

    return (int) value;
}


// sys/stat.h
int lstat(const char *path, struct stat *buf)
{
    debug_print ("lstat: [TODO]\n");

    if ( (void*) path == NULL ){
        errno = EINVAL;
        return -1;
    }

    if ( *path == 0 ){
        errno = EINVAL;
        return -1;
    }

    return (int) stat(path, buf);
}


// POSIX.1-2001, POSIX.1-2008, SVr4, 4.3BSD.
// See:
// https://man7.org/linux/man-pages/man2/alarm.2.html
// alarm() returns the number of seconds remaining until any
//       previously scheduled alarm was due to be delivered, or zero if
//       there was no previously scheduled alarm.
unsigned int alarm (unsigned int seconds)
{
    debug_print ("alarm: [This is a work in progress]\n");

// Is it an error?
    if (seconds == 0){
        return (unsigned int) 0;
    }

    return (unsigned int) gramado_system_call(
                              884,
                              (unsigned long) seconds, 
                              0,
                              0 );
}

int brk(void *addr)
{
    debug_print ("brk: [TODO]\n");
    return -1;
}

//#todo: definir o tipo intptr_t
/*
void *sbrk(intptr_t increment);
void *sbrk(intptr_t increment)
{
    return NULL;
}
*/

// #todo
// This function use the variable 'environ'.
int execvp (const char *file, char *const argv[])
{
    debug_print ("execvep: [TODO]\n");
    return -1;
    //return = execvpe ( file, argv, environ );
}


int 
execvpe ( 
    const char *file, 
    char *const argv[],
    char *const envp[] )
{
    debug_print ("execvpe: [TODO]\n");
    return -1;
}


// #todo: Not implementeed yet.

int chown(const char *pathname, uid_t owner, gid_t group)
{
    debug_print ("chown: [TODO]\n");

    if ( (void*) pathname == NULL ){
        errno = EINVAL;
        return (int) (-1);
    }
    if (*pathname == 0){
        errno = EINVAL;
        return (int) (-1);
    }
    if (owner < 0){
        errno = EINVAL;
        return (int) (-1);
    }
    if (group < 0){
        errno = EINVAL;
        return (int) (-1);
    }

// #todo

    return -1; 
}


// #todo
// Maybe it is easy.
int fchown(int fd, uid_t owner, gid_t group)
{
    debug_print ("fchown: [TODO]\n");

    if (fd<0){
        errno = EBADF;
        return -1;
    }
    if (owner<0){
        errno = EINVAL;
        return -1;
    }
    if (group<0){
        errno = EINVAL;
        return -1;
    }


    return -1; 
}

// Maybe it is easy.
int lchown(const char *pathname, uid_t owner, gid_t group)
{
    debug_print ("lchown: [TODO]\n");

    if ( (void*) pathname == NULL ){
        errno = EINVAL;
        return (-1);
    }
    if (*pathname == 0){
        errno = EINVAL;
        return (-1);
    }
    if (owner<0){
        errno = EINVAL;
        return (-1);
    }
    if (group<0){
        errno = EINVAL;
        return (-1);
    }

// #todo

    return (-1); 
}


int chdir(const char *path)
{
    int value=-1;

    debug_print ("chdir: [TODO]\n");

    if ( (void*) path == NULL ){
        errno = EINVAL;
        return (int) -1;
    }
    if (*path == 0){
        errno = EINVAL;
        return (int) -1;
    }

// Syscall 175.
    value = 
        (int) gramado_system_call ( 
                  175, 
                  (unsigned long) path,
                  (unsigned long) path, 
                  (unsigned long) path );

    if (value<0){
        errno = (-value);
        return (int) (-1);
    }

    return (int) value;
}


//#todo: not implemented yet.
int fchdir(int fd)
{
    debug_print ("fchdir: [TODO]\n");

    if (fd<0){
        errno = EBADF;
        return -1;
    }

    return -1; 
}
   

// sleep - sleep for a specified number of seconds
unsigned int sleep(unsigned int seconds)
{
    debug_print ("sleep: [TODO]\n");
    return -1;
}

// ??
// todo
// exit deveria ser o wrapper para _exit ?

void _exit(int status)
{
    debug_print ("_exit: [error=O]\n");
    exit(0);
}

void swab_w (const short *from, short *to, ssize_t n)
{
    //if ( (void*) from == NULL )
    //    return;

    //if ( (void*) to == NULL )
    //    return;

    n /= 2;

    while (--n >= 0)
    {
        *to++ = (*from << 8) + ((*from >> 8) & 0377);
        from++;
    };
}


void swab (const void *from, void *to, ssize_t n)
{
    //if ( (void*) from == NULL )
    //    return;

    //if ( (void*) to == NULL )
    //    return;

    swab_w ( (const short *) from, (short *) to, (ssize_t) n );
}


// #
// isso muda o posicionamento dentro do arquivo lá no kernel.
// isso está certo, pois precisamos disso pra sabermos
// o tamanho do arquivo.
// whence = "De onde ?".

off_t lseek(int fd, off_t offset, int whence)
{
    if (fd<0){
        errno = EBADF;
        return (-1);
    }

    return (off_t) gramado_system_call ( 
                       603, 
                       (unsigned long) fd,
                       (unsigned long) offset,
                       (unsigned long) whence );
}


off_t tell(int fildes)
{
    return (off_t) lseek(fildes, 0, SEEK_CUR);
    //maybe: return(lseek(fildes, 0, 1));
}


/*
// #crecits: templeos.
// Not tested.
int FSize(FILE *f);
int FSize(FILE *f)
{
    int res=0;
    int original=0; 
    
    original = ftell(f);
    fseek(f,0,SEEK_END);
    res=ftell(f);
    fseek(f,original,SEEK_SET);

    return (int) res;
}
*/

/*
// #crecits: templeos.
// Not tested.
int Bt(int bit_num, char *bit_field);
int Bt(int bit_num, char *bit_field)
{
    bit_field += bit_num>>3;
    bit_num &= 7;
    
    return (*bit_field & (1<<bit_num)) ? 1 : 0;
}
*/

/*
// #crecits: templeos.
// Not tested.
int Bts(int bit_num, char *bit_field);
int Bts(int bit_num, char *bit_field)
{
    int res=0;
    
    bit_field += bit_num>>3;
    bit_num &= 7;
    res = *bit_field & (1<<bit_num);
    *bit_field |= (1<<bit_num);

    return (res) ? 1 : 0;
}
*/

/*
// #crecits: templeos.
// Not tested.
int BFieldExtDWORD(char *src, int pos, int bits);
int BFieldExtDWORD(char *src, int pos, int bits)
{
  int i=0;
  int res=0;

    for (i=0; i<bits; i++)
    {
        if (Bt(pos+i,src)){
            Bts(i,(BYTE *)&res);
        }
    };

    return (int) res;
}
*/


/*
I64 HasLower(U8 *src);
I64 HasLower(U8 *src)
{
    I64 ch;
    while (ch = *src++)
        if ('a' <= ch <= 'z')
            return TRUE;
    return FALSE;
}
*/


int access(const char *pathname, int mode)
{
    debug_print ("access: [TODO]\n");

    if ( (void*) pathname == NULL ){
        errno = EINVAL;
        return (int) -1;
    }
    if ( *pathname == 0 ){
        errno = EINVAL;
        return (int) -1;
    }

// #todo
    //struct stat foo;
    //return ( stat(pathname, &foo) );

    return -1;
}


/*
int rtl_uname(struct utsname *buf, unsigned long flags);
int rtl_uname(struct utsname *buf, unsigned long flags)
{
    //#todo: flags
    return (int) uname(buf);
}
*/

// #see: sys/utsname.h
int uname(struct utsname *buf)
{
    int value = -1;

    debug_print("uname: TODO\n");

    if ( (void*) buf == NULL ){
        errno = EINVAL;
        return (-1);
    }

// Syscall 377
    value = 
        (int) gramado_system_call ( 
                  377, 
                  (unsigned long) buf, 
                  0, 
                  0 );

    if (value<0){
        errno = (-value);
        return (-1);
    }

    return (int) value;
}


/*
int getsuf (char s[]);
int getsuf (char s[])
{
    int c=0;
    char t, *os;

    c = 0;
    os = s;

    while (t = *s++)
        if (t=='/')
            c = 0;
        else
            c++;
    s =- 3;
    if (c<=8 && c>2 && *s++=='.' && *s=='f')
        return ('f');

    return (0);
}
*/

/*
void setsuf (char s[]);
void setsuf (char s[])
{
    while (*s++);
    s[-2] = 'o';
}
*/

/*
int nodup ( char *l[], char s[]);
int nodup ( char *l[], char s[])
{
    char *t, *os, c;

    os = s;

    while (t = *l++)
    {
        s = os;
        while(c = *s++)
            if (c != *t++) goto ll;
        if (*t++ == '\0') return (0);
ll:;
    }
    return (1);
}
*/


/*
//?
char *nxtarg ( int ap, int ac, char **av );
char *nxtarg ( int ap, int ac, char **av )
{
    if (ap>ac)
        return ( 0*ap++ );

    return ( av[ap++] );
}
*/

/*
// ?? tipos
int tio ( int a, int f);
int tio ( int a, int f)
{

// Open:

    a = open(a,f);
    if (a>=0){
        close(a);
        return (1);
    }

    return (0);
}
*/


/* ??
int tcreat (int a)
{
    return (1);
}
*/

int eq(char *a, char *b)
{
    int i=0;
l:
    if (a[i] != b[i])
    {
        return 0;
    }
    if (a[i++] == '\0')
    {
        return 1;
    }
    goto l;
}


// pega uma label em uma linha do arquivo
// para comparar strings;

int getlin(char s[])
{
    int ch=0;
    int i=0;

    i=0;
l:

    // Se acabou a string.
    if ( ( ch=getc(stdin) ) == '\0' )
    {
        return (1);
    }

    // Se não for o marcador de label.
    if ( ch != ':' )
    {
        // Avançamos até o fim da linha ou até o fim da string;
        while ( ch != '\n' && ch != '\0' )
        {
            ch = getc(stdin);
        };

        goto l;
    }

    // Nesse momento ja encontramos o ':'.
    // Vamos colocar o que segue dentro do array.
    // Não queremos os espaços.

    // Pulamos os espaços.
    while ((ch=getc(stdin))==' '){};

    
    while ( ch != ' '  && 
            ch != '\n' && 
            ch != '\0' ) 
    {
        s[i++] = ch;
        ch = getc(stdin);
    };

    s[i] = '\0';

    return (0);
}


/*
//label?
int unix_getlin ( char s[] )
{
    int ch, i;
    i = 0;

l:
    if ( (ch=getc())=='\0' ) 
        return(1);

	if (ch != ':') 
	{
		while ( ch != '\n' && ch != '\0' )
			ch = getc();
		
		goto l;
	}

	while ( (ch=getc()) == ' ' );


	while ( ch != ' '  && 
	        ch != '\n' && 
	        ch != '\0' )
	{
		s[i++] = ch;
		
		ch = getc();
	}
	
	s[i] = '\0';
	
	return(0);
}
*/


/*
void printn ( int n, int b);
void printn ( int n, int b)
{
	//extern putchar;
	//auto a;
    int a;
    
    if ( b == 0 )
        return;

	if(a=n/b)    //assignment, not test for equality
		printn (a, b);    //recursive 
		
		
	//if (b == 0)
	    //return;
	    	
	putchar (n%b + '0');
}
*/


/*
int match ( char *s, char *p );
int match ( char *s, char *p )
{
    if (*s=='.' && *p!='.') 
        return (0);

    return ( amatch(s, p) );
}
*/


/*
int amatch (char *s, char *p);
int amatch (char *s, char *p)
{
    int c, cc, ok, lc, scc;
    scc = *s;
    lc = 077777;

    switch (c = *p) {

	case '[':
		ok = 0;
		while (cc = *++p) 
		{
			switch (cc) 
			{
			    case ']':
				    if (ok)
					    return ( amatch(++s, ++p) );
				    else
					    return(0);

			    case '-':
				    ok =| lc <= scc & scc <= (cc=p[1]); //?? 
			}
			if (scc==(lc=cc)) ok++;
		}
		return(0);


	case '?':
	
	    caseq: //label
		    if(scc) return(amatch(++s, ++p));
		    return(0);
		
	case '*':
		return(umatch(s, ++p));
		
	case 0:
		return(!scc);
	}
	
	if (c==scc) 
	    goto caseq;
	
	return(0);
}
*/

/*
int umatch(char *s, char *p);
int umatch(char *s, char *p)
{
    if ( *p == 0 ) 
        return (1);

    while (*s)
        if ( amatch(s++,p) ){ return (1); }

    return(0);
}
*/


// Compare
// Not tested yet.

int compar(char *s1, char *s2)
{
    int c1=0;
    int c2=0;
loop:
    if ((c1 = *s1++) == 0)
    {
        return(0);
    }
    if ((c2 = *s2++) == 0)
    {
        return(1);
    }
    if (c1==c2){
        goto loop;
    }

// #ugly
    return (int) (c1 > c2);
}


/*
//copy
char *copy (char *s1);
char *copy (char *s1)
{
    char *ts;
    ts = string;
    while(*string++ = *s1++);
    return(ts);
}
*/

/*
// concatenate
char *cat ( char *s1, char *s2);
char *cat ( char *s1, char *s2)
{
    char *ts;
    ts = string;
    while(*string++ = *s1++);
    string--;
    while(*string++ = *s2++);
    return(ts);
}
*/

/*
int __dup ( char **l, char s[] );
int __dup ( char **l, char s[] )
{
    char *t, *os, c;
    os = s;

    while (t = *l++) 
    {
        s = os;

        while (c = *s++)
            if (c != *t++)
                break;
        if (*t++ == '\0') 
            return (1);
    };

    return (0);
}
*/

// #deprecated?
pid_t 
xxx_todo_int133 ( 
    unsigned long ax, 
    unsigned long bx, 
    unsigned long cx, 
    unsigned long dx )
{
    printf ("xxx_todo_int133: #deprecated\n");
    return -1;
}


// Count occurrences of a char.
// Credits: templeos.
int 
StrOcc (
    unsigned char *src, 
    int ch )
{
    int Times=0;
  
    if (!src){
        return 0; 
    }
    while (*src)
    {
        if (*src++ == ch)
        {
            Times++;
        }
    };

    return (int) Times;
}


// Point to 1st occurrence of marker set in str.
// Credits: templeos.

unsigned char *StrFirstOcc(
    unsigned char *src,
    unsigned char *marker )
{
    int ch=0;

    while ( (ch = *src++) && !StrOcc(marker,ch) )
    {
        // Nothing
    };

    if (ch){
        return (unsigned char *) (src-1);
    }else{
        return NULL;
    };
}


// Point to last occurrence of market set in str.
// Credits templeos.

unsigned char *StrLastOcc(
    unsigned char *src,
    unsigned char *marker)
{
    int ch=0;
    unsigned char *res=NULL;

    while (ch = *src++)
    {
        if ( StrOcc(marker,ch) )
        {
            res = (src-1);
        }
    };

   return (unsigned char *) res;
}

/*
// #todo
// Locate character in string
unsigned char *Str_strrchr(unsigned char *string1, int ch);
unsigned char *Str_strrchr(unsigned char *string1, int ch)
{
    char string2[2];

    string2[0] = (char) (ch & 0xFF);
    string2[1] = 0;

    return StrLastOcc(string1,&string2);
}
*/


// See:
// https://man7.org/linux/man-pages/man3/posix_spawn.3.html
int 
posix_spawn (
    pid_t *pid, 
    const char *path,
    const posix_spawn_file_actions_t *file_actions,
    const posix_spawnattr_t *attrp,
    char *const argv[], 
    char *const envp[] )
{
    int value = (-1);

    debug_print ("posix_spawn: [FIXME] It's a work in progress.\n"); 

    if ( (void*) path == NULL ){
        errno=EINVAL;
        return -1;
    }

// Syscall 900.
    value = 
        (int) gramado_system_call( 
                  900, 
                  (unsigned long) path, 
                  0, 
                  0 );

    if (value<0){
        errno=(-value);
        return -1;
    }

    return (int) value;
}


int 
posix_spawnp (
    pid_t *pid, 
    const char *file,
    const posix_spawn_file_actions_t *file_actions,
    const posix_spawnattr_t *attrp,
    char *const argv[], 
    char *const envp[] )
{
    debug_print ("posix_spawn: [FIXME] It's a work in progress.\n"); 
    return (int) gramado_system_call ( 900, (unsigned long) file, 0, 0 );
}


/*
pid_t 
spawn ( 
    const char * path,
    int fd_count, 
    const int fd_map[ ], 
    const struct inheritance * inherit, 
    char * const argv[ ], 
    char * const envp[ ] );
pid_t 
spawn ( 
    const char * path,
    int fd_count, 
    const int fd_map[ ], 
    const struct inheritance * inherit, 
    char * const argv[ ], 
    char * const envp[ ] )
{
    return -1;
}
*/


int spawnv(int mode, char *cmd, char **argv)
{
    int value=-1;

    debug_print ("spawnv: [FIXME] It's a work in progress.\n"); 

    if ( (void*) cmd == NULL ){
        errno = EINVAL;
        return -1;
    }

// Syscall 900.
    value = 
        (int) gramado_system_call( 
                  900, 
                  (unsigned long) cmd, 
                  0, 
                  0 );

    if (value<0){
        errno = (-value);
        return -1;
    }

    return (int) value;
}


int spawnve(int mode, char *path, char *argv[], char *envp[])
{
    int value=-1;

    debug_print ("spawnv: [FIXME] It's a work in progress.\n"); 

    if ( (void*) path == NULL ){
        errno = EINVAL;
        return -1;
    }

// Syscall 900.
    value = 
        (int) gramado_system_call( 
                  900, 
                  (unsigned long) path, 
                  0, 
                  0 );

    if (value<0){
        errno=(-value);
        return -1;
    }

    return (int) value;
}


int spawnvp(int mode, char *path, char *argv[])
{
    int value=-1;

    debug_print ("spawnv: [FIXME] It's a work in progress.\n"); 

    if ( (void*) path == NULL ){
        errno = EINVAL;
        return -1;
    }

// Syscall 900
    value = 
        (int) gramado_system_call ( 
                  900, 
                  (unsigned long) path, 
                  0, 
                  0 );

    if (value<0){
        errno = (-value);
        return -1;
    }

    return (int) value;
}


int 
spawnvpe(
    int mode, 
    char *path, 
    char *argv[], 
    char *envp[] )
{
    int value=-1;
    debug_print ("spawnv: [FIXME] It's a work in progress.\n"); 

    if ( (void*) path == NULL ){
        errno=EINVAL;
        return -1;
    }

// Syscall 900.
    value = 
        (int) gramado_system_call ( 
                  900, 
                  (unsigned long) path, 
                  0, 
                  0 );

    if (value<0){
        errno = (-value);
        return -1;
    }

    return (int) value;
}


int 
spawnveg(
    const char *command, 
    char **argv, 
    char **envv, 
    pid_t pgid )
{
    int value=-1;

    debug_print ("spawnv: [FIXME] It's a work in progress.\n"); 

    if( (void*) command == NULL )
    {
        errno=EINVAL;
        return -1;
    }

    if (pgid<0){
        errno=EINVAL;
        return -1;
    }

    value = 
        (int) gramado_system_call ( 
                  900, 
                  (unsigned long) command, 
                  0, 
                  0 );

    if (value<0){
       errno=(-value);
       return -1;
    }

    return (int) value;
}


// #todo
// get file descriptor table size
// See: https://www.man7.org/linux/man-pages/man2/getdtablesize.2.html
int getdtablesize(void)
{
    debug_print ("getdtablesize: [TODO] not implemented yet\n"); 
    return -1;
}


//
// End
//

