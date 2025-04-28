
// ktermios.h
// ring0, kernel base.
// Created by Fred Nora.

// #todo
// kernel side termios support.
//provavelmente suporte ao gerenciamento de terminais.
//http://man7.org/linux/man-pages/man3/termios.3.html
//http://kirste.userpage.fu-berlin.de/chemnet/use/info/libc/libc_12.html
//#obs:
//coisas pra fazer com terminal:
//turn off input echoing
//set serial line characteristics such as line speed and flow control
// change which characters are used for end-of-file,
//command-line editing,
//sending signals (ou MSG_)
// control functions.

#ifndef __KTERMIOS_H
#define __KTERMIOS_H    1

// Types.
// Terminal input output data types.
typedef unsigned char   cc_t;      // Terminal special characters.
typedef unsigned int    speed_t;   // Terminal baud rates.
typedef unsigned short  tcflag_t;  // Terminal modes.
// ...

/*
tcflag_t c_iflag
    A bit mask specifying flags for input modes; 
tcflag_t c_oflag
    A bit mask specifying flags for output modes; 
tcflag_t c_cflag
    A bit mask specifying flags for control modes;  
tcflag_t c_lflag
    A bit mask specifying flags for local modes; 
cc_t c_cc[NCCS]
    An array specifying which characters are associated with various control functions; 
*/

/*
Data Type: tcflag_t
    This is an unsigned integer type used to represent 
    the various bit masks for terminal flags. 
Data Type: cc_t
    This is an unsigned integer type used to represent characters 
    associated with various terminal control functions. 
Macro: int NCCS
    The value of this macro is the number of elements in the c_cc array. 
*/

// Indices into c_cc array.  
// Default values in parentheses. POSIX Table 7-5. 

#define VEOF         0	/* cc_c[VEOF] = EOF char (^D) */
#define VEOL         1	/* cc_c[VEOL] = EOL char (undef) */
#define VERASE       2	/* cc_c[VERASE] = ERASE char (^H) */
#define VINTR        3	/* cc_c[VINTR] = INTR char (DEL) */
#define VKILL        4	/* cc_c[VKILL] = KILL char (^U) */
#define VMIN         5	/* cc_c[VMIN] = MIN value for timer */
#define VQUIT        6	/* cc_c[VQUIT] = QUIT char (^\) */
#define VTIME        7	/* cc_c[VTIME] = TIME value for timer */
#define VSUSP        8	/* cc_c[VSUSP] = SUSP (^Z, ignored) */
#define VSTART       9	/* cc_c[VSTART] = START char (^S) */
#define VSTOP       10	/* cc_c[VSTOP] = STOP char (^Q) */
#define VERASEWORD  14	/* cc_c[VERASEWORD] = ERASEWORD char (^W) */

// ================================================

//
// termio
//

#define NCC  8

// #todo: struct termio_d
struct termio 
{
    unsigned short c_iflag;  // input mode flags 
    unsigned short c_oflag;  // output mode flags 

    unsigned short c_cflag;  // control mode flags 
    unsigned short c_lflag;  // local mode flags 

    unsigned char c_line;     // line discipline 

    unsigned char c_cc[NCC];  // control characters
};

// ================================================

//
// termios
//

// See:
// https://www.mkssoftware.com/docs/man5/struct_termios.5.asp
/* Primary terminal control structure. POSIX Table 7-1. */

// Size of cc_c array, some extra space for extensions.
#define NCCS  20

//struct termios 
struct termios_d
{
    tcflag_t c_iflag;      // input modes 
    tcflag_t c_oflag;      // output modes 

// control modes
// Describes the hardware control of the terminal.
    tcflag_t c_cflag; 
// local modes 
// Controls various terminal functions.
    tcflag_t c_lflag;

    //unsigned char c_line;
    cc_t c_cc[NCCS];       // control characters 

    speed_t  c_ispeed;     // input speed 
    speed_t  c_ospeed;     // output speed 
};

// ================================================


/* Values for termios c_iflag bit map.  POSIX Table 7-2. */
#define BRKINT    0x0001  /* signal interrupt on break */
#define ICRNL     0x0002  /* map CR to NL on input */
#define IGNBRK    0x0004  /* ignore break */
#define IGNCR     0x0008  /* ignore CR */
#define IGNPAR    0x0010  /* ignore characters with parity errors */
#define INLCR     0x0020  /* map NL to CR on input */
#define INPCK     0x0040  /* enable input parity check */
#define ISTRIP    0x0080  /* mask off 8th bit */
#define IXOFF     0x0100  /* enable start/stop input control */
#define IXON      0x0200  /* enable start/stop output control */
#define PARMRK    0x0400  /* mark parity errors in the input queue */


