// ipc.c
// System message support.
// Messages using the circular queue into the thread structure.
// Created by Fred Nora.

#include <kernel.h>

// #todo
// #test
// Tracking the last message.
// This way we can change it in the case of repetitions.
//struct msg_d *last_msg;

int
ipc_post_message_to_tid2 ( 
    tid_t sender_tid,
    tid_t receiver_tid,
    int msg, 
    unsigned long long1, 
    unsigned long long2,
    unsigned long long3,
    unsigned long long4 )
{
// #todo:
// Nao podemos deixar que toda vez que uma thread receber uma mensagem 
// ela exija um timeout. Pois uma aplicaçao pode enviar mensagens em loop.
// Talvez isso deva ser permitido somente para mensagens de input vindas
// dos device drivers.

// Target thread.
    struct thread_d *t;
    struct msg_d *m;
    int MessageCode=0;

// TIDs
    tid_t src_tid = (tid_t) (sender_tid & 0xFFFF);
    tid_t dst_tid = (tid_t) (receiver_tid & 0xFFFF);

// Message code.
    MessageCode = (int) (msg & 0xFFFFFFFF);
    if (MessageCode<0){
        goto fail;
    }

// tid
    if ( dst_tid < 0 || dst_tid >= THREAD_COUNT_MAX ){
        panic("ipc_post_message_to_tid2: dst_tid\n");
        //goto fail;
    }
// structure
    t = (struct thread_d *) threadList[dst_tid];
    if ((void *) t == NULL){
        panic ("ipc_post_message_to_tid2: t\n");
    }
    if ( t->used != TRUE || t->magic != 1234 ){
        panic ("ipc_post_message_to_tid2: t validation\n");
    }
    if (t->tid != dst_tid){
        panic("ipc_post_message_to_tid2: t->tid != dst_tid\n");
    }

//
// This thread needs a timeout.
//

// Let's tell to ts.c that this thread needs a timeout.
// So this way the ts ca break the round and give to this thread
// the opportunity to run immediately.
// #bugbug
// A malicious process could send a lot of messages
// causing starvation in the other threads?

    if (g_use_event_responder == TRUE)
    {
        t->has_pending_event = TRUE;
        t->quantum = QUANTUM_MAX;
        t->priority = PRIORITY_MAX;
        t->state = READY;

        //ev_responder_thread = (struct thread_d *) t;
        set_ev_responder(t);
    }

//
// The message
//


// ==========================================================
// Vamos colocar essa mensagem na outra fila de mensagens.
// Essa nova fila sera a única fila no futuro.

// Get the pointer for the next entry.
    m = (struct msg_d *) t->MsgQueue[ t->MsgQueueTail ];
    if ((void*) m == NULL){
        panic ("ipc_post_message_to_tid2: m\n");
    }
    if ( m->used != TRUE || m->magic != 1234 ){
        panic ("ipc_post_message_to_tid2: m validation\n");
    }

// -------------------------
// Standard header
    m->opaque_window = NULL;
    m->msg   = (int) (MessageCode & 0xFFFFFFFF);
    m->long1 = (unsigned long) long1;
    m->long2 = (unsigned long) long2;

// -------------------------
// Extras payload
    m->long3 = (unsigned long) long3;
    m->long4 = (unsigned long) long4;

// -------------------------
// Identification field
    m->sender_tid   = (tid_t) src_tid; 
    m->receiver_tid = (tid_t) dst_tid;
    //m->sender_pid = ?;
    //m->receiver_pid = ?;

// -------------------------
    m->next = NULL;

// Done
    t->MsgQueueTail++;
    if (t->MsgQueueTail >= MSG_QUEUE_MAX){
        t->MsgQueueTail = 0;
    }

    t->isResponder = TRUE;
    return 0;

fail2:
    if ((void*) t == NULL){
        return -1;
    }
    t->MsgQueueTail++;
    if (t->MsgQueueTail >= MSG_QUEUE_MAX){
        t->MsgQueueTail = 0;
    }
fail:
    return (int) -1;
}


