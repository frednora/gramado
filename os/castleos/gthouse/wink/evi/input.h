// input.h
// Created by Fred Nora.

#ifndef __EVI_INPUT_H
#define __EVI_INPUT_H    1

/*
What Your input_authority_d Structure Enables

Dynamic Input Routing: 
By abstracting input authority into a struct, you allow the kernel 
to switch between input modes based on system state.

Security & Control: 
Only the designated authority (kernel, display server, etc.) can 
set the foreground thread, preventing unauthorized input hijacking.

Extensibility: 
You can later add more modes (e.g., remote desktop, multi-user sessions) 
without rewriting core input logic.
*/

// Types of input authorities:
#define AUTH_KERNEL    1000
#define AUTH_NO_GUI    1001
#define AUTH_GUI       1002
// The window manager normally changes the foreground thread,
// when in Graphical Environment mode. But Gramado OS 
// has the window manager embedded into the display server.
#define AUTH_DISPLAY_SERVER    AUTH_GUI

// Who will be able to setup the current foreground thread.
// Funtamental for the input system.
struct input_authority_d {
    int used;
    int magic;
    int initialized;
// Options: 
// + AUTH_KERNEL when the kernel console is active, in boot or panic.
// + AUTH_NO_GUI when we do not have a running display server.
// + AUTH_DISPLAY_SERVER when we have a running display server.
    int current_authority;
};
// #test #todo
// This is a test yet.
// We're implementing it, but it is not fully in use. 
// Defined in input.c
extern struct input_authority_d  InputAuthority;

#define INPUT_TARGET_STDIN         1001
#define INPUT_TARGET_THREAD_QUEUE  1002
// ...

// ...

// -----------------------------------
// Input targets:
// We can sent input to some targets:
// + ASCII goes to the ttys associated with stdin.
// + Message queue of the foreground thread.
// Let's select the valid targets.
struct input_targets_d
{
// The kernel sends input to the tty associated with stdin.
    int target_stdin;
// The kernel sends input to a thread's queue.
    int target_thread_queue;
};
// see: input.c
extern struct input_targets_d  InputTargets;


// Basic block of data to handle input events.
// Used PS2 keyboard and PS2 mouse for now.
// See: grinput.c
struct input_block_d
{
    int ev_code;
    unsigned long long1;
    unsigned long long2;
};

struct input_event_d
{
// data
    struct input_block_d  in;
    int type;  // input type
    unsigned long jiffies;   // time
    // ...
};

//==========================================================

// From input.c
unsigned long ksys_mouse_event(int event_id,long long1, long long2);
unsigned long ksys_keyboard_event(int event_id,long long1, long long2);
unsigned long ksys_timer_event(int signature);

#endif    

