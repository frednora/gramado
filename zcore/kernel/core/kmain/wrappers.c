// wrappers.c
// Userful wrappers functions.
// See: gramk/ for overall wrapper functions.
// Created by Fred Nora.

#include <kernel.h>


// IPC stuff
int 
core_post_message( 
    tid_t sender_tid,
    tid_t receiver_tid,
    struct msg_d *message )
{
// #todo: Not tested yet.
// #todo: Explain this routine.
    int rv=0;

    if (sender_tid < 0 || sender_tid >= THREAD_COUNT_MAX)
    {
        goto fail;
    }
    if (receiver_tid < 0 || receiver_tid >= THREAD_COUNT_MAX)
    {
        goto fail;
    }
    if ((void*) message == NULL){
        goto fail;
    }
    message->sender_tid   = (tid_t) sender_tid;
    message->receiver_tid = (tid_t) receiver_tid;

// Post
    rv = (int) ipc_post_message_to_tid( 
                (tid_t) sender_tid,
                (tid_t) receiver_tid,
                (int) message->msg,
                (unsigned long) message->long1,
                (unsigned long) message->long2 );

    return (int) rv;

fail:
    return (int) -1;
}

void core_spawn_thread_by_tid(tid_t tid)
{
    if (tid<0 || tid>=THREAD_COUNT_MAX)
        return;
    psSpawnThreadByTID(tid);
}

// Usado pelo malloc em ring3.
void *core_alloc_shared_ring3_pages(pid_t pid, int number_of_bytes)
{
    int number_of_pages=0;

// #todo
// pid premission

// #todo
// Check max limit

    if ( number_of_bytes < 0 )
        number_of_bytes = 4096;

    if ( number_of_bytes <= 4096 ){
        return (void *) allocPages(1);
    }

// Alinhando para cima.
    number_of_pages = (int) ((number_of_bytes/4096) + 1);

    return (void *) allocPages(number_of_pages);
}

int core_get_current_runlevel(void)
{
    return (int) current_runlevel;
}

unsigned long core_get_memory_size_mb(void)
{
    unsigned long __mm_size_mb = 
        (unsigned long) (memorysizeTotal/0x400);

    return (unsigned long) __mm_size_mb;
}

unsigned long core_get_system_metrics(int index)
{
    if (index < 0){
        return 0;
    }
    return (unsigned long) doGetSystemMetrics((int) index);
}

// REAL (coloca a thread em standby para executar pela primeira vez.)
// MOVEMENT 1 (Initialized --> Standby).
int core_start_thread(struct thread_d *thread)
{

// Validation
    if ((void*) thread == NULL)
        goto fail;
    if (thread->used != TRUE)
        goto fail;
    if (thread->magic != 1234)
        goto fail;

    SelectForExecution((struct thread_d *) thread);
    return 0;
fail:
    return (int) (-1);
}

void core_reboot(void)
{
    keReboot();
}

void core_shutdown(int how)
{
    //hal_shutdown();
    panic("core_shutdown: #todo\n");
}

void core_die(void)
{
    keDie();
}

