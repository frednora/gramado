// ke.h
// Created by Fred Nora.

#ifndef __KE_KE_H
#define __KE_KE_H    1

void keDie(void);
void keSoftDie(void);
int keReboot(void);

unsigned long keGetSystemMetrics(int index);

int keIsQemu(void);

int keCloseInitProcess(void);

// Wrapper
struct te_d *keCreateProcess ( 
    struct cgroup_d *cg,
    unsigned long base_address, 
    unsigned long priority, 
    ppid_t ppid, 
    const char *name, 
    unsigned int cpl,
    unsigned long pml4_va,
    unsigned long pdpt0_va,
    unsigned long pd0_va,
    personality_t personality );

// Wrapper
struct thread_d *keCreateThread ( 
    thread_type_t thread_type,
    struct cgroup_d  *cg,
    unsigned long init_rip, 
    unsigned long init_stack, 
    ppid_t pid, 
    const char *name,
    unsigned int cpl );

// Called by main to execute the first process
int ke_x64ExecuteInitialProcess(int cpu_id);

int keInitialize(int phase);

#endif  

