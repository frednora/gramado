// input.h
// Created by Fred Nora.

#ifndef __EVI_INPUT_H
#define __EVI_INPUT_H    1


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

