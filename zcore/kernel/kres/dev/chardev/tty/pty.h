// see: 
//http://man7.org/linux/man-pages/man7/pty.7.html
//https://en.wikipedia.org/wiki/Pseudoterminal
//https://lwn.net/Articles/688809/

// See: ptmx.

#ifndef ____PTY_H
#define ____PTY_H  1

extern struct tty_d *legacy_pty_master;
extern struct tty_d *legacy_pty_slave;

void tty_initialize_legacy_pty(void);

#endif    



