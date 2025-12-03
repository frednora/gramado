// callback.c
// Created by Fred Nora.

#include <kernel.h>

// #todo
// Use call back only when the thread is in the alertable state.
// For example, when it is waiting for input, sleeping or etc.


//--------------------------------------

// #test
// Variáveis usadas em unit3hw.asm para implementar
// a chamada de um callback em ring3.
// O salto para ring3 acontece depois que o cr3 
// do processo alvo sofreu refresh. Para garantir
// que temos o espaço d endereçamento que desejamos.
// #todo: Podemos mudar esses nomes para ficar mais fácil
// para o assembly entender o que esta acontecendo.
// see: psTaskSwitch

// Exported to asm.
// See: sw2.asm, hw2.asm
// Globals consumed by assembly; 
// mark volatile to enforce ordering assumptions
unsigned long asmflagDoCallbackAfterCR3=0;

unsigned long ring3_callback_address=0;
unsigned long ring3_callback_parm1=0;
unsigned long ring3_callback_parm2=0;
unsigned long ring3_callback_parm3=0;
unsigned long ring3_callback_parm4=0;

// #suspended
unsigned long callback_restorer_done=0;

// Not exported to asm.
int _callback_status=FALSE;
unsigned long _callback_address=0;
unsigned long _callback_address_saved=0;

//struct callback_event_d  CallbackEventInfo;


// =====================================================

void 
setup_callback_parameters(
    unsigned long param1, 
    unsigned long param2, 
    unsigned long param3, 
    unsigned long param4 ) 
{
    ring3_callback_parm1 = (unsigned long) param1;
    ring3_callback_parm2 = (unsigned long) param2;
    ring3_callback_parm3 = (unsigned long) param3;
    ring3_callback_parm4 = (unsigned long) param4;
}

// service 44000
// Called by sci.c
// Also called by DeviceInterface_PIT in pit.c.
void setup_callback(unsigned long r3_address)
{
/*
    printk("setup_callback:\n");

// Not initialized at kernel initialization
    if ( CallbackEventInfo.initialized != TRUE )
        panic("setup_callback: Callback Not initialized\n");

// The callback event is locked for safety
    if (CallbackEventInfo.is_locked == TRUE)
        panic("setup_callback: Callback is locked\n");

// Ready? Something is wrong!
// If it's already ready we can't start a setup routine.
    //if ( CallbackEventInfo.ready == TRUE )
        //panic("setup_callback: Callback fail\n");

// Ring 3 address for the procedure inside the thread.
    CallbackEventInfo.r3_procedure_address = (unsigned long) r3_address;

// Now we're ready.
    //CallbackEventInfo.ready = TRUE;
*/
}

// The moment where the kernel update the variables in assembly ... 
// that is gonna be used by the iretq routine.
// This function stages the handoff for the iretq path: 
// it sets a flag the assembly watches, 
// publishes the ring‑3 callback address for the trampoline, 
// then clears the “ready” gate so the delivery is one‑shot. 
// The flow is coherent for your current single-global model.
void prepare_next_callback(void)
{
/*
// Not ready
    //if (CallbackEventInfo.ready != TRUE)
        //panic("prepare_next_callback: No ready\n");

// Invalid ring 0 address
    if (CallbackEventInfo.r3_procedure_address == 0)
        panic("prepare_next_callback: Invalid r3 address\n");

// Publish handler first
    ring3_callback_address = 
        (unsigned long) CallbackEventInfo.r3_procedure_address;

    CallbackEventInfo.r3_procedure_address = 0;

    // Optional: memory barrier to order writes before the flag
    // on x86 you can use a compiler barrier; on weaker archs use an explicit fence.
    // asm volatile("" ::: "memory");

    // Signal assembly to pivot after CR3 switch
    // This flags tells to the assembly that it is time to execute
    // the routine that iretq to the ring 3 procedure.
    asmflagDoCallbackAfterCR3 = (unsigned long)(0x1234 & 0xFFFF);

//
// == Reinitializing the c part of the callback support =======
//

    //callbackReinitialize();

    CallbackEventInfo.is_locked = FALSE;

// One-shot: consume the staging gate
// Not ready for the next one.
    //CallbackEventInfo.ready = FALSE;

// Clear the source field;
// The assembly is using another address, the 'ring3_callback_address'.
    //CallbackEventInfo.r3_procedure_address = 0;
*/
}

// Called during the callback restorer
// in order to prepare for the next callback.
// “reset after use” routine
int callbackReinitialize(void)
{
/*
    if (CallbackEventInfo.initialized != TRUE)
        panic ("callbackReinitialize: Not initialized\n");

// Let's lock it untill the moment everything is ok.
    //CallbackEventInfo.is_locked = TRUE;
    CallbackEventInfo.is_locked = FALSE;

// No target TID for now
    CallbackEventInfo.target_tid = -1;


// Ring 3 address
    CallbackEventInfo.r3_procedure_address = 0;

// Not ready yet
// Some other routine will update it right after we got a valid ring 3 address
    //CallbackEventInfo.ready = FALSE;
*/

    return 0;
}

//
// # 
// INITIALIZATION
//

// callbackInitialize:
// Initialize for the first time during the kernel initialization.
// #debug
// For now the callback support is operating only over the 
// display server. But the plain is perform on every ring 3 processes.
// Called by ke.c
int callbackInitialize(void)
{

/*
    if (CallbackEventInfo.initialized == TRUE)
        panic ("callbackInitialize: Already initialized\n");

    CallbackEventInfo.initialized = FALSE;

// Stage 0 – Initialization
    CallbackEventInfo.stage = 0;

// Let's lock it untill the moment everything is ok.
    CallbackEventInfo.is_locked = TRUE;
    //CallbackEventInfo.is_locked = FALSE;

// No target TID for now
    CallbackEventInfo.target_tid = -1;

// Ring 3 address
    CallbackEventInfo.r3_procedure_address = 0;

// Not ready for a callback
// Some other routine will update it right after we got a valid ring 3 address
    //CallbackEventInfo.ready = FALSE;

// Initialized for the first time
    CallbackEventInfo.initialized = TRUE;

// Stage 1 – Waiting for syscalls
    CallbackEventInfo.stage = 1;

    callback_restorer_done = 0;
*/

    return 0;
}


