// gns.c
// This is a client-side library for the GNSSRV.
// Created by Fred Nora.

#include <types.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <heap.h>
#include <sys/socket.h>
#include <rtl/gramado.h>

#include "include/gns.h"

#define SIZE_DONT_CHANGE  512
static char __gns_buffer[SIZE_DONT_CHANGE];


//========================
static void __gns_clear_msg_buff(void);


//=================================
static int __gns_hello_request(int fd);
static int __gns_hello_response(int fd);

//=================================


static void __gns_clear_msg_buff(void)
{
    register int i=0;
    for (i=0; i<512; i++){
        __gns_buffer[i] = 0;
    }; 
}


// System call.
// 0x80 System interrupt.
void *gns_system_call ( 
    unsigned long a, 
    unsigned long b, 
    unsigned long c, 
    unsigned long d )
{
/*
    unsigned long __Ret=0;
    asm volatile ( " int %1 \n"
                 : "=a"(__Ret)
                 : "i"(0x80), "a"(a), "b"(b), "c"(c), "d"(d) );
    return (void *) __Ret; 
*/
    return (void*) sc80(a,b,c,d);
}

// Debug vai serial port. (COM1)
void gns_debug_print(char *string)
{
    if ((void*) string == NULL){
        return;
    }
    gns_system_call ( 
        289, 
        (unsigned long) string,
        (unsigned long) string,
        (unsigned long) string );
}

// Yield this thread.
void gns_yield(void)
{
    gns_system_call (265,0,0,0);
}

// Get packet from kernel device.
int gns_get_packet( unsigned long buffer, int buffer_lenght )
{
    unsigned long res=0;
    unsigned long len = (unsigned long) (buffer_lenght & 0xFFFFFFFF);

    if (buffer == 0)
        goto fail;
    if (len == 0)
        goto fail;

// IN:
// msgcode, buffer address, buffer kenght, nothing.
    res = (unsigned long) gns_system_call ( 118, buffer, len, 0 );

    return (int) (res & 0xFFFFFFFF);
fail:
    return (int) -1;
}

// hello request
static int __gns_hello_request(int fd)
{
    unsigned long *message_buffer = (unsigned long *) &__gns_buffer[0];   
    int n_writes = 0;   // For sending requests.
    int i=0;

//
// Write
//

    // #debug
    // debug_print ("__gns_hello_request: Writing ...\n");      

// Enviamos um request para o servidor.
// ?? Precisamos mesmo de um loop para isso. ??
// Write!
// Se foi possível enviar, então saimos do loop.        
// obs: podemos usar send();

// Message: Solicita um hello!      
    message_buffer[0] = 0;       // window. 
    message_buffer[1] = GNS_Hello;  //1000;    // msg=hello.
    message_buffer[2] = 0;
    message_buffer[3] = 0;
    // ...

    while (1){
        n_writes = 
            (int) write (
                      fd, 
                      __gns_buffer, 
                      sizeof(__gns_buffer) );

        if (n_writes>0){
           break;
        }

        for (i=0; i<11; i++)
        {
            //gramado_system_call (265,0,0,0); //yield thread.
            //gnst_yield();
            rtl_yield();
        }
    };

    if (n_writes <= 0){
        goto fail;
    }

    return (int) n_writes;
fail:
    return (int) -1;
}

