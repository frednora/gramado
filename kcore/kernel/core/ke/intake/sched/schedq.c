// schedq.c
// Manage the queues used by the task switching and scheduler.
// Created by Fred Nora.

#include <kernel.h>

// List of heads.
// Each priority has its own queue.
unsigned long qList[SCHEQ_QUEUE_COUNT_MAX];

// The current list
// Global queue.
// This one is used by the taskswitching to peek the next thread.
struct thread_d  *currentq;

// ========================================

int qlist_set_element(int index, struct thread_d *head_thread)
{

// Parameters:
    if ( index < 0 || index >= SCHEQ_QUEUE_COUNT_MAX )
    {
        goto fail;
    }
    if ((void*) head_thread == NULL){
        goto fail;
    }
    // #todo: Check 'magic' element
    //if (head_thread->magic != 1234)
        //goto fail;

// The current queue
    if (index == SCHED_CURRENT_QUEUE){
        qList[SCHED_CURRENT_QUEUE] = (unsigned long) head_thread;
        return 0;
    }
// P1 ~ P6
    if ( index >= SCHED_P1_QUEUE || index <= SCHED_P6_QUEUE )
    {
        qList[index] = (unsigned long) head_thread;
        return 0;
    }

fail:
    return (int) -1;
}

struct thread_d *qlist_get_element(int index)
{

// Parameter:
    if ( index < 0 || index >= SCHEQ_QUEUE_COUNT_MAX )
    {
        goto fail;
    }

// The current queue
    if (index == SCHED_CURRENT_QUEUE){
        return (struct thread_d *) qList[SCHED_CURRENT_QUEUE];
    }
// P1 ~ P6    
    if ( index >= SCHED_P1_QUEUE || index <= SCHED_P6_QUEUE )
    {
        return (struct thread_d *) qList[index];
    }

fail:
    return NULL;
}

//
// $
// INITIALIZATION
//

int qlist_initialize(void)
{
    register int i=0;

    for (i=0; i<SCHEQ_QUEUE_COUNT_MAX; i++){
        qList[i] = (unsigned long) 0;
    };

    return 0;
}

