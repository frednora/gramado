// main.c
// netd.bin
// Gramado Network Server
// 2020 - Created by Fred Nora.

// This is the main server whe Gramado is acting like
// a server. When the 'product' is the 'Gramado Server'.
// And a client can use this service to connect
// with a Gramado Server for sharing resources
// as well as identifying it self.

// Servidor de configuração de rede local.
// Segurança/credenciais e inicialização de sessões.
// Através desse servidor, duas máquinas rodando Gramado
// podem sincronizar suas ações gerando certificados
// próprios para máquinas rodando Gramado.
// Gramado Tokens.
// Podemos gerar estatísticas sobre o uso da rede
// e descrições sobre os componentes.

/*
 Network server costuma ser um termo genérico que identifica
 um computador como sendo servidor em uma rede. 
 Esses servidores costumam ser das seguintes classes:
 + File servers -  files in the local network.
 + Database servers - files inside the server.
 + Application servers.
 */


#include <gnsint.h>

// static int __saved_sync_id = -1;


// Types of response
// Used in __send_respose().
#define RESPONSE_IS_REPLY  1
#define RESPONSE_IS_EVENT  2
#define RESPONSE_IS_ERROR  3

//
// == Gramado Network Protocol ===============================
//

// Standard. (First version)
// This is the standard header.
#define GNP_WID        0
#define GNP_MESSAGE    1
#define GNP_LONG1      2
#define GNP_LONG2      3
// #extension
// extended data.
#define GNP_LONG3      4
#define GNP_LONG4      5
#define GNP_LONG5      6
#define GNP_LONG6      7
// #body
// Here starts the data section.
// It goes untill the end of the socket.
// Maybe 512-(16*4).
#define GNP_BODY_OFFSET    16
// ...

// Status
static int running = FALSE;
// Server fd
static int ____saved_server_fd = -1;
// Current client fd
static int ____saved_current_client_fd = -1;
// Flag: Send or not a reoply.
static int NoReply = FALSE;

//
// == Prototypes =========================
//


//
// Local functions
//


static int ServerShutdown(void);
static void ServerYield(void);

// ServerSendResponse:
// Worker
// There is a vetor with values for the next response.
// Called by dispatcher().
// IN:
// fd, 1=REPLY | 2=EVENT | 3=ERROR
static int ServerSendResponse(int fd, int type);

static void 
ServerSendErrorResponse (int fd, int code, char *error_message);

// Process requests
static int 
ServerProcessRequests ( 
    void *window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 );

// Dispatch requests
static void ServerDispatch(int fd);

// Request loop
static int ServerLoop(void);

// Initialization
static void ServerInitializeGlobals(void);
static int ServerInitialization(void);

//
// ===============================
//


//char *
//eth_broadcast_addr = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
//char *
//eth_null_addr = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/*
void gnssrv_print_mac (char *string);
void gnssrv_print_mac (char *string)
{
    char *ptr = (char *) string;
    printf("MAC { %x:%x:%x:%x:%x:%x } \n", 
        ptr[0], 
        ptr[1], 
        ptr[2], 
        ptr[3], 
        ptr[4],
        ptr[5] );
}
*/

/*
int gnssrv_maccmp(char *a, char *b);
int gnssrv_maccmp(char *a, char *b)
{
    char *__a = a;
    char *__b = b;
    int i=0;

    for ( i=0; i<6; i++ )
    {
        if (__a[i] != __b[i]){ return (int) 1; }
    };

    return 0;
}
*/

/*
void
gnssrv_print_ip_addr(uint32_t ip);
void
gnssrv_print_ip_addr(uint32_t ip)
{
    char *__ip = (char *) &ip;
    print ("%d.%d.%d.%d \n", 
        (uint8_t) __ip[0], 
        (uint8_t) __ip[1], 
        (uint8_t) __ip[2], 
        (uint8_t) __ip[3]);
}
*/

/*
void
gnssrv_print_le_ip_addr(uint32_t ip);
void
gnssrv_print_le_ip_addr(uint32_t ip)
{
    char *__ip = (char *) &ip;
    print ("%d.%d.%d.%d \n", 
        (uint8_t) __ip[3], 
        (uint8_t) __ip[2], 
        (uint8_t) __ip[1], 
        (uint8_t) __ip[0]);
}
*/


