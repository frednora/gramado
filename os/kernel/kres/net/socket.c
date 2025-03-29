// socket.c
// The APIs for the syscalls related to sockets.
// Called by sci.c
// Created by Fred Nora.

// See:
// https://en.wikipedia.org/wiki/Berkeley_sockets

#include <kernel.h>  

// Used by sys_connect().
// Random base number for new port numbers for the clients.
#define BASE_NEW_CLIENT_PORT_NUMBER  11000
static unsigned short __new_client_port_number = 
    BASE_NEW_CLIENT_PORT_NUMBER;

// Used by sys_connect().
unsigned char localhost_ipv4[4] = { 127, 0, 0, 1 };



static int 
__connect_inet ( 
    int sockfd, 
    const struct sockaddr *addr,
    socklen_t addrlen );

static int 
__connect_local ( 
    int sockfd, 
    const struct sockaddr *addr,
    socklen_t addrlen );

// ====================

/*
 * sys_socket:
 * This is the socket() function implementation.
 * Estamos na klibc dentro do kernel base.
 */
// The function socket() creates an endpoint for communication and 
// returns a file descriptor for the socket.
// socket() creates a new socket of a certain type, 
// identified by an integer number, and 
// allocates system resources to it.
// See: 
// https://www.gnu.org/software/libc/manual/html_node/Sockets.html
// #example:
// tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
// udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
// raw_socket = socket(AF_INET, SOCK_RAW, protocol);
// #todo
// Fazer um switch para tratar os vários tipos.
// Se o protocolo for '0', então precisamos encontrar o 
// protocolo adequado.
// OUT:
//     fd if ok.
//     -1 if it fails.

int sys_socket( int family, int type, int protocol )
{
// Service 7000

    //#todo
    // call create_socket(...)
    // it will return a pointer.

// Socket structure
    struct socket_d *__socket;
// Current process
    struct process_d *p;
    pid_t current_process = -1;
// ip:port
// Used in the socket struture
    unsigned long _ipv6 = (unsigned long)  0x0000000000000000;
    unsigned int _ipv4  = (unsigned int)   0x00000000;
    unsigned short port = (unsigned short) 0x0000;
    int Verbose = FALSE;

    // #debug
    // Slow.

    /*
    if (Verbose == TRUE){
        printk ("\n======================================\n");
        printk ("sys_socket: PID %d | family %d | type %d | protocol %d \n",
            current_process, family, type, protocol );
        refresh_screen();
    }
    */

    //debug_print("sys_socket:\n");
    do_credits_by_tid(current_thread);

// Filters
// For now we're only accepting AF_UNIX, AF_INET and AF_GRAMADO.
    if (family < 0){
        debug_print ("sys_socket: family not supported\n");
        return (int) (-EINVAL);
    }

// #todo
// Based on the socket type when the domain is AF_INET.
// + SOCK_STREAM = tcp socket.
// + SOCK_DGRAM  = udp socket.
// + SOCK_RAW    = raw socket.
// #todo: SOCK_GRAMADO_MSG

    if (type != SOCK_STREAM &&
        type != SOCK_DGRAM  &&
        type != SOCK_RAW)
    {
        debug_print ("sys_socket: type not supported\n");
        return (int) (-EINVAL);
    }

// #todo
// We are accepting only the gramado native protocol for the sockets.
// Each kind of request has its own message structure.
// For now the message starts in the top of the buffer, 
// there is no ethernet header or tcp/ip header and the message
// is not in the payload. This is a plain for the future. #todo.
// 0 = GRAMADO NATIVE PROTOCOL.
    if (protocol < 0){
        debug_print ("sys_socket: protocol not supported\n");
        return (int) (-EINVAL);
    }

// #debug
// '0' is the only valid value for now.
    if (protocol != 0){
        panic ("sys_socket: protocol not supported yet.\n");
    }

//
// Process
//

// Current pid
    current_process = (pid_t) get_current_process();
    if (current_process < 0 || current_process >= PROCESS_COUNT_MAX){
        debug_print ("sys_socket: current_process fail\n");
        panic       ("sys_socket: current_process fail\n");
    }
// Process structure
    p = (struct process_d *) processList[current_process];
    if ((void *) p == NULL){
        debug_print ("sys_socket: p\n");
        panic       ("sys_socket: p\n");
    }
    if (p->magic != 1234){
        panic ("sys_socket: p validation\n");
    }

// Socket structure.
// #todo:
// Create a helper function to do this job.
// This functions need to create the object and need to have this switch
// for different families. create_socket( family, type, protocol)
// it returns the socket structure pointer.
// Criamos um socket vazio.
// IN: ip and port.
    __socket = (struct socket_d *) create_socket_object();
    if ((void *) __socket == NULL){
        debug_print ("sys_socket: __socket\n");
        printk      ("sys_socket: __socket\n");
        goto fail;
    }

// family, type and protocol.
    __socket->family   = family;
    __socket->type     = type;
    __socket->protocol = protocol;

// ip:port
// Initialized with '0'.
    __socket->ip_ipv6 = (unsigned long)  _ipv6;
    __socket->ip_ipv4 = (unsigned int)   _ipv4;
    __socket->port    = (unsigned short) port;

// pid, uid, gid.
// #todo: Use methods to grab these informations.
    __socket->pid = (pid_t) current_process;
    __socket->uid = (uid_t) current_user;
    __socket->gid = (gid_t) current_group;

// #bugbug
// Set the private socket of a process.
// This is not good. 
// Some process will create more than one socket?
    p->priv = (struct socket_d *) __socket;

// Create socket file.
// #importante
// As rotinas logo abaixo criarão o arquivo 
// e retornarão o fd.
// family
// Setup the addr.

    switch (family){

    // see: sockint.c
    case AF_GRAMADO:
        debug_print ("sys_socket: AF_GRAMADO\n");
        __socket->connection_type = 1;  // (local connection) (IPC)
        return (int) socket_gramado ( (struct socket_d *) __socket, 
                        AF_GRAMADO, type, protocol );
        break;

    // see: sockint.c
    case AF_UNIX:
    // AF_LOCAL has the same value.
        debug_print ("sys_socket: AF_UNIX\n");
        __socket->connection_type = 1;  // (local connection) (IPC)
        return (int) socket_unix ( (struct socket_d *) __socket, 
                        AF_UNIX, type, protocol );
        break;

    // see: sockint.c
    //#bugbug: 
    //talvez precisamos rever sockaddr 
    //para essa função, e usarmos outra estrutura.               
    case AF_INET:
        debug_print ("sys_socket: AF_INET\n");
        __socket->connection_type = 0;
        //__socket->connection_type = 1;  // Local connection.
        // (Remote or local connection)
        // It depends on the IP address.
        // If the IP address is Localhost, so it is a local connection.
        // #todo: For now we set it to '0'.

        // For now we're only accepting raw sockets
        // AF_INET sockets can also use a type of SOCK_RAW. 
        // If this type is set, the application connects directly to the IP layer and 
        // does not use either the TCP or UDP transport.
        // #bugbug #todo
        // But in this case, for local connections we're gonna use raw type
        // and do not use any protocol ... just the raw frame.
        
        // #debug
        if (type != SOCK_RAW)
            panic("sys_socket: Only SOCK_RAW in AF_INET for now\n");

        return (int) socket_inet( (struct socket_d *) __socket, 
                        AF_INET, type, protocol );
        break;

    // ...

    default:
        debug_print ("sys_socket: [FAIL] bad family\n");
        //debug_print ("sys_socket: Couldn't create the file\n");
        goto fail;
        break;
    };

    // ...

fail:
    debug_print ("sys_socket: [FAIL] Something is wrong!\n");
    //refresh_screen();
    return (int) (-1);
}

/*
 * sys_accept: 
 *     This is a work in progress.
 */
// accept() is used on the server side. 
// It accepts a received incoming attempt to create 
// a new TCP connection from the remote client, 
// and creates a new socket associated with 
// the socket address pair of this connection.
// #todo
// Pega um socket da lista de conexoes incompletas.
// Nosso maior objetivo aqui eh retornar o fd arquivo 
// de socket do cliente.
// #todo
// We need to create a new socket. Only this way the process
// will have a new file in p->Objects[].
// == todo =========================
// #solution
// We create a new socket and connect this 
// new socket with the client.
// This way we can return the fd of the socket.
// #obs:
// When write() writes in the new socket, the data
// will be copied to the client socket. But it can be only an option.
/*
 * From Linux 0.98.1:
 * For accept, 
 * we attempt to create a new socket, 
 * set up the link with the client, 
 * wake up the client, then 
 * return the new connected fd.
 */
