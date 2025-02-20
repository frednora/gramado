/*
 * File: rtl.c 
 * Low level code used by all the rtl files.
 * Environment:
 *     Ring3, Gramado userland/.
 * Created by Fred Nora.
 */

// The is the 'os base' interface between the
// ring 3 applications and the kernel.
// + We have the functions that implement the interrupts
// used by the syscalls.
// + We have syscalls requesting kernel sevices.
// + We have some workers doing some services here in ring 3.

// ----------------------
// Functions to exetute the syscalls via interrupts.
//   + 0x80 - gramado_system_call()
//   + 0x80 - sc80()
//   + 0x81 - sc81()
//   + 0x82 - sc82()

#include <stdio.h>
#include <types.h> 
#include <stdarg.h> 
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <rtl/gramado.h> 
#include <sysdeps/gramado/syscall.h>
#include <pthread.h>

// global
// rtl system events support.
// Buffer to save a event. (one msg)
unsigned long RTLEventBuffer[32];

// =========================

// System call.
// Interrupt 0x80.
void *gramado_system_call ( 
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d )
{
// Adapter. System dependent.
    return (void *) sc80(a,b,c,d);
}

// System call.
// Interrupt 0x80.
void *sc80 ( 
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d )
{
// Wisdom
// Adapter. System dependent.
    unsigned long __Ret=0;

    asm volatile ( " int %1 \n"
                 : "=a"(__Ret)
                 : "i"(0x80), "a"(a), "b"(b), "c"(c), "d"(d) );

    return (void *) __Ret; 
}

// System call.
// Interrupt 0x81.
void *sc81 ( 
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d )
{
// Power
// Adapter. System dependent.
    unsigned long __Ret=0;

    asm volatile ( " int %1 \n"
                 : "=a"(__Ret)
                 : "i"(0x81), "a"(a), "b"(b), "c"(c), "d"(d) );

    return (void *) __Ret; 
}

// System call.
// Interrupt 0x82.
void *sc82 ( 
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d )
{
// Love
// Adapter. System dependent.
    unsigned long __Ret=0;

    asm volatile ( " int %1 \n"
                 : "=a"(__Ret)
                 : "i"(0x82), "a"(a), "b"(b), "c"(c), "d"(d) );

    return (void *) __Ret; 
}

// System call.
// Interrupt 0x83.
void *sc83 ( 
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d )
{
// Rest
// Adapter. System dependent.
    unsigned long __Ret=0;

    asm volatile ( " int %1 \n"
                 : "=a"(__Ret)
                 : "i"(0x83), "a"(a), "b"(b), "c"(c), "d"(d) );

    return (void *) __Ret; 
}


// =============================================================

/*

  // #todo
  // Working on that thing.
  // bt, bts, btr, btc ...

inline void rtl_bt(void *bit_field, unsigned long bit_number);
inline void rtl_bt(void *bit_field, unsigned long bit_number) 
{
    asm("bt %1,%0" : "+m" (*bit_field) : "r" (bit_number) );
}

inline void rtl_bts(void *bit_field, unsigned long bit_number);
inline void rtl_bts(void *bit_field, unsigned long bit_number) 
{
    asm("bts %1,%0" : "+m" (*bit_field) : "r" (bit_number) );
}

inline void rtl_btr(void *bit_field, unsigned long bit_number);
inline void rtl_btr(void *bit_field, unsigned long bit_number) 
{
    asm("btr %1,%0" : "+m" (*bit_field) : "r" (bit_number) );
}

inline void rtl_btc(void *bit_field, unsigned long bit_number);
inline void rtl_btc(void *bit_field, unsigned long bit_number) 
{
    asm("btc %1,%0" : "+m" (*bit_field) : "r" (bit_number) );
}

*/


/*
// #test
// Open special devices, given a standard string.
// Ex: 
// "::ps2keyboard"; "::ps2mouse"; 
// "::console0"; "::console1"; "::console2"; "::console3";
// "::fb0"; (display device)
// ...
// #todo
int gramado_open_device(char *string);
int gramado_open_device(char *string)
{
    int value = -1;
    //#todo: Create the implementation in ring0.
    value = (int) sc82(22888,string,string,string);
    if( value < 0 )
    {
        errno = (-value);
        return -1;
    }
    return (int) value;
}
*/

// Input port.
int rtl_get_key_state(int vk)
{
    unsigned long value=0;
    int virtual_key = (int) (vk & 0xFF);

    // Input adapter. System dependent.
    value = 
        (unsigned long) sc80( 
                            138, 
                            virtual_key, 
                            virtual_key, 
                            virtual_key );

    return (int) (value & 0xFFFFFFFF);
}


// #test
// 2mb shared memory surface.
// ring3
void *rtl_shm_get_2mb_surface(void)
{

// #deprecated
// The 22777 syscall is deprecated.

    //void *ptr;
    //ptr = (void*) sc82(22777,0,0,0);
    //return (void*) ptr;

    printf("rtl_shm_get_2mb_surface: Deprecated\n");
    return NULL;
}



// ==========================================
// Exit on fail
// Entry point
// + Passamos esse símbolo para o kernel.
// + O kernel saltará para cá no caso de alguma erro fatal nesse aplicativo.
// + Esse aplicativo roda algumas rotinas de finalização do aplicativo
//   com base nas suas informações guardadas na estrutura app_d
// encontrada em gramado.h na rtl.
// + Por fim essa rotina deve finalizar a thread de controle.

void rtl_elegant_exit_on_fail(void)
{
    debug_print("rtl_elegant_exit_on_fail: #todo\n");
    exit(0);
}

// =============================================================

int rtl_get_input_mode(void)
{
    return (int) gramado_system_call(911,0,0,0);
}

// #todo:
// Explain it better.
void rtl_set_input_mode(int mode)
{
    if (mode < 0)
        return;

    gramado_system_call(
        912,    // What is that?
        mode,
        mode,
        mode );
}

//
// sync
//

//=====================================

void rtl_set_global_sync(int sync_id, int request, int data)
{
    if (sync_id<0)
        return;

    //if (request<0)
        //return;

    sc82 (10002,sync_id,request,data);
}

int rtl_get_global_sync(int sync_id, int request)
{
    //if(fd<0)
    //    return -1;

    //if(request<0)
    //    return -1;

    return (int) sc82 (10003,sync_id,request,0);
}

// ========================


void rtl_set_file_sync(int fd, int request, int data)
{
    if (fd<0)
        return;

    //if (request<0)
        //return;

    sc82 (10006,fd,request,data);
}

