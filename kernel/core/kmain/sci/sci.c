// sci.c - (Transfiguration)
// (System Call Interface).
// System Service Dispatcher.
// Handlers for the system interrupts.
// #todo: 
// There is another point:
// The 'event delivery' and 'return' support.
// Created by Fred Nora.

#include <kernel.h>

//
// Internal in sci/.
//

// Common
#include "sci.h" 
// Aliases
#include "sci0.h"
#include "sci1.h"
#include "sci2.h"
#include "sci3.h"

static unsigned long __default_syscall_counter=0;


//
// == private functions: prototypes =============
//

static void __servicePutChar(int c, int console_number);
static void __service897(void);
static void __invalidate_surface_rectangle(void);

static void __setup_surface_rectangle(
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height );

// =============================

// __servicePutChar:
// Put char into the fg console.
// Local worker.
static void __servicePutChar(int c, int console_number)
{
    if (c<0)
        return;
// 0~3
    if (console_number < 0){
        return;
    }
    if (console_number > 3){
        return;
    }
    console_putchar( (int) c, console_number );
}

// 897
// Set up and draw the main surface for a thread.
// #test
static void __service897(void)
{
    struct thread_d *myThread; 
    struct rect_d r;
    unsigned int _Color=0;
    int Draw = TRUE;

// Current thread
// This routine only can be called by the 
// init process. Its tid is INIT_TID.
    if ( current_thread < 0 || current_thread >= THREAD_COUNT_MAX ){
        return;
    }
    if (current_thread != INIT_TID){
        return;
    }

    _Color = (unsigned int) (COLOR_GREEN + 0);

// Setup rectangle
    r.left = 0;
    r.top = 0;
    r.width = 24;
    r.height = 24;
    r.dirty = FALSE; 
    r.used = TRUE;
    r.magic = 1234;


//  Thread
    myThread = (struct thread_d *) threadList[current_thread];
    if ((void*) myThread == NULL)
        return;
    if (myThread->used != TRUE)
        return;
    if (myThread->magic != 1234)
        return;

// Setup surface rectangle. 
    myThread->surface_rect = (struct rect_d *) &r;

// Paint
// Invalidate means that the rectangle need to be flushed
// into the framebuffer.
// If the rectangle is dirty, so it needs to be flushed into 
// the framebuffer.
// When we draw a window it needs to be invalidated.

    if (Draw == TRUE){
        // l,t,w,h,color,rop falgs.
        backbuffer_draw_rectangle( 
            r.left, r.top, r.width, r.height, _Color, 0 );
        r.dirty = TRUE;
    }
}

static void __setup_surface_rectangle(
    unsigned long left,
    unsigned long top,
    unsigned long width,
    unsigned long height )
{
    struct thread_d *t;
    struct rect_d *r;

/*
// dc: Clippint
    unsigned long deviceWidth  = (unsigned long) screenGetWidth();
    unsigned long deviceHeight = (unsigned long) screenGetHeight();
    if ( deviceWidth == 0 || deviceHeight == 0 ){
        debug_print ("__setup_surface_rectangle: [PANIC] w h\n");
        panic       ("__setup_surface_rectangle: [PANIC] w h\n");
    }
*/

    if ( current_thread<0 || current_thread >= THREAD_COUNT_MAX ){
        return;
    }

// Thread
    t = (struct thread_d *) threadList[current_thread];
    if ((void*) t == NULL){
        return;
    }
    if (t->magic != 1234){
        return;
    }

// Rect
    r = t->surface_rect;
    if ((void*) r == NULL){
        return;
    }
    if (r->magic != 1234){
        return;
    }

// Rect data
    r->left   = (left   & 0xFFFF);
    r->top    = (top    & 0xFFFF);
    r->width  = (width  & 0xFFFF);
    r->height = (height & 0xFFFF);
    r->dirty = FALSE;
}

// 893 - Invalidate the thread's surface rectangle.
static void __invalidate_surface_rectangle(void)
{
    struct thread_d *t;
    struct rect_d *r;

    if ( current_thread<0 || current_thread >= THREAD_COUNT_MAX ){
        return;
    }

// Thread
    t = (struct thread_d *) threadList[current_thread];
    if ((void*) t == NULL){
        return;
    }
    if (t->magic != 1234){
        return;
    }

// Rect
    r = t->surface_rect;
    if ((void*) r == NULL){
        return;
    }
    if (r->magic != 1234){
        return;
    }

// Rect data
    r->dirty = TRUE;
}

// =====================================================

