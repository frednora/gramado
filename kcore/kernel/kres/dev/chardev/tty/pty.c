
// pty.c
// Psedoterminal
// Virtual terminal support. 
// Created by Fred Nora.

// >> PTY master for the virtual terminal and 
// >> PTY slave for the application.
// PTY are used to connect the virtual terminal with an application or serial device.
// PTY uses the TTY structure.

// We can have a list of pty devices, the same way we have 
// a list of virtual consoles. But in this case we will have 
// limited number of virtual terminals.


#include <kernel.h>


struct pty_info_d *TTYInfo;


struct tty_d *legacy_pty_master;
struct tty_d *legacy_pty_slave;


struct tty_d *get_legacy_pty_master(void)
{
    return (struct tty_d *) legacy_pty_master;
}

struct tty_d *get_legacy_pty_slave(void)
{
    return (struct tty_d *) legacy_pty_slave;
}


//
// INITIALIZE LEGACY PTYs.
//

// Create two PTYs, link them and save the pointers.
int tty_initialize_legacy_pty(void)
{

// #bugbug
// We can not call this too easly in the kernel initialization.

    struct tty_d *pty_master;
    struct tty_d *pty_slave;

//
// Create
//


// Master
    pty_master = 
        (struct tty_d *) tty_create( TTY_TYPE_PTY, TTY_SUBTYPE_PTY_MASTER );
    if ((void *) pty_master == NULL){
        panic("tty_initialize_legacy_tty: pty_master\n");
    }
    tty_start(pty_master);

// Slave
    pty_slave = 
        (struct tty_d *) tty_create( TTY_TYPE_PTY, TTY_SUBTYPE_PTY_SLAVE );
    if ((void *) pty_slave == NULL){
        panic("tty_initialize_legacy_tty: pty_slave\n");
    }
    tty_start(pty_slave);

// Link
    pty_master->link = (struct tty_d *) pty_slave;
    pty_slave->link = (struct tty_d *) pty_master;

// Save
    legacy_pty_master = (struct tty_d *) pty_master;
    legacy_pty_slave = (struct tty_d *) pty_slave;

    return 0;
}

