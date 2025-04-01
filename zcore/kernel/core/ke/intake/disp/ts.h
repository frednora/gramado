// ts.h 
// Context switch support.
// Created Fred Nora.

#ifndef __TS_H
#define __TS_H    1

extern unsigned long task_switch_status;

// ------------------------

unsigned long get_taskswitch_status (void);
void set_taskswitch_status( unsigned long status );

void taskswitch_lock (void);
void taskswitch_unlock (void);

void tsCallExtraRoutines(void);

// Main task switching routine
// Called by irq0_TIMER() int pit.c.
// See also: hw.asm
void tsTaskSwitch (void);

//
// $
// INITIALIZATION
//

int init_ts(void);

#endif    

