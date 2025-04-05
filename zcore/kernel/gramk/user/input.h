// input.h
// Created by Fred Nora.

#ifndef __DISPLAY_GRINPUT_H
#define __DISPLAY_GRINPUT_H    1


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

// See: input.c
int gramk_process_cad_combination(unsigned long flags);
int gramk_set_input_targets(int stdin_target, int queue_target);
void gramk_enter_kernel_console(void);
void gramk_exit_kernel_console(void);

int 
wmRawKeyEvent(
    unsigned char raw_byte,
    int prefix );

//
// Input events:
//

int wmMouseEvent(int event_id,long long1, long long2);
int wmKeyboardEvent(int event_id,long long1, long long2);
int wmTimerEvent(int signature);

#endif    