int rtl_get_file_sync(int fd, int request)
{
    //if(fd<0)
    //    return -1;

    //if(request<0)
    //    return -1;

    return (int) sc82 (10007,fd,request,0);
}

unsigned char rtl_to_uchar (char ch)
{
    return (unsigned char) ch;
}

unsigned short rtl_to_ushort (short ch)
{
    return (unsigned short) ch;
}

unsigned int rtl_to_uint (int ch)
{
    return (unsigned int) ch;
}

unsigned long rtl_to_ulong (long ch)
{
    return (unsigned long) ch;
}

// #todo
// Explain it better.
// We need a better return value.
// Maybe a boolian 'int' for TRUE or FALSE.
unsigned long rtl_get_system_message(unsigned long message_buffer)
{
// (Input port).
// #todo: Handle the return value.
    //unsigned long res=0;
    if (message_buffer == 0){
        return 0;
    }
// Input adapter. System dependent.
    gramado_system_call ( 
        111,
        (unsigned long) message_buffer,
        (unsigned long) message_buffer,
        (unsigned long) message_buffer );
    return 0;
}

unsigned long 
rtl_get_system_message2(
    unsigned long message_buffer,
    int index,
    int restart)
{
// (Input port).
    if (message_buffer == 0){
        return 0;
    }
// (Input adapter). System dependent.
    gramado_system_call ( 
        120,
        (unsigned long) message_buffer,
        (unsigned long) index,
        (unsigned long) restart );

    return 0;
}

// #todo
// Explain it better.
// We need a better return value.
// Maybe a boolian 'int' for TRUE or FALSE.
unsigned long 
rtl_post_system_message( 
    int dest_tid, 
    unsigned long message_buffer )
{
    //unsigned long return_value=0;
    unsigned long tid=0;

    if (dest_tid < 0){
        return 0;
    }
    tid = (unsigned long) (dest_tid & 0xFFFFFFFF);

    if (message_buffer == 0){
        return 0;
    }

// #todo: Get return value in 'return_value'.
    gramado_system_call ( 
        (unsigned long) 112,
        (unsigned long) tid,
        (unsigned long) message_buffer,
        (unsigned long) message_buffer );

    return 0;
}

//====================================================

void 
rtl_post_to_tid(
    int tid, 
    int msg_code, 
    unsigned long long1, 
    unsigned long long2 )
{
// Send async hello. 44888.

    unsigned long message_buffer[32];

    int target_tid = tid;
    unsigned long msg = (unsigned long) (msg_code & 0xFFFFFFFF);

// Response support.
    //int __src_tid = -1;
    //int __dst_tid = -1;

// The hello message
    message_buffer[0] = 0; //window
    message_buffer[1] = (unsigned long) msg;  // message code
    message_buffer[2] = (unsigned long) long1;   // 
    message_buffer[3] = (unsigned long) long2;   // 
    message_buffer[4] = 0;  // Receiver
    message_buffer[5] = 0;  // Sender

// ---------------------------------
// Post
// Add the message into the queue. In tail.
// IN: tid, message buffer address
    rtl_post_system_message( 
        (int) target_tid, 
        (unsigned long) message_buffer );
}





//=====================================

// Get an event from the thread's event queue.
// That old 'get system message'
// Using a buffer
// #todo
// Let's build another routines that returns a pointer
// for a event structure.
// #todo: helper: xxxScanApplicationQueue()

int xxxScanApplicationQueue(void)
{
    register int i=0;

    for (i=0; i<32; i++){
        RTLEventBuffer[i] = 0;
    };

// Get event from the thread's event queue.
    rtl_enter_critical_section(); 
    rtl_get_system_message( (unsigned long) &RTLEventBuffer[0] );
    rtl_exit_critical_section(); 

// Check if it is a valid event.

// No, we do not have an event. 
// Just clear.
    if (RTLEventBuffer[1] == 0)
    {
        for (i=0; i<32; i++){
            RTLEventBuffer[i] = 0;
        };
        return FALSE;
    }

//#test
//jiffies
    //printf ("jiffies: %d\n",RTLEventBuffer[11]);

// Yes, we have an event.
    return TRUE;
}

int rtl_get_event(void)
{
    int Status = -1;
    
    // #todo: Com esse if podemos selecionar mais de um modelo 
    // para pegar input.
    // if( ....
    
    Status = xxxScanApplicationQueue();

    return (int) Status;
}

//=====================================

// Get an event from the thread's event queue.
// That old 'get system message'
// Using a buffer
// #todo
// Let's build another routines that returns a pointer
// for a event structure.
// #todo: helper: xxxScanApplicationQueue()

int xxxScanApplicationQueue2(int index, int restart)
{
    register int i=0;

    for (i=0; i<32; i++){
        RTLEventBuffer[i] = 0;
    };

// Get event from the thread's event queue.

    rtl_enter_critical_section(); 
    rtl_get_system_message2( 
        (unsigned long) &RTLEventBuffer[0],
        index,
        restart );
    rtl_exit_critical_section(); 


    // Check if it is a valid event.

    // No, we do not have an event. 
    // Yield and clear.
    if ( RTLEventBuffer[1] == 0 )
    {
        for (i=0; i<32; i++){
            RTLEventBuffer[i] = 0;
        };
        return FALSE;
    }

// Yes, we have an event.
    return TRUE;
}

int rtl_get_event2(int index, int restart)
{
    int Status = -1;

    if (index<0){
        return (int) -1;
    }

    // #todo: Com esse if podemos selecionar mais de um modelo 
    // para pegar input.
    // if( ....
    
    Status = xxxScanApplicationQueue2(index,restart);

    return (int) Status;
}

struct rtl_event_d *rtl_next_event (void)
{

// Not a pointer.
    struct rtl_event_d  rtlEvent;

// Clean
    rtlEvent.window = NULL;
    rtlEvent.msg    = 0;
    rtlEvent.long1  = 0;
    rtlEvent.long2  = 0;
    rtlEvent.long3  = 0;
    rtlEvent.long4  = 0;
    rtlEvent.long5  = 0;
    rtlEvent.long6  = 0;

 // Get event from the thread's event queue.
    
// #bugbug
// For this routine the system call needs to respect 
// the limit of this structure. Only 8 elements.

    rtl_enter_critical_section(); 
    gramado_system_call ( 111,
        (unsigned long) &rtlEvent,
        (unsigned long) &rtlEvent,
        (unsigned long) &rtlEvent );
    rtl_exit_critical_section(); 

