// inittask.h
// 

#ifndef __LIBINIT_LIBINIT_H
#define __LIBINIT_LIBINIT_H    1


extern int isTimeToQuitServer;

#define __MSG_BUFFER_SIZE  64
extern char __filename_local_buffer[64];
extern int NoReply;

struct next_msg_d
{
// TID
    int target_tid;
// Message
    int msg_code;
    unsigned long long1;
    unsigned long long2;
};
extern struct next_msg_d  NextMessage;


#define __MSG_CLOSE     7
#define __MSG_COMMAND  40


// ==========================

// See: msgloop.c
int msgloop_RunServer(void);
int msgloop_RunServer_HeadlessMode(void);


#endif   