// #importante: Retornando o fd do cliente.
// Dessa forma o servido pode escrever nele
// e o cliente poderá ler.

int 
sys_accept (
    int sockfd, 
    struct sockaddr *addr, 
    socklen_t *addrlen )
{
// Service 7002

// Server process.
    struct process_d *sProcess;
    pid_t current_process = -1;
// Server socket and client socket.
    struct socket_d *sSocket;
    struct socket_d *cSocket;
// Files for the server and client sockets.
    file *sFile;
    file *cFile;   //#todo
// File descriptors for the server and client sockets.
    int fdServer = -1;
    int fdClient = -1;
// Iterator for the pending connections queue.
    register int i=0;

// #debug
    int Verbose=FALSE;

    // #debug
    // debug_print ("sys_accept:\n");
    do_credits_by_tid(current_thread);

//
// fd Server 
//

// #todo
// O argumento dá o descritor para o socket do servidor.
// A função accept seleciona um dos sockets da lista
// de conexões pendentes criadas por listen() e conecta
// com o socket do servidor.
// Ao fim devemos retornar o descritor do socket selecionado
// na lista de conexões pendentes.

    fdServer = sockfd;
    if ( fdServer < 0 || fdServer >= OPEN_MAX ){
        debug_print ("sys_accept: [FAIL] fdServer\n");
        return (int) (-EINVAL);
    }

// Check addr structure.
// #bugbug: 
// Ainda não sabemos qual é a estrutura de endereços usada.
// #bugbug: 
// Ainda não estamos usando isso.

    if ((void *) addr == NULL){
        printk("sys_accept: addr\n");
        return (int) (-EINVAL);
    }

// #test
// Address validation.
// It needs to be a ring3 address.
// #todo: Check agains more limits.
    if (addr < CONTROLTHREAD_BASE){
        panic("sys_accept: addr is not ring3\n");
        //return (int) (-EINVAL);
    }

// Current process. (The server)
// The accept() was called by the server.
    current_process = (pid_t) get_current_process();
    if ( current_process < 0  || current_process >= PROCESS_COUNT_MAX ){
        debug_print ("sys_accept: [FAIL] current_process\n");
        printk      ("sys_accept: [FAIL] current_process\n");
        goto fail;
    }
// Server process
    sProcess = (struct process_d *) processList[current_process];
    if ((void *) sProcess == NULL){
        debug_print ("sys_accept: [FAIL] sProcess\n");
        printk      ("sys_accept: [FAIL] sProcess\n");
        goto fail;
    }
    if ( sProcess->used != TRUE || sProcess->magic != 1234 ){
        debug_print ("sys_accept: [FAIL] sProcess validation\n");
        printk      ("sys_accept: [FAIL] sProcess validation\n");
        goto fail;
    }

// file
// O objeto que se refere ao socket do servidor.
// The socket is a file and belongs to the process.

    sFile = (file *) sProcess->Objects[fdServer];
    if ((void *) sFile == NULL){
        debug_print ("sys_accept: sFile fail\n");
        printk      ("sys_accept: sFile fail\n");
        goto fail;
    }
    if ( sFile->used != TRUE || sFile->magic != 1234 ){
        debug_print ("sys_accept: [FAIL] sFile validation\n");
        printk      ("sys_accept: [FAIL] sFile validation\n");
        goto fail;
    }
// Is this file a socket?
    if ( is_socket(sFile) != TRUE ){
        debug_print ("sys_accept: sFile is not a socket object\n");
             printk ("sys_accept: sFile is not a socket object\n");
        goto fail;
    }
    if (sFile->sync.can_accept != TRUE){
        debug_print ("sys_accept: sFile can NOT accept connections\n");
             printk ("sys_accept: sFile can NOT accept connections\n");
        goto fail;
    }

// Get the server's private socket.
// And check its validation.

    sSocket = (struct socket_d *) sFile->socket;
    if ((void *) sSocket == NULL){
        debug_print ("sys_accept: [FAIL] sSocket\n");
        printk      ("sys_accept: [FAIL] sSocket\n");
        goto fail;
    }
    if ( sSocket->used != TRUE || sSocket->magic != 1234 ){
        debug_print ("sys_accept: [FAIL] sSocket validation\n");
        printk      ("sys_accept: [FAIL] sSocket validation\n");
        goto fail;
    }

// Isso significa que o cliente chamou connect antes mesmo 
// do servidor chamar accept ??

/*
    if (sSocket->state == SS_CONNECTED){
        printk("sys_accept: [FAIL] socket is already SS_CONNECTED\n");
        refresh_screen();
        return -1;
    }
 */

/*
    // #test
    // The indicated socket need to be the same of the privete socket.
    if (f->socket != s){
        //#debug Testing ...
        panic("sys_accept: [FIXME] Check the original socket in the process %d!",
            current_process);
    }
 */

// Socket ok!
// #todo
// Essa funcao nos enviou o fd do socket do servidor.
// Agora vamos olhar na lista de conexoes existentes nesse
// socket e pegarmos um dos fd da lista. Em ordem round robing.
// Esse fd será o socket do cliente.

/*
    // get next!
    int i=0;
    int max=1;
    //max = s->backlog_max;
    return (int) s->pending_connections[ s->backlog_pos ];
 */

// O que segue abaixo eh um improviso,
// ja que listen ainda nao funciona

    // #debug
    //printk ("sys_accept: process %d | family %d | len %d \n", 
        //current_process, addr->sa_family, addrlen  );

// sys_accept deve apenas pegar um fd da lista de conexoes
// pending_connections[].
// Lembre-se que o fd do cliente estah numa lista em outro processo.
 
// #test
// Se o socket do servidor já está conectado.
// Se ele jah esta conectado, entao o seu slave tem um arquivo e esse
// arquivo deve ir para a lista de arquivos abertos pelo processo?
// Estamos retornando o fd do proprio servidor porque o write() 
// copia de um socket para o outro. Mas a intençao nao eh essa.

// #bugbug
// O socket do servidor precisa estar desconectado.
// Pois cada accept eh cria uma nova conexao.

/*
    if (sSocket->state == SS_CONNECTED){
        debug_print ("sys_accept: Already connected!\n");
        //printk      ("sys_accept: Already connected!\n");
        //refresh_screen();
        //return -1;
        return (int) fdServer;
    }
 */

// #todo
// Na verdade precisamos pegar um da fila.
// #test
// Se a conexão do socket do servidor está pendente.
// Pega um socket da lista de conexoes pendentes.
// #bugbug
// Devemos retornar o descritor do socket cliente.
// #todo: Para devolver um descritor para o servidor,
// então o socket do cliente deve estar na lista de
// arquivos abertos pelo servidor.
// #bugbug
// A estado de SS_CONNECTING foi atribuido pelo connect().
// Se connect() preconectar os sockets, isso será ruim,
// pois poderíamos transmitir dados mesmo antes do accept()
// selecionar um da fila de conexoes pendentes. 
// ele tambem esta em um dos slots e no slot 31.

// Se o socket ja está conectado ou 
// esta esperando para se conectar.

    if ( sSocket->state == SS_CONNECTING || 
         sSocket->state == SS_CONNECTED )
    {
        //debug_print ("sys_accept: CONNECTING !\n");

        //Server socket. Pre-connect.
        //precisamos mudar no caso de erro no cliente.
        sSocket->state = SS_CONNECTED;

        // Se existe outro socket linkado ao socket do servidor.
        // #todo
        // Nesse momento pegamos o primeiro da lista de conexões pendentes.
        // Mas desejamos ter uma lista de conexões pendentes e 
        // nesse momento pegaremos um da lista seguindo uma ordem.

        // Backlog
        // Circula
        sSocket->backlog_head++;
        if (sSocket->backlog_head >= sSocket->backlog_max){
            sSocket->backlog_head=0;
        }
        i = sSocket->backlog_head;
        if ( i<0 || i >= sSocket->backlog_max ){
            panic("sys_accept: Backlog limits\n");
        }

        // Probe for a valid pointer.
        // Ok 
        // Vamos pegar o ponteiro para estrutura de socket do cliente.
        // Isso foi colocado aqui na estrutura de socket do servidor
        // pela função connect();
        while (i <= sSocket->backlog_max)
        {
            cSocket = (struct socket_d *) sSocket->pending_connections[i];
            if ((void*) cSocket != NULL){
                break;
            }
            // next
            i++;
        };

        // [NO] - Not valid Client socket
        if ((void*) cSocket == NULL)
        {
            // #bugbug
            // No clients yet?
            // debug_print ("sys_accept: [FAIL] cSocket\n");
            
            sSocket->state = SS_CONNECTING;  //anula.
            goto fail;
        }

        // [YES] - Valid Client socket
        // Vamos nos conectar a ele.
        if ((void *) cSocket != NULL)
        {
            // check validation
            if ( cSocket->used != TRUE || cSocket->magic != 1234 )
            {
                debug_print ("sys_accept: [FAIL] cSocket validation\n");
                sSocket->state = SS_CONNECTING;  //anula.
                goto fail;
            }

            // Na verdade o magic indica que eh 
            // uma conexao pendente.
            if ( cSocket->magic_string[0] == 'C' )
            {
                //debug_print("MAGIC C\n");
                //printk ("magic: %s\m",cSocket->magic_string);
            }

            //ok: usar isso só para debug
            //debug_print ("sys_accept: done\n");

            cSocket->state = SS_CONNECTED;
            
            // Pegando o ponteiro da estrutura de arquivo 
            // associada ao socket do cliente.
            cFile = (file *) cSocket->private_file;
            // Salvando o ponteiro de estrutura de arquivo 
            // no slot prealocado na inicializacao.
            // Essa é a estrutura de processo do servidor.
            // Essa é a lista de arquivos abertos pelo processo.
            //sProcess->Objects[ sProcess->_client_sock_fd ] = cFile;
            sProcess->Objects[31] = (unsigned long) cFile;  //last
            cFile->_file = 31;
            
            //debug_print ("sys_accept: done\n");

            // #importante: Retornando o fd do cliente.
            // Dessa forma o servido pode escrever nele
            // e o cliente poderá ler.

            if (Verbose == TRUE)
            {
                printk("sys_accept: Server's pid {%d}\n",current_process);
                printk("sys_accept: Server's socket fd {%d}\n",sockfd);
                printk("sys_accept: Breakpoint\n");
                refresh_screen();
                while (1){
                    asm (" hlt ");
                };
            }

            return (int) cFile->_file;  // 31 
        }

        //fail
        debug_print ("sys_accept: [FAIL] Pending connection\n");
        sSocket->state = SS_CONNECTING;  //anula.

        goto fail;
    }

// fail
// #todo:
// Are we already connected?
// So we need a flag to indicate this status.

fail:

    // #debug
    //debug_print ("sys_accept: [FAIL] Something is wrong!\n");
    //printk    ("sys_accept: [FAIL] Something is wrong!\n");
    //refresh_screen();
    
    if (Verbose == TRUE){
        refresh_screen();
    }
    return (int) -1;
}