 // Check if it is a valid event.

// No, we do not have an event. 
// Clean

    if ( rtlEvent.msg == 0 )
    {
        rtlEvent.window = NULL;
        rtlEvent.msg    = 0;
        rtlEvent.long1  = 0;
        rtlEvent.long2  = 0;
        rtlEvent.long3  = 0;
        rtlEvent.long4  = 0;
        rtlEvent.long5  = 0;
        rtlEvent.long6  = 0;
        return NULL; 
    }

// Yes, we have an event.
    return (struct rtl_event_d *) &rtlEvent;
}


// P (Proberen) testar.
// Pega o valor do spinlock principal.
// Se deixou de ser 0 então posso entrar.
// Se ainda for 0, continuo no while.
// Yield thread if we have no message.
// #todo: 
// Mudar o nome da systemcall porque isso não é
// um semáforo e sim um spinlock não atômico.

void rtl_enter_critical_section (void)
{
    int S=0;
    while (TRUE){
        S = (int) gramado_system_call ( 
                      SYSTEMCALL_GET_KERNELSEMAPHORE, 0, 0, 0 );
        if (S == 1){
            goto done;
        }
    };
// Nothing
done:
// #todo
// Muda para zero para que ninguem entre.
    gramado_system_call ( SYSTEMCALL_CLOSE_KERNELSEMAPHORE, 0, 0, 0 );
    return;
}

// V (Verhogen) incrementar.
// Hora de sair. Mudo para 1 para que outro possa entrar.
// #todo: 
// Mudar o nome da systemcall porque isso não é
// um semáforo e sim um spinlock não atômico.
void rtl_exit_critical_section (void)
{
    gramado_system_call ( 
        SYSTEMCALL_OPEN_KERNELSEMAPHORE, 
        0, 
        0, 
        0 );
}

// Create empty file.
// #todo: Change parameter type to const char 
int rtl_create_empty_file(char *file_name)
{
    unsigned long Value=0;

    if ((void*) file_name == NULL){
        debug_print("rtl_create_empty_file: [FAIL] file_name\n");
        return (int) -1;
    }
    if ( *file_name == 0 ){
        debug_print("rtl_create_empty_file: [FAIL] *file_name\n");
        return (int) -1;
    }

    Value = 
        (unsigned long) gramado_system_call ( 
                            43, 
                            (unsigned long) file_name, 
                            0, 
                            0 );

// #todo
// Error message.

    return (int) (Value & 0xF);
}

// Create empty directory.
// #todo: Change parameter type to const char 
int rtl_create_empty_directory(char *dir_name)
{
    unsigned long Value=0;

    if ((void*) dir_name == NULL){
        debug_print("rtl_create_empty_directory: [FAIL] dir_name\n");
        return (int)(-1);
    }
    if (*dir_name == 0){
        debug_print("rtl_create_empty_directory: [FAIL] *dir_name\n");
        return (int)(-1);
    }

// #todo
// Quais são os valores de retorno.
// TRUE or FALSE ?

    Value = 
        (unsigned long) gramado_system_call ( 
                            44, 
                            (unsigned long) dir_name, 
                            0, 
                            0 );

// #todo
// Error message

    return (int) (Value & 0xF);
}

void *rtl_create_process(const char *file_name)
{
    char pName[32];

    debug_print("rtl_create_process:\n #todo\n");
    //return NULL;
    
    strncpy(pName,file_name,16);
    pName[17] = 0;
    pName[31] = 0;

// Create process
// Retorna o ponteiro para uma estrutura de processo
// que esta em ring0, ou NULL.

    return (void*) gramado_system_call( 
        73, 
        (unsigned long) &pName[0],
        3,  //priority
        0 );
}

int rtl_start_process(void *process)
{
    debug_print("rtl_start_process:\n #todo\n");
    return -1;
}

/*
pid_t rtl_get_process_pid( void *process );
pid_t rtl_get_process_pid( void *process )
{
}
*/


/*
int rtl_start_process_pid( pid_t pid );
int rtl_start_process_pid( pid_t pid )
{
}
*/


/*
 * rtl_create_thread:
 *     Create a thread.
 *     #todo: 
 *     Precisamos uma função que envie mais argumentos.
 *     Essa será uma rotina de baixo nível para pthreads.
 */

void *rtl_create_thread ( 
    unsigned long init_rip, 
    unsigned long init_stack, 
    char *name )
{
    //#define	SYSTEMCALL_CREATETHREAD     72
    debug_print ("rtl_create_thread:\n");
    return (void *) gramado_system_call ( 
                        72,    //SYSTEMCALL_CREATETHREAD, 
                        init_rip, 
                        init_stack, 
                        (unsigned long) name );
}


/*
 * rtl_start_thread:
 *     Coloca no estado standby para executar pela primeira vez
 */

// #todo: Change return to 'int'
void rtl_start_thread (void *thread)
{
    debug_print ("rtl_create_thread:\n");
    gramado_system_call ( 
        SYSTEMCALL_STARTTHREAD, 
        (unsigned long) thread, 
        (unsigned long) thread, 
        (unsigned long) thread );
}


/*
int rtl_get_thread_tid( void *thread);
int rtl_get_thread_tid( void *thread)
{
}
*/


/*
int rtl_start_thread_tid(int tid);
int rtl_start_thread_tid(int tid)
{
}
*/


/* Get the next token in a string, 
 * returning a pointer to the byte following the token.
 */
char *rtl_get_next_token_in_a_string(
    char *buf,           // Input string pointer. 
    char *w,             // Output token string pointer.
    size_t buffer_size,  // The buffer size.
    size_t *size )       // Output to return an int value. The counter.
{

// #todo
// Not tested yet!

	*w = 0;
	*size = 0;

// Fail
    if ((void*) buf == NULL){
        goto fail;
    }
    if ((void*) w == NULL){
        goto fail;
    }
    if ((void*) size == NULL){
        goto fail;
    }


    size_t BufferSize = strlen(buf);
    if (BufferSize > buffer_size)
        goto fail;

// Pula os espaços iniciais se tiver.
	while (isspace(*buf))
	{
	    buf++;
	};

// Se depois dos espaços temos um 0.
// o size ainda eh 0.
	if (*buf == '\0')
	{
		return NULL;
    }

// Depois dos espaços iniciais pegamos os chars que nao sao espaços.
// Colocando eles no retorno.
    size_t Conter=0;
	while (!isspace(*buf) && *buf)
	{
		*w++ = *buf++;
	    Conter++;
	};

//  finaliza o retorno.
	*w = 0;
// The token size.
	*size = Conter;

//retorna o endereço do ultimo bytes encontrado.
	return (char *) buf;
fail:
    return NULL;
}