/*
uint16_t
ip_calculate_checksum(void *ip);
uint16_t
ip_calculate_checksum(void *ip)
{
    uint16_t *buffer = (uint16_t *) ip;
    uint32_t sum = 0;
    int i;
    int len = 20; //FIXME: get it from ip_ihl 

    for (i = 0; i < len / sizeof(uint16_t); i ++)
        sum += (buffer[i]);

    while (sum > 0xffff) {
        uint16_t val = (sum & 0xffff0000) >> 16;
        sum &= 0x0000ffff;
        sum += val;
    }

    sum = ~sum;

    return (sum);
}
*/

// Action
void serviceHello(void)
{
    //printf("\n");
    printf("netd: [1000] Hello from Gramado Network Server!\n");
    next_response[0] = 0;  //wid
    next_response[1] = SERVER_PACKET_TYPE_REPLY; // The response is a reply. 
    next_response[2] = 0;
    next_response[3] = 0;
    NoReply = FALSE;
    rtl_yield();
}

// Action
int serviceInitializeNetwork(void)
{
    // Ring0 routine to initialize network infrastructure.
    // #remember: At this moment we are in the user app memory space.
    gramado_system_call(968,0,0,0);
    return 0;
}

// #todo
static int ServerShutdown(void)
{
    printf("netd: [todo] ServerShutdown\n");
    // close(__server_fd);
}

static void ServerYield(void)
{
    gramado_system_call(265,0,0,0); 
    //  sc82 (265,0,0,0);
}

//
// $
// SEND RESPONSES
//

// ServerSendResponse:
// Worker
// There is a vetor with values for the next response.
// Called by dispatcher().
// IN:
// fd, 1=REPLY | 2=EVENT | 3=ERROR
static int ServerSendResponse(int fd, int type)
{
// Reusing the same buffer from the request.
    unsigned long *message_buffer = (unsigned long *) &__buffer[0];
    int n_writes=0;
    int Status=0;
    register int i=0;

/*
// #bugbug
// We gotta select a standard offset for strings.
    char *m = (char *) (&__buffer[0] + 16); //#wrong
    char *m = (char *) (&__buffer[8]);  //use this one?
    sprintf( m, "~ Response from gwssrv \n");
*/

//
// Primeiros longs do buffer.
//

// 0:
// wid
    message_buffer[0] = (unsigned long) next_response[0];
    //message_buffer[0] = (unsigned long) (next_response[0] & 0xFFFFFFFF);

// 1:
// Types of reply.

    switch(type){
    // 1 - Normal reply
    case RESPONSE_IS_REPLY:
        message_buffer[1] = SERVER_PACKET_TYPE_REPLY;
        break;
    // 2 - Event
    case RESPONSE_IS_EVENT:
        message_buffer[1] = SERVER_PACKET_TYPE_EVENT;
        break;
    // 3 - Error
    case RESPONSE_IS_ERROR:
    default:
        message_buffer[1] = SERVER_PACKET_TYPE_ERROR;
        break;
    };

// 2 and 3:
// Signature in some cases.
// Can we deliver values here?
    message_buffer[2] = (unsigned long) next_response[2];  // long1
    message_buffer[3] = (unsigned long) next_response[3];  // long2
// 4,5,6,7
// Data
    message_buffer[4] = (unsigned long) next_response[4];
    message_buffer[5] = (unsigned long) next_response[5];
    message_buffer[6] = (unsigned long) next_response[6];
    message_buffer[7] = (unsigned long) next_response[7];
// 8,9,10,11
// Data
    message_buffer[8]  = (unsigned long) next_response[8];
    message_buffer[9]  = (unsigned long) next_response[9];
    message_buffer[10] = (unsigned long) next_response[10];
    message_buffer[11] = (unsigned long) next_response[11];
// 12,13,14,15
// Data
    message_buffer[12] = (unsigned long) next_response[12];
    message_buffer[13] = (unsigned long) next_response[13];
    message_buffer[14] = (unsigned long) next_response[14];
    message_buffer[15] = (unsigned long) next_response[15];
// more
    message_buffer[16] = (unsigned long) next_response[16];
    message_buffer[17] = (unsigned long) next_response[17];
    message_buffer[18] = (unsigned long) next_response[18];
    message_buffer[19] = (unsigned long) next_response[19];
    // ...
//__again:

//
// == Response ============================
//

    // #debug
    //debug_print ("dsSendResponse: Sending response ...\n");

    // #todo:
    // while(1){...}

    /*
    // Is current client connected.
    if (currentClient->is_connected == 0)
    {
        // [FAIL] Not connected.
        // close?
    }
    */

//
// Send
//

// Limits
    if (fd<0 || fd>31)
    {
        Status = -1;
        goto exit1;
    }

// We can't write on our own socket.
    if (fd == ____saved_server_fd){
        printf("ServerSendResponse: fd == ____saved_server_fd\n");
        printf("The server can't write on your own socket\n");
        while (1){
        };
    }

// #test
// For now, the only valid fd is 31.
    if (fd != 31){
        printf("ServerSendResponse: fd != 31\n");
        while (1){
        };
    }

// Write
    for (i=0; i<8; i++){

    n_writes = write( fd, __buffer, sizeof(__buffer) );
    //n_writes = send ( fd, __buffer, sizeof(__buffer), 0 );

// No. 
// We couldn't send a response.
// O que acontece se nao conseguirmos enviar uma resposta?
// Certamente o cliente tentara ler e tera problemas.
// Deveriamos fechar a conexao?
// Deveriamos enviar um alerta

    if (n_writes <= 0)
    {
        //#debug
        debug_print ("ServerSendResponse: response fail\n");
        printf      ("ServerSendResponse: Couldn't send reply\n");
        //close(fd);
        Status = -1;
        //goto exit1;
    }

// YES, We sent the response.
    if (n_writes > 0)
    {
        Status = 0; //OK
        //goto exit0;
        break;
    }

    };

    if (Status < 0)
        goto exit0;

// Cleaning
// Limpa se a resposta der certo ou se der errado.
// If the sizes are equal, we can do both at the same time.
    for (i=0; i<MSG_BUFFER_SIZE; ++i){
        __buffer[i] = 0;
    };
    for (i=0; i<NEXTRESPONSE_BUFFER_SIZE; ++i){
        next_response[i] = 0;
    };

    // Fall through.

// Fail
exit1:
    message_buffer[0] = 0;
    message_buffer[1] = 0;
    message_buffer[2] = 0;
    message_buffer[3] = 0;
    message_buffer[4] = 0;
    message_buffer[5] = 0;
exit0:
// Sync. Set response.
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_REPLY );
    return (int) Status;
}

