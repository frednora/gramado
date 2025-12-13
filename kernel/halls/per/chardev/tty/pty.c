// pty.c
// Psedoterminal
// Kernel-side support for virtual terminals. 
// Created by Fred Nora.

// >> PTY master for the virtual terminal and 
// >> PTY slave for the application.
// PTY are used to connect the virtual terminal with an application or serial device.
// PTY uses the TTY structure.

// We can have a list of pty devices, the same way we have 
// a list of virtual consoles. But in this case we will have 
// limited number of virtual terminals.

/*
Minimal working path
 Keep the legacy globals for now.
 Route master writes into slave’s raw queue.
 Route slave writes into master’s raw queue.
 Expose the slave as /dev/ttyX so applications can open it.
 Let the emulator bind to the master.
 That will give you a usable PTY pair for testing shells inside your OS, 
 even before you implement canonical editing or job control.
*/

/*
Virtual Terminal (VT) → Master side
This is the kernel‑side endpoint. 
It’s what your console and input broker feed into. 
When you type on the keyboard, the broker pushes bytes into the master’s queues.

POSIX Shell (or any child process) → Slave side
This is the userland endpoint. 
When the master is linked to the slave, the kernel copies or 
redirects the master’s output queue into the slave’s input queue.
*/

/*
Flow in your architecture

User types in VT (GUI app)
 VT captures the keystroke event.
 VT writes the byte into the master PTY.

Master PTY → Slave PTY
 The kernel links master to slave (tty->link).
 Bytes written into master’s output queue are copied into slave’s input queue (tty_copy_output_buffer).
 The shell (slave) can read() and see the input.

Shell writes output
 Shell writes to its slave TTY.
 Kernel forwards slave’s output back to master.
 VT (master) receives the bytes and renders them in the GUI window.
*/

/*
If you want to make this concrete, you can:
 Treat your VT GUI app as a “userland master PTY driver.” It opens /dev/ptyM0 (for example) and sends/receives bytes.
 The kernel links that master to a slave /dev/ttyS0.
 The shell attaches to /dev/ttyS0 and runs normally.
 The kernel handles the redirection between them.
*/

/*
Device nodes:
Master PTY: expose something like /dev/ptyM0, /dev/ptyM1… (character devices).
Slave PTY:  expose /dev/ttyS0, /dev/ttyS1… (character devices).

ptyM0 (master): exposed for the terminal emulator (VT) to control the session.
ttyS0 (slave): exposed for the child process (shell) to use as its terminal.

Ah the slave intarects with its device file that represents a 
virtual terminal, /dev/ttyS0. Well in this case this is the target 
for printf data ... and the kernel will redirect it to the master and 
the vt will display the data
*/

#include <kernel.h>

struct pty_info_d *PTYInfo;

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

    const char *tty_master_name = "PTYM";
    const char *tty_slave_name  = "PTYS";

// Master
    pty_master = 
        (struct tty_d *) tty_create( TTY_TYPE_PTY, TTY_SUBTYPE_PTY_MASTER, tty_master_name );
    if ((void *) pty_master == NULL){
        panic("tty_initialize_legacy_tty: pty_master\n");
    }
    tty_start(pty_master);
    tty_reset_termios(pty_master);

// Slave
    pty_slave = 
        (struct tty_d *) tty_create( TTY_TYPE_PTY, TTY_SUBTYPE_PTY_SLAVE, tty_slave_name );
    if ((void *) pty_slave == NULL){
        panic("tty_initialize_legacy_tty: pty_slave\n");
    }
    tty_start(pty_slave);
    tty_reset_termios(pty_slave);

// Link
    pty_master->link = (struct tty_d *) pty_slave;
    pty_slave->link = (struct tty_d *) pty_master;

// Save
    legacy_pty_master = (struct tty_d *) pty_master;
    legacy_pty_slave = (struct tty_d *) pty_slave;

    return 0;
}