// Vamos escrever em uma janela indefinida. NULL.
// provavelmente a janela principal.
// #todo: Change string to 'const char *'
// #todo: Change the type for color parameter. Use 'unsigned int' instead.

int 
rtl_draw_text ( 
    unsigned long x, 
    unsigned long y, 
    unsigned long color, 
    char *string )
{
// ??

    unsigned long msg[8];

    if ( (void*) string == NULL ){
        debug_print("rtl_draw_text: string\n");
        return -1;
    }
    if ( *string == 0 ){
        debug_print("rtl_draw_text: *string\n");
        return -1;
    }

    msg[0] = (unsigned long) 0;  // window;
    msg[1] = (unsigned long) x;
    msg[2] = (unsigned long) y;
    msg[3] = (unsigned long) color;   //#todo
    msg[4] = (unsigned long) string;
    msg[5] = (unsigned long) 0;
    msg[6] = (unsigned long) 0;
    msg[7] = (unsigned long) 0; 

    return (int) gramado_system_call ( 
                     SYSTEMCALL_DRAWTEXT, 
                    (unsigned long) &msg[0], 
                    (unsigned long) &msg[0], 
                    (unsigned long) &msg[0] );
}

/*
 * rtl_show_backbuffer:
 *     Refresh Screen.
 *     Passa o conteúdo do backbuffer para o lfb. 
 */
void rtl_show_backbuffer (void)
{

// #todo
// trocar o nome dessa systemcall.
// refresh screen será associado à refresh all windows.
    
    gramado_system_call ( SYSTEMCALL_REFRESHSCREEN, 0, 0, 0 );
}


/*
 * rtl_get_system_metrics:
 *     Obtem informações sobre dimensões e posicionamentos. 
 *     #importante
 */

unsigned long rtl_get_system_metrics (int index)
{
    //if (index<0){
        //gde_debug_print ("gde_get_system_metrics: fail\n");
        //return 0;
    //}

    return (unsigned long) gramado_system_call ( 
                               SYSTEMCALL_GETSYSTEMMETRICS, 
                               (unsigned long) index, 
                               (unsigned long) index, 
                               (unsigned long) index );
}

int rtl_is_qemu(void)
{
    int isQEMU=-1;
    isQEMU = (int) rtl_get_system_metrics(300);
    //isVirtualBox = rtl_get_system_metrics(?);
    //isBochs      = rtl_get_system_metrics(?);

    return (int) (isQEMU & 0xFFFFFFFF);
}



pid_t rtl_current_process(void)
{
    return (int) rtl_get_system_metrics(140);
}

int rtl_current_thread(void)
{
    return (int) rtl_get_system_metrics(141);
}

// #todo: __pthread_self ?
pthread_t pthread_self(void)
{
    return (pthread_t) rtl_current_thread();
}

// #bugbug: 118 is the current jiffie.
unsigned long rtl_get_progress_time(void)
{
    return (unsigned long) rtl_get_system_metrics (118);
    //return (unsigned long) rtl_get_system_metrics (?);
}

/*
void XorSwap( int* x, int* y ); 
void XorSwap( int* x, int* y ) 
{
  if (x != y)
  {
    *x ^= *y;
    *y ^= *x;
    *x ^= *y;
  }
}
*/


/*
void AddSwap( unsigned int* x, unsigned int* y );
void AddSwap( unsigned int* x, unsigned int* y )
{
  if (x != y)
  {
    *x = *x + *y;
    *y = *x - *y;
    *x = *x - *y;
  }
}
*/


/*
 * rtl_copy_text:
 *      Copy a string of bytes given the source, the destination,
 * the width and the height.
 *      It copies in chuncks of 64 bytes.
 *    #?? doom style.
 */

// #bugbug
// Not tested yet.

int 
rtl_copy_text ( 
    unsigned long src, 
    unsigned long dest, 
    int width, 
    int height )
{
    char *__src  = (char *) src;
    char *__dest = (char *) dest; 
// text width and text height.
    int __width  = width;
    int __height = height;
    int x=0;
    int y=0; 

    if ( (void *) src == NULL || 
         (void *) dest == NULL )
    {
        printf("rtl_copy_text: src dest\n");
        return -1;
    }

    if ( width <= 0 || height <= 0 )
    {
        printf("rtl_copy_text: width height\n");
        return -1;
    }

    for ( y=0; y< __height; y++ ) 
    { 
        for (x=0 ; x<__width/64 ; x++) 
        { 
            memcpy ( __dest, __src + ((y&63)<<6), 64 );
            __dest += 64; 
        };
        if (__width & 63) 
        { 
            memcpy (__dest, __src + ((y&63)<<6), __width&63 ); 
            __dest += (__width&63); 
        } 
    };

    return -0; 
}


/*
char *johncarmack_strstr(const char *haystack, const char *needle);
char *johncarmack_strstr(const char *haystack, const char *needle)
{
	// ...
}
*/

/*
int rtl_file_length (FILE *f);
int rtl_file_length (FILE *f)
{
	int		pos;
	int		end;

	pos = ftell (f);
	fseek (f, 0, SEEK_END);
	end = ftell (f);
	fseek (f, pos, SEEK_SET);

	return end;
}
*/


/*
int rtl_file_exists (const char *filename);
int rtl_file_exists (const char *filename)
{
	FILE *f;

	// [RH] Empty filenames are never there
	if (*filename == 0)
		return FALSE;

	f = fopen (filename, "r");
	if (!f)
		return FALSE;
	fclose (f);
	return TRUE;
}
*/

// #todo
// We need some arguments here.
int rtl_reboot(void)
{
    int value = -1;
    debug_print ("rtl_reboot:\n");
    value = (int) sc80(110,0,0,0);
    if (value<0)
    {
        errno = (-value);
        return (int) -1;
    }
    return (int) value;
}

// check if a file is full or not.
// whe can't read an empty file.
// IN: fd
// OUT: -1= error; FALSE= nao pode ler; TRUE= pode ler.
int rtl_sleep_if_socket_is_empty(int fd)
{
    if (fd<0){
        return -1;   //error
    }
    return (int) gramado_system_call(913,fd,fd,fd);
}

