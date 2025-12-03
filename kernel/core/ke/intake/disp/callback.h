// callback.h
// Created by Fred Nora.

#ifndef __DISP_CALLBACK_H
#define __DISP_CALLBACK_H    1

extern unsigned long asmflagDoCallbackAfterCR3;

// Context for the function
extern unsigned long ring3_callback_address;
extern unsigned long ring3_callback_parm1;
extern unsigned long ring3_callback_parm2;
extern unsigned long ring3_callback_parm3;
extern unsigned long ring3_callback_parm4;

// #suspended
extern unsigned long callback_restorer_done;


// Callback event
// Callback Go No Go moment.
// It handles the synchronization for the next callback event.
// It can happen for any ring 3 thread.
// The thread needs to be in an alertable state (Waiting)
struct callback_event_d 
{
// The structure was initialized by the kernel 
// during the kernel initialization routine?
    int initialized;

// The thread that will receive the callback event.
    tid_t target_tid;

// Let's lock it untill the moment everything is ok.
    int is_locked;

// Ready to go?
    int ready;
// State 0 - Initialize for the first time.
    int stage;

// Ring 3 address
    unsigned long r3_procedure_address;
};
//extern struct callback_event_d  CallbackEventInfo;

//
// ==========================================
//

void 
setup_callback_parameters(
    unsigned long param1, 
    unsigned long param2, 
    unsigned long param3, 
    unsigned long param4 ); 

void setup_callback(unsigned long r3_address);
void prepare_next_callback(void);


int callbackReinitialize(void);

//
// # 
// INITIALIZATION
//

int callbackInitialize(void);

#endif    



