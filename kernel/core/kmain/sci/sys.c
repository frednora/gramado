// sys.c
// Created by Fred Nora.

// see: syscalls.h
// Kernel-mode callable interfaces.
// These are the wrappers for the real low-level 
// kernel services implementation implementation.
// Normally these routines are called by 
// the user mode applications via syscalls, 
// 0x80 and 0x82, 0x81 is not used for now.
// #todo
// Create some standard kernel-mode callable interfaces.
// That can be used by the ring0 kernel modules and
// by the syscalls.
// sys_xxxx is the prefix for the routines used by the syscalls.


#include <kernel.h> 


// See:
// kunistd.h
// https://man7.org/linux/man-pages/man2/alarm.2.html
// alarm() returns the number of seconds remaining until any
//       previously scheduled alarm was due to be delivered, or zero if
//       there was no previously scheduled alarm.

unsigned long sys_alarm(unsigned long seconds)
{
    struct thread_d *t;
    unsigned long old=0;
    unsigned long seconds_remaining=0;
    unsigned long extra_ms=0;

    if (current_thread<0 || current_thread > THREAD_COUNT_MAX )
        return 0; 
    t = (struct thread_d *) threadList[current_thread];
    if ( t->used != TRUE || t->magic != 1234 )
        return 0;

// Get the previous alarm.
    old = (unsigned long ) t->alarm;

// alarm() returns zero if
// there was no previously scheduled alarm.
    if (old<=0){
        // Zero seconds remaining.
        return 0;
    }

// Não ha o que fazer.
// O alarme que temos ja expirou.
// É trabalho do scheduler tratar ele.
// Não vamos chamar o scheduler durante uma syscall. 
    if (old < jiffies){
        // Zero seconds remaining.
        return 0;
    }

// O alarme que temos ainda é válido.
// O pit interrompe a 1000 ticks por segundo.
// Ex: 
// ( 8000-4000 )/1000 
// (4000/1000) 
// 4 segundos faltando.

    if (old > jiffies){
        seconds_remaining = (unsigned long) ((old - jiffies) / 1000);
    }


//
// Update.
//

// Update the thread info.

    if (seconds == 0){
        extra_ms = (unsigned long) 0;
    }
    if (seconds > 0){
        extra_ms = (unsigned long) (seconds*1000);
    }
    t->alarm = (unsigned long) (jiffies + extra_ms); 

// alarm() returns the number of seconds remaining until any
// previously scheduled alarm was due to be delivered.
    return (unsigned long) seconds_remaining;
}

// 3 | 250
// See: system.c
unsigned long sys_get_system_metrics(int n)
{
    if(n<0){
        return 0;  //#bugbug: ThIS is wrong!
    }
    return (unsigned long) doGetSystemMetrics((int)n);
}

// Only ring3 for now.
// OUT: ?
void *sys_create_process ( 
    struct cgroup_d *cg,
    unsigned long res1,          //nothing
    unsigned long priority, 
    int ppid, 
    char *name,
    unsigned long iopl ) 
{
    struct process_d *new;
    char NewName[32];
    struct thread_d *CurrentThread;
    struct process_d *CurrentProcess;
    int ProcessPersonality=0;

//
// Not tested
//

    debug_print("sys_create_process: [TODO]\n");
    printk     ("sys_create_process: [TODO]\n");
    //return NULL;

    // ==============
    
    // #todo
    // Check tid validation.

    CurrentThread = (struct thread_d *) threadList[current_thread];
    if ((void*)CurrentThread==NULL){
        return NULL;
    }
    if (CurrentThread->magic != 1234)
        return NULL;


// Create a ring0 copy of the name.
    strncpy(NewName,name,16);
    NewName[16]=0;  //finalize

//
// pml4
//

// Old pml4.
    unsigned long old_pml4=0;
    old_pml4 = CurrentThread->pml4_PA;  //save
// Switch
    x64_load_pml4_table( kernel_mm_data.pml4_pa );
// VA
    void *pml4_va = (void *) CloneKernelPML4();
    if (pml4_va == 0){
        panic ("sys_create_process: pml4_va\n");
        //goto fail;
    }
// PA
    unsigned long pml4_pa=0;
    pml4_pa = (unsigned long) virtual_to_physical ( 
                                  pml4_va, gKernelPML4Address );
    if ( pml4_pa == 0 ){
        panic ("sys_create_process: pml4_pa\n");
        //goto fail;
    }

//
// Create process
//

    pid_t current_pid = (pid_t) get_current_process();
    if (current_pid<0 || current_pid >= PROCESS_COUNT_MAX){
        panic("sys_create_process: current_pid\n");
    }
    CurrentProcess = (struct process_d *) processList[current_pid];
    if ( (void*) CurrentProcess == NULL )
        return NULL;
    if (CurrentProcess->magic!=1234)
        return NULL;

// Process personality
    ProcessPersonality = (int) CurrentProcess->personality;
    if (ProcessPersonality != PERSONALITY_GRAMADO){
        panic("sys_create_process: ProcessPersonality\n");
    }

// Create process.

    new = 
        (void *) create_process ( 
                     NULL,  // cg #todo: cgroup came from parameters.
                     (unsigned long) CONTROLTHREAD_BASE, 
                     PRIORITY_NORMAL_THRESHOLD, 
                     (int) current_pid, 
                     (char *) NewName, 
                     RING3, 
                     (unsigned long ) pml4_va,
                     (unsigned long ) kernel_mm_data.pdpt0_va,
                     (unsigned long ) kernel_mm_data.pd0_va,
                     ProcessPersonality );

    if ((void*) new == NULL){
        printk("sys_create_process: new\n");
        goto fail;
    }
    
    // #debug
    printk("sys_create_process: done :)\n");

// Switch back
    x64_load_pml4_table(old_pml4);

// done:
    return (void*) new;

fail:
    printk("sys_create_process: fail\n");
// Switch back
    x64_load_pml4_table(old_pml4);
    return NULL;
}