/*
void rtl_set_line_buffering(void);
void rtl_set_line_buffering(void)
{
    setvbuf(stdout, NULL, _IOLBF, 0);
}
*/

/*
 credits: apple chameleon boot loader.
uint16_t rtl_swap16(uint16_t toswap) {
	return (((toswap & 0x00FF) << 8) | ((toswap & 0xFF00) >> 8));
}

uint32_t rtl_swap32(uint32_t toswap) {
	return  ((toswap & 0x000000FF) << 24) |
	((toswap & 0x0000FF00) << 8 ) |
	((toswap & 0x00FF0000) >> 8 ) |
	((toswap & 0xFF000000) >> 24);
}	

uint8_t  rtl_read8(uint8_t *ptr, uint16_t offset) { 
	return ptr[offset];
}

uint16_t rtl_read16(uint8_t *ptr, uint16_t offset) {
	uint8_t ret[2];
	ret[0] = ptr[offset+1];
	ret[1] = ptr[offset];
	return *((uint16_t*)&ret);
}

uint32_t rtl_read32(uint8_t *ptr, uint16_t offset) {
	uint8_t ret[4];
	ret[0] = ptr[offset+3];
	ret[1] = ptr[offset+2];
	ret[2] = ptr[offset+1];
	ret[3] = ptr[offset];
	return *((uint32_t*)&ret);
}
*/


/*
int rtl_is_separator (int c);
int rtl_is_separator (int c) 
{
    if ((c == '/') || (c == '\\')){
        return TRUE;
    }else{
        return FALSE;
    };
}
*/


/*
//caracteres iguais em duas strings,
//consecutivos, da esquerda para direita.
int 
rtl_StrCommonSubset(
    unsigned char *a, 
    unsigned char *b );
int 
rtl_StrCommonSubset(
    unsigned char *a, 
    unsigned char *b ) 
{

    int len=0;

    while (*a && *b == *a) 
    {
        a++;  b++;  len++;
    };
    
    return (int) len;
}
*/


/*
int rtl_IsPath(unsigned char *str);
int rtl_IsPath(unsigned char *str)
{
  for (; *str; str++) {
    if (*str == '/')
      return TRUE;
  }
  return FALSE;
}
*/


/*
//credits: 8188eu driver
static int hex2num_i(char c);
static int hex2num_i(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}
*/


/*
 * hwaddr_aton - Convert ASCII string to MAC address
 * @txt: MAC address as a string (e.g., "00:11:22:33:44:55")
 * @addr: Buffer for the MAC address (ETH_ALEN = 6 bytes)
 * Returns: 0 on success, -1 on failure (e.g., string not a MAC address)
 */

/*
//credits: 8188eu driver
static int hwaddr_aton_i(const char *txt, u8 *addr);
static int hwaddr_aton_i(const char *txt, u8 *addr)
{
	int i;

	for (i = 0; i < 6; i++) {
		int a, b;

		a = hex2num_i(*txt++);
		if (a < 0)
			return -1;
		b = hex2num_i(*txt++);
		if (b < 0)
			return -1;
		*addr++ = (a << 4) | b;
		if (i < 5 && *txt++ != ':')
			return -1;
	}

	return 0;
}
*/


/*
void *rtl_zalloc (size_t size);
void *rtl_zalloc (size_t size)
{
    if( size<0 ){ return NULL; };
    
    void *n = (void *) malloc(size);

    if (n){
        memset (n, 0, size);
    }

    return n;
}
*/


/*
void *rtl_realloc_raw (void *p, int sz);
void *rtl_realloc_raw (void *p, int sz)
{
    // arg error 1:
    if ( (void*) p == NULL ){
        return (void *) malloc(sz);
    }
    
    // arg error 2:
    if (sz == 0)
    { 
        free(p); 
        return NULL; 
    }
    
    // arg ok:
    return (void *) realloc(p,sz);
}
*/


/*
static char *rtl__filename;
static int   rtl__fileline;
int rtl_record_fileline (const char *f, int n);
int rtl_record_fileline (const char *f, int n)
{
    rtl__filename = (char *) f;
    rtl__fileline = n;
    return 0;
}
*/


/*
void *rtl_copy_bytes(void *p, size_t sz);
void *rtl_copy_bytes(void *p, size_t sz)
{
   void *q = (void *) malloc(sz);
   memcpy(q, p, sz);
   return q;
}
*/


/*
// 2020 - Created by Fred Nora.
void *rtl_copy_bytes_offset(void *p, int offset, size_t sz);
void *rtl_copy_bytes_offset(void *p, int offset, size_t sz)
{
   void *q = (void *) malloc(sz);
   memcpy (q, p+offset, sz);
   return q;
}
*/


/*
// 2020 - Created by Fred Nora.
void *rtl_copy_bytes_offset_origin(void *p, int offset, size_t sz);
void *rtl_copy_bytes_offset_origin(void *p, int offset, size_t sz)
{
   void *q = (void *) malloc(sz);
   memcpy (q, p+offset, sz);
   
   //Return the origin.
   return (void *) (p+offset);
   //return q;
}
*/


/*
// 2020 - Created by Fred Nora.
int rtl_getchar_offset (char *ch, void *p, int offset);
int rtl_getchar_offset (char *ch, void *p, int offset)
{
    if ( (void*) p == NULL ){
        *ch = 0;
        return FALSE;
    }else{
        *ch = p[offset];
        return TRUE; //ok
    };
    return FALSE;
}
*/


/*
 //created by fred nora.
void rtl_free_environ (void *address, int len);
void rtl_free_environ (void *address, int len)
{
    void **____environ = (void **) address;
    
    register int i=0;
   
    for (i=0; i<len; i++)
    {
        free(____environ[i]);
    };
}
*/


/*
 //2020 - created by fred nora.
void rtl_free_environ_element (void *address, int element);
void rtl_free_environ_element (void *address, int element)
{
    void **____environ = (void **) address;
    
    if ( element<0)
        return;
    
    free (____environ[element]);
}
*/


/*
// 2020 - created by fred nora.
void rtl_free_environ_elements (void *address, int start, int end);
void rtl_free_environ_elements (void *address, int start, int end)
{
    void **____environ = (void **) address;
    
    register int i=0;
   
    if ( start<end )
        return;
    
    //if ( start == end )
    //    return;
    
   
    for (i=start; i<end; i++)
    {
        free(____environ[i]);
    };
}
*/