// This routine was called by the interrupt handler in x64sc.c.
void *sci0 ( 
    unsigned long number, 
    unsigned long arg2, 
    unsigned long arg3, 
    unsigned long arg4 )
{
// Getting requests from ring3 applications via systemcalls.
// :: Services in kernel.

// Thread
    struct thread_d *t;
// Process
    struct process_d *p;
    pid_t current_process = -1;
// Pointer for cgroup
    struct cgroup_d *cg;

    int layer = LAYER_UNDEFINED;

    unsigned long *message_address = (unsigned long *) arg2;
    unsigned long *a2 = (unsigned long*) arg2;
    unsigned long *a3 = (unsigned long*) arg3;
    unsigned long *a4 = (unsigned long*) arg4;
	
	int int_retval = 0;


    //?? #deprecated?
    g_profiler_ints_gde_services++;


// ---------------------------------
// Thread
    if ( current_thread<0 || current_thread >= THREAD_COUNT_MAX )
    { 
        return NULL;
    }
    t = (struct thread_d *) threadList[current_thread];
    if ((void*) t == NULL)
        return NULL;
    if (t->magic != 1234)
        return NULL;
    // Increment stime.
    // see: x64cont.c
    t->transition_counter.to_supervisor++;
    // Antecipando, ja que fica dificil
    // fazer isso na saida por enquanto.
    t->transition_counter.to_user++;
// ---------------------------------


    // #debug
    //debug_print("sc0:\n");
    //printk("sc0:\n");


// Profiling in the process structure.

// Permission

// Process
    current_process = (pid_t) get_current_pid();
    if (current_process<0 || current_process >= PROCESS_COUNT_MAX){
        panic("sci0: current_process\n");
    }
    p = (struct process_d *) processList[current_process];
    if ((void*) p == NULL){
        debug_print("sci0: p\n");
        panic("sci0: p\n");
    }
    if ( p->used != TRUE || p->magic != 1234 ){
        debug_print("sci0: p validation\n");
        panic("sci0: p validation\n");
    }

    layer = p->_layer;

// Feito pra rodar no Gramado OS.
    if (p->personality != PERSONALITY_GRAMADO){
        panic("sci0: Personality\n");
    }

// Counting ...
    p->syscalls_counter++;

// What is the number for SCI_EXIT?
// #todo: Put this in the right spot.
// exit
// Marcamos no processo nossa intenção de fechar.
// Marcamos na thread de controle nossa intenção de fechar.
// #todo: as outras threads do processo.
// see: sched.c
    if (number == SCI_EXIT)
    {
        debug_print("sci0: SCI_EXIT\n");
        //p->exit_in_progress = TRUE;
        
        // Quando o scheduler passar por ela,
        // vai pular ela e marca-la como zombie.
        if ((void*) p->control != NULL)
        {
            if (p->control->magic == 1234)
            {
                p->control->exit_in_progress = TRUE;
                p->exit_in_progress = TRUE;
            }
        }
        return NULL;
    }

// ==== 0 mark ====================================================

    // SCI_NULL
    if (number == 0)
        return NULL;

// 1 (i/o) Essa rotina pode ser usada por 
// um driver em user mode.
// #todo: This operation needs permition.
// #todo: Return value.
// IN: buffer address and lba address.
    if (number == SCI_READ_LBA){
        storage_read_sector( (unsigned long) arg2, (unsigned long) arg3 ); 
        return NULL;
    }

// 2 (i/o) Essa rotina pode ser usada por 
// um driver em user mode.
// #todo: This operation needs permition.
// #todo: Return value.
// IN: buffer address and lba address.
    if (number == SCI_WRITE_LBA){ 
        storage_write_sector( (unsigned long) arg2, (unsigned long) arg3 ); 
        return NULL;
    }

// 3 
// Carregar um arquivo do disco para a memória.
// See: fs/fs.c
// IN: name, flags, mode
    if (number == SCI_READ_FILE)
    {
        //if ( (void*) a2 == NULL )
           //return NULL;
        return (void *) sys_read_file_from_disk ( 
                            (char *) a2, 
                            (int)    arg3, 
                            (mode_t) arg4 ); 
    }

// 4 
// Save file.
// See: fs/fs.c
// IN: name, size in sectors, size in bytes, adress, flag.
    if (number == SCI_WRITE_FILE)
    {
        sys_write_file_to_disk ( 
            (char *)        message_address[0],
            (unsigned long) message_address[1],
            (unsigned long) message_address[2],
            (char *)        message_address[3],
            (char)          message_address[4] );
        return NULL;
    }

// 5
// See: sys.c 
    if (number == SCI_SYS_VSYNC)
    {
        sys_vsync();
        return NULL;
    }

// 6 - Put pixel. 
// Coloca um pixel no backbuffer.
// Isso pode ser usado por um servidor. 
// cor, x, y, rop_flags.
// todo: chamar kgws_backbuffer_putpixel
    if (number == SCI_BUFFER_PUTPIXEL)
    {
        backbuffer_putpixel ( 
            (unsigned long) a2,  // color
            (unsigned long) a3,  // x
            (unsigned long) a4,  // y
            0 );                 // rop_flags
        return NULL;
    }

// 7

// 8 
// #todo: #bugbug: 
// Aqui precisamos de mais parâmetros.
    if (number == SCI_BUFFER_DRAWLINE)
    {
        backbuffer_draw_horizontal_line ( 
            (unsigned long) a2, 
            (unsigned long) a3, 
            (unsigned long) a4, 
            COLOR_WHITE,
            0 );   // rop_flags
        return NULL;
    }

// 9 - Draw a rectangle into the backbuffer.
// This is the esrvice called by the display server.
// Any process can call this service?
// SCI_BUFFER_DRAWRECT
// see: rect.c
// #todo:
// Let create some better services for the display server in ring 0,
// for performance reasons. Maybe inside a kernel module, not inside
// the kernel image.
    if (number == 9)
    {
        // debug_print("sci0: [9]\n");
        // IN: l,t,w,h,color,rop flags.
        backbuffer_draw_rectangle ( 
            (unsigned long) message_address[0], 
            (unsigned long) message_address[1],
            (unsigned long) message_address[2],
            (unsigned long) message_address[3],
            (unsigned int)  message_address[4],
            (unsigned long) message_address[5] );
        return NULL;
    }

// 10 - Refresh rectangle.
// Region?
    if (number == 10)
    {
        // debug_print("sci0: [10]\n");
        // IN: l,t,w,h
        refresh_rectangle ( 
            (unsigned long) message_address[0], 
            (unsigned long) message_address[1],
            (unsigned long) message_address[2],
            (unsigned long) message_address[3] ); 
        return NULL;
    }

// 11: Schedule to flush the backbuffer into the LFB.
    if (number == SCI_REFRESHSCREEN){ 
        invalidate_screen();
        return NULL;
    }

// Reserved.
// Netword: 12,13,14,15

// 16 - open() implementation.
// see: fs.c
// IN: pathname, flags, mode
// OUT: fd
    if (number == SCI_SYS_OPEN)
    {
        debug_print("sci0: SCI_SYS_OPEN\n");
        return (void *) sys_open ( 
                                (const char *) arg2, 
                                (int)          arg3, 
                                (mode_t)       arg4 ); 
    }

// 17 - close() implementation.
// see: fs.c
// IN: fd
    if (number == SCI_SYS_CLOSE)
    {
        debug_print ("sci0: SCI_SYS_CLOSE\n");
        return (void *) sys_close((int) arg2);
    }

// 18 - read() implementation.
// IN: ?
// See: sys.c
    if (number == SCI_SYS_READ)
    {
        return (void *) sys_read ( 
                            (unsigned int) arg2, 
                            (char *)       arg3, 
                            (int)          arg4 ); 
    }

// 19 - write() implementation
// IN: ?
// See: sys.c
    if (number == SCI_SYS_WRITE)
    {
        return (void *) sys_write ( 
                            (unsigned int) arg2, 
                            (char *)       arg3, 
                            (int)          arg4 );  
    }

// == + ==================================================

// Reserved:
// 20~23 - Buffers support 
// 24~28 - Surface support 
// 29 - String stuff
// 30,31,32 - Putpixel?
// 33 - free number.

// 34
// Setup cursor position for the current virtual console.
    if (number == SCI_CALI_SET_CURSOR){
        gramk_set_cursor((unsigned long) arg2, (unsigned long) arg3);
        return NULL;
    }

// 35 - free number
// 36 - #deprecated
// 37 - #deprecated


// 38 - get host name
// see: kunistd.c
    if (number == SCI_GETHOSTNAME){
        return (void *) sys_gethostname((char *) arg2);
    }
// 39 - set host name 
// see: kunistd.c
    if (number == SCI_SETHOSTNAME)
    {
        // #todo: lenght
        // IN: name. lenght.
        return (void *) sys_sethostname((const char *) arg2,0);
    }


// 40 - get user name 
    if (number == SCI_GETUSERNAME){
        return (void *) sys_getusername((char *) arg2);
    }
// 41 - Set user name 
    if (number == SCI_SETUSERNAME){
        return (void *) sys_setusername((const char *) arg2);
    }

// 42 - #deprecated

// 43 - Create an empty file.
// Called by libgws.
// See: fs.c
    if (number == 43){
        return (void *) sys_create_empty_file( (char *) arg2 );
    }

// 44 - Create an empty directory.
// Called by libgws.
// See: fs.c
    if (number == 44){
        return (void *) sys_create_empty_directory( (char *) arg2 );
    }

// 45 - livre
// #todo: Usar para manipulação de arquivo ou diretório.
// 46 - livre
// 47 - livre
        // 48 - livre

// 49
// Show system info
// See: sys.c
    if (number == 49){
        sys_show_system_info((int) arg2);
        return NULL;
    }

// ...

// 65 - Put a char in the current virtual console.
// IN: ch, console id.
    if (number == SCI_KGWS_PUTCHAR){
        __servicePutChar( (int) arg2, (int) arg3 );
        return NULL;
    }

// 70 - Exit. (Again)
// Atende a funcao exit() da libc. 
// Criaremos um 'request' que sera atendido somente quando 
// houver uma interrupcao de timer. 
// Enquanto isso a thread deve esperar em um loop.
// #bugbug: Pode haver sobreposicao de requests?
// Assincrono.
// IN: ??
// #todo: 
// Criar um wrapper em sys.c ou exit.c
// See: request.c
// Request number 12. (Exit thread)
// #todo: 
// See at the beginning of this routine.

    if (number == SCI_EXIT)
    {
        panic("sci0: SCI_EXIT\n");
        return NULL;
    }

// 71 - fork?
// See: sys.c
    if (number == 71)
    {
        //panic("sci0: [71] fork()\n");
        debug_print("sci0: [71] fork()\n");
        return (void *) sys_fork();
    }

// 72
// See: sys.c
// Cria uma thread que fica no estado INITIALIZED.
// Outra syscall tem que colocar ela em STANDBY.
// See: sys.c
// IN:
// cg (cgroup), initial rip, ini
    if (number == SCI_SYS_CREATE_THREAD)
    {
        serial_printk("sci0: [72] Create thread\n");
        return (void *) sys_create_thread (
                            NULL,                     // cg
                            arg2,                     // initial rip
                            arg3,                     // initial stack
                            (pid_t) current_process,  // ppid
                            (char *) a4 );            // name
    }

// 73
// See: sys.c
// Cria um processo e coloca a thread primária pra rodar.
// #bugbug: 
// Na inicializacao do kernel, nos criamos um processo
// usando create_process. Mas nesse momento estavamos usando
// o diretorio de paginas do kernel e os registradores de segmento
// pertenciam ao kernel.
// Nessa tentativa de criarmos um processo usando create_process
// as coisas estao um pouco diferentes ... provavelmente
// estamos usando o diretorio de paginas do processo e os
// registradores de segmento podem estar em ring3.
// ?? Talvez poderiamos criar um request, da mesma maneira 
// que fazemos com a criaçao de threads e o spawn.
// #todo
// Aqui no kernel, precisamos criar mais rotinas de suporte
// a criacao de processos.
// Temos poucas opçoes e tudo esta meio fora de ordem ainda.
// syscall: 
// arg2 = name
// arg3 = process priority
// arg4 = nothing

    if (number == SCI_SYS_CREATE_PROCESS)
    {
        serial_printk("sci0: [73] Create process\n");
        return (void *) sys_create_process ( 
                            NULL,             // cgroup
                            (unsigned long) 0,     // Reserved
                            (unsigned long) arg3,  // priority
                            current_process,  // ppid
                            (char *) a2,      // name
                            (unsigned long) RING3 );          // iopl 
    }

// 74,75,76,77,78,79.

// 80 - Show current process info.
// #todo: Mostrar em uma janela própria.
// #todo: Devemos chamar uma função que 
// mostre informações apenas do processo atual. 
    if (number == SCI_CURRENTPROCESSINFO)
    {
        show_currentprocess_info();
        return NULL;
    }

// 81: Get parent process id.
// See: sys.c
    if (number == SCI_GETPPID){ 
        return (void *) sys_getppid();
    }

// 82 - Mostra informações sobre todos os processos.
    if (number == 82){
        show_process_information();
        return NULL;
    }

// 83
// Suporte a chamada da libc waitpid(...).
// schedi.c
// #todo.
// TID, PID 
// TID eh a thread atual.
// PID veio via argumento.
// IN: pid, status, option
// #todo: Change the name to sys_xxxx
    if (number == SCI_WAIT4PID)
    { 
        debug_print("sci0: [FIXME] SCI_WAIT4PID\n");
        return (void *) do_waitpid( 
                            (pid_t) arg2, 
                            (int *) arg3, 
                            (int)   arg4 );
                
       //block_for_a_reason ( (int) current_thread, (int) arg2 ); //suspenso
    }

// 84 - livre

// 85
    if (number == SCI_GETPID){
        return (void *) get_current_pid();
   }

// 86, 87 - livre

// Testa se o processo é válido
// se for valido retorna 1234
// testando ...
    if (number == SCI_88){
        return (void *) processTesting(arg2);
    }

// 89 - livre

// ------------------
// 90~99 Reservado para thread support

// 94
    if (number == SCI_STARTTHREAD)
    {
        debug_print("sci0: SCI_STARTTHREAD\n");
        // #bugbug
        // Why the user has a ponter to the ring0 thread structure?
        return (void *) core_start_thread((struct thread_d *) arg2);
    }

// ------------------

// #todo:
// Reserve these for some comon purpose.
// 100~109: free

// 110
// IN: flags.
// see: sys.c
    int reb_ret=-1;
    if (number == SCI_SYS_REBOOT)
    {
        debug_print("sci0: SCI_SYS_REBOOT\n");
        reb_ret = (int) sys_reboot(0);
        return (void *) (reb_ret & 0xFFFFFFFF);
    }

// 111
// Get the next system message.
// IN: User buffer for message elements.
// See: msg.c
    if (number == SCI_SYS_GET_MESSAGE){
        return (void *) sys_get_message( (unsigned long) &message_address[0] );
    }

// 112
// Post message to tid.
// Asynchronous.
// IN: tid, message buffer address.
// See: msg.c
    if (number == 112){
        return (void *) sys_post_message_to_tid( 
                            (int) arg2, 
                            (unsigned long) arg3 );
    }

// #todo: Reseve these for a common purpose,
// just like msg or networking.
// 113~117: 
// 118~119: 

// Pop data from network queue.
// IN: user buffer, buffer lenght.
    if (number == 118){
        return (void*) network_pop_packet( 
                           (unsigned long) &message_address[0], 
                           (int) arg3 );
    }

// Push data into the network queue?
    //case 119:
        //break;

// 120
// Get a message given the index.
// With restart support.
    if (number == SCI_SYS_GET_MESSAGE2)
    {
        return (void *) sys_get_message2( 
                (unsigned long) &message_address[0], arg3, arg4 );
    }

// 121, 122, 123

// sys_signal() support.
// #todo
// We need to implement the parameters for this function.
// see: ksignal.c in clib/
    if (number == 121){
        debug_print("sci0: [121] sys_sigaction not implemented\n");
        //return (void*) sys_sigaction();
        return NULL;
    }
    if (number == 122){
        debug_print("sci0: [122] sys_signal not implemented\n");
        //return (void*) sys_signal();
        return NULL;
    }

// 124 (teste)
// Defered system procedure call.
// #todo: 
// Precisamos armazenasr os argumentos em algum lugar.
// #bugbug: Precisamos criar um request.
    if (number == 124){
        kernel_request = KR_DEFERED_SYSTEMPROCEDURE;
        return NULL;
    }

// 125

// 126
// Permitindo que drivers e servidores em usermode acessem as portas.
// #todo: This operation needs permition?
// #bugbug
// #todo: 
// Tem que resolver as questoes de privilegios.
// IN: bits, port
    if (number == SCI_PORTSX86_IN)
    {
        return (void *) portsx86_IN ( 
                            (int) (arg2 & 0xFFFFFFFF), 
                            (unsigned short) (arg3 & 0xFFFF) );
    }

// 127
// Permitindo que drivers e servidores em usermode acessem as portas.
// #todo: This operation needs permition?
// #bugbug
// #todo: 
// Tem que resolver as questoes de privilegios.
// IN: bits, port, value
    if (number == SCI_PORTSX86_OUT)
    {
        portsx86_OUT ( 
            (int) arg2, 
            (unsigned short) (arg3 & 0xFFFF), 
            (unsigned int)   (arg4 & 0xFFFFFFFF) );
        return NULL;
    }

// 128~129: free
// --------
// 130, 131

// 132 - d_draw_char
// Desenha um caractere e pinta o pano de fundo.
// #todo: We do not have an api routine yet.
// IN: x, y, c, fg color, bg color
    if (number == 132)
    { 
        d_draw_char(
            (unsigned long)  message_address[0],    // x
            (unsigned long)  message_address[1],    // y 
            (unsigned long)  message_address[2],    // c
            (unsigned long)  message_address[3],    // fg
            (unsigned long)  message_address[4] );  // bg
        return NULL;
    }

// 133 - d_drawchar_transparent
// Desenha um caractere sem alterar o pano de fundo.
// IN: x, y, color, c
    if (number == 133)
    {
        d_drawchar_transparent(
            (unsigned long)  message_address[0],   // x
            (unsigned long)  message_address[1],   // y 
            (unsigned long)  message_address[2],   // color
            (unsigned long)  message_address[3] ); // c
        return NULL;
    }

// 134~137: free

// 138 - Get key state.
// IN: vk.
    if (number == 138){
        return (void *) keyboardGetKeyState((unsigned char) arg2);
    }

// 139

// -----------------
// 140~149
// -----------------

// 150~156 User and group support.

// 152 - get uid
    if (number == SCI_GETCURRENTUSERID){
        return (void *) current_user; 
    }

// 154 - get gid
    if (number == SCI_GETCURRENTGROUPID){
        return (void *) current_group; 
    }

// 157~159: Security

// 157 - get user session id
    if (number == SCI_GETCURRENTUSERSESSION)
    {
        return (void *) current_usersession; 
    }

// 158 - free

// 159 - get current cgroup id
// #todo: Chnage this name: SCI_GETCURRENTDESKTOP
    if (number == SCI_GETCURRENTDESKTOP)
    {
        return (void *) get_current_cg_id();
    }

// ----------------
// 160~169: Reserved to network support.

// 161
// get socket IP
// Gramado API socket support. (not libc)
    if (number == 161){
        return (void *) getSocketIPV4( (struct socket_d *) arg2 );
    }

// 162
// get socket port
// Gramado API socket support. (not libc)
    if (number == 162){
        return (void *) getSocketPort( (struct socket_d *) arg2 );
    }

// 163
// update socket  
// retorno 0=ok 1=fail
// Gramado API socket support. (not libc)
    if (number == 163){
        return (void *) update_socket ( 
                            (struct socket_d *) arg2, 
                            (unsigned int)     (arg3 & 0xFFFFFFFF), 
                            (unsigned short)   (arg4 & 0xFFFF) );
    }

// ----------------

// 170 - command 'pwd'.
// Cada processo tem seu proprio pwd.
// Essa rotina mostra o pathname usado pelo processo.
// See: fs.c
// #test
// Isso é um teste. Essa chamada não precisa disso.
    if (number == SCI_PWD)
    {
        if (is_superuser() == TRUE)
        {
            debug_print("sci0: [SCI_PWD] Yes, I'm the super user\n");
            printk     ("sci0: [SCI_PWD] Yes, I'm the super user\n");
        }
        sys_pwd();
        return NULL;
    }

// 171 - retorna o id do volume atual.
    if (number == SCI_GETCURRENTVOLUMEID){
        return (void *) current_volume;
    }

//172 - configura o id do volume atual.
//#bugbug: Estamos modificando, sem aplicar nenhum filtro.
    if (number == SCI_SETCURRENTVOLUMEID){
        current_volume = (int) arg2;
        return NULL;
    }

// 173
// Lista arquivos de um diretório, dado o número do disco,
// o numero do volume e o número do diretório,
// args in: disk id, volume id, directory id
// See: fs.c
    if (number == SCI_LISTFILES){
        fsListFiles( arg2, arg3, arg4 );
        return NULL;
    }

// 174
    if (number == SCI_SEARCHFILE){
        debug_print ("sci0: SCI_SEARCHFILE\n");
        return (void *) search_in_dir ( 
                            (const char *) arg2, 
                            (unsigned long)   arg3 );
    }


// 175 - 'cd' command support.
// +Atualiza o pathname na estrutura do processo atual.
// +Atualiza o pathname na string global.
// +Carrega o arquivo referente ao diretório atual.
// See: fs.c
    if (number == 175){
        debug_print("sci0: 175\n");
        sys_cd_command((char *) arg2);
        return NULL;
    }

// 176
// Remove n nomes de diretório do pathname do processo 
// indicado no argumento.
// Copia o nome para a string global.
    if (number == 176)
    {
        debug_print("sci0: 176\n");
        fs_pathname_backup( current_process, (int) arg3 );
        return NULL;
    }

// 177 -  'dir' command.
// Comando dir no shell.
// Listando arquivos em um diretório dado seu nome.
// #bugbug: Talvez tenhamos que usr a sci2.
// See: fs.c
    if (number == 177)
    {
        debug_print("sci0: [177]\n");
        fsList((const char *) arg2);
        return NULL;
    }

// 178
// See: sys.c
    if (number == 178){
        return (void *) sys_get_file_size((unsigned char *) arg2);
    }

// 179

//----------
// 180~189: memory support.
// (Privilegies)

// 184
// Pega o endereço do heap do processo dado seu id.
// See: process.c
    if (number == SCI_GETPROCESSHEAPPOINTER)
    {
        debug_print("sci0: [184]\n");
        return (void *) GetProcessHeapStart((int) arg2);
    }

//----------
// 190~199: free
// 200~209: free

//----------
// 210~219: terminal/virtual console support.

//211
    if (number == SCI_GETCURRENTTERMINAL){
        return (void *) current_terminal;
    }

//212
    if (number == SCI_SETCURRENTTERMINAL)
    {
        // #todo: Permissions.
        current_terminal = (int) arg2;
        return NULL;
    }

//----------

// 223 - Get sys time info.
// informaçoes variadas sobre o sys time.
    if (number == 223){
        return (void *) get_systime_info((int) arg2);
    }

// 224
    if (number == SCI_GETTIME){
        return (void *) hal_get_time();
    }
// 225
    if (number == SCI_GETDATE){
        return (void *) hal_get_date();
    }

// 226 - get
// Obs: 
// #todo: 
// Poderia ser uma chamada para configurar o posicionamento 
// e outra para configurar as dimensões.
// #todo: Atomic stuff.
    if (number == SCI_GET_KERNELSEMAPHORE){
        return (void *) __spinlock_ipc;
    }

// 227 - close gate
// Entering critical section.
// See: process.c
    if (number == SCI_CLOSE_KERNELSEMAPHORE){
        process_close_gate(current_process);
        return NULL;
    }

// 228 - open gate
// Exiting critical section.
// #todo: Quando um processo fechar e estiver
// em sua sessão crítica, então devemos liberar
// essa flag. Isso fica mais fácil de lembrar se
// existir uma flag na estrutura de processo.
// See: process.c
    if (number == SCI_OPEN_KERNELSEMAPHORE){
        process_open_gate(current_process);
        return NULL;
    }

// 229

//---------------------
// 230~239: Reserved for tty support.

// 236 - get tty id
    if (number == 236){
        return (void *) current_tty;
    }

//---------------------

// 240~249: Reserved for text editing support.

// 240, 241
    if (number == SCI_GETCURSORX){
        return (void *) get_cursor_x();
    }
    if (number == SCI_GETCURSORY){
        return (void *) get_cursor_y();
    }



// #test
// 248 - sys_execve()
// see: kunistd.c in libk/
    if (number == 248)
    {
        printk("[248]: sys_execve() Not implemented\n");
        refresh_screen();
        return (void *) -1;
    }

// =====================================
// (250 ~ 255) - Info support.

// 250
// Get system metrics.
// IN: index
    if (number == SCI_SYS_GET_SYSTEM_METRICS){
       return (void *) sys_get_system_metrics((int) arg2);
    }

// == - ==================================================


// ==== 260 mark ===================================================

// Outro numero fará esse trabalhao.
    if (number == 260)
    {
        return (void *) sys_read ( 
                            (unsigned int) arg2, 
                            (char *)       arg3, 
                            (int)          arg4 );
    }

// Outro numero fará esse trabalho.
    if (number == 261)
    {
        return (void *) sys_write ( 
                            (unsigned int) arg2, 
                            (char *)       arg3, 
                            (int)          arg4 );
    }


// read on virtual console!
// range: 0 ~ 3
// chamado por read_VC em ring3.
// IN: fd, buf, count
    if (number == 262){
        return (void *) console_read((int) arg2, 
                            (const void *) arg3, (size_t) arg4 );
    }

// write on virtual console!
// range: 0 ~ 3
// chamado por write_VC em ring3.
// IN: fd, buf, count
    if (number == 263){
        return (void *) console_write ((int) arg2, 
                            (const void *) arg3, (size_t) arg4 );
    }



// 266
// Pega o número da tty de um processo, dado o pid.
// process.c
// IN: PID.
// OUT: tty id.
    if (number == 266){
        return (void *) process_get_tty((int) arg2);
    }

// Ligar duas tty, dados os pids dos processos que possuem as tty.
// tty/pty.c
// IN: master pid, slave pid.
    //if (number == 267){
    //    return (void *) pty_link_by_pid ( (int) arg2, (int) arg3 );
    //}

// Channel is a file descriptor in the file list 
// of the current process.
// IN: fd, buf, count.
    if (number == 272)
    {
        return (void *) tty_read ( 
                            (unsigned int) arg2,    // channel 
                            (char *)       arg3,    // buf
                            (int)          arg4 );  // nr
    }

// Channel is a file descriptor in the file list 
// of the current process.
// IN: fd, buf, count.
    if (number == 273)
    {
        return (void *) tty_write ( 
                            (unsigned int) arg2,    // channel 
                            (char *)       arg3,    // buf
                            (int)          arg4 );  // nr
    }

// Get current virtual console.
    if (number == 277){
        return (void *) console_get_current_virtual_console();
    }

// Set current cirtual console.
// #todo: precisa de privilégio. 
    if (number == 278){
        console_set_current_virtual_console((int) arg2);
        return NULL;
    }
    
// Returns the current runlevel.
    if (number == 288){
        return (void *) core_get_current_runlevel();
    }

// Serial debug print string.
// See: sys.c
    if ( number == 289 ){
        return (void *) sys_serial_debug_printk( (char *) arg2 );
    }

// Get memory size.
    if (number == 292){
        return (void *) core_get_memory_size_mb();
    }

// #bugbug: cuidado.
// get boot info.
// See: info.c
// IN: index to select the info.
    if ( number == 293 ){
        return (void *) info_get_boot_info((int) arg2);
    }

// Inicializar ou reinicializar componentes do sistema
// depois da inicialização completa do kernel.
// Isso poderá ser chamado pelo init.bin, pelo shell
// ou qualquer outro.
// see: 
    if (number == 350){
        printk("sci0: 350\n"); 
        return (void *) sys_initialize_component((int) arg2);
    }

// 377 - uname
// Get info to fill the utsname structure.
// See: sys.c
    if (number == SCI_SYS_UNAME)
	{
        int_retval = (-EFAULT); //Bad address
        if ((void*) arg2 == NULL){
            return (void*) (int_retval & 0xFFFFFFFF);
        }
        int_retval = (int) sys_uname((struct utsname *) arg2);
        if (int_retval < 0){
            return (void*) (int_retval & 0xFFFFFFFF);
		}
		// OK
		return NULL;
    }

// #bugbug
// It crashes the system.
// Clear the screen.
    if (number==390)
    {
        debug_print("sci0: [390]\n");
        //displayInitializeBackground(,COLOR_BLUE,TRUE);
        return NULL;
    }

// #bugbug: Is it failing when we try to draw the whole screen?
    if (number == 391)
    {
        // debug_print("sci0: [391]\n");
        // IN: l,t,w,h,color,rop flags.
        backbuffer_draw_rectangle ( 
            (unsigned long) message_address[0], 
            (unsigned long) message_address[1],
            (unsigned long) message_address[2],
            (unsigned long) message_address[3],
            (unsigned int)  message_address[4],
            (unsigned long) message_address[5] );
        return NULL;
    }

// 512 - Get display server PID for a given cgroup ring0 pointer.
// IN: 
//   arg2 = ring 0 cgroup structure pointer.
// OUT: 
//   pid
    if (number == SCI_GET_DS_PID)
    {
        debug_print("sci0: SCI_GET_DS_PID\n");
        cg = (struct cgroup_d *) arg2;
        if ((void *) cg == NULL){
            return NULL;
        }
        if ( cg->used == TRUE && cg->magic == 1234 )
        {
            return (void *) cg->__display_server_pid; 
        }        
        // It means pid=0.
        return NULL;
    }

// 513
// Register the ring3 display server.
// Set display PID for a given cgroup structure.
// Register a display server.
// gramado_ports[11] = ws_pid
// Called by the ring 3 display server.
// >> arg2 = ring 0 cgroup structure pointer.
// >> arg3 = The display erver PID.
// IN: 
// cgroup, 
// caller pid.
// see: network.c

    int __ds_ok = FALSE;
    if (number == SCI_SET_DS_PID)
    {
        debug_print("sci0: SCI_SET_DS_PID\n");

        __ds_ok = 
            (int) network_register_ring3_display_server(
                (struct cgroup_d *) arg2, (pid_t) arg3 );
        
        if (__ds_ok == TRUE){
            return (void*) TRUE;
        }
        return NULL;
    }   

// #deprecated
// 514 - get wm PID for a given cgroup
    if (number == SCI_GET_WM_PID){
        panic("sci0: SCI_GET_WM_PID\n");
        return NULL;
    }
// #deprecated
// 515 - set wm PID for a given cgroup
    if (number == SCI_SET_WM_PID){
        panic("sci0: SCI_SET_WM_PID\n");
        return NULL;
    }


// 518
// Register the ring3 browser.
// Set browser PID for a given cgroup structure.
// Register a display server.
// gramado_ports[11] = ws_pid
// Called by the ring 3 display server.
// >> arg2 = cgroup structure pointer.
// >> arg3 = The display erver PID.
// IN: cgroup, caller pid.
// see: network.c

    int browser_server_ok=FALSE;
    if (number == 518)
    {
        debug_print("sci0: 518\n");
        
        browser_server_ok = 
            (int) network_register_ring3_browser(
                (struct cgroup_d *) arg2, (pid_t) arg3);
        
        if (browser_server_ok == TRUE){
            return (void*) TRUE;
        }
        return NULL;
    }   

// The main cgroup.
// This is used to register system components.
// #bugbug
// This is a ring0 pointer.
// A ring3 process can't handle this thing.
// Get current cgroup. (ring0 pointer hahaha)
    if (number == 519){
        return (void *) system_cg;
    }

// 521 - set ns PID for a given cgroup
// network server
// Register a network server.
    if (number == 521)
    {
        cg = (struct cgroup_d *) arg2;
        if ((void *) cg != NULL)
        {
            if ( cg->used == TRUE && cg->magic == 1234 )
            {
                cg->__network_server_pid = (pid_t) arg3;
                socket_set_gramado_port(
                    GRAMADO_PORT_NS,
                    (pid_t) current_process );

                return (void *) TRUE;  //ok 
            }
        }
        return NULL; //fail
    }    

// 600 - dup
    if (number == 600){
        return (void *) sys_dup( (int) arg2 );  
    }
// 601 - dup2
    if (number == 601){
        return (void *) sys_dup2( (int) arg2, (int) arg3 );
    }
// 602 - dup3
    if (number == 602){
        return (void *) sys_dup3( (int) arg2, (int) arg3, (int) arg4 );
    }

// 603 - lseek support.
// See: kunistd.c
// IN: fd, offset, whence.
    if (number == SCI_SYS_LSEEK)
    {
        return (void *) sys_lseek ( 
                            (int)   arg2, 
                            (off_t) arg3, 
                            (int)   arg4 );
    }

// 640
// Lock the taskswtiching.
// Only the init thread can call this service.
    if (number == 640)
    {
        if (current_thread == INIT_TID){
            taskswitch_lock();
        }
        return NULL;
    }

// 641
// Unlock taskswitching.
// Only the init thread can call this service.
    if (number == 641)
    {
        if (current_thread == INIT_TID){
            taskswitch_unlock();
        }
        return NULL;
    }

// 642
// Lock the scheduler.
// Only the init thread can call this service.
    if (number == 642)
    {
        if (current_thread == INIT_TID){
            scheduler_lock();
        }
        return NULL;
    }

// 643
// Unlock scheduler.
// Only the init thread can call this service.
    if (number == 643)
    {
        if (current_thread == INIT_TID){
            scheduler_unlock();
        }
        return NULL;
    }



// 770 - Show device list.
    if (number == 770)
    {
        // #bugbug
        // Showing only one type of object,
        devmgr_show_device_list(ObjectTypeTTY);
        return NULL;
    }

// 777 - cpu usage for idle thread.
    if (number == 777){
        return (void *) profiler_percentage_idle_thread;
    }

// 801 - get host name
// see: kunistd.c
    if ( number == 801 ){
        return (void *) sys_gethostname((char *) arg2);
    }
// 802 - set host name
// see: kunistd.c
    if (number == 802)
    {
        // #todo: Lenght
        // IN: name, lenght
        return (void *) sys_sethostname((const char *) arg2,0); 
    }

// 803 - Get user name.
    if (number == 803){
        return (void *) sys_getusername((char *) arg2);
    }
// 804 - Set user name.
    if (number == 804){
        return (void *) sys_setusername((const char *) arg2); 
    }

// #todo
// supporting ptsname libc function
// get_ptsname
// #todo: Change the name to sys_ptsname()
// IN: fd do master, buffer em ring3 para o nome, buflen.
    if (number == 808){
        return (void *) __ptsname( (int) arg2, 
                            (char *) arg3, (size_t) arg4 ); 
    }

// (#bugbug: The same as above?)
//#todo
//supporting ptsname_r libc function
// #todo: Change the name to sys_ptsname()
//IN: fd do master, buffer e buflen.
    if ( number == 809 ){
        return (void *) __ptsname ( (int) arg2, 
                            (char *) arg3, (size_t) arg4 ); 
    } 

// 880 - Get process stats given pid
// IN: pid, index
    if ( number == 880 ){
       return (void *) get_process_stats ( (pid_t) arg2, (int) arg3 );
    }

// 881 - Get thread stats given tid
// IN: tid, number
    if ( number == 881 ){
        return (void *) GetThreadStats ( (int) arg2, (int) arg3 );
    }

// 882 - Get process name
// IN: PID, ubuffer.
    if ( number == 882 ){
        return (void *) getprocessname( (pid_t) arg2, (char *) arg3 );
    }

// 883 - Get thread name
    if ( number == 883 ){
        return (void *) getthreadname ( (int) arg2, (char *) arg3 );
    }

// 884 - alarm()
// See: sys.c
    if (number == SCI_SYS_ALARM){
        return (void *) sys_alarm((unsigned long) arg2);
    }

// 891 - Allocate shared ring3 pages.
    if (number == 891){
        debug_print("sci0: 891, Allocate shared ring3 pages\n");
        return (void *) core_alloc_shared_ring3_pages( 
            (pid_t) current_process, 
            (int) arg2 );
    }

// 892 - Setup the thread's surface rectangle.
    if (number == 892){
        // l,t,w,h
        __setup_surface_rectangle( 
            (unsigned long) message_address[0], 
            (unsigned long) message_address[1],
            (unsigned long) message_address[2],
            (unsigned long) message_address[3] );
        return NULL;
    }

// 893 - Invalidate the thread's surface rectangle.
    if (number == 893){
        __invalidate_surface_rectangle();
        return NULL;
    }

// 896 - Invalidate the whole screen
    if (number == 896){
        invalidate_screen();
        return NULL;
    }

// Set up and draw the main surface for a thread.
    if (number == 897){
        __service897();
        return NULL;
    }

// 898 - Start the kernel console.
    if (number == 898){
        input_enter_kernel_console();
        return NULL;
    }
// 899 - Exit the kernel console.
    if (number == 899){
        input_exit_kernel_console();
        return NULL;
    }

// is the socket full?
// IN: fd
// OUT: -1= error; FALSE= nao pode ler; TRUE= pode ler.
// See: sys.c
    if (number == 913){
        return (void *) sys_sleep_if_socket_is_empty(arg2);
    }

// get screen window.
// #todo. checar validade
    //if ( number == 955 ){  return (void *) gui->screen;  } 
    
    //if ( number == 956 ){  return (void *) gui->background; } 

// get main window.
// #todo. checar validade
    //if ( number == 957 ){ return (void *) gui->main; }  

// 970 - Create request.
// ?? #bugbug
// A interrupção não conseguirá retornar para a mesma thread.
// Chamará o scheduler por conta própria.
// IN: reason, reason
    if (number == 970)
    {
        // #suspended
        /*
        create_request ( 
            (unsigned long) 15,      // number 
            (int) 1,                 // status 
            (int) 0,                 // timeout. 0=imediatamente.
            (pid_t) current_process,   // target_pid
            (tid_t) current_thread,    // target_tid
            (int) 0,                 // msg  
            (unsigned long) arg2,    // long1  
            (unsigned long) arg3 );  // long2
        */
        return NULL;
    }


// api - load file (string ???)
// #todo: Tem que retornar algum identificador para a api.
// poderia ser um indice na tabela de arquivos abertos pelo processo.
// #todo: rever.
// See: kstdio.c
    //if ( number == 4002 ){
    //    return (void *) k_fopen ( (const char *) arg2, "r+" );
    //}

// 4444
// Show root files system info.
// Print into the raw kernel console.
    if (number == 4444){
        fs_show_root_fs_info();
        return NULL;
    }



    //7000 ~ 7020 for network sockets

// 7000 - socket() 
// See: socket.c
// family, type, protocol
    if (number == SCI_SYS_SOCKET){
        return (void *) sys_socket( (int) arg2, (int) arg3, (int) arg4 );
    }

// 7001 - connect()
// fd, sockaddr struct pointer, addr len.
    if (number == SCI_SYS_CONNECT){
        return (void *) sys_connect ( 
                            (int) arg2, 
                            (const struct sockaddr *) arg3,
                            (socklen_t) arg4 );
    }

// 7002 - accept()
// This is the unix standard method.
// Our major goal is to return the fd for the client socket file.
// #bugbug: Work in progress.
// fd, sockaddr struct pointer, addr len pointer.
    if (number == SCI_SYS_ACCEPT){
        return (void *) sys_accept ( 
                            (int) arg2, 
                            (struct sockaddr *) arg3, 
                            (socklen_t *) arg4 ); 
    }

// 7003 - bind()
// fd, sockaddr struct pointer, addr len.
    if (number == SCI_SYS_BIND){
        return (void *) sys_bind ( 
                            (int) arg2, 
                            (const struct sockaddr *) arg3,
                            (socklen_t) arg4 );
    }

// 7004 - listen() support.
// IN: fd, backlog
// see: 
    if (number == SCI_SYS_LISTEN){
        return (void *) sys_listen((int) arg2, (int) arg3);  
    }

// 7005

// 7006
// Salvar um pid em uma das portas.
// IN: gramado port, PID
    if (number == 7006){
        return (void *) socket_set_gramado_port( (int) arg2, (int) arg3 );
    }

// 7007 - sys_getsockname()
// fd, sockaddr struct pointer, addr len.
    if ( number == 7007 ){
        return (void *) sys_getsockname ( 
                            (int) arg2, 
                            (struct sockaddr *) arg3,
                            (socklen_t *) arg4 );
     }

// 7008 - show socket info for a process.
// IN: pid
    if (number == 7008){
        show_socket_for_a_process((int) arg2);
        return NULL;
    }

// 7009 - libc: shutdown() IN: fd, how
    if (number == 7009){
        sys_socket_shutdown( (int) arg2, (int) arg3 );
        return NULL;
    }

// 8000 - ioctl() implementation.
// IN: fd, request, arg
// See: fs.c
    if (number == SCI_SYS_IOCTL){
        return (void *) sys_ioctl ( 
                            (int) arg2, 
                            (unsigned long) arg3, 
                            (unsigned long) arg4 );
    }

// 8001 - fcntl()
// See: sys.c    
    if (number == SCI_SYS_FCNTL){
        return (void *) sys_fcntl ( 
                            (int) arg2, 
                            (int) arg3, 
                            (unsigned long) arg4 );
    }

// ?? #bugbug
// Setup stdin pointer
// See: kstdio
// IN: fd
    if (number == 8002){
        return (void *) sys_setup_stdin((int) arg2);
    }

// Pegando o endereço de um buffer de icone.
// queremos saber se ele eh compartilhado.
// shared_buffer_terminal_icon
// #bugbug: Static size for the icons. Static buffer size.
// See: wm.c
    if (number == 9100){
        if (arg2<0)
            return NULL;
        return (void *) ui_get_system_icon((int) arg2);
    }

// ========================================================
//done:
    __default_syscall_counter++;
    return NULL;
}


