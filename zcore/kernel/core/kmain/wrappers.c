// wrappers.c
// Userful wrappers functions.
// See: gramk/ for overall wrapper functions.
// Created by Fred Nora.

#include <kernel.h>


// IPC stuff
int 
cali_post( 
    tid_t sender_tid,
    tid_t receiver_tid,
    struct msg_d *message )
{
// #todo: Not tested yet.
// #todo: Explain this routine.

    if (sender_tid<0){
        return -1;
    }
    if (receiver_tid<0){
        return -1;
    }
    if ((void*) message == NULL){
        return -1;
    }
    message->sender_tid   = (tid_t) sender_tid;
    message->receiver_tid = (tid_t) receiver_tid;
// Post
    return (int) ipc_post_message_to_tid( 
                     (tid_t) sender_tid,
                     (tid_t) receiver_tid,
                     (int) message->msg,
                     (unsigned long) message->long1,
                     (unsigned long) message->long2 );
}

void cali_reboot(void)
{
    keReboot();
}

void cali_shutdown(int how)
{
    //hal_shutdown();
    panic("cali_shutdown: #todo\n");
}

void cali_die(void)
{
    keDie();
}

void cali_spawn_thread_by_tid(tid_t tid)
{
    if (tid<0 || tid>=THREAD_COUNT_MAX)
        return;
    psSpawnThreadByTID(tid);
}

// Usado pelo malloc em ring3.
void *cali_alloc_shared_ring3_pages(pid_t pid, int number_of_bytes)
{
    int number_of_pages=0;

// #todo
// pid premission

// #todo
// Check max limit

    if ( number_of_bytes < 0 )
        number_of_bytes = 4096;

    if ( number_of_bytes <= 4096 ){
        return (void *) allocPages(1);
    }

// Alinhando para cima.
    number_of_pages = (int) ((number_of_bytes/4096) + 1);

    return (void *) allocPages(number_of_pages);
}

int cali_get_current_runlevel(void)
{
    return (int) current_runlevel;
}

unsigned long cali_get_memory_size_mb(void)
{
    unsigned long __mm_size_mb = 
        (unsigned long) (memorysizeTotal/0x400);

    return (unsigned long) __mm_size_mb;
}

unsigned long cali_get_system_metrics(int index)
{
    if (index<0){
        return 0;
    }
    return (unsigned long) doGetSystemMetrics ( (int) index );
}

// REAL (coloca a thread em standby para executar pela primeira vez.)
// MOVEMENT 1 (Initialized --> Standby).
int cali_start_thread(struct thread_d *thread)
{

// Validation
    if ((void*) thread == NULL)
        goto fail;
    if (thread->used != TRUE)
        goto fail;
    if (thread->magic != 1234)
        goto fail;

    SelectForExecution((struct thread_d *) thread);
    return 0;
fail:
    return (int) (-1);
}

// 34 - Setup cursor for the current virtual console.
// See: core/system.c
// IN: x,y
// #todo: Essa rotina dever pertencer ao user/
void cali_set_cursor( unsigned long x, unsigned long y )
{

// #todo
// Maybe check some limits.

    set_up_cursor ( 
        (unsigned long) x, 
        (unsigned long) y );
}

// As estruturas de console sao estruturas de tty,
// mas sao um array de estruturas, nao precisa de malloc,
// por isso podem ser chamadas nesse momento.
// #test
// We need to test it better.
// see:
// dev/chardev/tty/console.c
// crt/kstdio.c
void zero_initialize_virtual_consoles(void)
{
    int status = -1;
// The early initialization of the virtual consoles,
// it will happen again in kstdio.c if it fails here.
    status = (int) VirtualConsole_early_initialization();
    if (status < 0)
        x_panic("zero_initialize_virtual_consoles");
}

// See: 
// dev/chardev/display/bldisp/bldisp.c
void zero_initialize_video(void)
{
    //#breakpoint: BLACK ON WHITE.
    //ok, funcionou na maq real no modo jail, provavelmente 320x200.
    //for (i=0; i< 320*25; i++){ fb[i] = 0; };
    //while(1){asm("hlt");};

    Video_initialize();
    //PROGRESS("zero_initialize_video: Initialized\n");
}

// see:
// gre/bg.c
void zero_initialize_background(void)
{
    displayInitializeBackground(COLOR_KERNEL_BACKGROUND,TRUE);
    //PROGRESS("zero_initialize_background: ok\n");
}

// Setup Default kernel font.
// ROM BIOS 8x8 font.
// see: gre/font.c
void zero_initialize_default_kernel_font(void)
{
    font_initialize();
}

// =================================
// Console:
// We have a virtual console and we can use the printk.
// This is the first message in the screen.
// see: tty/console.c
void zero_show_banner(void)
{
// Called by keInitialize() in ke.c.

    char product_string[256];
    char build_string[256];
    size_t size=0;

    memset(product_string,0,256);
    memset(build_string,0,256);

// product string
    ksprintf(product_string,PRODUCT_NAME);
    size = sizeof(PRODUCT_NAME);
    if (size >= 256)
        return;
    product_string[size+1]=0;

// build string
    ksprintf(build_string,BUILD_STRING);
    size = sizeof(BUILD_STRING);
    if (size >= 256)
        return;
    build_string[size+1]=0;
    
// Crear screen and print version string.
    PROGRESS("zero_show_banner:\n");
    console_banner( product_string, build_string, 0 );
}

