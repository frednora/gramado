// sys.h
// The handlers for the services.
// Created by Fred Nora.

// See: sci.c.

// #todo
// Actually the kernel has 3 interrupts for system services.
// So, this way we need 3 lists of numbers.
// So, this way we have 3 headers for this purpose.
// sci0.h, sci1.h and sci2.h 

#ifndef  __SCI_SYS_H
#define  __SCI_SYS_H    1

//
// == prototypes ====
//

// --------------------------------

// See: sys.c
unsigned long sys_get_system_metrics(int n);

// See: sys.c
void *sys_create_process ( 
    struct cgroup_d *cg,
    unsigned long res1,          //nothing
    unsigned long priority, 
    ppid_t ppid, 
    char *name,
    unsigned long iopl );

// See: sys.c
void *sys_create_thread ( 
    thread_type_t thread_type,
    struct cgroup_d *cg,
    unsigned long initial_rip, 
    unsigned long initial_stack, 
    ppid_t ppid, 
    char *name );

// See: sys.c
int sys_exit_thread(tid_t tid);
int sys_fork(void);
pid_t sys_getpid(void);
pid_t sys_getppid(void);
int sys_initialize_component(int n);
int sys_serial_debug_printk(char *s);
void sys_show_system_info(int n);
int sys_uname(struct utsname *ubuf);
void sys_vsync(void);

// See: sys.c
int sys_reboot(unsigned long flags);
void sys_shutdown(unsigned long flags);

#endif    