// This routine was called by the interrupt handler in x64sc.c.
void *sci1 ( 
    unsigned long number, 
    unsigned long arg2, 
    unsigned long arg3, 
    unsigned long arg4 )
{
// Getting requests from ring3 applications via systemcalls.
// :: Services in mod0.

    struct process_d *p;
    struct thread_d *t;
    int layer = LAYER_UNDEFINED;

    debug_print("sci1: [TODO]\n");

    pid_t current_process = (pid_t) get_current_process();


// #test
// ---------------------------------
    if ( current_thread<0 || current_thread >= THREAD_COUNT_MAX )
    { 
        return NULL; 
    }
    t = (struct thread_d *) threadList[current_thread];
    if ((void*) t == NULL)
        return NULL;
    if (t->magic != 1234)
        return NULL;
    // Increment stime.
    // see: x64cont.c
    t->transition_counter.to_supervisor++;
    // Antecipando, ja que fica dificil
    // fazer isso na saida por enquanto.
    t->transition_counter.to_user++;
// ---------------------------------

// permission
    if (current_process<0 || current_process >= PROCESS_COUNT_MAX){
        panic("sci1: current_process\n");
    }
    p = (struct process_d *) processList[current_process];
    if ((void*) p == NULL){
        debug_print("sci0: p\n");
        panic("sci1: p\n");
    }
    if ( p->used != TRUE || p->magic != 1234 ){
        debug_print("sci1: p validation\n");
        panic("sci1: p validation\n");
    }

// The display server was not initialized yet.
    if (DisplayServerInfo.initialized != TRUE){
        return 4321;
    }

// #test
// Only the display server can access this service.
    if (current_thread != DisplayServerInfo.tid)
    {
        // OUT: Access denied.
        return 4321;
    }
// #test
// Only the display server can access this service.
    if (p->pid != DisplayServerInfo.pid)
    {
        // OUT: Access denied.
        return 4321;
    }

//++
//-------------------------------------
// #test
// #todo: This is a work in progress.
// Maybe this interrupt can be used 
// to call the services provided by the first module, mod0.bin.
// see: mod.c and mod.h.

    unsigned long return_value=0;

    if ((void*) kernel_mod0 == NULL)
        return NULL;
    if (kernel_mod0->magic != 1234)
        return NULL;
    if (kernel_mod0->initialized != TRUE)
        return NULL;


// Validation
    if ((void*) kernel_mod0->entry_point == NULL){
        goto fail;
    }

// #test
// Calling the virtual function, and
// getting the return value.

    return_value = 
        (unsigned long) kernel_mod0->entry_point(
            0x81,    // sc? system id.
            number,  // Reason
            arg2,    // l2
            arg3,    // l3
            arg4 );  // l3

// Done
    return (void*) return_value;

//-------------------------------------
//--

fail:
    return NULL;
}