static int __gns_hello_response(int fd)
{
    unsigned long *message_buffer = (unsigned long *) &__gns_buffer[0];   
    int n_reads = 0;    // For receiving responses.
    int i=0;
    char response_buffer[128];

    if (fd < 0){
        goto fail;
    }

//
// Read
//
    // #debug
    //debug_print ("__gns_hello_response: reading ...\n");      

    n_reads = 
        (int) read ( 
                  fd, 
                  __gns_buffer, 
                  sizeof(__gns_buffer) );

// #bugbug
// If we do not read the file, so the flag will not switch
// and we will not be able to write into the socket.

    if (n_reads <= 0){
        printf ("__gns_hello_response: recv fail\n");
        goto fail;
    }

// Get the message sended by the server.
    int msg = (int) message_buffer[1];

    switch (msg){
 
    // reply (ok)
    case GNS_SERVER_PACKET_TYPE_REPLY:
        debug_print ("__gns_hello_response: GNS_SERVER_PACKET_TYPE_REPLY received\n"); 
        goto process_reply;
        break;

    // error
    case GNS_SERVER_PACKET_TYPE_ERROR:
        debug_print ("__gns_hello_response: GNS_SERVER_PACKET_TYPE_ERROR\n");
        goto fail;
        break;

    // request
    case GNS_SERVER_PACKET_TYPE_REQUEST:
        debug_print ("__gns_hello_response: GNS_SERVER_PACKET_TYPE_REQUEST\n");
        goto fail;
        break;

    // event
    case GNS_SERVER_PACKET_TYPE_EVENT:
        debug_print ("__gns_hello_response: GNS_SERVER_PACKET_TYPE_EVENT\n");
        goto fail;
        break;

    default:
        goto fail;
        break;
    };

process_reply:

// Clear the buffer.
// #bugbug: Overflow. We need a limit here.
// #todo: Get the size of the message
// and then put the message into the tmp buffer.
// Se tem alguma coisa no buffer, mostra

    for (i=0; i<128; i++){
        response_buffer[i] = 0;
    };

// Get the payload.
// The message is in the offset 128?
    sprintf( response_buffer, (__gns_buffer+128) );
    response_buffer[127] = 0;  //finaliza

    if ( *response_buffer != 0 ){
        printf("RESPONSE: { %s }\n", response_buffer );
    }

    return (int) n_reads;

fail:
    return (int) -1;
}

// Send async command
void
gns_async_command ( 
    int fd, 
    unsigned long request,
    unsigned long sub_request,
    unsigned long data )
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gns_buffer[0];
    int n_writes=0;
    int Value=0;
    register int i=0;

// Enviamos um request para o servidor.
// Precisamos mesmo de um loop para isso?
// Write!
// Se foi possível enviar, então saimos do loop.  
// Nesse caso, corremos o risco de ficarmos presos
// caso não seja possível escrever.

// --------------------
// Clean the main buffer.
    for (i=0; i<512; i++)
        __gns_buffer[i] = 0;

// wid, message code, request, subrequest, data1
    message_buffer[0] = 0;
    message_buffer[1] = GNS_AsyncCommand;
// #todo: We need a list of services we can all with this function.
    message_buffer[2] = request;           // request
    message_buffer[3] = sub_request;       // sub request
// data
// #todo: We can deliver more data if we want.
    message_buffer[4] = data;  // data1
    // ...

// Parameters
    if (fd<0){
        debug_print("gns_async_command: fd\n");
        goto fail;
    }
// ...

// Sending ...
    n_writes = 
        (int) send ( 
                  fd,
                  __gns_buffer, 
                  sizeof(__gns_buffer), 
                  0 );

    if (n_writes <= 0){
        goto fail;
    }

    rtl_set_file_sync ( 
        fd, 
        SYNC_REQUEST_SET_ACTION, 
        ACTION_REQUEST );

// No return.
    while (1){
        Value = 
            (int) rtl_get_file_sync( 
                      fd, 
                      SYNC_REQUEST_GET_ACTION );
        // Essa é a sincronização esperada.
        // Não teremos uma resposta, mas precisamos
        // conferir a sincronização.
        //if (Value == ACTION_REQUEST){}
        if (Value == ACTION_NULL )  { goto done; }
        if (Value == ACTION_ERROR ) { goto done; }
        //#debug
        debug_print ("gns_async_command: Waiting sync flag\n"); 
    };

done:
    __gns_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return; 
fail:
    __gns_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return; 
}