/*
 * sys_bind:
 *    When a socket is created with socket(), it exists in a 
 *    name space (address family) but has no address assigned to it. 
 *    bind() assigns the address specified by addr to the socket 
 *    referred to by the file descriptor sockfd.
 */
// bind() is typically used on the server side, and 
// associates a socket with a socket address structure, 
// i.e. a specified local 'IP address' and a 'port number'.
// See:
// https://man7.org/linux/man-pages/man2/bind.2.html

int 
sys_bind ( 
    int sockfd, 
    const struct sockaddr *addr,
    socklen_t addrlen )
{
// Service 7003

    struct process_d  *p;   // Process
    pid_t current_process = -1;
    struct file_d     *f;   // File
    struct socket_d   *s;   // Socket
    register int i=0;
    int Verbose=FALSE;

    // #debug
    //debug_print ("sys_bind:\n");
    do_credits_by_tid(current_thread);

// fd
    if ( sockfd < 0 || sockfd >= OPEN_MAX )
    {
        debug_print("sys_bind: sockfd\n");
        printk     ("sys_bind: sockfd\n");
        return (int) (-EINVAL);
    }

// Check addr structure.
    if ((void *) addr == NULL)
    {
        debug_print("sys_bind: addr\n");
        printk     ("sys_bind: addr\n");
        goto fail;
    }

// #test
// Address validation.
// It needs to be a ring3 address.
// #todo: Check agains more limits.
    if (addr < CONTROLTHREAD_BASE){
        panic("sys_bind: addr is not ring3\n");
        //return (int) (-EINVAL);
    }

// Process
    current_process = (pid_t) get_current_process();
    // #debug
    if (Verbose==TRUE){
        printk("sys_bind: PID %d | fd %d | \n",
            current_process, sockfd );
    }
    if (current_process < 0 || current_process >= PROCESS_COUNT_MAX){
        printk("sys_bind: current_process\n");
        goto fail;
    }
    p = (struct process_d *) processList[current_process];
    if ((void *) p == NULL){
        debug_print("sys_bind: p\n");
        printk     ("sys_bind: p\n");
        goto fail;
    }
    if (p->magic != 1234){
        printk("sys_bind: p validation\n");
        goto fail;
    }

// file:
// The objects type is 'socket'.

    f = (file *) p->Objects[sockfd];
    if ((void *) f == NULL){
        debug_print ("sys_bind: f\n");
        printk      ("sys_bind: f\n");
        goto fail;
    }
    if (f->magic != 1234){
       printk("sys_bind: f validation\n");
       goto fail; 
    }

// Is it a socket file?
    if ( is_socket(f) != TRUE ){
        debug_print ("sys_bind: f is not a socket\n");
        printk      ("sys_bind: f is not a socket\n");
        goto fail;
    }

// socket structure:
// A socket object has a socket structure associates with the file.
    s = (struct socket_d *) f->socket;
    if ((void *) s == NULL){
        debug_print("sys_bind: s\n");
        printk     ("sys_bind: s\n");
        goto fail; 
    }

// family
// Binding the name to the socket.
// So, now we need to include the 'name' into the socket structure,
// respecting the socket's family.
// Each family has a different size?
// For now we have only two server, gwssrv and gnssrv,
// and they are using this type of family.


/*
// #todo
// Maybe we can use this method to update the cache
// when we realized what is the server that is calling this routine.
    socket_set_gramado_port(
        GRAMADO_PORT_WS,
        (pid_t) current_process );
*/

//
// Domains
//


// ---------------------
// AF_GRAMADO
// Copy. Always 14.
// #important:
// For this type of family, the information associated with the
// servers socket is a two byte string. Example: 'ws' or 'ns'.
    if (s->addr.sa_family == AF_GRAMADO)
    {
        debug_print ("sys_bind: [AF_GRAMADO] Binding the name to the socket.\n");
        for (i=0; i<14; i++){
            s->addr.sa_data[i] = addr->sa_data[i];
        };
        // #debug
        if (Verbose==TRUE){
            printk ("sys_bind: process %d | family %d | len %d\n", 
                current_process, addr->sa_family, addrlen  );
        }
        return 0;
    }

// ---------------------
// AF_UNIX ou AF_LOCAL
// See: http://man7.org/linux/man-pages/man7/unix.7.html
    if (s->addr.sa_family == AF_UNIX)
    {
        debug_print ("sys_bind: AF_UNIX not supported yet\n");
        printk      ("sys_bind: AF_UNIX not supported yet\n");
        // Copy.
        //for (i=0; i<14; i++){ s->addr.sa_data[i] = addr->sa_data[i]; }; 
        return -1;
    }

// ---------------------
// AF_INET
    if (s->addr.sa_family == AF_INET)
    {
        debug_print ("sys_bind: AF_INET not supported yet\n");
        printk      ("sys_bind: AF_INET not supported yet\n");
        // Copy.
        //for (i=0; i<14; i++){ s->addr.sa_data[i] = addr->sa_data[i]; }; 
        return -1;
    }

// #fail
// A família é de um tipo não suportado.
    debug_print ("sys_bind: [FAIL] family not valid\n");
    printk      ("sys_bind: [FAIL] family not valid\n");

// fail
fail:
    debug_print ("sys_bind: [FAIL] Something is wrong!\n");
    printk      ("sys_bind: [FAIL] Something is wrong!\n");
    //refresh_screen();
    return (int) (-1);
}

/*
 * __connect_inet:
 *     Connecting to a server given an address.
 */