// This routine was called by the interrupt handler in x64sc.c.
void *sci2 ( 
    unsigned long number, 
    unsigned long arg2, 
    unsigned long arg3, 
    unsigned long arg4 )
{
// Getting requests from ring3 applications via systemcalls.
// :: Services in kernel.

    struct process_d *p;
    struct thread_d *t;
    int layer = LAYER_UNDEFINED;

    pid_t current_process = (pid_t) get_current_process();

// #test
// ---------------------------------
    if ( current_thread<0 || current_thread >= THREAD_COUNT_MAX )
    { 
        return NULL;
    }
    t = (struct thread_d *) threadList[current_thread];
    if ((void*) t == NULL)
        return NULL;
    if (t->magic != 1234)
        return NULL;
    // Increment stime.
    // see: x64cont.c
    t->transition_counter.to_supervisor++;
    // Antecipando, ja que fica dificil
    // fazer isso na saida por enquanto.
    t->transition_counter.to_user++;
// ---------------------------------

    // debug_print("sci2: [TODO]\n");

// Profiling in the process structure.

// Permission

    if ( current_process < 0 || current_process >= PROCESS_COUNT_MAX ){
        panic("sci2: current_process\n");
    }

    p = (struct process_d *) processList[current_process];
    if ((void*) p == NULL){
        debug_print("sci2: p\n");
        panic("sci2: p\n");
    }
    if ( p->used != TRUE || p->magic != 1234 ){
        debug_print("sci2: p validation\n");
        panic("sci2: p validation\n");
    }

// Personality
// Feito pra rodar no Gramado OS.
    if (p->personality != PERSONALITY_GRAMADO){
        panic("sci2: Personality\n");
    }

// The layer
    layer = p->_layer;

// Counting syscalls ...
    p->syscalls_counter++;

//
// switch
//

// 1 - Set magic (in kernel console)
// #todo: This operation needs permition?
    if (number == 1){
        //CONSOLE_TTYS[fg_console].magic = arg2;
        return NULL;
    }

// 2 - Get magic (in kernel console)
    if (number == 2){
        return (void*) CONSOLE_TTYS[fg_console].magic;
    }

// 3 - Get system metrics
    if (number == 3){
        return (void*) sys_get_system_metrics(arg2);
    }

// 4 - ioctl() handler.
// See: fs.c
// IN: fd, request, arg
    if (number == 4){
        debug_print("sci2: [4] ioctl\n");
        // #todo
        //return (void*) sys_ioctl ( (int) arg2, (unsigned long) arg3, (unsigned long) arg4 );
        return NULL;
    }

// 5 - fcntl() implementation.
// See: ?
    if (number == 5){
        debug_print("sci2: [5] fcntl\n");
        return (void*) sys_fcntl( (int) arg2, (int) arg3, (unsigned long) arg4 );
    }

// 18 - read() implementation.
// See: fs.c
    if (number == SCI_SYS_READ){
        return (void *) sys_read( 
                            (unsigned int) arg2, 
                            (char *)       arg3, 
                            (int)          arg4 );
    }

// 19 - write() implementation.
// See: fs.c
    if (number == SCI_SYS_WRITE){
        return (void *) sys_write ( 
                            (unsigned int) arg2, 
                            (char *)       arg3, 
                            (int)          arg4 );
    }

    // ...

// 265 - yield
//  + Set a flag that this thread will be preempted.
// See: schedi.c
    if (number == 265){
        sys_yield(current_thread); 
        return NULL; 
    }

// 266 - sleep
// Sleep until.
// #todo: Explaint it better here.
// #bugbug
// We cant drastically change the state of a thread,
// we need to schedule that operation.
// given to the ts the opportunity to do that 
// in the right moment. As well as we do for the yield operation.
// Agendando a operação de sleep.
// O ts vai fazer isso quando for seguro.
// IN: tid, ms
    if (number == 266){
        sys_sleep( (tid_t) current_thread, (unsigned long) arg2 );
        return NULL;
    }

// #test
// 777 - kinda nice() 
    if (number == 777){
        sys_broken_vessels(current_thread);
        return NULL;   
    }

// Get process info given an index.
// Just some few elements.
    if (number == 800)
    {
        switch (arg2) {
        // PID
        case 100:
            return (void*) p->pid;
            break;
        // PPID
        case 101:
            return (void*) p->ppid;
            break;
        // Instance ID. The physiscal address.
        case 102:
            return (void*) p->ImagePA;
            break;
        // ...
        default:
            return NULL;
            break;
        };
    }

// Get thread info given an index.
// Just some few elements.
    if (number == 801)
    {
        switch (arg2) {
        case 100:
            return (void*) t->tid;
            break;
        case 101:
            return (void*) t->owner_pid;
            break;
        // ...
        default:
            return NULL;
            break;
        };
    }


// ---------------------------
// 900 - copy process 
// For rtl_clone_and_execute().
// Clona e executa o filho dado o nome do filho.
// O filho inicia sua execução do início da imagem.
// #bugbug: Isso às vezes falha na máquina real.
// #todo: Use more arguments.
// See: clone.c
// IN: 
//   + file name, 
//   + parent pid, 
//   + clone flags.
// OUT: Child's PID.

    unsigned long clone_flags = (unsigned long) arg3;
    //unsigned long extra = (unsigned long) arg4;
    if (number == SCI_COPY_PROCESS)
    {
        debug_print("sci2: [SCI_COPY_PROCESS] clone and execute\n");
        // #debug
        //printk("sci2: copy_process called by pid{%d}\n",current_process);
        return (void *) copy_process( 
                            (const char *) arg2, 
                            (pid_t) current_process, 
                            (unsigned long) clone_flags );
    }

// This process is telling us that he wants
// to be treated as a terminal,
// giving him the permission to create Connectors[i].
    if (number == 901)
    {
        if (p->_is_terminal == TRUE)
            return NULL;
        if (arg2 != 1234)
            return NULL;
        p->Connectors[0] = -1;
        p->Connectors[1] = -1;
        p->_is_terminal = TRUE;
        return NULL;
    }

// Get the file descriptor for a given connector
// #todo: Create a worker for this.
// return: 
// NULL = fail.
    int conn_fd = 0;
    if (number == 902)
    {
        if (p->_is_terminal == TRUE || 
            p->_is_child_of_terminal == TRUE )
        {
            // The first connector
            if (arg2 == 0)
            {
                conn_fd = (int) p->Connectors[0];
                if (conn_fd < 0)
                    return NULL;
                if (conn_fd < 3)
                    return NULL;
                // 31 is the socket.
                if (conn_fd >= 30)
                    return NULL;
                return (void*) (conn_fd & 0xFF);
            }
            // The second connector
            if (arg2 == 1)
            {
                conn_fd = (int) p->Connectors[1];
                if (conn_fd < 0)
                    return NULL;
                if (conn_fd < 3)
                    return NULL;
                // 31 is the socket.
                if (conn_fd >= 30)
                    return NULL;
                return (void*) (conn_fd & 0xFF);
            }
            // Fail
            // Invalid connector number.
            return NULL;
        }
        // Fail
        return (void*) NULL;
    }

// 8000 - ioctl() implementation.
// See: fs.c
// IN: fd, request, arg
    if (number == SCI_SYS_IOCTL)
    {
        debug_print("sci2: [8000] ioctl\n");
        //printk("sci2: [8000] ioctl\n");
        return (void *) sys_ioctl ( 
                            (int) arg2, 
                            (unsigned long) arg3, 
                            (unsigned long) arg4 );
    }

// 8001 - fcntl() implementation. 
// (second time) see: number 5.
// See: sys.c
    if (number == SCI_SYS_FCNTL){
        debug_print("sci2: [8001] fcntl\n");
        return (void *) sys_fcntl (
                            (int) arg2, 
                            (int) arg3, 
                            (unsigned long) arg4 );
    }

// 8003
// Clear the fg console background with a given color.
// Do not change the colors.
    unsigned int bg_color = COLOR_BLACK;
    unsigned int fg_color = COLOR_WHITE;
    if (number == 8003)
    {
        if (fg_console < 0)
            return NULL;
        if (fg_console >= CONSOLETTYS_COUNT_MAX){
            return NULL;
        }
        bg_color = (unsigned int) CONSOLE_TTYS[fg_console].bg_color;
        fg_color = (unsigned int) CONSOLE_TTYS[fg_console].fg_color;
        // IN: bg color, fg color, console number.
        // OUT: Number of bytes written.
        console_clear_imp( bg_color, fg_color, fg_console );
        return NULL;
    }

// 8004
// Change the foreground color of the current console.
    if (number == 8004)
    {
        if (fg_console<0 || fg_console > 3){
            return NULL;
        }

        // #bugbug
        // #deprecated
        // Cant change the kernel consoles.
        // Only the other ttys.
        // CONSOLE_TTYS[fg_console].fg_color = (unsigned int) arg2;

        return NULL;
    }

// 10000 - sys_set_file_sync
// Configurando sincronização de leitura e escrita em arquivo.
// principalmente socket.
// A estrutura de arquivo contém uma estrutura de sincronização de leitura e escrita.
// #ok: podemos usar ioctl
// See: sys.c
    if (number == 10000)
    {
        debug_print("sci2: [10000] sys_set_file_sync\n");
        // IN: fd, request, data
        sys_set_file_sync( (int) arg2, (int) arg3, (int) arg4 );
        return NULL;
    }

// 10001 - sys_get_file_sync
// Pegando informação sobre sincronização de leitura e escrita de arquivos.
// principalmente para socket.
// A estrutura de arquivo contém uma estrutura de sincronização de leitura e escrita.
// #ok: podemos usar ioctl
// See: sys.c
// IN: fd, request
    if (number == 10001){
        //debug_print("sci2: [10000] sys_get_file_sync\n");
        return (void*) sys_get_file_sync( (int) arg2, (int) arg3 );
    }

//
// Global sync
//

// Global sync - Not used anymore.

// ============
// See: sys.c
// Set action.
    if (number == 10002){
        sys_set_global_sync( (int) arg2, (int) arg3, (int) arg4 );
        return NULL;
    }
// Get action.
    if (number == 10003){
        return (void*) sys_get_global_sync( (int) arg2, (int) arg3 );
    }
// Create sync.
// OUT: sync id.
    if (number == 10004){
        return (void*) sys_create_new_sync();
    }
// Get sync id.
// Provisorio para teste
    if(number == 10005){
        return (void*) get_saved_sync();
    }
//=====

//
// Sync in file.
//

// ===============
// Set file sync action
// IN: fd, request, data
// see: fs.c
    if (number == 10006){
        sys_set_file_sync( (int) arg2, (int) arg3, (int) arg4 );
        return NULL;
    }
// Get file sync action
// IN: fd, request
// see: fs.c
    if (number == 10007){
        return (void*) sys_get_file_sync( (int) arg2, (int) arg3 );
    }
// ===============

// 10008
// Save FAT cache into the disk.
// FAT cache.
// This is the FAT cache for the system disk.
// The boot partition.
    int savefat_Status = -1;
    if (number == 10008)
    {
        savefat_Status = (int) fs_save_fat16_cache();
        if ( savefat_Status < 0 || 
             g_fat_cache_saved != FAT_CACHE_SAVED )
        {
            panic("sci2: [10008] Couldn't save FAT\n");
        }
        return NULL;
    }

// 10010 - Get the tid of the current thread.
    if (number == 10010){
        //debug_print("sci2: [10010] GetCurrentTID\n");
        return (void*) GetCurrentTID();
    }

// -----------------------------
// 10011
// Set the foreground thread given it's tid.
// #todo: We need a method for that.
// IN: arg2=tid.
    if (number == 10011)
    {
        debug_print("sci2: [10011] set foreground thread tid\n");
        //Change the priority of the old foreground thread?
        //set_thread_priority( threadList[foreground_thread], PRIORITY_NORMAL);
        if (arg2<0 || arg2>=THREAD_COUNT_MAX)
        {
            return NULL;
        }
        t = (struct thread_d *) threadList[arg2];
        if ((void*) t == NULL){
            return NULL;
        }
        if (t->used != TRUE){
            return NULL;
        }
        if (t->magic != 1234){
            return NULL;
        }
        // Redundant (Something is very wrong here)
        if (arg2 != t->tid){
            return NULL;
        }
        // Invalid state,
        // We can't give input focus to a thread in zombie state.
        if (t->state == RUNNING || t->state == READY)
        {
            //Giving more credits. But the scheduler will balance
            //it at the and of the round.
            //t->quantum = QUANTUM_FIRST_PLANE;
            t->quantum = (QUANTUM_MAX + 88);
            t->priority = PRIORITY_MAX;
        
            //#test
            //t->signal |= 1<<(SIGALRM-1);
            //t->signal |= 1<<(SIGKILL-1);

            foreground_thread = (int) arg2;

            // #deprecated
            // it will select the next input reponder.
            // set_input_responder_tid(foreground_thread);

            do_credits(foreground_thread);
            do_credits(foreground_thread);

            // #test: Selecting the timeout thread, that will have priority in the round.
            // Cutting the round and selecting it as next.
            //ev_responder_thread = (struct thread_d *) t;
            //ev_responder_thread->has_pending_event = TRUE;
        }

        return NULL;
    }

    // Lose the input focus.
    // We don't wanna be the foreground thread anymore.
    if (number == 10012)
    {
        debug_print("sci2: [10012] lose input focus\n");
        //Change the priority of the old foreground thread?
        //set_thread_priority( threadList[foreground_thread], PRIORITY_NORMAL);
        if (arg2<0 || arg2>=THREAD_COUNT_MAX)
        {
            return NULL;
        }
        t = (struct thread_d *) threadList[arg2];
        if ((void*) t == NULL){
            return NULL;
        }
        if (t->used != TRUE){
            return NULL;
        }
        if (t->magic != 1234){
            return NULL;
        }
        // Redundant (Something is very wrong here)
        if (arg2 != t->tid){
            return NULL;
        }
        // Invalid state
        if (t->state == DEAD){
            return NULL;
        }
        // Invalid state
        if (t->state == ZOMBIE){
            return NULL;
        }
        if (t->state == RUNNING || t->state == READY)
        {
            if (arg2 == foreground_thread){
                foreground_thread = -1;
            }
        }

        return NULL;
    }


// Get Init PID
    if (number == 10020){ 
        return (void*) GRAMADO_PID_INIT;
    }
// Get Init TID
    if (number == 10021){
        return (void*) INIT_TID;
    }

//
// Network
//

// #todo:
// We can put all the network services in a single dialog function.
// Just like a procedure. networkProcesure(....);

// 22001 
// Lock or unlock the network.

    if (number == 22001)
    {
        if (arg2 == TRUE)
            networkUnlock();
        if (arg2 == FALSE)
            networkLock();
        return NULL;
    }

// 22002 - Get network status.
    if (number == 22002){
        return (void*) networkGetStatus();
    }

// 22003 - Test some net component.
    if (number == 22003)
    {
        switch (arg2){
        case 1:  network_send_arp_request();  break;
        case 2:  network_test_udp();          break;
        case 3:  network_initialize_dhcp();   break;
        // case 4: break;
        // case 5: break;
        // case 6: break;
        // default: break;
        };
        return NULL;
    }

// =================================

// #test
// 22004 - Send ARP

    /*
    //#todo
    if (number == 22004)
    {
        network_send_arp(...); //see arp.c
        return NULL;
    }
    */

// #test
// 22005 - Send ETHERNET

    /*
    //#todo
    if (number == 22005)
    {
        ethernet_send(...); //see ethernet.c
        return NULL;
    }
    */

// #test
// 22006 - Send IP

// #test
// 22007 - Send UDP


    /*
    // #todo
    // arg2 needs to have a pointer to a table
    // full of parameters.
    if (number == 22007)
    {
        network_send_udp( 
            dhcp_info.your_ipv4,   // scr ip
            &message_buffer[0],    // dst ip  (4bytes + 4bytes pad)
            &message_buffer[1],    // dst mac  (6bytes + 2bytes pad)
            &message_buffer[2],    // source port  (2bytes + 6pads)
            &message_buffer[3],    // dst port     (2bytes + 6pads)
            &message_buffer[4],    // frame address
            &message_buffer[5] );  // frame lenght
        return NULL;
    }
    */


// #test
// 22008 - Send TCP

// #test
// 22009 - Send XXX

    if (number == 22009)
    {
        network_send_raw_packet (
            (struct intel_nic_info_d *) currentNIC, 
            (size_t) arg3,          // Frame lenght
            (const char *) arg2 );  // Frame address

        return NULL;
    }

//--------------

// #test
// 22010 -

// 22011 - PS2 full initialization
// see: hv.c
    if (number == 22011){
        hv_ps2_full_initialization();
        return NULL;
    }

// #deprecated.
// shared memory 2mb surface.
// ring 3.
    if (number == 22777)
    {

        // #bugbug
        // Is it true?
        // I guess we are using all the extraheaps for slab allocations.
        // This syscall was called to provide shared memory for surfaces 
        // for ring3.

        //if (g_extraheap3_initialized==TRUE)
        //    return (void*) g_extraheap3_va;

        panic("[22777] Deprecated syscall\n");
        return NULL;
    }



// 44000
// #important: We're avoiding the callback support.
// Callback support.
// see: ts.c
// see: pit.c
// arg2 = address
// arg3 = pid
// arg4 = signature
    pid_t ws_pid = -1;
    unsigned long r3_handler=0;
    if (number == 44000)
    {
        // Somente o window server pode chamar esse serviço.
        ws_pid = (pid_t) socket_get_gramado_port(GRAMADO_PORT_WS);
        // Checando se caller foi o window server.
        if (current_process != ws_pid){
            panic("sci2: [44000] current_process!=ws_pid\n");
        }
        // PID
        if (arg3 != ws_pid){
            panic("sci2: [44000] Invalid PID\n");
        }
        // nao foi inicializado pela inicialização do kenrel.
        if ( ds_callback_info.initialized != TRUE ){
            panic("sci2: [44000] callback support Not initialized\n");
        }
        // Se ele ja esta pronto para efetuarmos o iretq
        // é porque tem alguma coisa errada.
        if ( ds_callback_info.ready == TRUE ){
            panic("sci2: [44000] called again\n");
        }
        // Enable for the first time.
        // Configuramos o callback em ts.c.
        r3_handler = (unsigned long) arg2;
 
        //arg4: desired ms
        if (arg4<1 || arg4>1000){
            printk("sci2: [44000] Invalid ms\n");
            panic ("sci2: [44000] Invalid ms\n");
        }
         //setup_callback( (unsigned long) r3_handler, 16 );
        setup_callback( (unsigned long) r3_handler, arg4 );
        return NULL;
    }

// Counter
    __default_syscall_counter++;

// #todo
// Maybe kill the caller. 
// Maybe return.

    panic("sci2: [FIXME] default syscall\n");
    return NULL;
}

