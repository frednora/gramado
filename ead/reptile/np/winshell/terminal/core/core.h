// core.h
// Backend part for the terminal application.

#ifndef __TERMINAL_CORE_CORE_H
#define __TERMINAL_CORE_CORE_H    1

#define IP(a, b, c, d) \
    (a << 24 | b << 16 | c << 8 | d)

struct terminal_core_d 
{
    int socket_fd;
    int is_connected;
};
extern struct terminal_core_d  TerminalCore; 

extern int isWaitingForOutput;

int terminal_core_launch_child(const char *filename);
int terminal_core_launch_from_cmdline(int fd, const char *cmdline);

void terminal_poweroff_machine(int fd);
void __test_post_async_hello(void);

#endif  