// ------------------------------------
// ipc_post_message_to_tid:
// #bugbug
// long1 and long2 will mask to single byte.
// IN: tid, window, message code, ascii code, raw byte.
// Post message.
// Async
int
ipc_post_message_to_tid ( 
    tid_t sender_tid,
    tid_t receiver_tid,
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{
// #todo:
// Nao podemos deixar que toda vez que uma thread receber uma mensagem 
// ela exija um timeout. Pois uma aplicaçao pode enviar mensagens em loop.
// Talvez isso deva ser permitido somente para mensagens de input vindas
// dos device drivers.

// Target thread.
    struct thread_d *t;
    struct msg_d *m;
    int MessageCode=0;

// TIDs
    tid_t src_tid = (tid_t) (sender_tid & 0xFFFF);
    tid_t dst_tid = (tid_t) (receiver_tid & 0xFFFF);

// Message code.
    MessageCode = (int) (msg & 0xFFFFFFFF);
    if (MessageCode <= 0){
        goto fail;
    }
// tid
    if ( dst_tid < 0 || dst_tid >= THREAD_COUNT_MAX ){
        panic("ipc_post_message_to_tid: dst_tid\n");
        //goto fail;
    }
// structure
    t = (struct thread_d *) threadList[dst_tid];
    if ((void *) t == NULL){
        panic ("ipc_post_message_to_tid: t\n");
    }
    if ( t->used != TRUE || t->magic != 1234 ){
        panic("ipc_post_message_to_tid: t validation\n");
    }
    if (t->tid != dst_tid){
        panic("ipc_post_message_to_tid: t->tid != dst_tid\n");
    }

// #test
// This thread needs a timeout.
// Let's tell to ts.c that this thread needs a timeout.
// So this way the ts can break the round and 
// give to this thread the opportunity to run immediately.
// #bugbug
// A malicious process could send a lot of messages
// causing starvation in the other threads?

    if (g_use_event_responder == TRUE)
    {
        t->has_pending_event = TRUE;
        t->quantum = QUANTUM_MAX;
        t->priority = PRIORITY_MAX;
        t->state = READY;

        //ev_responder_thread = (struct thread_d *) t;
        set_ev_responder(t);
    }

//
// The message
//

// ==========================================================
// Vamos colocar essa mensagem na outra fila de mensagens.
// Essa nova fila sera a única fila no futuro.

// Get the pointer for the next entry.
    m = (struct msg_d *) t->MsgQueue[ t->MsgQueueTail ];
    if ((void*) m == NULL){
        panic ("ipc_post_message_to_tid: m\n");
    }
    if ( m->used != TRUE || m->magic != 1234 ){
        panic ("ipc_post_message_to_tid: m validation\n");
    }

// --------------------------
// Standard header
    m->opaque_window = NULL;
    m->msg   = (int) (MessageCode & 0xFFFFFFFF);
    m->long1 = (unsigned long) long1;
    m->long2 = (unsigned long) long2;

// --------------------------
// Extra payload
    m->long3 = (unsigned long) jiffies;
    m->long4 = (unsigned long) jiffies;

// --------------------------
// Identification field
    m->sender_tid   = (tid_t) src_tid; 
    m->receiver_tid = (tid_t) dst_tid;
    //m->sender_pid = ?;
    //m->receiver_pid = ?;

// --------------------------
    m->next = NULL;

// Done
    t->MsgQueueTail++;
    if (t->MsgQueueTail >= MSG_QUEUE_MAX){
        t->MsgQueueTail = 0;
    }

    t->isResponder = TRUE;
    return 0;

fail2:
    if ((void*) t == NULL){
        return (int) -1;
    }
    t->MsgQueueTail++;
    if (t->MsgQueueTail >= MSG_QUEUE_MAX){
        t->MsgQueueTail = 0;
    }
fail:
    return (int) -1;
}

// Post message to the foreground thread.
int
ipc_post_message_to_foreground_thread ( 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{
    int RetValue=0;
// #todo: 
// Sender?
    tid_t SenderTID = 0;
    tid_t ReceiverTID = foreground_thread;

// Target thread
    if ( ReceiverTID < 0 || 
         ReceiverTID >= THREAD_COUNT_MAX )
    {
        goto fail;
    }
 
 // Parameter:
    if (msg < 0){
        goto fail;
    }

    RetValue = (int) ipc_post_message_to_tid(
                     (tid_t) SenderTID, 
                     (tid_t) ReceiverTID,
                     (int) msg, 
                     (unsigned long) long1,
                     (unsigned long) long2 );

    return (int) RetValue;

fail:
    return (int) -1;
}

int
ipc_post_message_to_init ( 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{
//  Post msg to the display server's tid.

// #bugbug
// We can't send messages to the display server
// when the server was closed.
// Ex: Right after we close the demo ENG.BIN
// using control + c.
// It is because the thread will have an invalid pointer.
    // #test: KERNEL_MESSAGE_TID
    tid_t SenderTID = KERNEL_MESSAGE_TID;   // sender tid #todo
    tid_t ReceiverTID = INIT_TID;  // receiver tid

// Is this a valid destination?
    if ( ReceiverTID < 0 || 
         ReceiverTID >= THREAD_COUNT_MAX )
    {
        goto fail;
    }

    if (msg < 0)
        goto fail;

    //if(msg == MSG_MOUSEMOVE){
    //    printk ("x:%d y:%d\n",long1, long2);
    //    refresh_screen();
    //}

// #todo
// precisamos de uma flag que indique que isso deve ser feito.
// See: tlib.c
// IN: tid, window pointer, msgcode, data1, data2.

    ipc_post_message_to_tid(
        (tid_t) SenderTID,  // sender tid
        (tid_t) ReceiverTID,  // receiver tid
        (int) msg,
        (unsigned long) long1,
        (unsigned long) long2 );

   return 0;
fail:
    return (int) -1;
}

int
ipc_post_message_to_ds ( 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{
//  Post msg to the display server's tid.

// #bugbug
// We can't send messages to the display server
// when the server was closed.
// Ex: Right after we close the demo ENG.BIN
// using control + c.
// It is because the thread will have an invalid pointer.
    // #test: KERNEL_MESSAGE_TID
    tid_t SenderTID = 0;   // sender tid #todo
    tid_t ReceiverTID = -1;  // receiver tid

// see: dispsrv.h in user/.
    if (DisplayServerInfo.initialized != TRUE){
        goto fail;
    }
    ReceiverTID = (tid_t) DisplayServerInfo.tid;
    if ( ReceiverTID < 0 || ReceiverTID >= THREAD_COUNT_MAX )
    {
        goto fail;
    }

// More credits to the receiver.
    do_credits_by_tid(ReceiverTID);
    do_credits_by_tid(ReceiverTID);

// Wake up the server, it sleeps frequently.
    wakeup_thread(ReceiverTID);

// ---------------------
    if (msg < 0)
        goto fail;

    //if(msg == MSG_MOUSEMOVE){
    //    printk ("x:%d y:%d\n",long1, long2);
    //    refresh_screen();
    //}

// #todo
// precisamos de uma flag que indique que isso deve ser feito.
// See: tlib.c
// IN: tid, window pointer, msgcode, data1, data2.

    ipc_post_message_to_tid(
        (tid_t) SenderTID,  // sender tid
        (tid_t) ReceiverTID,  // receiver tid
        (int) msg,
        (unsigned long) long1,
        (unsigned long) long2 );

   return 0;
fail:
    return (int) -1;
}

// Wrapper for different types of input events.
// Posting these events to the thread, 
// not processing internally.
int ipc_post_input_event_to_ds(int event_id, long long1, long long2)
{
    unsigned long button_number=0;

    if (event_id < 0)
        goto fail;




    switch (event_id)
    {
        // Mouse
        case MSG_MOUSEMOVE:
            ipc_post_message_to_ds( event_id, long1, long2 );
            return 0;
            break;
        case MSG_MOUSEPRESSED:
        case MSG_MOUSERELEASED:
            button_number = (unsigned long) (long1 & 0xFFFF);
            ipc_post_message_to_ds( event_id, button_number, button_number );
            break;

        // Keyboard
        case MSG_KEYDOWN:
        case MSG_KEYUP:
        case MSG_SYSKEYDOWN:
        case MSG_SYSKEYUP:
            ipc_post_message_to_ds( event_id, long1, long1 );
            break;

        // Timer
        case MSG_TIMER:
            if ( (jiffies % JIFFY_FREQ) == 0 ){
                ipc_post_message_to_ds( MSG_TIMER, 1234, jiffies );
            }   
            break;

        default:
            goto fail;
            break;
    };

fail:
    return (int) -1;
}

// #todo
// Broadcast system message to all the threads.
// IN: Buffer
unsigned long ipc_broadcast_system_message(
    tid_t sender_tid,
    int msg, 
    unsigned long long1, 
    unsigned long long2,
    unsigned long long3,
    unsigned long long4 )
{
    register int i=0;
    struct thread_d *t;
    int rv=0;
    size_t Counter = 0;

    tid_t src_tid = (tid_t) sender_tid;  // caller's tid.
    tid_t dst_tid = (tid_t) -1;          // targt tid.

// #todo

    for (i=0; i<THREAD_COUNT_MAX; i++)
    {
        t = (struct threads_d *) threadList[i];
        if ((void*) t != NULL)
        {
            if (t->magic == 1234)
            {
                dst_tid = (tid_t) t->tid;  // Receiver

                // IN:
                // sender, receiver,msg,l1,l2,l3,l4
                rv = 
                (int) ipc_post_message_to_tid2(
                        src_tid,  // Sender 
                        dst_tid,  // Receiver
                        msg,
                        long1, long2, long3, long4 );
                
                if (rv >= 0)
                    Counter++;
            }
        }
    };

    unsigned long long_rv = (unsigned long) (Counter & 0xFFFFFFFF);

// Return the number of posted messages.
    return (unsigned long) long_rv;
}



/*
 * sys_get_message:
 *     Get a message from the current thread and 
 * put it into the given buffer.
 *     The message has 6 standard elements.
 */
// Called by sci.c
// Service 111.
// It is called by the applications.
// It is also used for ipc.

void *sys_get_message(unsigned long ubuf)
{
    unsigned long *message_address = (unsigned long *) ubuf;
    register struct thread_d *t;
    register struct msg_d *m;

// buffer
// #todo: Check some other invalid address.
    if (ubuf == 0){
        panic ("sys_get_message: ubuf\n");
        //return NULL;
    }

// ===========================================================
// Thread
// This is the thread that called this service.
    if (current_thread < 0 || current_thread >= THREAD_COUNT_MAX){
        return NULL;
    }
    t = (void *) threadList[current_thread];
    if ((void *) t == NULL){
        panic ("sys_get_message: t\n");
    }
    if (t->used != TRUE || t->magic != 1234){
        panic ("sys_get_message: t validation\n");
    }

// ===========================================================
// usando a fila de mensagens com estrutura.

// Get the next head pointer.
    m = (struct msg_d *) t->MsgQueue[ t->MsgQueueHead ];
    if ((void*) m == NULL){
        goto fail0;
    }
    if (m->used != TRUE || m->magic != 1234){
        goto fail0;
    }

// Invalid message code
    if (m->msg <= 0){
        goto fail0;
    }

// ---------------------------------
// Get standard header
    message_address[0] = (unsigned long) m->opaque_window;
    message_address[1] = (unsigned long) (m->msg & 0xFFFFFFFF);
    message_address[2] = (unsigned long) m->long1;
    message_address[3] = (unsigned long) m->long2;

    /*
    // #test
    // If the display server is getting the
    // mouse move events in it's own thread.
    if (message_address[1] == MSG_MOUSEMOVE)
    {
        do_credits(t);
        // The scheduler will change it latter.
        t->quantum = QUANTUM_SYSTEM_TIME_CRITICAL;
    }
    */

// ---------------------------------
// Get the extra payload
// The data here depends on the message code,
    message_address[4] = (unsigned long) m->long3;
    message_address[5] = (unsigned long) m->long4;

// ---------------------------------
// Get the identification field
    message_address[8] = (unsigned long) m->sender_tid;
    message_address[9] = (unsigned long) m->receiver_tid;
    // We also have these elements. 
    //m->sender_pid
    //m->receiver_pid

//
// Jiffies
//

// ---------------------------------
// Jiffies when the message was posted by the kernel.
    message_address[10] = (unsigned long) m->long3; 
// ---------------------------------
// Jiffies when gotten by the app.
    message_address[11] = (unsigned long) jiffies;

// ----------------------------
// Clear the entry.
// Consumimos a mensagem. Ela não existe mais.
// Mas preservamos a estrutura.

    // Standard header
    m->opaque_window = NULL;
    m->msg = 0;
    m->long1 = 0;
    m->long2 = 0;
    // Extra payload
    m->long3 = 0;
    m->long4 = 0;
    // Identification field
    m->sender_tid = 0;
    m->receiver_tid = 0;
    m->sender_pid = 0;
    m->receiver_pid = 0;

    // End of queue. Round it.
    t->MsgQueueHead++;
    if (t->MsgQueueHead >= MSG_QUEUE_MAX){
        t->MsgQueueHead=0;
    }
    // Yes, We have a message.
    // #bugbug: 
    // Maybe we can do this in a different way.
    // But it could break the user application.
    return (void *) 1;

fail0:
    // Invalid message pointer.
    if ((void*) t == NULL){
        return NULL;
    }
    // End of queue. Round it.
    t->MsgQueueHead++;
    if (t->MsgQueueHead >= MSG_QUEUE_MAX){
        t->MsgQueueHead=0;
    }
    // No message.
    return NULL;
}

// 120
// Get a message given the index.
// With restart support.
// IN: buffer, index, flag: TRUE=restart the queue at the end.

void *sys_get_message2(
    unsigned long ubuf, 
    int index, 
    int restart)
{
    unsigned long *message_address = (unsigned long *) ubuf;
    register struct thread_d *t;
    register struct msg_d *m;

// buffer
// #todo: Check some other invalid address.
    if ( ubuf == 0 ){ 
        panic ("sys_get_message2: ubuf\n");
        //return NULL;
    }

// Thread
// Essa é a thread que chamou esse serviço.

    if (current_thread<0 || current_thread>=THREAD_COUNT_MAX){
        return NULL;
    }

// Structure
    t = (void *) threadList[current_thread];
    if ((void *) t == NULL){
        panic ("sys_get_message2: t\n");
    }
    if ( t->used != TRUE || t->magic != 1234 ){
        panic ("sys_get_message2: t validation\n");
    }

// Get the index.
    if( index<0 || index >= MSG_QUEUE_MAX){
        goto fail0;
    }
    t->MsgQueueHead = (int) (index & 0xFFFFFFFF);

// ===========================================================
// usando a fila de mensagens com estrutura.

// Get the next head pointer.
    m = (struct msg_d *) t->MsgQueue[ t->MsgQueueHead ];
    if ((void*) m == NULL){
        goto fail0;
    }
    if (m->used != TRUE || m->magic != 1234){
        goto fail0;
    }

// Invalid message code.
    if (m->msg <= 0){
        goto fail0;
    }

// -----------------------------
// Get standard header
    message_address[0] = (unsigned long) m->opaque_window;
    message_address[1] = (unsigned long) (m->msg & 0xFFFFFFFF);
    message_address[2] = (unsigned long) m->long1;
    message_address[3] = (unsigned long) m->long2;

// -----------------------------
// Get extra payload
    message_address[4] = (unsigned long) m->long3;
    message_address[5] = (unsigned long) m->long4;

// -----------------------------
// Get identification field
    message_address[8] = (unsigned long) m->sender_tid;
    message_address[9] = (unsigned long) m->receiver_tid;
    // We also have these elements. 
    //m->sender_pid
    //m->receiver_pid

//
// Jiffies
//

// -----------------------------
// Jiffies when the message was posted by the kernel.
    message_address[10] = (unsigned long) m->long3; 

// -----------------------------
// Jiffies when gotten by the app.
    message_address[11] = (unsigned long) jiffies;

// -------------------------------------------------
// Clear the entry.
// Consumimos a mensagem. Ela não existe mais.
// Mas preservamos a estrutura.

    // Standard header
    m->opaque_window = NULL;
    m->msg = 0;
    m->long1 = 0;
    m->long2 = 0;
    // Extra payload
    m->long3 = 0;
    m->long4 = 0;
    // Identification field
    m->sender_tid = 0;
    m->receiver_tid = 0;
    m->sender_pid = 0;
    m->receiver_pid = 0;

// ==================================

// Done
// It is a valid thread pointer.
    if (restart == TRUE)
    {
        t->MsgQueueHead=0;
        t->MsgQueueTail=0;
        return (void*) 1;
    }

// Yes, We have a message.
// round
    t->MsgQueueHead++;
    if (t->MsgQueueHead >= MSG_QUEUE_MAX){
        t->MsgQueueHead = 0;
    }
    return (void *) 1;

// Is it a valid thread pointer?
fail0:
// No message.
// round
    if ((void*) t == NULL){
        return NULL;
    }
    if (restart == TRUE)
    {
        if ((void*) t != NULL){
            t->MsgQueueHead=0;
            t->MsgQueueTail=0;
        }
        return NULL;
    }
    if ((void*) t != NULL){
        t->MsgQueueHead++;
        if (t->MsgQueueHead >= MSG_QUEUE_MAX){
            t->MsgQueueHead = 0;
        }
    }
    return NULL;
}

// Service 112
// Post message to tid.
// Asynchronous.
unsigned long
sys_post_message_to_tid( 
    int tid, 
    unsigned long message_buffer )
{
    tid_t src_tid = (tid_t) current_thread;   // caller's tid.
    tid_t dst_tid = (tid_t) tid;              // targt tid.

// Invalid target tid.
    if ( dst_tid < 0 || dst_tid >= THREAD_COUNT_MAX ){
        return 0;
    }
// Message buffer
// Buffer in ring3.
    if (message_buffer == 0){
        return 0;
    }
    unsigned long *ubuf = (unsigned long *) message_buffer;
// Message code
    int MessageCode = (int) ( ubuf[1] & 0xFFFFFFFF );

// Post message.
// Asynchronous.
// IN: target tid, opaque struct pointer, msg code, data1, data2.
// #todo: get the return value?

    // #ps: Old implementation
    /*
    post_message_to_tid(
        (tid_t) src_tid,    // sender tid
        (tid_t) dst_tid,    // receiver tid
        (int) MessageCode,
        (unsigned long) ubuf[2],
        (unsigned long) ubuf[3] );
    */

    // #ps: Newm implementation. Sending more data.
    ipc_post_message_to_tid2(
        (tid_t) src_tid,    // sender tid
        (tid_t) dst_tid,    // receiver tid
        (int) MessageCode,
        (unsigned long) ubuf[2],
        (unsigned long) ubuf[3],
        (unsigned long) ubuf[4],
        (unsigned long) ubuf[5] );

// Let's notify the scheduler
// that we need some priority for the receiver in this case.

    return 0;
}

// #todo
// Not implemented i ring 3 yet.
// Broadcast system message to all the threads.
// IN: Buffer
// OUT: The number of posted messages.
unsigned long sys_broadcast_system_message(unsigned long message_buffer)
{
    tid_t Sender_TID = (tid_t) current_thread;
    unsigned long long_rv=0;

// #todo
// Check the validation of this address agaisnt the valid user area,
// cause this function will be called by the applications.
// It needs to be abouve the user base area.
    if (message_buffer == 0)
        return 0;

    unsigned long *ubuf = (unsigned long *) message_buffer;

// Message code
    int MessageCode = (int) ( ubuf[1] & 0xFFFFFFFF );

// Call the worker.
    long_rv = 
    (unsigned long) ipc_broadcast_system_message(
        (tid_t) Sender_TID, // The sender is always the current_thread.
        (int) MessageCode,
        (unsigned long) ubuf[2],
        (unsigned long) ubuf[3],
        (unsigned long) ubuf[4],
        (unsigned long) ubuf[5] );
    
    return (unsigned long) long_rv;
}

