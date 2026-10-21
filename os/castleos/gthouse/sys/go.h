// go.h

#ifndef __GO_GO_H
#define __GO_GO_H    1


unsigned long go_get_system_metrics(int n);
int go_fcntl(int fd, int cmd, unsigned long arg);


ssize_t go_read(int fd, const char *ubuf, size_t count);
ssize_t go_write(int fd, const char *ubuf, size_t count);

void go_yield(tid_t tid);
void go_sleep(tid_t tid, unsigned long ms);

void go_nice(unsigned long decrement, int lapic_info_id);

int go_ioctl(int fd, unsigned long request, unsigned long arg);

void go_set_file_sync(int fd, int request, int data);
int go_get_file_sync(int fd, int request);

void go_set_global_sync(int sync_id, int request, int data);
int go_get_global_sync (int sync_id, int request);

int go_create_new_sync(void);


int go_initialize(void);

#endif  