// connect() is used on the client side, and 
// assigns a free local port number to a socket. 
// In case of a TCP socket, it causes an attempt 
// to establish a new TCP connection.
// connect.
// Em nosso exemplo o cliente quer se conectar com o servidor.
// Conectando a um servidor dado um endereço.
// O endereço pode vir em formatos diferentes dependendo
// do domínio indicado na estrutura.
// #atenção
// O servidor poderá estar conectado a vários clientes,
// mas estará ouvindo a apenas um por vez. Ou precisaremos 
// de instâncias.
// #todo
// O socket do cliente precisa ter um fd no processo servidor.
// #??
// connect() eh chamado uma vez pelo cliente quando
// ele inicializa. Essa chamada fornece o fd do cliente.
// O servidor quando chama accept() seleciona um dos
// clientes conectados. 
// O servidor nao pode simplesmente aceitar
// o ultimo que se conectou.
// #bugbug: lembrando que nosso write copia de um socket para outro.
// Entao podemoriamos deixar as conexoes pendentes e selecionarmos
// somete uma a cada accept. 
// O write ira falhar se nao estiver conectado?
/*
 The backlog argument defines the maximum length to which the queue of
 pending connections for sockfd may grow.  If a connection request
 arrives when the queue is full, the client may receive an error with
 an indication of ECONNREFUSED or, if the underlying protocol supports
 retransmission, the request may be ignored so that a later reattempt
 at connection succeed.
*/
// At this moment, the operating system has to assign a not used 
// random port number to the client.
// IN: client fd, address, address len
// OUT: 0=ok <0=fail

