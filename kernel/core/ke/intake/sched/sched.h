// sched.h
// Scheduling support.
// Created by Fred Nora.

#ifndef __PS_SCHED_H
#define __PS_SCHED_H    1


// ----------------------------------------
// Event responter thread
extern int g_use_event_responder;
extern struct thread_d  *ev_responder_thread;

//#define NR_TASKS ??
//#define FIRST_TASK ?
//#define LAST_TASK ? 

// Status do scheduler.
#define LOCKED    0
#define UNLOCKED  1  

extern unsigned long g_scheduler_status;

/*
 * Scheduling policies
 */

#define SCHED_POLICY_RR    0
#define SCHED_POLICY_QUEUES  1
// ...

// Scheduler information
struct scheduler_info_d
{
    int initialized;
    int policy;
    unsigned long rr_round_counter;
    unsigned long flags;
};
extern struct scheduler_info_d  SchedulerInfo;


// ==========================================

//
// == prototypes ============
//

struct thread_d *get_init_thread(void);
struct thread_d *get_next_on_queue_or_the_init_thread(struct thread_d *q);
struct thread_d *get_ev_responter(void);
int set_ev_responder(struct thread_d *thread);
int has_pending_event(struct thread_d *thread);

//
// In schedi.c
//

void do_credits(struct thread_d *thread);
void do_credits_by_tid(tid_t tid);
void do_thread_blocked(tid_t tid);
void do_thread_dead(tid_t tid);
void do_thread_initialized(tid_t tid);
void do_thread_ready(tid_t tid);
void do_thread_running(tid_t tid);
void do_thread_standby(tid_t tid);
void do_thread_waiting(tid_t tid, unsigned long ms);
void do_thread_zombie(tid_t tid);
int do_waitpid (pid_t pid, int *status, int options);

void schediSelectForExecution(struct thread_d *Thread);

void schedi_drop_quantum(struct thread_d *thread);
void schedi_check_for_standby(void);

tid_t get_current_thread(void);
void set_current_thread(tid_t tid);
void set_foreground_thread(tid_t tid);

void wait_for_a_reason ( tid_t tid, int reason );
int wakeup_thread_reason ( tid_t tid, int reason );
int wakeup_scan_thread_reason (int reason);
void wakeup_thread(tid_t tid);

// yield
void yield(tid_t tid);

// sleep
void sleep_until(tid_t tid, unsigned long ms);
void sleep(tid_t tid, unsigned long ms);


void sched_cut_round(struct thread_d *last_thread);

void scheduler_lock(void);
void scheduler_unlock(void);
unsigned long scheduler_get_status(void);


//
// $
// SCHEDULER
//

// See: sched.c
tid_t scheduler(void);
tid_t psScheduler(void);

//
// $
// SYSCALL HANDLERS
//

unsigned long sched_count_active_threads(void);

// See: sched.c
void sys_broken_vessels(tid_t tid);
void sys_sleep(tid_t tid, unsigned long ms);
void sys_yield(tid_t tid);

//
// $
// INITIALIZATION
//

int init_scheduler (unsigned long sched_flags);

#endif    

