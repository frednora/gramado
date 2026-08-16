/*
 * File: wait.c
 *     wait, wait3, wait4, waitpid -- wait for process termination 
 */


#include <sys/types.h>    //for pid_t
//#include <unistd.h>     //for pid_t
#include <sys/wait.h>   
#include <rtl/gramado.h>  


/*
 * wait:
 *
 */

pid_t wait( int *status )
{
    //if( (void*) status == NULL )
        //return -1;
    return (pid_t) waitpid( (pid_t) -1, (int *) status, (int) 0 );
}


/*
 * waitpid:
 *
 */
//acho que isso é o retorno.??sei lá.
//< -1	meaning wait for any child process whose process group ID is equal to the absolute value of pid.
//-1	meaning wait for any child process.
//0	meaning wait for any child process whose process group ID is equal to that of the calling process.
//> 0	meaning wait for the child whose process ID is equal to the value of pid.

pid_t waitpid (pid_t pid, int *status, int options)
{
    // #todo: 
     // Criar o identificador da system call 43
    // SYSTEMCALL_WAIT4PID 

	// #importante
	// >>se o retorno indicar que já havia um processo filho no estado zombie 
	// e retornou seu pid, então apenas retornamos o pid para o aplicativo que 
	// chamou wait.
	// >>se o retorno indicar que não há um processo filho terminado
	// então devemos esperar até que algum processo filho termine.

    pid_t __pid;

again:

    __pid = (pid_t) gramado_system_call ( 83, 
                         (unsigned long) pid, 
                         (unsigned long) status, 
                         (unsigned long) options );

    if ( __pid > 0 ){
        return (pid_t) __pid;
    }

//
// # wait #
//

	//#todo:
	//devemos esperar até que um processo filho seja terminado,
	//para depois checarmos novamente qual deles terminou e retornarmos 
	//com seu pid

	//sleep()
	//wait_for_a_reason(...)

	//#bugbug
	//estamos retornando para não entrarmos no loop,
	//depois que criarmos a rotina sleep, deletaremos esse retorno.

    return (pid_t) __pid;

    goto again;

// done:

    return (pid_t) __pid;
}


