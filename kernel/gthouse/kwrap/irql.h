// irql.h
// Interrupt Request Level (IRQL)
// Created by Fred Nora.

#ifndef __KWRAP_IRQL_H
#define __KWRAP_IRQL_H    1

/*
How your IRQLs map conceptually:
 + IRQL_TIMER_INTERRUPT → The periodic tick, driving scheduling.
 + IRQL_DISPATCHER → Context save/restore, task switching, spawning.
 + IRQL_SCHEDULER → The scheduler itself deciding who runs next.
 + IRQL_IRQ → Hardware interrupts (keyboard, mouse, NIC).
 + IRQL_SYSCALL → System call entry (int 0x80, syscall).
 + IRQL_TRAPS → Exceptions and faults (page fault, GPF, double fault).
*/

/*
The three phases during the timer interrupt are:

IRQL_TIMER_INTERRUPT
 Entering/exiting the IRQ routine.
 The CPU is strictly in interrupt context: acknowledge the timer, save minimal state.
 Must be very short, no blocking.

IRQL_DISPATCHER
 Context save, task switching, restore context, spawn.
 This is the “dispatcher moment” where the kernel decides which thread to resume.
 Equivalent to Windows’ DISPATCH_LEVEL — can’t block, but can manipulate scheduling structures.

IRQL_SCHEDULER
 Rebuilding the queues, recalculating priorities, balancing workloads.
 This is the “scheduler moment” where the system updates its run queues before handing control back.
 Heavy but deterministic work, still in Ring 0, but not as restrictive as pure IRQ context.
*/

#define IRQL_NULL    0

//
// The next three operates together,
// they are 3 phases of the same momentum.
//

//++
// Timer Interrupt Request Level
#define IRQL_TIMER_INTERRUPT  1
// Dispacher Request Level
// (save context, Task switching, restore context, spawn)
#define IRQL_DISPATCHER       2     
// Scheduler Request Level
#define IRQL_SCHEDULER        3
//--

// Hardware Interrupt Request Level (keyboard, mouse, nic ...)
#define IRQL_IRQ              4

// System Call Request Level (int 0x80, syscall ...)
#define IRQL_SYSCALL          5

// Traps and Exceptions Request Level 
// (page fault, general protection fault, double fault ...)
#define IRQL_TRAPS            6

// ...

// #todo: #important:
// It's good for the panic() function to show us the current 
// macro and micro phase we are. Showing the values for 
// system_state and gInterruptRequestLevel.

extern int gInterruptRequestLevel;

#endif   




