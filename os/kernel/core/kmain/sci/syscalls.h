// syscalls.h
// Created by Fred Nora.

// See: sci.c.

// #todo
// Actually the kernel has 3 interrupts for system services.
// So, this way we need 3 lists of numbers.
// So, this way we have 3 headers for this purpose.
// sci0.h, sci1.h and sci2.h 

// #todo
// We can change these names,
// Maybe startinng with Syscall_xxx

#ifndef  __GRAMADO_SYSCALLS_H
#define  __GRAMADO_SYSCALLS_H    1


//
// == prototypes ====
//

// visitor/sci.c
void *sci0 ( 
    unsigned long number, 
    unsigned long arg2, 
    unsigned long arg3, 
    unsigned long arg4 );

// visitor/sci.c
void *sci1 ( 
    unsigned long number, 
    unsigned long arg2, 
    unsigned long arg3, 
    unsigned long arg4 );

// visitor/sci.c
void *sci2 ( 
    unsigned long number, 
    unsigned long arg2, 
    unsigned long arg3, 
    unsigned long arg4 );

// visitor/sci.c
void *sci3 ( 
    unsigned long number, 
    unsigned long arg2, 
    unsigned long arg3, 
    unsigned long arg4 );


// --------------------------------

/* zero/sys/sys.c */
unsigned long sys_get_system_metrics(int n);

/* sys.c */
void *sys_create_process ( 
    struct cgroup_d *cg,
    unsigned long res1,          //nothing
    unsigned long priority, 
    int ppid, 
    char *name,
    unsigned long iopl );

/* sys.c */
void *sys_create_thread ( 
    struct cgroup_d *cg,
    unsigned long init_rip, 
    unsigned long priority, 
    int ppid, 
    char *name );

/* zero/sys/sys.c */
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