/*
// dafuck
int number_status ( unsigned long number );
int number_status ( unsigned long number )
{
    if (number < 2020){
         return NORMAL;
    }else if (number == 2020){
         return NOT_NORMAL;
    }else{
         return NEW_NORMAL;
    };
}
*/


/*
static int rtl__memcmpoffset, rtl__memcmpsize;
int rtl_memcmp_offset (const void *a, const void *b)
{
   return (int) memcmp (
                    (char *) a + rtl__memcmpoffset, 
                    (char *) b + rtl__memcmpoffset, 
                    rtl__memcmpsize );
}
*/


/*
//2020 - created by fred nora
int rtl_memcmp_offset2 (const void *a, const void *b, int offset, size_t size);
int rtl_memcmp_offset2 (const void *a, const void *b, int offset, size_t size)
{
   if (offset<0){ return -1; }
   if (size<0){ return -1; }
   return (int) memcmp (
                    (char *) a + offset, 
                    (char *) b + offset, 
                    size );
}
*/


/*
void rtl_string_tolower (char *s);
void rtl_string_tolower (char *s)
{
    while (*s) 
    {
        *s = tolower(*s);
        s++;
    };
}
*/

/*
// 2020 - created by fred nora.
void rtl_string_offset_tolower (char *s, int offset);
void rtl_string_offset_tolower (char *s, int offset)
{
    if( offset<0)
        return;

    char *String = (char *) (s+offset);

    while (*String) 
    {
        *String = tolower(*String);
        String++;
    };
}
*/


/*
void rtl_changepath_to_right(char *path);
void rtl_changepath_to_right(char *path)
{
    for (; *path; ++path)
    {
        if (*path == '\\'){  *path = '/';  }
    };
}
*/


/*
void rtl_changepath_to_left(char *path);
void rtl_changepath_to_left(char *path)
{
    for (; *path; ++path)
    {
        if (*path == '/'){  *path = '\\';  }
    };
}
*/


/*
//2020 - created by fred nora.
void rtl_changepath_to_dash(char *path);
void rtl_changepath_to_dash(char *path)
{
    for (; *path; ++path)
    {
        if (*path == '/' || *path == '\\')
        {  
            *path = '-';  
        }
    };
}
*/


/*
//2020 - created by fred nora.
void rtl_changepath_to_underline(char *path);
void rtl_changepath_to_underline(char *path)
{
    for (; *path; ++path)
    {
        if (*path == '/' || *path == '\\')
        {  
            *path = '_';  
        }
    };
}
*/


/*
//2020 - created by fred nora.
void rtl_changepath_to_star(char *path);
void rtl_changepath_to_star(char *path)
{
    for (; *path; ++path)
    {
        if (*path == '/' || *path == '\\')
        {  
            *path = '*';  
        }
    };
}
*/


/*
 // 2020 - created by fred nora,
//write from 'ptr' to 'f'.
void rtl_fwrite32(const void *ptr, FILE *f);
void rtl_fwrite32(const void *ptr, FILE *f)
{
    //Fred's 'caltela'.
    if ( (void *) ptr == NULL ){ return; }
    if ( (void *) f   == NULL ){ return; }
    
    fwrite (&ptr, 4, 1, f);
}
*/


/*
//2020 - created by fred nora
ssize_t rtl_write_out (const void *data);
ssize_t rtl_write_out (const void *data)
{
    //stdout
    return (ssize_t) write (1, (const void *) data, (size_t) sizeof(data) );
}
*/


/*
//2020 - created by fred nora
ssize_t rtl_write_err (const void *data);
ssize_t rtl_write_err (const void *data)
{
    //stderr
    return (ssize_t) write (2, (const void *) data, (size_t) sizeof(data) );
}
*/


/*
char *rtl_fgets2(char *buffer, int buflen, FILE *f);
char *rtl_fgets2(char *buffer, int buflen, FILE *f)
{
    char *p;
    buffer[0] = 0;
    register int n=0;

    
    // Fred's caltela.
    if( (void*) buffer == NULL ){ return NULL; }
    if(buflen<0){ return NULL; }
 
    // fgets
    p = fgets(buffer, buflen, f);
    
    if (p) 
    {
        n = (int) ( strlen(p) - 1 );
        
        // finaliza.
        if (n >= 0)
        {
            if (p[n] == '\n'){ p[n] = 0; }
        }
    }
    
    return p;
}
*/


/*
//learning ...
// ?? what??
static int isdotdirname(char *name);
static int isdotdirname(char *name)
{
   if (name[0] == '.')
      return (name[1] == '.') ? !name[2] : !name[1];
   return 0;
}
*/


/*
// do a getc, but give error message if EOF encountered.
int rtl_safegetc(FILE *f)
int rtl_safegetc(FILE *f)
{
    register int c = getc(f);
    
    if (c == EOF){
        printf ("rtl_safegetc: Unexpected end of file\n");
        exit(1);
    }
    return c;
}
*/


/*
//Generate a string from the integer I.
//Return a ptr to internal memory containing the string.  
char *rtl_int_to_string (int i);
char *rtl_int_to_string (int i)
{
    static char buf[20];
    
    sprintf(buf, "%d", i);
    return buf;
}
*/


/*
int snprintf2(char *str, size_t size, const char *format, ...);
int snprintf2(char *str, size_t size, const char *format, ...)
{
    va_list ap;
    int rv;

    va_start(ap, format);
    rv = vsnprintf(str, size, format, ap);
    va_end(ap);

    return rv;
}
*/


/*
int rtl_eq (char *a, char *b);
int rtl_eq (char *a, char *b)
{
    int i=0;

l:
    // Se for diferente.
    if(a[i] != b[i])
        return (0);

    // Se terminou a string.
    if (a[i++] == '\0')
        return (1);

    goto l;
}
*/



/*
 //ugly
char *rtl_ptsname2(int fd);
char *rtl_ptsname2(int fd)
{
  static char buffer[32];  // Big enough to hold even a 64-bit pts no 
  unsigned int ptyno;

  if ( ioctl(fd, TIOCGPTN, &ptyno) )
    return NULL;
  
    snprintf(buffer, sizeof buffer, "/dev/pts/%u", ptyno);
  
    return buffer;
}
*/


/*
int rtl_unlockpt2(int fd);
int rtl_unlockpt2(int fd)
{
  int unlock = 0;

  return ioctl(fd, TIOCSPTLCK, &unlock);
}
*/


/*
//Basically getdelim() with the delimiter hard wired to '\n'.
ssize_t rtl_getline2 (char **linebuf, size_t *n, FILE *file);
ssize_t rtl_getline2 (char **linebuf, size_t *n, FILE *file)
{
    return (getdelim (linebuf, n, '\n', file));
}
*/