static int 
__connect_inet ( 
    int sockfd, 
    const struct sockaddr *addr,
    socklen_t addrlen )
{
// Worker, called by sys_connect().
// AF_INET domains only.

// #todo
// O socket do cliente precisa de um arquivo aberto no
// processo alvo.
// #bugbug:
// Se nao fecharmos o arquivo ao fim da conexao, entao
// a lista de arquivos abertos se esgotara rapidamente.

// Client process and server process.
    struct process_d *cProcess;
    struct process_d *sProcess;
    pid_t target_pid = (-1);  //fail
// Client socket and server socket.
    struct socket_d *client_socket;
    struct socket_d *server_socket;
    int client_socket_fd = -1;
// File.
    struct file_d *f;
// #importante
// No caso de endereços no estilo inet
// vamos precisar de outra estrututura.
    struct sockaddr_in *addr_in;
    int Verbose = FALSE;
    register int i=0;

    unsigned char *given_ip;

    do_credits_by_tid(current_thread);

    pid_t current_process = (pid_t) get_current_process();

    if (Verbose == TRUE){
        printk ("__connect_inet: Client's pid {%d}\n", current_process );
        printk ("__connect_inet: Client's socket id {%d}\n", sockfd );
    }

// Client fd.
// client_socket_fd é um soquete de quem quer se conectar.
// O addr indica o alvo.
    client_socket_fd = sockfd;
    if ( client_socket_fd < 0 || client_socket_fd >= OPEN_MAX ){
        debug_print ("__connect_inet: client_socket_fd\n");
        printk      ("__connect_inet: client_socket_fd\n");
        //refresh_screen();
        return (int) (-EINVAL);
    }

// addr
// Usando a estrutura que nos foi passada.
    if ((void *) addr == NULL){
        printk ("__connect_inet: addr\n");
        //refresh_screen();
        return (int) (-EINVAL);
    }


//  ==============

// Getting the target PID.
// #todo:
// Devemos obter o número do processo alvo dado um endereço.
// O tipo de endereço depende do tipo de domínio (família).

// Tente inet, ao menos que mudemos de planos por 
// encontrarmos af_gramado, af_unix ou af_local.

// #importante
// opções de domínio se o endereço é no estilo unix. 
// >>> sockaddr

// ------------------------------------------
// Local
// This routine do not accept local domains.
// It is only for AF_INET.

// Check for invalid domains.
    switch (addr->sa_family)
    {
        // For pathnames.
        //case AF_LOCAL:
        case AF_UNIX:
            goto fail;
            break;
       
       // For strings.
        case AF_GRAMADO:
            goto fail;
            break;
    };

// ------------------------------------------
// Not Local
// Tentaremos com outro formato de endereço, o sockaddr_in.
// Agora trabalharemos com IP e PORTA. Não mais com string
// ou pathname.
// Vamos pegar o pid do processo associado a determinada porta.
// Para a família AF_INET.

// The client is trying to cennect to the localhost
// in the AF_INET domain.
    int in_localhost = FALSE;

// #bugbug
// Qual e' o tipo dessa estrutura passada?
    //addr_in = (struct sockaddr *) addr;
    addr_in = addr;

    // Opções de domínio se o endereço é no estilo internet.
    switch (addr_in->sin_family){

    case AF_INET:
    // Trabalhando com ip e porta.
    // Estamos usando inet em conexão local.
    // Então precisamos usar localhost como ip.

        debug_print("__connect_inet: AF_INET\n");
        //#debug
        //printk("sys_connect: AF_INET port {%d}\n", addr_in->sin_port);

        // Is this the localhost ip address?
        given_ip = &addr_in->sin_addr.s_addr;
        // Yes, it is!.
        // If not, so try to connect to a different machine.
        if ( given_ip[3] == localhost_ipv4[0] &&
             given_ip[2] == localhost_ipv4[1] &&
             given_ip[1] == localhost_ipv4[2] &&
             given_ip[0] == localhost_ipv4[3] )
        {
             in_localhost = TRUE;
             // #debug
             //printk("It's the localhost\n");
             //refresh_screen();
             //while(1){}
        }
        // No, it's not.
        // Try to connect to a different machine.
        if (in_localhost != TRUE){
            // #todo
        }

        // Yes
        // Check the port.
        // This way we know what is the server's pid.

        // 21 - FTP
        if (addr_in->sin_port == 21){
            printk("__connect_inet: [21] FTP #todo\n");
            goto fail;
        }

        // 23 - Telnet
        if (addr_in->sin_port == 23){
            printk("__connect_inet: [23] Telnet #todo\n");
            goto fail;
        }

        // 67 - DHCP
        if (addr_in->sin_port == 67){
            printk("__connect_inet: [67] DHCP #todo\n");
            goto fail;
        }

        // 80 - HTTP
        if (addr_in->sin_port == 80){
            printk("__connect_inet: [80] HTTP #todo\n");
            goto fail;
        }

        // 443 - HTTPS 
        if (addr_in->sin_port == 443){
            printk("__connect_inet: [443] HTTPS #todo\n");
            goto fail;
        }

        // 4040 - Display server
        // Se a porta for , então usaremos o pid do display server.
        if (addr_in->sin_port == __PORTS_DISPLAY_SERVER)
        {
            target_pid = (pid_t) socket_get_gramado_port(GRAMADO_PORT_WS);
            if (Verbose==TRUE)
            {
                printk("__connect_inet: [AF_INET] Connecting to the Window Server\n");
                printk("__connect_inet: IP {%x}\n", 
                    addr_in->sin_addr.s_addr );
                printk("__connect_inet: PORT {%d}\n", 
                    addr_in->sin_port);
                //#debug
                //while(1){}
            }
            break;
        }

        // 4041 - Network server
        // Se a porta for , então usaremos o pid do Network server.
        if (addr_in->sin_port == __PORTS_NETWORK_SERVER)
        {
            target_pid = (pid_t) socket_get_gramado_port(GRAMADO_PORT_NS);
            if (Verbose==TRUE)
            {
                printk("__connect_inet: [AF_INET] Connecting to the Network Server\n");
                printk("__connect_inet: IP {%x}\n", 
                    addr_in->sin_addr.s_addr );
                printk("__connect_inet: PORT {%d}\n", 
                    addr_in->sin_port);
                //#debug
                //while(1){}
            }
            break;
        }

        // 4042 - PORTS_FS #todo
        // Web server #todo
        // FTP server #todo
        // Telnet server #todo

        // #debug
        printk("__connect_inet: [FAIL] Port not valid {%d}\n",
            addr_in->sin_port);

        goto fail;
        break;

        //...

    default:
        debug_print("__connect_inet: domain not supported\n");
        printk("__connect_inet: [FAIL] Family not supported {%d}\n",
            addr_in->sin_family);
        goto fail;
        break;

    };  // switch

// ====================================================================
// Check
// Have a valid target_pid ?

//__go:

// #bugbug
// Daqui pra frente só faz sentido continuarmos
// se a intenção do cliente foi conectar-se com um servidor
// dentro do localhost.
    if (in_localhost != TRUE){
        printk ("__connect_inet: #todo Trying to connect to another machine\n");
        goto fail;
    }

//
// == Client process =============================
//

// Vamos obter o arquivo do tipo soquete que pertence ao sender.
    if (current_process<0 || current_process >= PROCESS_COUNT_MAX){
        printk ("__connect_inet: current_process\n");
        goto fail;
    }
// Client process.
// sender's process structure.
    cProcess = (struct process_d *) processList[current_process];
    if ((void *) cProcess == NULL){
        debug_print("__connect_inet: cProcess fail\n");
        printk     ("__connect_inet: cProcess fail\n");
        goto fail;
    }

// #todo
// Check structure validation
    // if (cProcess->magic ...

// The client's socket file.
// #bugbug
// Estamos pegando o arquivo de socket do cliente
// Então estamos presumindo que é uma conexão local.
// Se o servidor fechar esse arquivo, então
// o cliente será incapaz de enviar mensagens?
// Por isso no momento do accept devemos
// criar um arquivo para o cliente. Que poderá ser fechado
// pelo sevidor para encessar a conexão ... até que
// o cliente se conecte novamente.
// #importante: Mas para isso o cliente precisa
// possuir um arquivo de socket ainda válido.
// O servidor não pode fechar o arquivo de socket do cliente remoto.
// E também não devemos permitir que isso aconteça na conexão local.

    f = (file *) cProcess->Objects[client_socket_fd];
    if ((void *) f == NULL){
        printk("__connect_inet: [FAIL] f. The client's socket\n");
        goto fail;
    }

// Is it a socket?

    int __is = -1;
    __is = is_socket((file *) f);
    if (__is != TRUE){
        printk("__connect_inet: [FAIL] f is not a socket\n");
        goto fail;
    }

// This file doesn't accept connections.
// #todo:
// This way we can reject connections before the server binds with an address.

    if (f->sync.can_connect != TRUE){
        printk("__connect_inet: [PERMISSION FAIL] Client doesn't accept connections.\n");
        goto fail;
    }

// Client socket structure
// Socket structure in the sender's file.
// Pega a estrutura de socket associada ao arquivo.

    client_socket = (struct socket_d *) f->socket;
    if ((void *) client_socket == NULL){
        printk("__connect_inet: [FAIL] client_socket\n");
        goto fail;
    }

// The client socket needs to be unconnected.
// #perigo
// Só podemos conectar se o socket estiver nesse estado.
// #todo
// Caso fechar conexão podemos perder esse estado 
// naõ conseguirmos mais conectar.

    if (client_socket->state != SS_UNCONNECTED) {
        printk("__connect_inet: [FAIL] client socket is not SS_UNCONNECTED\n");
        goto fail;
    }


// #test
// Assign a port number to the client socket.
// #todo #bugbug We gotta work on this routine.
    __new_client_port_number++;
    client_socket->port = (unsigned short) __new_client_port_number;

// Server process
// #todo
// O arquivo de socket do cliente 
// precisa ter um fd no processo servidor.
// target pid.
// O pid do processo servidor.
// Pegamos o target_pid logo acima na porta solicitada.

    if (target_pid<0 || target_pid >= PROCESS_COUNT_MAX)
    {
        debug_print("__connect_inet: target_pid\n");
        printk     ("__connect_inet: target_pid\n");
        goto fail;
    }

// The server's process structure.
// O processo cliente chamou essa função e
// então pegaremos agora o processo alvo,
// que é um servidor.

    sProcess = (struct process_d *) processList[target_pid];
    if ((void *) sProcess == NULL){
        debug_print("__connect_inet: sProcess fail\n");
        printk     ("__connect_inet: sProcess fail\n");
        goto fail;
    }

// #todo
// Checar a validade da estrutura de processo.
    // if ( sProcess->magic != ...

// Procurando um slot livre na lista de objetos abertos
// presente na estrutura do processo servidor.

    register int __slot = -1;  // Default is fail.
    for (i=3; i<31; i++)
    {
        if (sProcess->Objects[i] == 0)
        {
            __slot = (int) i;
            goto __OK_new_slot;
        }
    };

// #fail: 
// No more slots.
    panic("__connect_inet: [FIXME] We need a slot in the server\n");

// ok
__OK_new_slot:

    if (__slot == -1){
        printk("__connect_inet: No empty slot\n");
        goto fail;
    }

// Esse é o socket do processo servidor.
// Sim, porque é o cliente que está tentando se conectar.
// Dessa forma o alvo é o servidor.

    server_socket = (struct socket_d *) sProcess->priv;
    if ((void *) server_socket == NULL){
        printk ("__connect_inet: [FAIL] server_socket\n");
        goto fail;
    }

// Salvando o fd do socket do cliente.
    server_socket->clientfd_on_server = __slot;
    client_socket->clientfd_on_server = __slot;

// #todo
// Temos que rever as coisas por aqui. kkk
// incluimos como objeto do servidor o ponteiro para 
// o arquivo que representa o socket do cliente.
// O arquivo de socket do cliente agora tem um fd
// no processo servidor.
// Nossa intenção aqui é usar um dos slots livres
// encontrado em p->Objects[], mas por enquanto estamos usando 
// apenas o slot número 31.

// :: The free slot we found before.
    //sProcess->Objects[__slot] = (unsigned long) f;
// :: The standard fd used by the servers in GramadoOS.
    sProcess->Objects[31] = (unsigned long) f;


// Connecting!
// #bugbug
// connect() eh chamado somente uma vez pelo cliente.
// A conexao deve ficar pendente numa lista e o accept
// seleciona uma delas.
// #test: 
// Pra começar, vamos deixar apenas uma pendente
// e o accept() selecionara ela.
// #todo
// Temos que colocar os pedidos de conexão em uma fila.
// O tamanho dessa fila foi determinado pelo servidor
// com a chamada listen(). Mas podemos ter um tamanho padrão.
// Talvez tamanho 1 para começar.
// Link:
// Conectando o socket do processo alvo ao ponto de
// conexão do processo cliente.
// Conectando o socket do cliente ao ponto de conecção do
// processo servidor.
// Flags:
// State:
// Acionando as flags que indicam a conecção.
// Nesse momento poderíamos usar a flag SS_CONNECTING
// e a rotina accept() mudaria para SS_CONNECTED. 
// #bugbug
// Nao podemos mudar o estado do servidor,
// estamos apenas entrando na fila e implorando para nos conectarmos.
// Quem realizara a conexão sera o accept(), 
// pegando o cliente da fila de conexões pendentes.

// Linking:
// Connecting
    client_socket->link = (struct socket_d *) server_socket;
    server_socket->link = (struct socket_d *) client_socket;
// Flags:
// State
    client_socket->state = SS_CONNECTING;
    server_socket->state = SS_CONNECTING;
// Magic signature
    client_socket->magic_string[0] = 'C';
    client_socket->magic_string[1] = 0; 

//
// Backlog
//

// #todo
// Precisamos de uma lista de conexões pendentes.
// O cliente invocou a conexão apenas uma vez
// e precisa usar o servidor varias vezes
// #obs: 
// Isso funcionou. Vamos tentar com lista.
// #obs: 
// Algumas implementacões usam lista encadeada de conexões pendentes. 
// 'server_socket->iconn'
// #bugbug: 
// Essa lista fica na estrutura de socket so servidor, 
// dessa forma o servidor pode ter mais de uma socket?

    //server_socket->connections_count++;
    //if(server_socket->connections_count >= server_socket->backlog_max )
    //    return ECONNREFUSED;

// Circula
// #bugbug:
// Actually we're gonna reject the new connections when the queue is full.

    int BacklogTail = 0;

    server_socket->backlog_tail++;
    if (server_socket->backlog_tail >= server_socket->backlog_max)
    {
        server_socket->backlog_tail = 0;
    }
    BacklogTail = server_socket->backlog_tail;

// coloca na fila.
// Coloca o ponteiro para estrutura de socket
// na fila de conexões pendentes na estrutura do servidor.
// Isso será usado pelo accept() para encontrar
// a estrutura do socket do cliente.
    server_socket->pending_connections[BacklogTail] = 
        (unsigned long) client_socket;

// Em que posiçao estamos na fila.
    client_socket->client_backlog_pos = BacklogTail;

// #
// O cliente está esperando que sua conexão seja aceita pelo servidor.

    // #debug
    //debug_print("sys_connect: Pending connection\n");

    // #debug
    //if (Verbose==TRUE){
    //printk     ("sys_connect: Pending connection\n");
    //refresh_screen();
    //}

    // #debug 
    // #breakpoint
    if (Verbose == TRUE)
    {
        printk("__connect_inet: Breakpoint :)\n");
        refresh_screen();
        while (1){
            asm (" hlt ");
        };
    }
//ok
    return 0;
fail:
    debug_print("__connect_inet: Fail\n");
    printk     ("__connect_inet: Fail\n");
    return (int) -1;
}

/*
 * __connect_local:
 *     Connecting to a server given an address.
 */
