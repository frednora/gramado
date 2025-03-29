// tty.h
// Created by Fred Nora.

#ifndef ____TTY_H
#define ____TTY_H  1

#define TTY_MAGIC  1234
#define TTY_BUF_SIZE  1024

#define TTYCHARS_COUNT_MAX  25    //80 
#define TTYLINES_COUNT_MAX  25    //25 

//TTY_DRIVER_TYPE_SYSTEM
//TTY_DRIVER_TYPE_CONSOLE
//TTY_DRIVER_TYPE_SERIAL
//TTY_DRIVER_TYPE_PTY

// type
#define TTY_TYPE_UNDEFINED  0
#define TTY_TYPE_CONSOLE    1000  //(Kernel Console)
#define TTY_TYPE_PTY        2000  //(Virtual terminal)
#define TTY_TYPE_SERIAL     3000 
// ...

// subtype
#define TTY_SUBTYPE_UNDEFINED  0
#define TTY_SUBTYPE_CONSOLE_MASTER  100
#define TTY_SUBTYPE_CONSOLE_SLAVE   200
#define TTY_SUBTYPE_PTY_MASTER      300
#define TTY_SUBTYPE_PTY_SLAVE       400
// ...

//These bits are used in the flags field of the tty structure.
#define TTY_THROTTLED         0	/* Call unthrottle() at threshold min */
#define TTY_IO_ERROR          1	/* Cause an I/O error (may be no ldisc too) */
#define TTY_OTHER_CLOSED      2	/* Other side (if any) has closed */
#define TTY_EXCLUSIVE         3	/* Exclusive open mode */
#define TTY_DEBUG             4	/* Debugging */
#define TTY_DO_WRITE_WAKEUP   5	/* Call write_wakeup after queuing new */
#define TTY_PUSH              6	/* n_tty private */
#define TTY_CLOSING           7	/* ->close() in progress */
#define TTY_LDISC             9	/* Line discipline attached */
#define TTY_LDISC_CHANGING   10	/* Line discipline changing */
#define TTY_LDISC_OPEN       11	/* Line discipline is open */
#define TTY_HW_COOK_OUT      14	/* Hardware can do output cooking */
#define TTY_HW_COOK_IN       15	/* Hardware can do input cooking */
#define TTY_PTY_LOCK         16	/* pty private */
#define TTY_NO_WRITE_SPLIT   17	/* Preserve write boundaries to driver */
#define TTY_HUPPED           18	/* Post driver->hangup() */
#define TTY_FLUSHING         19	/* Flushing to ldisc in progress */
#define TTY_FLUSHPENDING     20	/* Queued buffer flush pending */

// Chars
// Some special chars from termios
// used in the tty.

#define START_CHAR(tty)  ((tty)->termios.c_cc[VSTART])
#define STOP_CHAR(tty)   ((tty)->termios.c_cc[VSTOP])
#define EOF_CHAR(tty)    ((tty)->termios.c_cc[VEOF])
#define INTR_CHAR(tty)   ((tty)->termios.c_cc[VINTR])
#define ERASE_CHAR(tty)  ((tty)->termios.c_cc[VERASE])

// Flags
// Some flags from termios
// used in the tty.

#define _L_FLAG(tty,f)  ((tty)->termios.c_lflag & f)
#define _I_FLAG(tty,f)  ((tty)->termios.c_iflag & f)
#define _O_FLAG(tty,f)  ((tty)->termios.c_oflag & f)

#define L_CANON(tty)    _L_FLAG((tty),ICANON)
#define L_ISIG(tty)     _L_FLAG((tty),ISIG)
#define L_ECHO(tty)     _L_FLAG((tty),ECHO)
#define L_ECHOE(tty)    _L_FLAG((tty),ECHOE)
#define L_ECHOK(tty)    _L_FLAG((tty),ECHOK)
#define L_ECHOCTL(tty)  _L_FLAG((tty),ECHOCTL)
#define L_ECHOKE(tty)   _L_FLAG((tty),ECHOKE)