/*
int rtl__libc_tcdrain(int fd);
int rtl__libc_tcdrain(int fd)
{
  return ioctl(fd, TCSBRK, 1);
}
*/


/*
int rtl__libc_tcflush2(int fd, int queue_selector);
int rtl__libc_tcflush2(int fd, int queue_selector)
{
  return ioctl(fd, TCFLSH, queue_selector);
}
*/


/*
int rtl__write1 (const char* s);
int rtl__write1 (const char* s) 
{
    return write (1, s, strlen(s));
}
*/


/*
int rtl__write2 (const char* s); 
int rtl__write2 (const char* s) 
{
    return write (2, s, strlen(s));
}
*/


/*
    path           dirname        basename
    "/usr/lib"     "/usr"         "lib"
    "/usr/"        "/"            "usr"
    "usr"          "."            "usr"
    "/"            "/"            "/"
    "."            "."            "."
    ".."           "."            ".."
*/


/*
// not tested
// credits: dietlibc-0.20
char *rtl_get_basename (char *path);
char *rtl_get_basename (char *path) 
{
    char *c;

again:
    if ( !( c = strrchr(path,'/') ) ) 
        return path;
    
    if (c[1]==0) 
    {
        if (c == path){
            return c;
        } else {
            *c=0;
            goto again;
        };
    }
    
    return c+1;
}
*/


/*
char * 
index (s, c)
char *s, c;
{
    return ((char *) strchr (s, c));
}
*/


/*
char * 
rindex (s, c)
char *s, c;
{
    return ((char *) strrchr (s, c));
}
*/


/*
static unsigned short rtl_b2u16 (unsigned char c0, unsigned char c1);
static unsigned short rtl_b2u16 (unsigned char c0, unsigned char c1)
{
    return (unsigned short) ( c0 | (c1 << 8) );
}
*/


/*
static unsigned long rtl_b2u32 ( unsigned char c0, unsigned char c1, unsigned char c2, unsigned char c3 );
static unsigned long rtl_b2u32 ( unsigned char c0, unsigned char c1, unsigned char c2, unsigned char c3 )
{
    return (unsigned long)( ((unsigned long) c0      ) | 
                            ((unsigned long) c1 <<  8) | 
                            ((unsigned long) c2 << 16) | 
                            ((unsigned long) c3 << 24) );
}
*/


/*
int rtl_GetTimeInMillis(void);
int rtl_GetTimeInMillis(void)
{
    struct timeval  tp;

    gettimeofday (&tp, 0);
    
    // #bugbug
    // Why 'int' ?
    // is it unsigned long?
    
    return (int) (tp.tv_sec * 1000) + (tp.tv_usec / 1000);
}
*/


/*
 #danger
int rtl_vector_count (char **vector);
int rtl_vector_count (char **vector)
{
    int i     =0;
    int count =0;

    // #danger!!!
    if (vector == NULL) 
        return 0;

    for (i=0; vector[i] != NULL; i++){  count++;  }

    return (int) count;
}
*/



void rtl_test_pipe (void)
{
    int pipefd[2];
    int res=0;
    char buf[512];
    int nwrite=0;
    int nread=0;


    printf ("rtl_test_pipe:\n");

    //0 if no error.
    res = pipe (pipefd);
    if (res<0){
        printf("pipe() fail\n");
        return;
    }

    printf("PIPES: %d %d\n",pipefd[0], pipefd[1]);
 
    // write on pipe 1
    nwrite = write (pipefd[1], "hello", sizeof ("hello"));
    if (nwrite<=0){
        printf("write() fail\n");
        return;
    }
    // clear buffer.
    memset (buf, 0, sizeof (buf));

// Read pipe 0.
    nread = (int) read (
                      pipefd[0], 
                      buf, 
                      sizeof(buf) - 1 );

    if (nread<0){
        printf("read() fail\n");
        return;
    }
    // finalize the string.  
    buf[nread] = '\0';

    // Close
    close (pipefd[0]);
    close (pipefd[1]);

    // show buffer.
    printf("BUFFER={%s}\n",buf);

    return;
}


// =========================
// path count
// Credits: Sirius OS.
// #todo: Change to 'const char *'

size_t rtl_path_count (unsigned char *path)
{
    size_t Value = 0;
    int i=0;
    int max = (80*25);

    if ( (void*) path == NULL ){
        printf ("rtl_path_count: [FAIL] path\n");
        return (size_t) -1;
    }

    if (*path == 0){
        printf ("rtl_path_count: [FAIL] *path\n");
        return (size_t) -1;
    }

    for ( i=0; i < max; i++ )
    {
        if (path[i] == '/') {  Value++;  }
        if (path[i] == '\0'){  break;  }
    };

    return (size_t) Value;
}


int 
rtl_load_path ( 
    char *path, 
    unsigned long buffer, 
    unsigned long buffer_len )
{
    int status = -1;

    if ( (void*) path == NULL ){
        printf ("rtl_load_path: [FAIL] path\n");
        return (int) -1;
    }

    if (*path == 0){
        printf ("rtl_load_path: [FAIL] *path\n");
        return (int) -1;
    }


    //if ( buffer = 0 )
    //{
        // msg
    //    return -1;
    //}

    status = (int) gramado_system_call ( 4004, 
                       (unsigned long) path, 
                       (unsigned long) buffer, 
                       (unsigned long) buffer_len );

    return (int) status;
}


ssize_t rtl_console_beep(void)
{
    char BeepChar = '\a';
    return (ssize_t) write ( 
                         fileno(stdout), 
                         (const void *) &BeepChar, 
                         1 );
}

// Raising up the broken to life.
void rtl_broken_vessels(void)
{
    sc82( 777, 0, 0, 0 );
}

// OUT: Child's PID.
int rtl_clone_and_execute(char *name)
{
    if ((void *) name == NULL){
        printf ("rtl_clone_and_execute: [FAIL] name\n");
        return (int) -1;
    }
    if ( *name == 0 ){
        printf ("rtl_clone_and_execute: [FAIL] *name\n");
        return (int) -1;
    }

    //rewind(stdin);
    //fprintf(stdin,"%s",name);
    //fprintf(stdin,"One Two Three ...");

// #todo
// Parameters vector.
// Maybe we can provide a default parameters vector.
// Maybe we can send a raw command line.

    return (int) sc82( 900, (unsigned long) name, 0, 0 );
}