// connect() is used on the client side, and 
// assigns a free local port number to a socket. 
// In case of a TCP socket, it causes an attempt 
// to establish a new TCP connection.
// connect.
// Em nosso exemplo o cliente quer se conectar com o servidor.
// Conectando a um servidor dado um endereço.
// O endereço pode vir em formatos diferentes dependendo
// do domínio indicado na estrutura.
// #atenção
// O servidor poderá estar conectado a vários clientes,
// mas estará ouvindo a apenas um por vez. Ou precisaremos 
// de instâncias.
// #todo
// O socket do cliente precisa ter um fd no processo servidor.
// #??
// connect() eh chamado uma vez pelo cliente quando
// ele inicializa. Essa chamada fornece o fd do cliente.
// O servidor quando chama accept() seleciona um dos
// clientes conectados. 
// O servidor nao pode simplesmente aceitar
// o ultimo que se conectou.
// #bugbug: lembrando que nosso write copia de um socket para outro.
// Entao podemoriamos deixar as conexoes pendentes e selecionarmos
// somete uma a cada accept. 
// O write ira falhar se nao estiver conectado?
/*
 The backlog argument defines the maximum length to which the queue of
 pending connections for sockfd may grow.  If a connection request
 arrives when the queue is full, the client may receive an error with
 an indication of ECONNREFUSED or, if the underlying protocol supports
 retransmission, the request may be ignored so that a later reattempt
 at connection succeed.
*/
// At this moment, the operating system has to assign a not used 
// random port number to the client.
// IN: client fd, address, address len
// OUT: 0=ok <0=fail

static int 
__connect_local ( 
    int sockfd, 
    const struct sockaddr *addr,
    socklen_t addrlen )
{
// Worker, called by sys_connect().
// AF_GRAMADO, AF_UNIX/AF_LOCAL domains only.

// #todo
// O socket do cliente precisa de um arquivo aberto no
// processo alvo.
// #bugbug:
// Se nao fecharmos o arquivo ao fim da conexao, entao
// a lista de arquivos abertos se esgotara rapidamente.

// Client process and server process.
    struct process_d *cProcess;
    struct process_d *sProcess;
    pid_t target_pid = (-1);  //fail
// Client socket and server socket.
    struct socket_d *client_socket;
    struct socket_d *server_socket;
    int client_socket_fd = -1;
// File.
    struct file_d *f;
// #importante
// No caso de endereços no estilo inet
// vamos precisar de outra estrututura.

//No inet. This is for local domains only.
    //struct sockaddr_in *addr_in;

    int Verbose = FALSE;
    register int i=0;

    unsigned char *given_ip;

    do_credits_by_tid(current_thread);

    pid_t current_process = (pid_t) get_current_process();

    if (Verbose == TRUE){
        printk ("__connect_local: Client's pid {%d}\n", current_process );
        printk ("__connect_local: Client's socket id {%d}\n", sockfd );
    }

// Client fd.
// client_socket_fd é um soquete de quem quer se conectar.
// O addr indica o alvo.
    client_socket_fd = sockfd;
    if ( client_socket_fd < 0 || client_socket_fd >= OPEN_MAX ){
        debug_print ("__connect_local: client_socket_fd\n");
        printk      ("__connect_local: client_socket_fd\n");
        //refresh_screen();
        return (int) (-EINVAL);
    }

// addr
// Usando a estrutura que nos foi passada.
    if ((void *) addr == NULL){
        printk ("__connect_local: addr\n");
        //refresh_screen();
        return (int) (-EINVAL);
    }


//  ==============

// Getting the target PID.
// #todo:
// Devemos obter o número do processo alvo dado um endereço.
// O tipo de endereço depende do tipo de domínio (família).

// Tente inet, ao menos que mudemos de planos por 
// encontrarmos af_gramado, af_unix ou af_local.

// Let's try the AF_INET domain.
    //int IsRemote = FALSE;

// The client is trying to cennect to the localhost
// in the AF_INET domain.
    int in_localhost = FALSE;

// #importante
// opções de domínio se o endereço é no estilo unix. 
// >>> sockaddr

// ------------------------------------------
// Local
// Vamos pegar o pid do processo associado a determinada porta.
// Apenas as famílias AF_GRAMADO e AF_UNIX.
    switch (addr->sa_family){

    // #todo: This is a work in progress.
    //case AF_LOCAL:
    case AF_UNIX:
        // Trabalhando com pathname.
        debug_print ("__connect_local: AF_UNIX\n");
        printk      ("__connect_local: AF_UNIX\n");        
        goto fail;
        break;

    // AF_GRAMADO = 8000
    // Vamos obter o número do processo alvo dado o endereço.
    // 32 ports only.
    case AF_GRAMADO:
        // Trabalhando com string.

        debug_print ("__connect_local: AF_GRAMADO ok\n");

        // ds: Display server.
        if ( addr->sa_data[0] == 'd' && addr->sa_data[1] == 's' ){
            target_pid = (pid_t) socket_get_gramado_port(GRAMADO_PORT_WS);
        }
        // wm: ...
        // ns: Network server
        if ( addr->sa_data[0] == 'n' && addr->sa_data[1] == 's' ){
            target_pid = (pid_t) socket_get_gramado_port(GRAMADO_PORT_NS);
        }
        // fs: File system server
        if ( addr->sa_data[0] == 'f' && addr->sa_data[1] == 's' ){
            target_pid = (pid_t) socket_get_gramado_port(GRAMADO_PORT_FS);
        }
        // #todo: we: Web server
        //if (addr->sa_data[0] == 'w' && addr->sa_data[1] == 'e'){
        //    target_pid = (pid_t) socket_get_gramado_port(GRAMADO_PORT_WE);
        //}
        // #todo:  ft: FTP server
        //if (addr->sa_data[0] == 'f' && addr->sa_data[1] == 't'){
        //    target_pid = (pid_t) socket_get_gramado_port(GRAMADO_PORT_FT);
        //}
        // #todo:  tn: Telnet server
        //if (addr->sa_data[0] == 't' && addr->sa_data[1] == 'n'){
        //   target_pid = (pid_t) socket_get_gramado_port(GRAMADO_PORT_TN);
        //}

        if ( target_pid<0 || target_pid >= PROCESS_COUNT_MAX ){
            printk ("__connect_local: AF_GRAMADO target_pid\n");
            goto fail;
        }
        if (Verbose == TRUE){
            printk ("__connect_local: target pid %d \n", target_pid);
        }
        break;

        // Not a local domains
        default:
            printk("__connect_local: This is not a valid local domain\n");
            goto fail;
            break;
    };

// ====================================================================
// OK, now we have a target pid for local domains.

// Check
// Have a valid target_pid ?

//__go:

//
// == Client process =============================
//

// Vamos obter o arquivo do tipo soquete que pertence ao sender.

    if (current_process<0 || current_process >= PROCESS_COUNT_MAX){
        printk ("__connect_local: current_process\n");
        goto fail;
    }

// Client process.
// sender's process structure.
    cProcess = (struct process_d *) processList[current_process];
    if ((void *) cProcess == NULL){
        debug_print("__connect_local: cProcess fail\n");
        printk     ("__connect_local: cProcess fail\n");
        goto fail;
    }

// #todo
// Check structure validation
    // if (cProcess->magic ...

// The client's socket file.
// #bugbug
// Estamos pegando o arquivo de socket do cliente
// Então estamos presumindo que é uma conexão local.
// Se o servidor fechar esse arquivo, então
// o cliente será incapaz de enviar mensagens?
// Por isso no momento do accept devemos
// criar um arquivo para o cliente. Que poderá ser fechado
// pelo sevidor para encessar a conexão ... até que
// o cliente se conecte novamente.
// #importante: Mas para isso o cliente precisa
// possuir um arquivo de socket ainda válido.
// O servidor não pode fechar o arquivo de socket do cliente remoto.
// E também não devemos permitir que isso aconteça na conexão local.

    f = (file *) cProcess->Objects[client_socket_fd];
    if ((void *) f == NULL){
        printk("__connect_local: [FAIL] f. The client's socket\n");
        goto fail;
    }

// Is it a socket?

    int __is = -1;
    __is = is_socket((file *) f);
    if (__is != TRUE){
        printk("__connect_local: [FAIL] f is not a socket\n");
        goto fail;
    }

// This file doesn't accept connections.
// #todo:
// This way we can reject connections before the server binds with an address.

    if (f->sync.can_connect != TRUE){
        printk("__connect_local: [PERMISSION FAIL] Client doesn't accept connections.\n");
        goto fail;
    }

// Client socket structure
// Socket structure in the sender's file.
// Pega a estrutura de socket associada ao arquivo.

    client_socket = (struct socket_d *) f->socket;
    if ((void *) client_socket == NULL){
        printk("__connect_local: [FAIL] client_socket\n");
        goto fail;
    }

// The client socket needs to be unconnected.
// #perigo
// Só podemos conectar se o socket estiver nesse estado.
// #todo
// Caso fechar conexão podemos perder esse estado 
// naõ conseguirmos mais conectar.

    if (client_socket->state != SS_UNCONNECTED) {
        printk("__connect_local: [FAIL] client socket is not SS_UNCONNECTED\n");
        goto fail;
    }


// #test
// Assign a port number to the client socket.
// #todo #bugbug We gotta work on this routine.
    __new_client_port_number++;
    client_socket->port = (unsigned short) __new_client_port_number;

// Server process
// #todo
// O arquivo de socket do cliente 
// precisa ter um fd no processo servidor.
// target pid.
// O pid do processo servidor.
// Pegamos o target_pid logo acima na porta solicitada.

    if (target_pid<0 || target_pid >= PROCESS_COUNT_MAX)
    {
        debug_print("__connect_local: target_pid\n");
        printk     ("__connect_local: target_pid\n");
        goto fail;
    }

// The server's process structure.
// O processo cliente chamou essa função e
// então pegaremos agora o processo alvo,
// que é um servidor.

    sProcess = (struct process_d *) processList[target_pid];
    if ((void *) sProcess == NULL){
        debug_print("__connect_local: sProcess fail\n");
        printk     ("__connect_local: sProcess fail\n");
        goto fail;
    }

// #todo
// Checar a validade da estrutura de processo.
    // if ( sProcess->magic != ...

// Procurando um slot livre na lista de objetos abertos
// presente na estrutura do processo servidor.

    register int __slot = -1;  // Default is fail.
    for (i=3; i<31; i++)
    {
        if (sProcess->Objects[i] == 0)
        {
            __slot = (int) i;
            goto __OK_new_slot;
        }
    };

// #fail: 
// No more slots.
    panic("__connect_local: [FIXME] We need a slot in the server\n");

// ok
__OK_new_slot:

    if (__slot == -1){
        printk("__connect_local: No empty slot\n");
        goto fail;
    }

// Esse é o socket do processo servidor.
// Sim, porque é o cliente que está tentando se conectar.
// Dessa forma o alvo é o servidor.

    server_socket = (struct socket_d *) sProcess->priv;
    if ((void *) server_socket == NULL){
        printk ("__connect_local: [FAIL] server_socket\n");
        goto fail;
    }

// Salvando o fd do socket do cliente.
    server_socket->clientfd_on_server = __slot;
    client_socket->clientfd_on_server = __slot;

// #todo
// Temos que rever as coisas por aqui. kkk
// incluimos como objeto do servidor o ponteiro para 
// o arquivo que representa o socket do cliente.
// O arquivo de socket do cliente agora tem um fd
// no processo servidor.
// Nossa intenção aqui é usar um dos slots livres
// encontrado em p->Objects[], mas por enquanto estamos usando 
// apenas o slot número 31.

// :: The free slot we found before.
    //sProcess->Objects[__slot] = (unsigned long) f;
// :: The standard fd used by the servers in GramadoOS.
    sProcess->Objects[31] = (unsigned long) f;


// Connecting!
// #bugbug
// connect() eh chamado somente uma vez pelo cliente.
// A conexao deve ficar pendente numa lista e o accept
// seleciona uma delas.
// #test: 
// Pra começar, vamos deixar apenas uma pendente
// e o accept() selecionara ela.
// #todo
// Temos que colocar os pedidos de conexão em uma fila.
// O tamanho dessa fila foi determinado pelo servidor
// com a chamada listen(). Mas podemos ter um tamanho padrão.
// Talvez tamanho 1 para começar.
// Link:
// Conectando o socket do processo alvo ao ponto de
// conexão do processo cliente.
// Conectando o socket do cliente ao ponto de conecção do
// processo servidor.
// Flags:
// State:
// Acionando as flags que indicam a conecção.
// Nesse momento poderíamos usar a flag SS_CONNECTING
// e a rotina accept() mudaria para SS_CONNECTED. 
// #bugbug
// Nao podemos mudar o estado do servidor,
// estamos apenas entrando na fila e implorando para nos conectarmos.
// Quem realizara a conexão sera o accept(), 
// pegando o cliente da fila de conexões pendentes.

// Linking:
// Connecting
    client_socket->link = (struct socket_d *) server_socket;
    server_socket->link = (struct socket_d *) client_socket;
// Flags:
// State
    client_socket->state = SS_CONNECTING;
    server_socket->state = SS_CONNECTING;
// Magic signature
    client_socket->magic_string[0] = 'C';
    client_socket->magic_string[1] = 0; 

//
// Backlog
//

// #todo
// Precisamos de uma lista de conexões pendentes.
// O cliente invocou a conexão apenas uma vez
// e precisa usar o servidor varias vezes
// #obs: 
// Isso funcionou. Vamos tentar com lista.
// #obs: 
// Algumas implementacões usam lista encadeada de conexões pendentes. 
// 'server_socket->iconn'
// #bugbug: 
// Essa lista fica na estrutura de socket so servidor, 
// dessa forma o servidor pode ter mais de uma socket?

    //server_socket->connections_count++;
    //if(server_socket->connections_count >= server_socket->backlog_max )
    //    return ECONNREFUSED;

// Circula
// #bugbug:
// Actually we're gonna reject the new connections when the queue is full.

    int BacklogTail = 0;

    server_socket->backlog_tail++;
    if (server_socket->backlog_tail >= server_socket->backlog_max)
    {
        server_socket->backlog_tail = 0;
    }
    BacklogTail = server_socket->backlog_tail;

// coloca na fila.
// Coloca o ponteiro para estrutura de socket
// na fila de conexões pendentes na estrutura do servidor.
// Isso será usado pelo accept() para encontrar
// a estrutura do socket do cliente.
    server_socket->pending_connections[BacklogTail] = 
        (unsigned long) client_socket;

// Em que posiçao estamos na fila.
    client_socket->client_backlog_pos = BacklogTail;

// #
// O cliente está esperando que sua conexão seja aceita pelo servidor.

    // #debug
    //debug_print("sys_connect: Pending connection\n");

    // #debug
    //if (Verbose==TRUE){
    //printk     ("sys_connect: Pending connection\n");
    //refresh_screen();
    //}

    // #debug 
    // #breakpoint
    if (Verbose == TRUE)
    {
        printk("__connect_local: Breakpoint :)\n");
        refresh_screen();
        while (1){
            asm (" hlt ");
        };
    }
//ok
    return 0;
fail:
    debug_print("__connect_local: Fail\n");
    printk     ("__connect_local: Fail\n");
    return (int) -1;
}


