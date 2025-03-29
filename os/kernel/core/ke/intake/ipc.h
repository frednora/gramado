// ipc.h
// System message support.
// Messages using the circular queue into the thread structure.
// Created by Fred Nora.

#ifndef __PS_MSG_H
#define __PS_MSG_H    1

// -----------------------------------------------------
// Message structure.
struct msg_d 
{

// -----------------------
// validation
    int used;
    int magic;

// -----------------------
// Standard header
    void *opaque_window;
    int msg;
    unsigned long long1;
    unsigned long long2;
// -----------------------
// Extra payload
    unsigned long long3;
    unsigned long long4;

// -----------------------
// Identification field
    tid_t sender_tid;
    tid_t receiver_tid;
    pid_t sender_pid;
    pid_t receiver_pid;

// -----------------------
// Navigation
    struct msg_d *next;
};

//
// ========================================================
//

int
ipc_post_message_to_tid2 ( 
    tid_t sender_tid,
    tid_t receiver_tid,
    int msg, 
    unsigned long long1, 
    unsigned long long2,
    unsigned long long3,
    unsigned long long4 );

int
ipc_post_message_to_tid ( 
    tid_t sender_tid,
    tid_t receiver_tid,
    int msg, 
    unsigned long long1, 
    unsigned long long2 );

int
ipc_post_message_to_foreground_thread ( 
    int msg, 
    unsigned long long1, 
    unsigned long long2 );

int
ipc_post_message_to_init ( 
    int msg, 
    unsigned long long1, 
    unsigned long long2 );
 
int
ipc_post_message_to_ds( 
    int msg, 
    unsigned long long1, 
    unsigned long long2 );


// ----------

// Service 111.
// Get a message from the current thread and 
// put it into the given buffer.
// The message has 6 standard elements.
// See: thread.c
void *sys_get_message(unsigned long ubuf);
// Service 120
void *sys_get_message2(unsigned long ubuf, int index, int restart);

// Service 112
unsigned long
sys_post_message_to_tid( 
    int tid, 
    unsigned long message_buffer );

#endif    




