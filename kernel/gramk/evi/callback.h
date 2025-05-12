// callback.h
// Created by Fred Nora.

#ifndef __GRAMK_CALLBACK_H
#define __GRAMK_CALLBACK_H    1

extern unsigned long asmflagDoCallbackAfterCR3;

//extern unsigned long asmRing3CallbackAddress;
extern unsigned long ring3_callback_address;

/*
// #test: This is a work in progress.
// General callback support for every process.
struct callback_info_d
{
// #todo

    int initialized;  // initialized for the first time.

    pid_t pid;
    tid_t tid;
    
    int ready;  // pronto para uso.
    unsigned long callback_address;
    unsigned long callback_address_saved;

    unsigned long each_n_ms;
    unsigned long times_per_second;
};
*/

// #test: This is a work in progress.
// Callback support for the display server.
struct ds_callback_info_d
{
    int initialized;  // initialized for the first time.
    
    int ready;  // pronto para uso.
    unsigned long callback_address;
    unsigned long callback_address_saved;

    unsigned long each_n_ms;
    unsigned long times_per_second;
};

//extern int _callback_status;
//extern unsigned long _callback_address;
//extern unsigned long _callback_address_saved;

extern struct ds_callback_info_d  ds_callback_info;

//
// ==========================================
//


void setup_callback(unsigned long r3_address, unsigned long ms);
void prepare_next_ds_callback(void);

//
// # 
// INITIALIZATION
//

int callbackInitialize(void);

#endif    



