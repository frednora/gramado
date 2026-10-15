// go.c
// #todo: Lets create a new experimental interface for the Gramado OS kernel.
// The functions will start with the prefix 'go', ex: go_xxx(). We can also 
// use the same prefix in user mode.

#include <kernel.h>


unsigned long go_get_system_metrics(int n)
{
    if (n<0)
        return 0;
    return (unsigned long) sys_get_system_metrics(n);
}

int go_fcntl(int fd, int cmd, unsigned long arg)
{
    return (int) sys_fcntl( (int) fd, (int) cmd, (unsigned long) arg );
}


ssize_t go_read(int fd, const char *ubuf, size_t count)
{
    return (ssize_t) sys_read((int) fd, (const char *)ubuf, (size_t) count);
}

ssize_t go_write(int fd, const char *ubuf, size_t count)
{
    return (ssize_t) sys_write((int) fd, (const char *)ubuf, (size_t) count);
}

void go_yield(tid_t tid)
{
    if (tid<0)
        return;
    sys_yield(tid);
}

void go_sleep(tid_t tid, unsigned long ms)
{
    if (tid<0)
        return;
    sys_sleep(tid, ms);
}


void go_nice(unsigned long decrement, int lapic_info_id)
{
    sys_nice(decrement, lapic_info_id);
}


int go_ioctl(int fd, unsigned long request, unsigned long arg)
{
    //if (fd<0)
        //return -1;
    return (int) sys_ioctl(fd, request, arg);
}

void go_set_file_sync(int fd, int request, int data)
{
    sys_set_file_sync(fd, request, data);
}

int go_get_file_sync(int fd, int request)
{
    return (int) sys_get_file_sync(fd, request);
}

//
//
//

void go_set_global_sync(int sync_id, int request, int data)
{
    sys_set_global_sync(sync_id, request, data);
}

int go_get_global_sync (int sync_id, int request)
{
    return (int) sys_get_global_sync(sync_id, request);
}

int go_create_new_sync(void)
{
    return (int) sys_create_new_sync();
}


int go_initialize(void)
{
    return 0;
}

