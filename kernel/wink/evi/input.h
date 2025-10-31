// input.h
// Created by Fred Nora.

#ifndef __EVI_INPUT_H
#define __EVI_INPUT_H    1


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

// -----------------------------------
// Input targets:
// We can sent input to some targets:
// + stdin file.
// + Message queue of the foreground thread.
// Let's select the valid targets.
struct input_targets_d
{
// The structure initialization.
    int initialized;

// The input goes to stdin
    int target_stdin;
// The input goes to the thread's queue.
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

