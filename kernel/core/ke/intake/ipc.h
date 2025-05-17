// ipc.h
// System message support.
// Messages using the circular queue into the thread structure.
// Created by Fred Nora.

#ifndef __PS_MSG_H
#define __PS_MSG_H    1


/* Define magic values for message reusability. */
#define MSG_MAGIC_VALID       1234   /* Message is valid and in use. */
#define MSG_MAGIC_REUSABLE    4231   /* Message can be reused (freed and ready). */

/* 
 * Message structure.
 * This structure is used for internal system messages.
 * It contains a header for validation, a standard header with primary
 * fields and extra payload fields, identification for 
 * the sender and receiver, and a pointer for linking 
 * in a message queue.
 */
struct msg_d 
{

// -----------------------
// Validation fields
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
// Identification Fields 
    tid_t sender_tid;
    tid_t receiver_tid;
    pid_t sender_pid;
    pid_t receiver_pid;

// #todo: External Buffer 
//    void   *ext_buf;        // Pointer to an external buffer.
//    size_t  ext_buf_size;   // Size of the data in the external buffer.

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


// Wrapper for different types of input events.
// Sending these events to the thread, 
// not processing internally.
int ipc_post_input_event_to_ds(int event_id, long long1, long long2);

unsigned long ipc_broadcast_system_message(
    tid_t sender_tid,
    int msg, 
    unsigned long long1, 
    unsigned long long2,
    unsigned long long3,
    unsigned long long4 );

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

// #todo
// Broadcast system message to all the threads.
// IN: Buffer
unsigned long sys_broadcast_system_message(unsigned long message_buffer);

#endif    