#define I_UCLC(tty)  _I_FLAG((tty),IUCLC)
#define I_NLCR(tty)  _I_FLAG((tty),INLCR)
#define I_CRNL(tty)  _I_FLAG((tty),ICRNL)
#define I_NOCR(tty)  _I_FLAG((tty),IGNCR)

#define O_POST(tty)   _O_FLAG((tty),OPOST)
#define O_NLCR(tty)   _O_FLAG((tty),ONLCR)
#define O_CRNL(tty)   _O_FLAG((tty),OCRNL)
#define O_NLRET(tty)  _O_FLAG((tty),ONLRET)
#define O_LCUC(tty)   _O_FLAG((tty),OLCUC)


/*
struct ttybuffer_d
{
    int used;
    int magic;
// #importante:
// Usaremos um stream sem  nome para gerenciar a 
// área de memória que a tty precisa como buffer.
    file *stream;
    struct ttybuffer_d *next;
};
struct ttybuffer_d *CurrentTTYBUFFER;
*/


struct tty_line_d
{
    //int index;
    char CHARS[80];
    char ATTRIBUTES[80];  //Isso poderia ser unsigned long.	
// Início e fim da string dentro da linha. O resto é espaço.
    int left;
    int right;
//Posição do cursor dentro da linha.
    int pos;
};


// Where the bytes are stored.
struct tty_queue 
{
// Save the bytes here.
    char buf[TTY_BUF_SIZE];
    int buffer_size;

// Quantidade de bytes dentro do buffer.
// Se colocar mais isso aumenta, se retirarmos, isso diminui.
    int cnt;

// Offsets
    unsigned long head;  // In
    unsigned long tail;  // Out

// Contador de line feed.
    //int lf_counter;

// Vamos acordar as threads que estão nessa lista.
// Eh uma lista encadeada de 
// threads esperando nessa fila.
    struct thread_d *thread_list;
};

/*
// tty_d:
// This is tty structure.
// The tty structure need to have a pointer to
// a buffer and a pointer to a thread. These is the where
// we're gonna send the data.
// uma estrutura de tty deve estar associado a
// uma janela de terminal virtual.
*/

#define TTY_NAME_SIZE  64
#define CHARSET_NAME_SIZE  64

struct tty_d
{
    object_type_t objectType;
    object_class_t objectClass;
    int used;
    int magic;
// Where?
    int index;

// File pointer:
// To setup the device.
    file *fp;

// Name support
    char name[TTY_NAME_SIZE];
    size_t Name_len;

// type
//#define TTY_TYPE_CONSOLE    1000 (Kernel Console)
//#define TTY_TYPE_PTY        2000 (Virtual terminal)
//#define TTY_TYPE_SERIAL     3000 

// ...
// type of tty
    short type;

// subtype
//#define TTY_SUBTYPE_PTY_MASTER   100
//#define TTY_SUBTYPE_PTY_SLAVE    200
// ...
// subtype of tty
    short subtype;

// TTY mode.
    //unsigned short tty_mode;

// Ownership
    tid_t __owner_tid;

    int initialized;

// The TTY is blocked by the system,
// and we can't read or write on it.
// The TTY can't receive or send data.
// If a process try to write in a blocked TTY it will be blocked.
    int is_blocked;

//
// == Security ============================================
//

// What is the user logged in this terminal?
// see: user.h
    struct user_info_d *user_info;
    struct usession_d  *user_session;
// cgroup
    struct cgroup_d  *cgroup;
// ===================================================

//
// == Storage ========
//

// The buffer. The box.

//
// Buffers.
//

// If the buffer are used or not.
// options: TRUE, FALSE.
    int nobuffers;

// Canonical. (cooked mode)
// Applications programs reading from the terminal 
// receive entire lines, after line editing has been 
// completed by the user pressing return.

    struct tty_queue raw_queue;        // Raw input buffer.
    struct tty_queue canonical_queue;  // Canonical buffer.

