
// state.h
// System state

#ifndef __SYSTEM_STATE_H
#define __SYSTEM_STATE_H    1

// Nothing
#define SYSTEM_NULL        0
// Getting info from bootloader tables.
// Setup global, and configurations.
// Starting serial debuyg support and virtual console support.
#define SYSTEM_PREINIT     1
// The system is starting ...
#define SYSTEM_BOOTING     2
// The system is scheduling ...
#define SYSTEM_SCHEDULING  3
// The system is running.
#define SYSTEM_RUNNING     4
// The system is running the reboot routine.
#define SYSTEM_REBOOT      5
// The system is running the poweroff routine.
#define SYSTEM_POWEROFF    6
// The system is running the routine to restart the kernel.
#define SYSTEM_RESTART     7
// The system was aborted ... ex: initialization fail
#define SYSTEM_ABORTED     8
// Running the panic routine.
#define SYSTEM_PANIC       9
// The initialization fail and we are
// halted for ever.
#define SYSTEM_DEAD        10
// ...

extern int system_state;

// ============================

#endif   

