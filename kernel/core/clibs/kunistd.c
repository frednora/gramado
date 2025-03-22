// kunistd.c
// Created by Fred Nora.

#include <kernel.h>    

// sys_gethostname:
// Get host name.
// pegando host name
// faremos igual fizemos com usuário.
// suporte a rotina da libc.
// #todo: How many bytes we can copy into the buffer.
// We need a parameter for that.
int sys_gethostname(char *ubuff)
{
    char *hostname_buffer = (char *) ubuff;
    int size=0;

// Parameters
// Invalid buffer. Bad address.
    if ((void*) ubuff == NULL){
        return (int) -EFAULT;
    }
// Estrutura default para informações sobre o host.
// see: host.h
    if ((void *) HostInfo == NULL){
        printk("sys_gethostname: HostInfo\n");
        goto fail;
    }
// Copy 64 bytes.
    strcpy ( 
        hostname_buffer, 
        (const char *) HostInfo->__hostname );

// Get the size.
    size = (int) HostInfo->hostName_len;
    return (int) size;

fail:
    refresh_screen();
    return (int) -1;
}

// sys_sethostname:
// Set host name.
// #todo
// configurando o hostname.
// do mesmo jeito que configuramos o username,
// só que em estruturas diferentes
int sys_sethostname(const char *new_hostname, size_t len)
{
// #todo: This operation needs permition

    size_t StringLen=0;
    size_t GivenLen = len;

// Parameters
    if ((void*) new_hostname == NULL){
        goto fail;
    }
    if (*new_hostname == 0){
        goto fail;
    }

    StringLen = (size_t) strlen(new_hostname);
    StringLen = (size_t) (StringLen + 1);

// Estrutura HostInfo em host.h 
// (network)
    if ((void *) HostInfo == NULL){
        printk("sys_sethostname: HostInfo\n");
        goto fail;
    }
    HostInfo->hostName_len = (size_t) StringLen;
    // ...

// Copy 64 bytes.
    strcpy ( 
        HostInfo->__hostname, 
        (const char *) new_hostname );

    return 0;
fail:
    refresh_screen();
    return (int) -1;
}

long fpathconf(int fildes, int name)
{
    debug_print("fpathconf: [TODO]\n");
    return -1;
}

long pathconf(const char *pathname, int name)
{
    debug_print("pathconf: [TODO]\n");    
    return -1;
} 

// sys_lseek - reposition read/write file offset
// See:
// https://man7.org/linux/man-pages/man2/lseek.2.html
// #todo:
// IN: ??
// OUT: ??
off_t sys_lseek(int fd, off_t offset, int whence)
{
    file *f;
    struct process_d *p;
    pid_t current_process = -1;

    // #debug
    // debug_print("sys_lseek: [WORK IN PROGRESS]\n");

    if (fd<0 || fd >= 32){
        return (off_t) (-EBADF);
    }

//#todo
    //if(offset<0)
        //return (off_t) (-EINVAL);

    if (whence<0){
        return (off_t) -EINVAL;
    }

// pid
    current_process = (pid_t) get_current_process();
    if (current_process<0 || current_process >= PROCESS_COUNT_MAX){
        goto fail;
    }

// Process structure.
    p = (struct process_d *) processList[current_process];
    if ((void *) p == NULL){
        debug_print("sys_lseek: p\n");
        goto fail;
    }
    if (p->magic != 1234){
        goto fail;
    }

// File
    f = (file *) p->Objects[fd];
    if ((void *) f == NULL){
        debug_print("sys_lseek: f\n");
        return -1;  // (off_t) (-EBADF); 
    }
    if (f->magic != 1234){
        return -1;  // (off_t) (-EBADF);
    }

// #todo
// Check if it is a pipe, socket or fifo.
// '-ESPIPE' fd is associated with a pipe, socket, or FIFO.

// fseek
// See: kstdio.c
    k_fseek( (file *) f, (long) offset, (int) whence );

    if (f->_p < f->_base)
    {
        // #debug
        panic("sys_lseek: #fixme f->_p < f->_base\n");
    }

// Final result.
    off_t FinalResult = (off_t) (f->_p - f->_base);
    return (off_t) FinalResult;

fail:
    return (off_t) -1;
}

//
// End
//