    struct tty_queue output_queue;     // Output buffer.

//
// == Synchronization ========
//

// Flag para sincronizaçao de leitura e escrita de eventos.
    int new_event;

// Synch and job control.
// This way the TTY driver can send the input to the forground process.

//
// == transmition ========
//

// linked socket?
    struct tty_d *link;
    int is_linked;

//
// == Device info ==================
//

// Device.
    //struct device_d *device;
    struct ttydrv_d *driver;
// i don't like this
// line discipline
// (Virtual functions).
    struct ttyldisc_d *ldisc;

//
// ==  properties ========================
//

// Qual eh o modo de operacao do console virtual.
// The kernel can print string into the display device.
//#define VC_MODE_KERNEL_VERBOSER  1000
// The kernel can print string only when reached the kernel panic.
//#define VC_MODE_KERNEL_KP        2000
// The GUI application can paint into the display device.
//#define VC_MODE_USER_PAINTER     3000

    int vc_mode;


// tty flags.
    unsigned long flags;

//
// == Actions ==============
//

// Qual terminal virtual esta usando essa tty.
    pid_t virtual_terminal_pid;
    //tid_t virtual_terminal_tid;

//------------------------------

// #test
// The winsize structure used in ioctl().
// see: kioctl.h
    struct winsize_d winsize;

//------------------------------

// see: ktermios.h
    struct termios_d  termios;

//------------------------------

//
// Echo support
//

    // Maybe we can create a structure for echo support. 
    // tty->Echo.cursor_x;

//
// Cursor support.
//

// Cursor position in bytes.
    unsigned long cursor_x;
    unsigned long cursor_y;
// Margins in bytes.
// The cursor respect these limits.
    unsigned long cursor_left;    // Left margin. In chars.
    unsigned long cursor_top;     // Top margin. In lines.
    unsigned long cursor_right;   // Right margin. In chars.
    unsigned long cursor_bottom;  // Bottom margin. In lines.
// Cursor dimentions in pixel.
    unsigned long cursor_width_in_pixels;
    unsigned long cursor_height_in_pixels;

//--------------------------------

// Charset support.
// See: kbdmap.c, kbdmap.h
// #todo
// Maybe we can create a structure for that.

// see:
// https://man7.org/linux/man-pages/man7/charsets.7.html

// id do charset.
    int charset_id;

// Language id.
// x = 'en-br'
// English BR for abnt2.
    int charset_lang_id;

    void *charset_lowercase;    // Lowercase
    void *charset_uppercase;    // Uppercase (Shift + key)
    void *charset_controlcase;  // (control + key)
    size_t charset_size;

// Charset name
    char charset_name[CHARSET_NAME_SIZE];
    size_t charset_name_size;

//--------------------------------

// Font support
// see: font.c, char.c
    void *font_address;

// Char support.
// bg and fg colors.
    unsigned int bg_color;
    unsigned int fg_color;

// Is it a fullscreen console?
    int fullscreen_flag;

// Connections:
// pty associa a tty 'to' com a tty 'from'.
// master/slave.

// Navigation
    struct tty_d  *next;
};

//
// == prototypes ===============================================
//

int tty_copy_raw_buffer( struct tty_d *tty_to, struct tty_d *tty_from );

int 
__tty_read ( 
    struct tty_d *tty, 
    char *buffer, 
    int nr );

int 
__tty_write ( 
    struct tty_d *tty, 
    char *buffer, 
    int nr );

int 
tty_read ( 
    int fd, 
    char *buffer, 
    int n );

int 
tty_write ( 
    int fd, 
    char *buffer, 
    int n );

int tty_reset_termios (struct tty_d *tty);

struct tty_d *file_tty (file *f);
int tty_delete ( struct tty_d *tty );
void tty_flush( struct tty_d *tty );
void tty_start (struct tty_d *tty);
void tty_stop (struct tty_d *tty);

int 
tty_gets ( 
    struct tty_d *tty, 
    struct termios_d *termiosp );

int 
tty_sets ( 
    struct tty_d *tty, 
    int options, 
    struct termios_d *termiosp );

int 
tty_ioctl ( 
    int fd, 
    unsigned long request, 
    unsigned long arg );

//
// $
// CREATE
//

struct tty_d *tty_create(short type, short subtype);

#endif    