// syscall implementation
int 
sys_connect ( 
    int sockfd, 
    const struct sockaddr *addr,
    socklen_t addrlen )
{
// Service 7001
// #warning
// We can have two types of address.
// One for local and another one for inet.

// Invalid address.
    if ((void *) addr == NULL){
        printk ("sys_connect: addr\n");
        return (int) (-EINVAL);
    }

// #test
// Address validation.
// It needs to be a ring3 address.
// #todo: Check agains more limits.
    if (addr < CONTROLTHREAD_BASE){
        panic("sys_connect: addr is not ring3\n");
        //return (int) (-EINVAL);
    }


// -----------------------------
// Testing for local domains.
// If ti fails, try the inet domains.
    int IsLocal = FALSE;

    switch (addr->sa_family)
    {
        //case AF_LOCAL:
        case AF_UNIX:
        case AF_GRAMADO:
            IsLocal = TRUE;
            break;
        default:
            IsLocal = FALSE;
            break;
    };

// ----------------------------------
// We're gonna need the same parameters
// in both cases.
// #test
// Ate o momento, todos os clientes em zz/omega2/apps/
// estao usando AF_INET e o IP e' localhost.
// #todo
// see: socket.h and in.h for the structures.

    if (IsLocal == TRUE){
        return (int) __connect_local( sockfd, addr, addrlen );
    } else if (IsLocal == FALSE){
        return (int) __connect_inet( sockfd, addr, addrlen );
    }; 

// Unexpected error
    return (int) -1;
}