void
gns_async_command2 ( 
    int fd, 
    unsigned long request,
    unsigned long sub_request,
    unsigned long data1,
    unsigned long data2,
    unsigned long data3,
    unsigned long data4 )
{
    unsigned long *message_buffer = 
        (unsigned long *) &__gns_buffer[0];
    int n_writes=0;
    int Value=0;
    register int i=0;

    // #debug
    // debug_print ("gns_async_command2: send...\n"); 

// Enviamos um request para o servidor.
// Precisamos mesmo de um loop para isso?
// Write!
// Se foi possível enviar, então saimos do loop.  
// Nesse caso, corremos o risco de ficarmos presos
// caso não seja possível escrever.

// --------------------
// Clean the main buffer.
    for (i=0; i<512; i++)
        __gns_buffer[i] = 0;

// Window ID
    message_buffer[0] = 0;
// Message code. (2222?)
    message_buffer[1] = GNS_AsyncCommand;
// #todo: We need a list of services we can all with this function.
    message_buffer[2] = request;           // request
    message_buffer[3] = sub_request;       // sub request
// data
// #todo: We can deliver more data if we want.
    message_buffer[4] = (unsigned long) data1;  // data1
    message_buffer[5] = (unsigned long) data2;  // data2
    message_buffer[6] = (unsigned long) data3;  // data3
    message_buffer[7] = (unsigned long) data4;  // data4
    // ...

    if (fd<0){
        debug_print("gns_async_command2: fd\n");
        goto fail;
    }

    n_writes = 
        (int) send ( 
                  fd,
                  __gns_buffer, 
                  sizeof(__gns_buffer), 
                  0 );

    if (n_writes <= 0){
        goto fail;
    }

    rtl_set_file_sync ( 
        fd, 
        SYNC_REQUEST_SET_ACTION, 
        ACTION_REQUEST );

// No return.
    while (1){
        Value = 
            (int) rtl_get_file_sync( 
                      fd, 
                      SYNC_REQUEST_GET_ACTION );
        // Essa é a sincronização esperada.
        // Não teremos uma resposta, mas precisamos
        // conferir a sincronização.
        //if (Value == ACTION_REQUEST){}
        if (Value == ACTION_NULL )  { goto done; }
        if (Value == ACTION_ERROR ) { goto done; }
        //#debug
        debug_print ("gns_async_command2: Waiting sync flag\n"); 
    };

done:
    __gns_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return; 
fail:
    __gns_clear_msg_buff();
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
    return; 
}


// =============================================
int gns_hello (int fd)
{
    int status = -1;

    if (fd < 0){
        debug_print("gns_hello: fd\n");
        goto fail;
    }

// Reset flags.
// Configuramos a flag para que possamos escrever.
   debug_print("gns_hello: reset flags\n");      

   rtl_set_file_sync( 
       fd,   // 
       216,  // request. (We can write)
       0 );  // data

//=============================
// Request.
// Avisamos que um request foi enviado.
   debug_print ("gns_hello: Send request\n");      

    int req_status = -1;
    req_status = (int) __gns_hello_request(fd);
    if (req_status < 0){
        goto fail;
    }

// Set a request.
    rtl_set_file_sync( 
        fd, 
        SYNC_REQUEST_SET_ACTION, 
        ACTION_REQUEST );

//=============================
// Response.
// Waiting to read the response.
   debug_print ("gns_hello: Read response\n");      

    int value=0;
    while (1)
    {
        value = 
            (int) rtl_get_file_sync( 
                      fd, 
                      SYNC_REQUEST_GET_ACTION );
        
        if (value == ACTION_REQUEST){
            rtl_yield();
        }
        if (value == ACTION_REPLY){
            break;
        }
        if (value == ACTION_ERROR){
            goto fail;
        }
        // No reply!
        if (value == ACTION_NULL){
            goto fail;
        }
    };

// A sicronização nos diz que ja temos um reply,
// então não precisamos esperar.

// ??
// Se não conseguimos ler a resposta.
// Então vamos reconfigurar as flags do arquivo
// dizendo que queremos autorização para escrever.
// Na verdade esse erro na hora de ler a resposta
// ja é motivo pra fechar a conexão.

    status = (int) __gns_hello_response(fd);
    if (status <= 0){
        goto fail;
    }
    
    return TRUE;
fail:
    return (int) -1;
}

