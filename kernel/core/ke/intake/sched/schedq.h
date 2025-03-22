// schedq.h
// Manage the queues used by the task switching and scheduler.
// Created by Fred Nora.

#ifndef __SCHEDQ_H
#define __SCHEDQ_H    1

// The index for the head of the current queue.
#define SCHED_CURRENT_QUEUE  0

// Priority classes
// See: prio.h

// class 1: Normal priorities
#define SCHED_P1_QUEUE  1
#define SCHED_P2_QUEUE  2
#define SCHED_P3_QUEUE  3
// class 2: System priorities
#define SCHED_P4_QUEUE  4
#define SCHED_P5_QUEUE  5
#define SCHED_P6_QUEUE  6

// Max number of elements.
#define SCHEQ_QUEUE_COUNT_MAX  7

// List of heads.
// see: schedq.c
extern unsigned long qList[SCHEQ_QUEUE_COUNT_MAX];

// The current list
// Global queue.
// This one is used by the taskswitching to peek the next thread.
extern struct thread_d  *currentq;

//
// == Prototypes =================================
//

int qlist_set_element(int index, struct thread_d *head_thread);
struct thread_d *qlist_get_element(int index);

int qlist_initialize(void);

#endif    