/*
 * sys_create_thread:
 *     Create thread system interface.
 */

// 72 - Create thread.
// #todo: 
// Enviar os argumentos via buffer.
// #todo: We can create threads not just in the current process,
// but also in some other process. Privilegies are necessary.
void *sys_create_thread ( 
    struct cgroup_d *cg,
    unsigned long init_rip, 
    unsigned long priority, 
    int ppid, 
    char *name )
{
    struct thread_d  *Thread;

    debug_print ("sys_create_thread:\n");

// #todo:
// Filtros, para ponteiros NULL.
    
    if (init_rip == 0){
        printk ("sys_create_thread: [FAIL] init_rip\n");
        return NULL;
    }

// #test
// Usermode buffer validation
// #todo: Check against more limits.
    if (init_rip < CONTROLTHREAD_BASE){
        panic ("sys_create_thread: init_rip\n");
        return NULL;
    }

// #todo
// Temos que checar o iopl do processo que chamou
// e a thread tem que estar no mesmo ring.
    int iopl = RING3;

// #bugbug
// isso significa que a tid0 só pode criar threads
// que vão rodar em ring0 ?

    //if ( ppid = TID0_TID )
        //iopl = RING0;

// Create thread.

// #bugbug #todo
// Only ring3 for now.
// We need to receive a parameter for that.
    
    Thread = 
        (struct thread_d *) create_thread ( 
                                cg,           // cgroup  
                                init_rip, 
                                priority, 
                                ppid, 
                                name,
                                iopl ); 

    if ((void *) Thread == NULL){
        debug_print("sys_create_thread: [FAIL] Thread\n");
        return NULL;
    }

    Thread->saved = FALSE;

// #suspended:
// We have another syscall to put the thread in the standby state.

// Put the state in STANDBY.
    //SelectForExecution ( (struct thread_d *) Thread );

    return (struct thread_d *) Thread;
}

// Exit thread.
int sys_exit_thread (tid_t tid)
{
    if (tid < 0 || tid >= THREAD_COUNT_MAX)
    {
        goto fail;
        //#todo: return (int) -EINVAL;
    }
    exit_thread(tid);
    return 0;

fail:
    return (int) -1;
}

// #todo:
// We're working in a helper function for clonning processes.
// See: clone.c
// #todo: Maybe the return type is pid_t.
int sys_fork(void)
{
    debug_print ("sys_fork: \n");
    // #todo
    // Call copy_process(...)
    return (int) -1;
}

// 85 
// Get PID of the current process.
pid_t sys_getpid (void)
{
    return (pid_t) get_current_pid();
}

// 81
// Get the PID of the father.
pid_t sys_getppid(void)
{
    struct process_d *p;

    pid_t current_pid = (pid_t) get_current_pid();
    if (current_pid < 0 || current_pid >= PROCESS_COUNT_MAX){
        goto fail;
    }
    p = (void *) processList[current_pid];
    if ((void *) p == NULL){
        goto fail;
    }
    if ( p->used != TRUE || p->magic != 1234 ){
        goto fail;
    }
// OK:
    return (pid_t) p->ppid;
fail:
    return (pid_t) -1;
}


