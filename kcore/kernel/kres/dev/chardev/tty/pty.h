// see: 
//http://man7.org/linux/man-pages/man7/pty.7.html
//https://en.wikipedia.org/wiki/Pseudoterminal
//https://lwn.net/Articles/688809/

// See: ptmx.

//
// Independent of kernel console
// PTYs are entirely virtual and exist in user space, providing a way for terminal emulators 
// to function independently of kernel consoles.
// The graphical system operates on top of one kernel console (like /dev/tty7), 
// while PTYs simulate terminal functionality within the graphical environment.
//

// Keyboard Input:
// When the user types on the keyboard, the input is sent directly to the PTY master, 
// not the PTY slave. The PTY master is the entry point for all data coming from the terminal emulator or 
// graphical system.

// Forwarding to the PTY Slave:
// After the virtual terminal processes the input, it sends the data to the PTY slave, 
// which acts like a traditional TTY device. The PTY slave passes the processed input 
// to the system or shell (e.g., bash).

// Canonical Mode: Input is processed line-by-line. The user can edit the input locally 
// in the terminal emulator before pressing Enter, at which point the whole line is sent 
// to the PTY slave and, in turn, to the application.

// Non-Canonical Mode: Input is processed character-by-character without waiting for the user 
// to press Enter. Special characters (like Ctrl+C) are handled immediately.

// So the input system needs to check what is the pty master that is receiving the input 
// when the system has more than one virtual terminal opened at the same time.

// The master side talks to your emulator (Virtual terminal), 
// while the slave side talks to the kernel.

#ifndef ____PTY_H
#define ____PTY_H  1


// Lets manage all the pty devices.
// #todo: Maybe the best place for this is in ptmx.
struct tty_info_d 
{
    int initialized;

    // Pointer to the first element of the list.
    struct tty_d *pty_first;
    // Number of element in the list.
    unsigned int number_of_ptys;
};
extern struct pty_info_d *TTYInfo;


// Exporting
extern struct tty_d *legacy_pty_master;
extern struct tty_d *legacy_pty_slave;

// ====================================================
struct tty_d *get_legacy_pty_master(void);
struct tty_d *get_legacy_pty_slave(void);

int tty_initialize_legacy_pty(void);

#endif    