// This routine was called by the interrupt handler in x64sc.c.
void *sci3 ( 
    unsigned long number, 
    unsigned long arg2, 
    unsigned long arg3, 
    unsigned long arg4 )
{
// Getting requests from ring3 applications via systemcalls.
// :: Services in mod0.

    struct process_d *p;
    struct thread_d *t;
    int layer = LAYER_UNDEFINED;

    debug_print("sci1: [TODO]\n");

    pid_t current_process = (pid_t) get_current_process();

// #test
// ---------------------------------
    if ( current_thread<0 || current_thread >= THREAD_COUNT_MAX )
    { 
        return NULL;
    }
    t = (struct thread_d *) threadList[current_thread];
    if ((void*) t == NULL)
        return NULL;
    if (t->magic != 1234)
        return NULL;
    // Increment stime.
    // see: x64cont.c
    t->transition_counter.to_supervisor++;
    // Antecipando, ja que fica dificil
    // fazer isso na saida por enquanto.
    t->transition_counter.to_user++;
// ---------------------------------

// permission
    if (current_process<0 || current_process >= PROCESS_COUNT_MAX){
        panic("sci3: current_process\n");
    }
    p = (struct process_d *) processList[current_process];
    if ((void*) p == NULL){
        debug_print("sci3: p\n");
        panic("sci3: p\n");
    }
    if ( p->used != TRUE || p->magic != 1234 ){
        debug_print("sci3: p validation\n");
        panic("sci3: p validation\n");
    }

// The display server was not initialized yet.
    if (DisplayServerInfo.initialized != TRUE){
        return 4321;
    }

// #test
// Only the display server can access this service.
    if (current_thread != DisplayServerInfo.tid)
    {
        // OUT: Access denied.
        return 4321;
    }
// #test
// Only the display server can access this service.
    if (p->pid != DisplayServerInfo.pid)
    {
        // OUT: Access denied.
        return 4321;
    }


//++
//-------------------------------------
// #test
// #todo: This is a work in progress.
// Maybe this interrupt can be used 
// to call the services provided by the first module, mod0.bin.
// see: mod.c and mod.h.

    unsigned long return_value=0;

    if ((void*) kernel_mod0 == NULL)
        return NULL;
    if (kernel_mod0->magic != 1234)
        return NULL;
    if (kernel_mod0->initialized != TRUE)
        return NULL;


// Validation
    if ((void*) kernel_mod0->entry_point == NULL){
        goto fail;
    }

// #test
// Calling the virtual function, and
// getting the return value.

    return_value = 
        (unsigned long) kernel_mod0->entry_point(
            0x83,    // sc? system id.
            number,  // Reason
            arg2,    // l2
            arg3,    // l3
            arg4 );  // l3

// Done
    return (void*) return_value;

//-------------------------------------
//--

fail:
    return NULL;
}

//
// End
//

