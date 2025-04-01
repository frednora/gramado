// console.h
// Created by Fred Nora.

#ifndef  __CONSOLE_H
#define __CONSOLE_H    1

//
// Modes
//

// There is no text mode in Gramado.

// The kernel can print string into the display device.
#define VC_MODE_KERNEL_VERBOSER  1000
// The kernel can print string only when reached the kernel panic.
#define VC_MODE_KERNEL_KP        2000
// The GUI application can paint into the display device.
#define VC_MODE_USER_PAINTER     3000


// Main consoles.
extern struct tty_d  *console0_tty;
extern struct tty_d  *console1_tty;
extern struct tty_d  *console2_tty;
extern struct tty_d  *console3_tty;

// Enumerating them.
#define CONSOLE0  0
#define CONSOLE1  1
#define CONSOLE2  2
#define CONSOLE3  3
// Based on their colors.
#define DEFAULT_CONSOLE  CONSOLE0
#define AUX_CONSOLE      CONSOLE1
#define WARNING_CONSOLE  CONSOLE2
#define DANGER_CONSOLE   CONSOLE3
// Enumerating
#define FIRST_CONSOLE  CONSOLE0
#define LAST_CONSOLE   CONSOLE3


//
// The list of virtual consoles.
//

// Fullscreen kernel console.
// Handmade by the kernel at the initialization.
#define CONSOLETTYS_COUNT_MAX    4
// see: console.c
extern struct tty_d  CONSOLE_TTYS[CONSOLETTYS_COUNT_MAX];

/* Address of a tty structure. */
#define CONSOLE_GET_TTY_ADDRESS(n)  \
    (&CONSOLE_TTYS[n])


// Index
extern int fg_console;
#define DEFAULT_FG_DEFAULT_CONSOLE  DEFAULT_CONSOLE

extern struct tty_struct *redirect;

// Se o console esta atuando como um shell comparando palavras.
// See: console.c and kgwm.c.
extern int ShellFlag;

#define CONSOLE_DEVICE_KEYBOARD  1
#define CONSOLE_DEVICE_SERIAL    2
#define CONSOLE_DEVICE_NETWORK   3

// Control sequence introducer 
// Parameter    - (zero or more characters)
// Intermediate - (zero or more characters)
// Final        - (one character)
// See: https://vt100.net/emu/ctrlseq_dec.html

struct virtual_console_login_d
{
    int used;
    int magic;
    int id;

    // Index in the list CONSOLE[i].
    // See: tty.h
    int virtual_console_index;

    // 0 = 'text'
    // 1 = 'x-server'
    int mode;
    
    struct cgroup_d  *cgroup;

    struct usession_d *usession;
    struct user_info_d *user_info;
};

//
// == prototypes =================
//

void 
console_banner(
    const char *product_string, 
    const char *build_string, 
    unsigned long banner_flags );

void 
console_interrupt(
    int target_thread, 
    int device_type, 
    int data );

void console_set_current_virtual_console(int console_number);
int console_get_current_virtual_console(void);
void jobcontrol_switch_console(int n);

//
// cursor
//

void set_up_cursor ( unsigned long x, unsigned long y );
unsigned long get_cursor_x (void);
unsigned long get_cursor_y (void);

unsigned long get_bg_color(void);
unsigned long get_fg_color(void);


// Scroll
void console_scroll (int console_number);

//
// Char
//

void console_outbyte(int c, int console_number);
void console_outbyte2(int c, int console_number);

void console_echo(int c, int console_number);


void console_putchar ( int c, int console_number );
void console_print_indent(int indent, int console_number);
int consoleInputChar( int c );

//
// Prompt and compare.
//

void consolePrompt (void);

void csi_P (int nr, int console_number);
void csi_at (int nr, int console_number);

ssize_t 
__console_write ( 
    int console_number, 
    const void *buf, 
    size_t count );

void __local_delete_char(int console_number);
void __local_delete_line(int console_number);

void 
__local_gotoxy ( 
    int new_x, 
    int new_y, 
    int console_number );
    
void __local_insert_char (int console_number);
void __local_insert_line (int console_number);
void __local_save_cur (int console_number);
void __local_restore_cur (int console_number);
void __respond (int console_number);

ssize_t 
console_read ( 
    int console_number, 
    const void *buf, 
    size_t count );

ssize_t 
console_write ( 
    int console_number, 
    const void *buf, 
    size_t count );

ssize_t console_write_string(int console_number, const char *string);

int 
console_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg );

int 
console_clear_imp (
    unsigned int bg_color, 
    unsigned int fg_color, 
    int console_number );

int console_clear(void);

int VirtualConsole_early_initialization(void);

// Device driver initialization.
// Virtual console
void 
DDINIT_console(
    int n, 
    unsigned int bg_color, 
    unsigned int fg_color );

#endif    

