int rtl_spawn_process( const char *path )
{
    if ( (void *) path == NULL ){
        printf ("rtl_spawn_process: [FAIL] name\n");
        return -1;
    }
    if (*path == 0){
        printf ("rtl_spawn_process: [FAIL] *name\n");
        return -1;
    }

    return (int) rtl_clone_and_execute((char*)path);
}

// get current thread
// set foreground thread.
int rtl_focus_on_this_thread(void)
{
    int tid = (int) pthread_self();
    sc82 (10011,tid,tid,tid);

    return (int) tid;
}

int rtl_focus_on_me(void)
{
    return (int) rtl_focus_on_this_thread();
}

// Yield this thread.
// Wait for the next round.
void rtl_yield(void)
{
    gramado_system_call (265,0,0,0);
}

// This is a worker called bu rtl_sleep()
// Mayme we can have more parameters here
// allowing us to create more different sort of wrappers.
void rtl_sleep_until(unsigned long ms)
{
    if (ms==0){
        ms=1;
    }
    sc82( 266, ms, ms, ms );
}

void rtl_sleep(unsigned long ms)
{
    rtl_sleep_until(ms);
}

// The whole screen is dirty.
// It can be flushed into the framebuffer.
void rtl_invalidate_screen(void)
{
    gramado_system_call (896,0,0,0); 
}

//
// Heap info
//

extern unsigned long HEAP_START;
extern unsigned long HEAP_END;

// lib lobal heap.
void rtl_show_heap_info(void)
{
    printf ("heap: Start=%x End=%x\n",
        HEAP_START,
        HEAP_END );
}

// Use the kernel allocator for ring 3 shared memory.
void *shAlloc(size_t size_in_bytes)
{
    if (size_in_bytes==0){
        size_in_bytes++;
    }
    return (void*) gramado_system_call (891,size_in_bytes,0,0); 
}

/* compare two ASCII strings ignoring case */
// #todo: toupper support.
/*
int 
rtl_strcaseequal (
    const char *d, 
    const char *s );
int 
rtl_strcaseequal (
    const char *d, 
    const char *s )
{
    char ch=0;

    while ((ch = toupper(*s++)) == toupper(*d++))
        if (ch == '\0')
            return 1;

    return 0;
}
*/


/*
int     __argc_save;
char  **__argv_save;
int rtl_check_parm (char *check);
int rtl_check_parm (char *check)
{
    int i=0;

    for (i = 1;i<__argc_save;i++)
    {
        if ( !strcasecmp(check, __argv_save[i]) ){
            return (int) i;
        }
    };

    return 0;
}
*/


/*
int rtl_ipow (int base, int exp);
int rtl_ipow (int base, int exp)
{
    int Result = 1;

    for (;;)
    {
        if (exp & 1){
            Result *= base;
        }

        exp >>= 1;
        
        if (!exp){  break;  }
        
        base *= base;
    };

    return (int) Result;
}
*/


/*
uint32_t uipow (uint32_t base, uint32_t exp);
uint32_t uipow (uint32_t base, uint32_t exp)
{

    uint32_t i=0;
    uint32_t Result = base;


    if (exp == 0) {
        return 1;
    }

    for ( i=1; i < exp; i++ ) 
    {
        Result *= base;
    };

    return (uint32_t) Result;
}
*/


unsigned long rtl_jiffies(void)
{
    return (unsigned long) rtl_get_system_metrics (118);
}

unsigned long rtl_memory_size_in_kb(void)
{
    return (unsigned long) rtl_get_system_metrics (33);
}

// #nottested
// Send the command lint to stdin,
// and execute a cloned process where it's name
// is in the first word of the cmdline string.
int rtl_execute_cmdline( char *cmdline )
{
    char cmd[512];
    char filename_buffer[12]; //8+3+1
    char *p;
    int ii=0;    

    if ( (void*) cmdline == NULL )
        return -1;

    if (*cmdline == 0)
        return -1;

    p = cmdline;

// copy cmdline
    int i=0;
    for (i=0; i<512; i++)
    {
        cmd[i] = cmdline[i];
    };
    cmd[511]=0;

// Send command line.
    rewind(stdin);
    write(fileno(stdin), cmd, 512);

// Grab the first word.
    while (1)
    {
        if (ii>=12){
            filename_buffer[ii] = 0;  //finalize
            break;
        }
        if ( *p == 0 || 
             *p == ' ' ||
             *p == '\t' )
        {
            filename_buffer[ii] = 0;  //finalize
            break;
        }
        
        // printable.
        // Put the char into the buffer.
        if ( *p >= 0x20 && *p <= 0x7F )
        {
            filename_buffer[ii] = (char) *p;
        }
        p++;    // next char in the command line.
        ii++;   // next byte into the filename buffer.
    };

// Clone and execute.
    rtl_clone_and_execute(filename_buffer);

    return 0;
}

/*
int rtl_open_device(char *dev_number);
int rtl_open_device(char *dev_number)
{
    int value = -1;

    char dev_name[10];
    strcpy(dev_name, "DEV");
    strcat(dev_name, dev_number);
    
    value = (int) open(dev_name, O_RDWR|O_NDELAY);
    
    if(value < 0)
    {
        errno = (int)(-value);
        return (int) (-1);
    }
    
    return (int) value;
}
*/


int rtl_swap32(int *x, int *y)
{
    int temp=0;

// Invalid buffer.
    if ( (void*) x == NULL ){return -1;}
    if ( (void*) y == NULL ){return -1;}

    temp = (int) *x;
    *x   = (int) *y;
    *y   = (int) temp;

    return 0;
}

int rtl_swap64(long *x, long *y)
{
    long temp=0;

// Invalid buffer.
    if ( (void*) x == NULL ){return -1;}
    if ( (void*) y == NULL ){return -1;}

    temp = (long) *x;
    *x   = (long) *y;
    *y   = (long) temp;

    return 0;
}

//#todo: Not tested yet.
long rtl_round_up(long num_to_round, long multiple)
{
    long Result=0;
    long Remainder=0;

    if (multiple == 0){
        return (long) num_to_round;
    }
    
    Remainder = (long) (abs(num_to_round) % multiple);
    if (Remainder == 0){
        return (long) num_to_round;
    }

    if (num_to_round < 0){
        Result = (long) -(abs(num_to_round) - Remainder);
    }else{
        Result = (long) (num_to_round + multiple - Remainder);
    };
    
    return (long) Result;
}