/* Values for termios c_oflag bit map.  POSIX Sec. 7.1.2.3. */
#define  OPOST  0x0001  /* perform output processing */
// #todo: more flags.
//...


/* Values for termios c_cflag bit map.  POSIX Table 7-3. */
#define  CLOCAL  0x0001  /* ignore modem status lines */
#define  CREAD   0x0002  /* enable receiver */
#define  CSIZE   0x000C  /* number of bits per character */
#define  CS5     0x0000  /* if CSIZE is CS5, characters are 5 bits */
#define  CS6     0x0004  /* if CSIZE is CS6, characters are 6 bits */
#define  CS7     0x0008  /* if CSIZE is CS7, characters are 7 bits */
#define  CS8     0x000C  /* if CSIZE is CS8, characters are 8 bits */
#define  CSTOPB  0x0010  /* send 2 stop bits if set, else 1 */
#define  HUPCL   0x0020  /* hang up on last close */
#define  PARENB  0x0040  /* enable parity on output */
#define  PARODD  0x0080  /* use odd parity if set, else even */

// Local mode flags.
/* Values for termios c_lflag bit map.  POSIX Table 7-4. */
#define  ECHO      0x0001  /* enable echoing of input characters */
#define  ECHOE     0x0002  /* echo ERASE as backspace */
#define  ECHOK     0x0004  /* echo KILL */
#define  ECHONL    0x0008  /* echo NL */
#define  ICANON    0x0010  /* canonical input (erase and kill enabled) */
#define  IEXTEN    0x0020  /* enable extended functions */
#define  ISIG      0x0040  /* enable signals */
#define  NOFLSH    0x0080  /* disable flush after interrupt or quit */
#define  TOSTOP    0x0100  /* send SIGTTOU (job control, not implemented*/


/* 
 * You can't even generate this character with 'normal' keyboards.
 * But some language specific keyboards can generate 0xFF. It seems 
 * that all 256 are used, so cc_t should be a short ...
 * (unsigned char)
 */

#define _POSIX_VDISABLE  (cc_t)0xFF


/* Values for the baud rate settings.  POSIX Table 7-6. */
#define  B0      0x0000  /* hang up the line */
#define  B50     0x1000  /* 50 baud */
#define  B75     0x2000  /* 75 baud */
#define  B110    0x3000  /* 110 baud */
#define  B134    0x4000  /* 134.5 baud */
#define  B150    0x5000  /* 150 baud */
#define  B200    0x6000  /* 200 baud */
#define  B300    0x7000  /* 300 baud */
#define  B600    0x8000  /* 600 baud */
#define  B1200   0x9000  /* 1200 baud */
#define  B1800   0xA000  /* 1800 baud */
#define  B2400   0xB000  /* 2400 baud */
#define  B4800   0xC000  /* 4800 baud */
#define  B9600   0xD000  /* 9600 baud */
#define  B19200  0xE000  /* 19200 baud */
#define  B38400  0xF000  /* 38400 baud */


/* Optional actions for tcsetattr().  POSIX Sec. 7.2.1.2. */
#define TCSANOW    1  /* changes take effect immediately */
#define TCSADRAIN  2  /* changes take effect after output is done */
#define TCSAFLUSH  3  /* wait for output to finish and flush input */

//#if defined(_NETBSD_SOURCE)
//#define TCSASOFT  0x10  /* flag - don't alter h.w. state */
//#endif

/* 
 * Queue_selector values for tcflush(). 
 * POSIX Sec. 7.2.2.2. 
 */

// Flush accumulated input data.
// Flushes data received, but not read.
#define TCIFLUSH  1
// Flush accumulated output data.
// Flushes data written but not transmitted.
#define TCOFLUSH  2
// Flush accumulated input and output data.
// Flushes both data received but not read, and 
// data written but not transmitted.
#define TCIOFLUSH  3


/* Action values for tcflow().  POSIX Sec. 7.2.2.2. */
#define TCOOFF  1  /* suspend output */
#define TCOON   2  /* restart suspended output */
#define TCIOFF  3  /* transmit a STOP character on the line */
#define TCION   4  /* transmit a START character on the line */


/* modem lines */
/*
#define TIOCM_LE	0x001
#define TIOCM_DTR	0x002
#define TIOCM_RTS	0x004
#define TIOCM_ST	0x008
#define TIOCM_SR	0x010
#define TIOCM_CTS	0x020
#define TIOCM_CAR	0x040
#define TIOCM_RNG	0x080
#define TIOCM_DSR	0x100
#define TIOCM_CD	TIOCM_CAR
#define TIOCM_RI	TIOCM_RNG
#define TIOCM_OUT1	0x2000
#define TIOCM_OUT2	0x4000
#define TIOCM_LOOP	0x8000
*/

#endif    


//
// End.
//