// service 350. sci0
// Helper function to initialize system's component
// after kernel initialization.
// This is called by ring3 apps.

int sys_initialize_component (int n)
{
    if (n<0)
        return -1;

    switch (n)
    {
        // Full PS2 initialization
        // #suspended
        case 1:
            return 0;
            break;

        //case 2:
            //break;

        //case 3:
            //break;

        //case 4:
            //break;

        // ...

        default:
            return -1;
            break;
    };
    
    return (int) 0;
}



/*
 * sys_reboot:
 *     Reboot, Serviço do sistema.
 *     Chamando uma rotina interna de reboot do sistema.
 */
// The service 110.
// It's called by sci.c.
// The higher level routine for reboot.
// It's a wrapper, an interface.
// #todo: maybe we can use arguments.
// # We need to return when a non-superuser process call this
// service. We don't wanna hang the system in this case.

// # We need to return when 
// a non-superuser process call this service.

int sys_reboot(unsigned long flags)
{
    debug_print("sys_reboot: Rebooting\n");

    //#todo
    // Here is a goot place to check the flags 
    // and choose the bast way to do this syscall.

    // Call a safe routine.
    // see: system.c
    return (int) do_reboot(flags);
}

// 289
// See: debug.c
int sys_serial_debug_printk(char *s)
{

// Validate the parameter.
// This is a ring3 address.
    if ((void *) s == NULL){
        return (int) (-EFAULT);  // Bad address
    }

// #test
// Usermode buffer validation
// #todo: Check against more limits.
    if (s < CONTROLTHREAD_BASE)
    {
        panic ("sys_read: Invalid s\n");
        //return (ssize_t) -EFAULT;  // bad address
    }

    if (*s == 0){
        return (int) (-EINVAL);
    }	
    debug_print((char *)s);
    return 0;
}

// Wrapper
// Shutdown routine.
// Not tested yet.
void sys_shutdown(unsigned long flags)
{
    debug_print("sys_shutdown: [TODO]\n");
    cali_shutdown(0);
}

// Usada por vários serviços de debug.
// Usada para debug.
void sys_show_system_info(int n)
{
    if (n<0){
        debug_print("sys_show_system_info: [FAIL] n\n");
        return;
    }

    switch (n){
    case 1:  
        disk_show_info();        
        break;
    case 2:  
        volume_show_info();      
        break;
    case 3:  
        //mmShowMemoryInfo();  
        break;
    case 4:
        //systemShowDevicesInfo();
        //pciInfo();
        break;
    case 5:
        break;
    // See: detect.c
    case 6:
        //show_cpu_info();
        break;
    // ...
    };
}

// service 377.
// For now we're using default definitions.
// Copy
// We gotta get these information from pointer,
// not from 'defines'
// See: 
// utsname.h, u.h, version.h
// IN: Imported pointe to utsname structure.
int sys_uname(struct utsname *ubuf)
{

// Bad address
    if ((void *) ubuf == NULL){
        return (int) -EFAULT;
    }
// #todo: validate it against more limits.
    if (ubuf < CONTROLTHREAD_BASE){
        panic ("sys_uname: Invalid ubuf\n");
    }

    memcpy( 
        (void *) ubuf->sysname, 
        (const void *) kernel_utsname.sysname,
        sizeof(kernel_utsname.sysname) );

    memcpy( 
        (void *) ubuf->version, 
        (const void *) kernel_utsname.version,
        sizeof(kernel_utsname.version) ); 

    memcpy( 
        (void *) ubuf->release, 
        (const void *) kernel_utsname.release,
        sizeof(kernel_utsname.release) );    

    memcpy( 
        (void *) ubuf->machine, 
        (const void *) kernel_utsname.machine,
        sizeof(kernel_utsname.machine) );    

    memcpy( 
        (void *) ubuf->nodename, 
        (const void *) kernel_utsname.nodename,
        sizeof(kernel_utsname.nodename) );

    memcpy( 
        (void *) ubuf->domainname, 
        (const void *) kernel_utsname.domainname,
        sizeof(kernel_utsname.domainname) );

    return 0;
}

// ??
// Sync the vertical retrace of the monitor.
void sys_vsync(void)
{
    hal_vsync();
}