int 
sys_getsockname ( 
    int sockfd, 
    struct sockaddr *addr, 
    socklen_t *addrlen )
{
// Service 7007

// Process, file and socket.
    struct process_d  *p;
    pid_t current_process = -1;
    struct file_d     *f;
    struct socket_d   *s;

// Parameter
    if ( sockfd < 0 || sockfd >= OPEN_MAX ){
        printk ("sys_getsockname: [FAIL ] sockfd\n");
        goto fail;
    }

// process
    current_process = (pid_t) get_current_process();
    p = (struct process_d *) processList[current_process]; 
    if ((void *) p == NULL){
        printk ("sys_getsockname: p fail\n");
        goto fail;
    }

// file
    f = (file *) p->Objects[sockfd];
    if ((void *) f == NULL){
        printk ("sys_getsockname: f fail\n");
        goto fail;
    }

//socket
    s = (struct socket_d *) p->priv;
    if ((void *) s == NULL){
        printk ("sys_getsockname: s fail\n");
        goto fail;
    }

// addr
// Usando a estrutura que nos foi passada.
    if ((void *) addr == NULL){
        printk ("sys_getsockname: addr fail\n");
        goto fail;
    }

// Everything is ok.
// So now we need to include the 'name' into the socket structure
// respecting the socket's family.
    register int n = 0;
    
    if (s->addr.sa_family == AF_GRAMADO)
    {
        // Binding the name to the socket.
        printk ("sys_getsockname: Getting the name and the size\n");
        addrlen[0] = n;
        // Always 14.
        for (n=0; n<14; n++){
            addr->sa_data[n] = s->addr.sa_data[n];
        };
        debug_print ("sys_getsockname: copy ok\n");

        // Done!
        return 0;
    }

    // #todo
    // More families.
    //if ( ...
    //if ( ...
    //if ( ...
    // ...
    
    printk ("sys_getsockname: process %d ; family %d ; len %d \n", 
        current_process, addr->sa_family, addrlen  );
 
 fail:
    printk ("sys_getsockname: fail\n");
    refresh_screen();
    return (int) -1;
}

/*
 * sys_listen:
 *     É usado pra dizer que o servidor esta 
 * pronto para receber conecxões e 
 * a quantidade de clientes que podem ser conectados.
 */
// See:
// https://man7.org/linux/man-pages/man2/listen.2.html
/*
 The backlog argument defines the maximum length to which the queue of
 pending connections for sockfd may grow.  If a connection request
 arrives when the queue is full, the client may receive an error with
 an indication of ECONNREFUSED or, if the underlying protocol supports
 retransmission, the request may be ignored so that a later reattempt
 at connection succeed.
*/
// listen() is used on the server side, and 
// causes a bound TCP socket to enter listening state.
// IN:
// sockfd  = The fd of the server's socket.
// backlog = The server indicates the 'size of the list'.

int sys_listen(int sockfd, int backlog) 
{
// Service 7004
// Called by sci.c

    struct process_d  *p;
    pid_t current_process = -1;
    file *f;
    struct socket_d  *s;
    int Backlog=0;

    do_credits_by_tid(current_thread);

// #debug
    //debug_print ("sys_listen: [TODO]\n");
    //printk      ("sys_listen: [TODO] fd=%d backlog=%d\n",
        //sockfd, backlog);

// Parameter
// The fd of the server's socket.
    if ( sockfd < 0 || sockfd >= OPEN_MAX )
    {
        debug_print ("sys_listen: sockfd\n");
        printk      ("sys_listen: sockfd\n");
        return (int) (-EINVAL);
    }

// backlog:
// The server tell us the the 'size of the list'.
// Wrong n. Ajusting to default.
// It can't be bigger than the size of the array.
// #todo: Use SOCKET_MAX_PENDING_CONNECTIONS
// SOMAXCONN

    Backlog = backlog;
    if (Backlog <= 0) { Backlog=1; }
    if (Backlog >= 32){ Backlog=31; }

// We need to get the socket structure in the process structure.
// We need to clean the list. Not here. when creating the socket.

/*
    //int i=0;
    //for(i=0; i<32; i++) { s->pending_connections[i] = 0;};
    // Updating the list support.
    //s->backlog_max = backlog;  //max
    //s->backlog_pos = 0;        //current 
 */

// ==============================================
    current_process = (pid_t) get_current_process();
    if ( current_process < 0 || current_process >= PROCESS_COUNT_MAX )
    {
        printk("sys_listen: current_process\n");
        goto fail;
    }
// process
    p = (struct process_d *) processList[current_process];
    if ((void *) p == NULL){
        debug_print("sys_listen: p fail\n");
        printk     ("sys_listen: p fail\n");
        goto fail;
    }

// file 
// sender's file
// Objeto do tipo socket.
    f = (file *) p->Objects[sockfd];
    if ((void *) f == NULL){
        debug_print("sys_listen: f fail\n");
        printk     ("sys_listen: f fail\n");
        goto fail;
    }

// Is it a socket object?
    int IsSocket = -1;
    IsSocket = (int) is_socket((file *) f);
    if (IsSocket != TRUE){
        debug_print ("sys_listen: f is not a socket\n");
        printk      ("sys_listen: f is not a socket\n");
        //return (int) (-EINVAL);
        goto fail;
    }

// This is right place for doing this.

    //f->sync.can_accept = TRUE;

// Socket structure.
// Pega a estrutura de socket associada ao arquivo.
// socket structure in the senders file.

    //s = (struct socket_d *) p->priv; 
    s = (struct socket_d *) f->socket;
    if ((void *) s == NULL){
        debug_print("sys_listen: s fail\n");
        printk     ("sys_listen: s fail\n");
        goto fail;
    }

    if (f->socket != p->priv){
        panic("sys_listen: [TEST] f->socket != p->priv\n");
    }

// Updating the socket structure.
    s->backlog_max = (int) Backlog;
// This server is accepting new connections.
    s->isAcceptingConnections = TRUE;
    // ...

    //debug_print ("sys_listen: [TODO] continue...\n");
    //printk      ("sys_listen: [TODO] continue...\n");

    // ...

//fake ok.
    return 0;
// ==============================================
fail:
    debug_print("sys_listen: [FAIL]\n");
    printk     ("sys_listen: [FAIL]\n");
    //refresh_screen();
    //while(1){}
    return (int) -1;
}

// libc shutdown() function.
// See: https://linux.die.net/man/3/shutdown
int sys_socket_shutdown(int socket, int how)
{
// Service 7009

    // #todo
    // Desconectar dois sockets,
    // mas nao destruir o socket?

// The current process.
    struct process_d *p;
    pid_t current_process = -1;
// The file indicated by the fd.
    file *f;
// The socket structure for the file.
    struct socket_d *s;
// Is this file a socket object?
    int IsSocketObject = -1;

    debug_print ("sys_socket_shutdown: [TODO]\n");
    //printk      ("sys_socket_shutdown: [TODO] fd=%d how=%d\n",
    //    socket, how );

// Invalid fd.
    if ( socket < 0 || socket >= OPEN_MAX ){
        debug_print ("sys_socket_shutdown: [FAIL] fd\n");
        printk      ("sys_socket_shutdown: [FAIL] fd\n");
        return (int) (-EBADF);
    }

// Process
    current_process = (pid_t) get_current_process();
    if (current_process < 0  || current_process >= PROCESS_COUNT_MAX){
        printk      ("sys_socket_shutdown: p fail\n");
        goto fail;        
    }
    p = (struct process_d *) processList[current_process];
    if ((void *) p == NULL){
        debug_print ("sys_socket_shutdown: p fail\n");
        printk      ("sys_socket_shutdown: p fail\n");
        goto fail;
    }

// File
// sender's file
// Objeto do tipo socket.

    f = (file *) p->Objects[socket];
    if ((void *) f == NULL){
        debug_print("sys_socket_shutdown: f fail\n");
        printk     ("sys_socket_shutdown: f fail\n");
        goto fail;
    }

// Is this file a socket object?
    IsSocketObject = is_socket((file *) f);
    if (IsSocketObject != 1){
        debug_print("sys_socket_shutdown: f is not a socket\n");
        printk     ("sys_socket_shutdown: f is not a socket\n");
        goto fail;
    }

// Socket
// Yes, This is a socket object.
// Let's get the socket structure associated with the file.
// Let's simply change the flag for this socket.
    s = (struct socket_d *) f->socket;
    if ((void *) s == NULL){
        debug_print ("sys_socket_shutdown: s fail\n");
        printk      ("sys_socket_shutdown: s fail\n");
        goto fail;
    // permanece conectado, mas usaremos outro da fila.
    } else {
        s->state = 216;  // ?
        return 0;
    };

    // ...

fail:
    debug_print("sys_socket_shutdown: [FAIL]\n");
    printk     ("sys_socket_shutdown: [FAIL]\n");
    return (int) -1;
}

//
// End
//

