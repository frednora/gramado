
// gramado.h
// gramado system calls.
// #bugbug: It is not properly stub routines,
// used by shared libraries. It's is o.s. dependent routines.
// We need to change the name of this document, and change
// the name in ALL apllications.
// 2022 - Fred Nora


#ifndef __GRAMADO_CALL
#define __GRAMADO_CALL    1

#include <types.h>
#include <sys/types.h>
#include <ctype.h>
#include <stddef.h>
#include <pthread.h>


// Modes.
#define GRAMADO_JAIL        0x00
#define GRAMADO_P1          0x01
#define GRAMADO_HOME        0x02
#define GRAMADO_P2          0x03
#define GRAMADO_CASTLE      0x04
#define GRAMADO_CALIFORNIA  0x05
// ...

// Commands for synchronization.
#define SYNC_REQUEST_SET_ACTION  1
#define SYNC_REQUEST_GET_ACTION  2
#define SYNC_REQUEST_RESET_WR    216  //Now we can write.
#define SYNC_REQUEST_RESET_RD    217  //Now we can read.

// Action flags.
// usado em 'file sync'
#define ACTION_NULL       0
#define ACTION_REQUEST    1000
#define ACTION_REPLY      2000
#define ACTION_EVENT      3000
#define ACTION_ERROR      4000


// #define RTL_ABS(X)    (((X) < 0) ? (-(X)) : (X))



// #todo: create cool defines.
//#define SC80_NR    0x80
//#define SC81_NR    0x81
//#define SC82_NR    0x82


//
// == prototypes =============================================
//

// See: rtl.c
void rtl_elegant_exit_on_fail(void);

// =====================================================

// system call
// 0x80
void *gramado_system_call ( 
    unsigned long a,  //Service number. 
    unsigned long b, 
    unsigned long c, 
    unsigned long d );


void *sc80 ( 
    unsigned long a,  //Service number. 
    unsigned long b, 
    unsigned long c, 
    unsigned long d );

void *sc81 ( 
    unsigned long a,  //Service number. 
    unsigned long b, 
    unsigned long c, 
    unsigned long d );

void *sc82 ( 
    unsigned long a,  //Service number. 
    unsigned long b, 
    unsigned long c, 
    unsigned long d );

void *sc83 ( 
    unsigned long a,  //Service number. 
    unsigned long b, 
    unsigned long c, 
    unsigned long d );

// =====================================================


void *rtl_shm_get_2mb_surface(void);

// input mode
int rtl_get_input_mode(void);
void rtl_set_input_mode(int mode);


// ========================
// global sync
void rtl_set_global_sync(int sync_id, int request, int data);
int rtl_get_global_sync(int sync_id, int request);

// ========================
// file sync
void rtl_set_file_sync(int fd, int request, int data);
int rtl_get_file_sync(int fd, int request);

//==========================================

unsigned char  rtl_to_uchar  (char ch);
unsigned short rtl_to_ushort (short ch);
unsigned int   rtl_to_uint   (int ch);
unsigned long  rtl_to_ulong  (long ch);

//==========================================


//
// == events ===========================================
//

#define MSG_QUEUE_MAX  64

// System messages in the thread's event queue.

#define RTL_WAIT_FOR_EVENTS  1
#define RTL_POOL_FOR_EVENTS  2


//  The buffer for the event elements.
//unsigned long RTLEventBuffer[32];
extern unsigned long RTLEventBuffer[32];


// Get an event from the thread's event queue.
// That old 'get system message'
// Using a buffer
int xxxScanApplicationQueue(void);
int rtl_get_event(void);

int xxxScanApplicationQueue2(int index, int restart);
int rtl_get_event2(int index, int restart);

// ===========================================================


// ===========================================================
//  rtl event

// #todo
// Not tested yet.

struct rtl_event_d
{
    // #bugbug
    // The system call do not know the limit of arguments
    // in this structure.
    // maybe we need a pad for a 32 longs array,
    
    void *window;  // opaque
    int msg;
    unsigned long long1;
    unsigned long long2;
    
    unsigned long long3;
    unsigned long long4;
    unsigned long long5;
    unsigned long long6;

    // 32-8
    //unsigned long pad[24];
};

struct rtl_event_d *rtl_next_event(void);

// ===========================================================


// Only for a given valid range of keys?
int rtl_get_key_state(int vk);

// Critical section
// Loop in a global spinlock.
void rtl_enter_critical_section(void);
void rtl_exit_critical_section(void);

// Heap info
void rtl_show_heap_info(void);

//
// Create file or directory.
//

int rtl_create_empty_file(char *file_name);
int rtl_create_empty_directory(char *dir_name);

//
// == process ===============================
//

void *rtl_create_process( const char *file_name );
int rtl_start_process( void *process );


//
// == thread ===============================
//

void *rtl_create_thread ( 
    unsigned long init_rip, 
    unsigned long init_stack, 
    char *name );

void rtl_start_thread (void *thread);

// ===================================================

//vamos escrever em uma janela indefinida. NULL.
//provavelmente a janela principal.
int 
rtl_draw_text ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long color, 
    char *string );

char *rtl_get_next_token_in_a_string(
    char *buf,           // Input string pointer. 
    char *w,             // Output token string pointer.
    size_t buffer_size,  // The buffer size.
    size_t *size );      // Output to return an int value. The counter.

void rtl_show_backbuffer (void);

unsigned long rtl_get_system_message(unsigned long message_buffer);
unsigned long 
rtl_post_system_message(
    int dest_tid, 
    unsigned long message_buffer );

void 
rtl_post_to_tid(
    int tid, 
    int msg_code, 
    unsigned long long1, 
    unsigned long long2 );

unsigned long rtl_get_system_message2(unsigned long message_buffer,int index,int restart);

unsigned long rtl_get_system_metrics (int index);
int rtl_is_qemu(void);


unsigned long rtl_jiffies(void);
unsigned long rtl_memory_size_in_kb(void);

pid_t rtl_current_process(void);
int rtl_current_thread(void);

pthread_t pthread_self(void);

// ms
// tempo total em ms.
// usado para calcular o tempo de execuçao de uma funcao.
unsigned long rtl_get_progress_time(void);


// #bugbug
// Not tested yet.
int 
rtl_copy_text ( 
    unsigned long src, 
    unsigned long dest, 
    int width, 
    int height );


int rtl_reboot(void);

int rtl_sleep_if_socket_is_empty(int fd);

void rtl_test_pipe (void);

size_t rtl_path_count (unsigned char *path);

int 
rtl_load_path ( 
    char *path, 
    unsigned long buffer, 
    unsigned long buffer_len );
    
ssize_t rtl_console_beep(void);

void rtl_broken_vessels(void);

// Clone and execute a process.
// #todo: maybe use 'const char*'
int rtl_clone_and_execute( char *name );

int rtl_spawn_process( const char *path );

// get current thread
// set foreground thread.
int rtl_focus_on_this_thread(void);
int rtl_focus_on_me(void);

void rtl_yield(void);

void rtl_sleep_until(unsigned long ms);
void rtl_sleep(unsigned long ms);

// The whole screen is dirty.
// It can be flushed into the framebuffer.
void rtl_invalidate_screen(void);

// Use the kernel allocator for ring 3 shared memory.
void *shAlloc( size_t size_in_bytes );

int rtl_execute_cmdline( char *cmdline );

int rtl_swap32(int *x, int *y);
int rtl_swap64(long *x, long *y);

long rtl_round_up(long num_to_round, long multiple);


#endif


