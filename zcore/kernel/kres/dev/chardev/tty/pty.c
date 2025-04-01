
// pty.c
// Psedoterminal
// Virtual terminal support. 
// Created by Fred Nora.

#include <kernel.h>

struct tty_d *legacy_pty_master;
struct tty_d *legacy_pty_slave;

void tty_initialize_legacy_pty(void)
{

// #bugbug
// We can not call this too easly in the kernel initialization.

    struct tty_d *pty_master;
    struct tty_d *pty_slave;

//
// Create
//

    pty_master = (struct tty_d *) tty_create(
        TTY_TYPE_PTY, TTY_SUBTYPE_PTY_MASTER );
    if ((void *) pty_master == NULL){
        panic("tty_initialize_legacy_tty: pty_master\n");
    }
    tty_start(pty_master);

    pty_slave = (struct tty_d *) tty_create(
        TTY_TYPE_PTY, TTY_SUBTYPE_PTY_SLAVE );
    if ((void *) pty_slave == NULL){
        panic("tty_initialize_legacy_tty: pty_slave\n");
    }
    tty_start(pty_slave);

//
// Link
//

    pty_master->link = (struct tty_d *) pty_slave;
    pty_slave->link = (struct tty_d *) pty_master;

// Save
    legacy_pty_master = (struct tty_d *) pty_master;
    legacy_pty_slave = (struct tty_d *) pty_slave;
}


