/*
 * File: time.c 
 * 2018 - Created by Fred Nora.
 */


#include <types.h>
//#include <errno.h> 
#include <time.h>
#include <sys/time.h>
#include <stdio.h> 
#include <rtl/gramado.h> 


// time 
// #obs: 
// O que representa esse retorno ?
// Isso funciona.
// system call. (224) get time

time_t time(time_t *timer)
{
    time_t Ret=0;

/*
    if( (void*) timer == NULL )
    {
        errno = EINVAL;
        return 0;
    }
*/

    Ret = (time_t) gramado_system_call ( 224, 0, 0, 0 );

    *timer = Ret;

    return (time_t) Ret;
}


int gettimeofday (struct timeval *tv, struct timezone *tz)
{
    debug_print ("gettimeofday: [TODO]\n");
    return -1;
}


int settimeofday (const struct timeval *tv, const struct timezone *tz)
{
    debug_print ("settimeofday: [TODO]\n");
    return -1;
}