static void 
ServerSendErrorResponse (int fd, int code, char *error_message)
{
    // 500: internal server error!!
    //#todo
    debug_print("netd: [TODO] ServerSendErrorResponse\n");
}


//
// $
// PROCESS REQUESTS - (event Procedure)
//

// Do a service. 
// Called by dispath();
static int 
ServerProcessRequests ( 
    void *window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{
// Route:
// This is the main route. It connects the request with the 
// controller action. And the action handles the data.

    int my_pid = -1;

    if (msg<0){
        return -1;
    }

    switch (msg){

        // Com essa mensagem o cliente solicita input.
        // então devemos drenar input usado loop de mensagens e
        // repassar para o cliente via socket.
        case 8080:
            //debug_print("netd: [TODO] 8080. drain messages ...\n");
            break;

        case MSG_SYSKEYUP:
            switch (long1)
            {
                // #debug
                case VK_F1:
                    //reboot();
                    break;
                    
                // #debug
                // mostrar informações sobre o socket 
                // do gws.    
                case VK_F2:
                    my_pid = (int) getpid();
                    gramado_system_call(7008,my_pid,0,0);
                    break; 
             
                // Enviar a mensagem para o processo associado
                // com a janela que tem o foco de entrada.
                default:
                    debug_print("netd: MSG_SYSKEYUP\n");
                    break;
            }    
            break;

        // #test
        // Tentando receber uma mensagem de digitação.
        case MSG_KEYDOWN:
            
            //#bugbug: Isso está falhando.
            //mas o f1 funciona.
            //printf ("%c", (char) long1); 
            //gws_show_backbuffer ();

            debug_print("netd: MSG_KEYDOWN\n");
            break;

        // MSG_GNS_HELLO
        // case 1000:
        case GNS_Hello:
            serviceHello();         
            NoReply = FALSE;  // The client-side library is waiting for response.
            return 0;
            break;

        // MSG_GNS_INITIALIZENETWORK
        case 1001:
            printf("\n");
            printf("netd: [1001]\n");
            // serviceInitializeNetwork();
            //printf ("\n");
            NoReply = FALSE;
            return 0;
            break;

        //case 1002:  break;
        //case 1003:  break;

        //case 2000:  break;
        //case 2001:  break;
        //case 2002:  break;
        //case 2003:  break;

        //MSG_GNS_SHUTDOWN
        case 2010:
            debug_print("netd: [2010] Disconnect\n");
            break;

        case 2020:
            break;

        // Refresh rectangle ... 
        case 2021:
            break;

        // 2222
        // + Asynchronous commands.
        // + No response.
        case GNS_AsyncCommand:
            //#todo: serviceAsyncCommand();

            //#debug
            printf("netd: GNS_AsyncCommand #debug\n");

            NoReply = TRUE;
            break;

        // 2240:
        // Set a text into a buffer in the ? structure.
        // No response.
        case GNS_SetText:
            // #todo: serviceSetText();
            NoReply = TRUE;
            break;

        // 2241:
        // Get a text from a buffer in the window structure.
        // #test: Things like this can be used to 
        // deliver a file as a response to the caller. html?
        case GNS_GetText:
            // #todo: serviceGetText();
            NoReply = FALSE;
            return 0;
            break;

        // 4080
        // Quit the process if it's possible.
        case GNS_Quit:
            //IsTimeToQuit=TRUE;
            break;

        //MSG_GNS_PROTOCOL
        case 3000:
            break;

        // ...

        default:
            //printf ("msg=%d ",msg);
            break;
    }

// #todo: 
// Call the system's window procedure.    
// Rever esse retorno.
    //return (int) xxxxxx_system_procedure (window,msg,long1,long2);
    return 0;
}


//
// $
// DISPATCH - (Handle requests)
//

// ServerDispatch:
// Messages sended via socket.
// obs: read and write use the buffer '__buffer'
// in the top of this file.
// Called by main.
static void ServerDispatch(int fd)
{
    unsigned long *message_buffer = (unsigned long *) &__buffer[0];
    int n_reads = 0;     // For requests.
    int n_writes = 0;    // For responses.

    int Status = -1;
    int SendErrorResponse=FALSE;
    int SendEvent=FALSE;


// Fail. Cleaning
    if (fd<0){
        debug_print("netd: ServerDispatch fd\n");
        message_buffer[0] = 0;
        message_buffer[1] = 0;
        message_buffer[2] = 0;
        message_buffer[3] = 0;
        ServerYield(); 
        return;
    }

// Check if we have a new request.
    int value = rtl_get_file_sync( fd, SYNC_REQUEST_GET_ACTION );
// Not a request.
    if (value != ACTION_REQUEST){
        //message_buffer[1] = 0;
        ServerYield();
        return;
    }

// =====================
// Read the request.
    n_reads = (int) read( fd, __buffer, sizeof(__buffer) );
    if (n_reads <= 0)
    {
        debug_print("netd: ServerDispatch n_reads\n");
        // No reply
        rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
        // Cleaning
        message_buffer[0] = 0;
        message_buffer[1] = 0;
        message_buffer[2] = 0;
        message_buffer[3] = 0;
        ServerYield(); 
        return; 
    }

// Nesse momento já lemos alguma coisa.

// Invalid message code.
    if (message_buffer[1] == 0)
    { 
        debug_print("netd: ServerDispatch Unknown message\n");
        // No reply
        rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
        // Cleaning
        message_buffer[0] = 0;
        message_buffer[1] = 0;
        message_buffer[2] = 0;
        message_buffer[3] = 0;
        ServerYield();
        return;
    }

// Um cliente solicitou um evento.
// Vamos sinalizar o tipo de resposta que temos que enviar,
// caso nenhum erro aconteça.
    if (message_buffer[1] == GNS_GetNextEvent){
        SendEvent = TRUE;  // The response is an EVENT, not a REPLY.
    }


// OK:
// Valid message
// Call the service!

    int procStatus = -1;

    procStatus = 
        ServerProcessRequests ( 
            (void *)        message_buffer[0], 
            (int)           message_buffer[1], 
            (unsigned long) message_buffer[2], 
            (unsigned long) message_buffer[3] );

    if (procStatus < 0){
        // oh !
        SendErrorResponse = TRUE;
    }

// #todo
// Se o request foi um request de evento,
// significa que o cliente deseja receber o próximo evento da 
// lista de eventos.
// podemos passar mensagens recebidas pelo gws para o cliente.

    if (NoReply == TRUE){
        rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
        //message_buffer[0] = 0;
        return;
        //goto exit0;
    }

// ==============================================

//
// == reponse ================
//

// Types of respose.
// IN:
// fd, 1=REPLY | 2=EVENT | 3=ERROR

// ==========================================
// ERROR: (3)
// Se o serviço não pode ser prestado corretamente.
// Error message.
    if (SendErrorResponse == TRUE){
        Status = (int) ServerSendResponse(fd,RESPONSE_IS_ERROR);
        goto exit2;
    }
// ==========================================
// EVENT: (2)
// Se o serviço foi prestado corretamente.
// Era uma solicitação de evento
// Event.
    if (SendEvent == TRUE){
        Status = (int) ServerSendResponse(fd,RESPONSE_IS_EVENT);
        goto exit2;
    }
// ==========================================
// REPLY: (1)
// Se o serviço foi prestado corretamente.
// Era uma solicitação de serviço normal,
// então vamos enviar um reponse normal. Um REPLY.
// Normal reply.
    if (SendEvent != TRUE){
        Status = (int) ServerSendResponse(fd,RESPONSE_IS_REPLY);
        goto exit2;
    }

/*
// ===================

//
// Send reply!
//

// #ps
// This routine for REPLY is suspended.

// Insert message string int to the response buffer.
    char *m = (char *) (__buffer + 128);
    sprintf( m, "GRAMADO 501 Not Implemented!");
    //sprintf( m, "HTTP/1.1 501 Not Implemented\n\n");
    //sprintf( m, "HTTP/1.1 400 Bad Request\n Content-Type: text/html\n Content-Length: 0\n");

// Standard message fields.
// Primeiros longs do buffer.
// #bugbug: Invalid values for next_response[i];
    message_buffer[0] = next_response[0];         // Window ID.
    message_buffer[1] = SERVER_PACKET_TYPE_REPLY; // next_response[1] 
    message_buffer[2] = next_response[2];         // Return value (long1)
    message_buffer[3] = next_response[3];         // Return value (long2)

// #todo:
// Talvez aqui possamos usar alguma função chamada post_message().

// Sending the reply.
    n_writes = (int) write( fd, __buffer, sizeof(__buffer) );
    if (n_writes<=0)
    {
        debug_print("netd: ServerDispatch Response fail\n");
        // No response. It fails.
        rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_NULL );
        return;
    }

// Cleaning
    message_buffer[0] = 0;
    message_buffer[1] = 0;
    message_buffer[2] = 0;
    message_buffer[3] = 0;

// Cleaning the buffer for the response.
    register int c=0;
    for (c=0; c<32; c++){
        next_response[c] = 0;
    };

// Set response.
    rtl_set_file_sync( fd, SYNC_REQUEST_SET_ACTION, ACTION_REPLY );
*/

// ===================

// Fall

exit2:
    message_buffer[0] = 0;
    message_buffer[1] = 0;
    message_buffer[2] = 0;
    message_buffer[3] = 0;
    message_buffer[4] = 0;
    message_buffer[5] = 0;
exit1:
exit0:
    return;
}

//
// $
// LOOP
//

// Called by main().
static int ServerLoop(void)
{

//=======================
    struct sockaddr server_address;
    socklen_t addrlen=0;
    server_address.sa_family = AF_GRAMADO;
    server_address.sa_data[0] = 'n';
    server_address.sa_data[1] = 's';
    addrlen = sizeof(server_address);
//=======================

    int server_fd = -1; 
    int bind_status = -1;
    register int i=0;
    int _status = -1;
    //int InitializeFirstClient = TRUE;
    int InitializeFirstClient = FALSE;
     
    //unsigned long w=0;
    //unsigned long h=0;

// Sincronizaçao provisoria
// vamos precisar disso antes de tudo;
// vamos pegar a que foi criada pelo primeiro cliente.
// ele cria no começo da rotina.
// Dai usaremos essa id por enquanto, pois o sistema so tem ela ainda.

/*
    while(1){
        __saved_sync_id = sc82 (10005,0,0,0);
        if( __saved_sync_id > 0 && __saved_sync_id < 1024 )
            break;
    };
 */

// -------------------
// Register
// Register this process as the network server.
// See: connect.c
    _status = (int) register_ns();
    if (_status<0){
        printf("netd: Couldn't register the server\n");
        goto fail;
    }
    debug_print("netd: Registration ok\n");

// -------------------
// socket
// Create socket and save the into a global variable.
    server_fd = (int) socket(AF_GRAMADO, SOCK_STREAM, 0);
    if (server_fd < 0){
        printf("netd: on socket()\n");
        goto fail;
    }
    ____saved_server_fd = (int) server_fd;

// -------------------
// bind
    bind_status = 
        (int) bind (
                  server_fd, 
                  (struct sockaddr *) &server_address, 
                  addrlen );

    if (bind_status < 0){
        printf("netd: on bind()\n");
        goto fail;
    }

// Calling child and wait.
// Just for fun, not for profit.
    if (InitializeFirstClient == TRUE){
        rtl_clone_and_execute("gns.bin");
    }

// Wait
    for (i=0; i<11; i++){
        ServerYield();
    };

//
// == Accept =====================================
//

// Messages sent via sockets.
// Mensagens enviadas pelos clientes via socket.
// Inclusive a mensagem que pede para drenar input e 
// via mensagens e repassar via socket. 

// Clients
    register int newconn = -1;
    int curconn = ____saved_server_fd;

// Accept connection from a client. 

    running = TRUE;

    while (1)
    {
        newconn = 
            (int) accept ( 
                      ____saved_server_fd, 
                      (struct sockaddr *) &server_address, 
                      (socklen_t *) addrlen );

        if (newconn < 0){
            debug_print("netd: on accept()\n");
            ServerYield(); 
        }else{
            // Valid fd.
            if (newconn == 31){
                ServerDispatch(newconn);
            }
        };
    };

// =======================================
    return 0;
fail:
    return (int) -1;
}


//
// $
// INITIALIZATION
//

static void ServerInitializeGlobals(void)
{
    register int i=0;

// Global flag for the loop.
    running = FALSE;
    ____saved_server_fd = -1;
    ____saved_current_client_fd = -1;

// Clear the buffer for the messages.
// see: globals.c
    for (i=0; i<GNS_BUFFER_SIZE; i++){
        __buffer[i] = 0;
    };
// Clear the tmp buffer for next response metadata.
// see: globals.c
    for (i=0; i<32; i++){
        next_response[i] = 0;
    };
}

static int ServerInitialization(void)
{
// debug
    printf("\n");
    printf("NETD.BIN: Initializing\n");

// Initialize global variables.
    ServerInitializeGlobals();

    return 0;
}

//
// $
// MAIN
//

int main (int argc, char **argv)
{
    int Status = -1;
    register int i=0;
// #test: Flags
    int f1= FALSE;
    int f2= FALSE;
    int f3= FALSE;
    int f4= FALSE;

    if (argc < 0)
        goto fail;

    for (i=0; i<argc; i++)
    {
        //printf("%d: {%s}\n",i,argv[i]);

        if ( strncmp( argv[i], "-1", 2 ) == 0 )
            f1=TRUE;
        if ( strncmp( argv[i], "-2", 2 ) == 0 )
            f2=TRUE;
        if ( strncmp( argv[i], "-3", 2 ) == 0 )
            f3=TRUE;
        if ( strncmp( argv[i], "-4", 2 ) == 0 )
            f4=TRUE;
    };


// OS dependent code.
    Status = (int) GramadoInitialize();
    if (Status != TRUE){
        printf("Couldn't initialize variables for Gramado OS.\n");
        goto fail;
    }

// Server initialization
    Status = (int) ServerInitialization();
    if (Status<0)
        goto fail;

// Request loop
    Status = (int) ServerLoop();
    if (Status == 0)
    {
        ServerShutdown();
        debug_print("netd: Exit\n");
             printf("netd: Exit\n");
        return (int) EXIT_SUCCESS;
    }

fail:
    return (int) EXIT_FAILURE;
}

//
// End
//

