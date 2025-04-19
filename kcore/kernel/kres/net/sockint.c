// sockint.c
// Internal routines.
// workers.
// Low level routines called by socket.c
// Created by Fred Nora.

#include <kernel.h>

// Internal
#define SYS_SOCKET_IP(a, b, c, d) \
    (a << 24 | b << 16 | c << 8 | d)


// globals
struct socket_d  *CurrentSocket;
struct socket_d  *LocalHostHTTPSocket;
// ...


unsigned long socketList[SOCKET_COUNT_MAX];

// private:
// A small list of PIDs.
// A server can register its PID here
// telling the system that it is responsible for this kind of service.
static pid_t gramado_ports[GRAMADO_PORT_MAX];

// ----------------------------

static int __socket_initialize_gramado_ports(void);

// -------------------------------

static int __socket_initialize_gramado_ports(void)
{
    register int i=0;
    for (i=0; i<GRAMADO_PORT_MAX; i++){
        gramado_ports[i] = 0;
    };
    return 0;
}

pid_t socket_get_gramado_port (int port)
{
    if (port<0 || port >31){
        debug_print ("socket_set_gramado_port: port fail\n");
        return (pid_t) -1;
    }
    return (pid_t) gramado_ports[port];
}

// Conjunto especiais de portas.
// Usados apenas na famíla AF_GRAMADO.
int socket_set_gramado_port (int port, pid_t pid)
{
    if (port<0 || port >31){
        debug_print ("socket_set_gramado_port: port fail\n");
        return -1;
    }
    if (pid<0 || pid >= PROCESS_COUNT_MAX){
        debug_print ("socket_set_gramado_port: port fail\n");
        return -1;
    }
    gramado_ports[port] = (pid_t) pid;

    return 0;
}

/*
 * create_socket_object: 
 *     It creates a socket structure.
 *     Every process has its own socket structure.
 *     The status is: NOT CONNECTED.
 */
struct socket_d *create_socket_object(void)
{
// Crete the socket structure.
// see: socket.h

    struct socket_d *s;
    register int i=0;

// Create and clean the structure.
    s = (void *) kmalloc( sizeof(struct socket_d) );
    if ((void *) s ==  NULL){
        printk("create_socket_object: s\n");
        goto fail;
    }
    memset( s, 0, sizeof(struct socket_d) );

    //s->objectType =
    //s->objectClass =

    s->id = 0;  //#todo

    s->pid = (pid_t) get_current_process();
    s->uid = (uid_t) current_user;
    s->gid = (gid_t) current_group;

// #todo
    //s->addr = 0;

    s->family = 0;
    s->type = 0;   // (SOCK_STREAM, SOCK_DGRAM, SOCK_RAW ...)
    s->protocol = 0;

// IP:Port
    s->ip_ipv6 = (unsigned long) 0;
    s->ip_ipv4 = (unsigned int) 0;
    s->port = (unsigned short) 0;


// The buffer.
// The data goes into the stream.
    s->private_file = (file *) 0;

// Socket flags
// The flags that describe the state of this socket.
    s->flags = 0;

// The flags used in TCP connections,
// Data Offset (4bits) | Reserved (6bits) | Control bits (6bits).
    s->tcp__do_res_flags = 0;


// Initializing pointers.
// We don't want this kinda crash in the real machine.

// It's a pointer to another socket. 
    s->link = NULL;

// The counter
    s->connections_count = 0;

// max backlog = 32.
    for (i=0; i<32; i++){
        s->pending_connections[i] = 0;
    };
    s->backlog_head = 0;   //sai
    s->backlog_tail = 0;   //entra
    s->backlog_max = 4;    //default 

// Not yet.
// listen() will set this flag.
    s->isAcceptingConnections = FALSE;
// The socket needs to be initialized 
// in the disconnected state.
    s->state = SS_UNCONNECTED;
// #bugbug
// It tells us that write() will copy 
// the data to the connected socket.
    s->conn_copy = TRUE;  //YES, copy!
// The server finds a place in the server_process->Objects[i].
    s->clientfd_on_server = -1;

    // ...

    s->used = TRUE;
    s->magic = 1234;

    return (struct socket_d *) s;

fail:
    //refresh_screen();
    return NULL;
}

// #debug
// Show the private socket for a process.
void show_socket_for_a_process (pid_t pid)
{
    struct process_d  *p;
    struct socket_d  *s;

    // #debug
    printk ("Socket info for pid %d: \n", pid);

// Parameter
    if (pid<0 || pid >= PROCESS_COUNT_MAX){
        printk ("pid limits\n");
        goto fail;
    }

// process
    p = (struct process_d *) processList[pid];
    if ((void *) p == NULL){
        printk("p\n");
        goto fail;
    }
    if ( p->used != TRUE || p->magic != 1234 ){
        printk("p validation\n");
        goto fail;
    }

// Socket struture.
// Pega o ponteiro para a estrutura privada de soquete.
    s = (struct socket_d *) p->priv;
    if ((void *) s == NULL){
        printk("s\n");
        goto fail;
    }
    if ( s->used != TRUE || s->magic != 1234 ){
        printk ("s validation\n");
        goto fail;
    }

//done:
// Show
// sockaddr structure.
    printk ("family %d \n",s->addr.sa_family);
    printk ("data %s   \n",s->addr.sa_data);
    //printk ("",s->);
    //printk ("",s->);
    //printk ("",s->);
    //refresh_screen();
    return;

fail:
   //refresh_screen();
   return;
}

// Get the pointer for the socket structure 
// given the fd.
// OUT: Returning a pointer for a 
// ring0 socket structure.
struct socket_d *get_socket_from_fd(int fd)
{
// Get from the current process.

    struct process_d *p;
    pid_t current_process = -1;
    file *_file;

// Parameter
    if (fd<0 || fd>=OPEN_MAX){
        goto fail;
    }

//----------------
// #todo: We need a worker for that routine.
// process
    current_process = (pid_t) get_current_process();
    if (current_process<0 || current_process>=PROCESS_COUNT_MAX){
        goto fail;
    }
    p = (struct process_d *) processList[current_process];
    if ((void *) p == NULL){
        goto fail;
    }
    // magic?
//----------------

// The file.
    _file = (file *) p->Objects[fd];
    if ((void *) _file == NULL){
        goto fail;
    }
    if (_file->magic != 1234){
        goto fail;
    }

// Return the pointer for the socket structure given the fd.
    return (struct socket_d *) _file->socket;
fail:
    return NULL;
}

// Os dois são arquivos no mesmo processo. O processo atual.
int
sock_socketpair ( 
    int family, 
    int type, 
    int protocol, 
    int usockvec[2] )
{
    int fd1 = -1; 
    int fd2 = -1;
    struct socket_d *sock1;
    struct socket_d *sock2;

// #todo
// Check parameters.

// ===============
    fd1 = (int) sys_socket(family,type,protocol);
    // #todo: Check validation.
    sock1 = (struct socket_d *) get_socket_from_fd(fd1);
    if ((void*) sock1 == NULL){
        debug_print("sock_socketpair: sock1\n");
        return -1;
    }
// ===============
    fd2 = (int) sys_socket(family,type,protocol);
    // #todo: Check validation.
    sock2 = (struct socket_d *) get_socket_from_fd(fd2);
    if ((void*) sock2 == NULL){
        debug_print("sock_socketpair: sock2\n");
        return -1;
    }
// #todo
// Antes é preciso verificar a área de memória.
    usockvec[0] = (int) fd1;
    usockvec[1] = (int) fd2;

    return 0;
}

// socket_dialog
// Serviços de soquetes da klibc
// #todo: rever os números.
// #todo: devemos mudar os argumentos 
// para ficar no padrão da libc.
// OUT: ?
unsigned long 
socket_dialog ( 
    unsigned long number, 
    unsigned long arg2, 
    unsigned long arg3, 
    unsigned long arg4 )
{

    // #debug
    printk ("socket_dialog: number=%d \n", number);

    if ( number < 7000 || number >= 8000 ){
        //message
        return 0;
    }

// Service numbers
    switch (number)
    {
        // socket(...)
        // family, type, protocol
        // vai retornar o descritor de uma stream.
        case 7000:
            debug_print ("socket_dialog: 7000. sys_socket\n");
            return (unsigned long) sys_socket ( (int) arg2, 
                                       (int) arg3, (int) arg4 );
            break;

        // send
        // case 7001:
        //     return (unsigned long) 1;
        //     break;

        // ...

        default:
            debug_print ("socket_dialog: default\n");
            printk      ("socket_dialog: default\n");
            refresh_screen();
            break;
    };

    // Fail
    return 0;
}

unsigned int getSocketIPV4(struct socket_d *socket)
{
    if ((void *) socket == NULL){
        return 0;
    }
    if (socket->magic != 1234){
        return 0;
    }
    return (unsigned int) socket->ip_ipv4;
}

unsigned long getSocketIPV6(struct socket_d *socket)
{
    if ((void *) socket == NULL){
        return 0;
    }
    if (socket->magic != 1234){
        return 0;
    }
    return (unsigned long) socket->ip_ipv6;
}

unsigned short getSocketPort(struct socket_d *socket)
{
    if ((void *) socket == NULL){
        return 0;
    }
    if (socket->magic != 1234){
        return 0;
    }
    return (unsigned short) socket->port;
}

// Get the socket structure in the process list given the port number.
struct socket_d *get_socket_in_process_list(unsigned short target_port)
{
    struct socket_d *sock;
    register int i=0;
    struct process_d *process;

    for (i=0; i<PROCESS_COUNT_MAX; i++)
    {
        // Get process
        process = (struct process_d *) processList[i];
        if ((void*) process != NULL)
        {
            if (process->magic == 1234)
            {
                // Get the socket structure.
                sock = (struct socket_d *) process->priv;
                if ((void*) sock != NULL)
                {
                    if (sock->magic == 1234)
                    {
                        // Is it a socket structure?
                        if (sock->objectType == ObjectTypeSocket)
                        {
                            // OK, we have a valid socket structure.
                            // Let's find out if this is socket we're
                            // looking for.
                            if (sock->port == target_port){
                                return (struct socket_d *) sock;
                            }
                        }
                    }
                }
            }
        }
    };
    
fail:
    return NULL;
}

int 
update_socket ( 
    struct socket_d *socket, 
    unsigned int ip_ipv4, 
    unsigned short port )
{
// Update ipv4 and port info.

    if ((void *) socket == NULL){
        return (int) -1;
    }
    socket->ip_ipv4 = (unsigned int) ip_ipv4;
    socket->port = (unsigned short) port;
    return 0;
}

int socket_read ( int fd, char *buf, int count )
{
    debug_print ("socket_read:[TODO]\n");

    if (fd<0)
        return (int) -EINVAL;

// #todo:
// Here we can check if it is really a socket.

    return (int) __read_imp(fd,buf,count);
}

// IN:
// fd = The file that handles the socket structure.
// buf
// count.
int socket_write ( int fd, char *buf, int count )
{
    debug_print ("socket_write:[TODO]\n");

    if (fd<0)
        return (int) -EINVAL;

// #todo:
// Here we can check if it is really a socket.

    return (int) __write_imp(fd,buf,count);
}

int socket_ioctl ( int fd, unsigned long request, unsigned long arg )
{
    struct process_d *p;
    pid_t current_process = -1;
    file *f;

    debug_print ("socket_ioctl: TODO\n");

// Parameters
    if (fd < 0 || fd >= OPEN_MAX){
        return (int) (-EBADF);
    }
// ...

// process
    current_process = (pid_t) get_current_process();
    p = (void*) processList[current_process];
    if ((void *) p == NULL){
        debug_print("socket_ioctl: p\n");
        goto fail;
    }

// file
    f = (file *) p->Objects[fd];
    if ((void *) f == NULL){
        debug_print("socket_ioctl: f\n");
        goto fail;
    }
    if (f->used != TRUE || f->magic != 1234 ){
        panic("socket_ioctl: validation\n");
    }

// Request

    switch (request){

    // #bugbug
    // Nao conseguimos usar direito os elementos da estrutura
    // Precisamos trabalhar na interrupçao do sistema,
    // na coisa dos segmentos de dados ...
    // Por causa do tipo de segmento, estamos escrevendo 
    // ou lendo no lugar errado.

    case 4000:
        debug_print ("socket_ioctl: [4000]\n");
        printk("socket_ioctl: [4000] fd %d pid %d #debug\n", fd, arg);
        //refresh_screen();
        // Is it a valid pid?
        f->sync.sender_pid = (pid_t) arg;
        return 0;
        break;

    case 4001:
        return (int) f->sync.sender_pid;
        break;

    case 4002:
        return (int) f->sync.can_read;
        break;

    case 4003:
        return (int) f->sync.can_write;
        break;

    case 4004:
        return (int) f->sync.can_execute;
        break;

    // ...

    default:
        return (int) (-EINVAL);
        break;
    };

fail:
    return (int) -1;
}


// #todo:
// Configura a estrutura de socket para um socket da família AF_GRAMADO.
// Vai retornar o descritor de um arquivo.
int 
socket_gramado ( 
    struct socket_d *sock, 
    int family, 
    int type, 
    int protocol )
{
// Called by sys_socket().

    struct process_d *Process;
    pid_t current_process = -1;
    file *_file;
    char *buff;
    register int i=0;
    int __slot = -1;

    //debug_print ("socket_gramado:\n");

// Parameters
    if ((void*) sock == NULL){
        debug_print ("socket_gramado: sock\n");
        goto fail;
    }
    if (sock->magic != 1234){
        debug_print ("socket_gramado: sock validation\n");
        goto fail;
    }
    if (family != AF_GRAMADO){
        debug_print ("socket_gramado: family\n");
        goto fail;
    }

    sock->addr.sa_family = family;
    sock->addr.sa_data[0] = 'x';
    sock->addr.sa_data[1] = 'x';

// #todo
// For AF_GRAMADO the type needs to be SOCK_RAW
// and the protocol needs to be GRAMADO_PROTOCOL.

// Process
    current_process = (pid_t) get_current_process();
    if ( current_process < 0 || current_process >= PROCESS_COUNT_MAX ){
        printk ("socket_gramado: current_process\n");
        goto fail;
    }
    Process = (void *) processList[current_process];
    if ((void *) Process == NULL){
        printk("socket_gramado: Process\n");
        goto fail;
    }
    if (Process->used != TRUE || Process->magic != 1234){
        printk("socket_gramado: Process validation\n");
        goto fail;
    }

//#todo
//temos que criar uma rotina que procure slots em Process->Objects[]
//e colocarmos em process.c
//essa é afunção que estamos criando.
    
    // process_find_empty_stream_slot ( struct process_d *process );

// #improvisando
// 0, 1, 2 são reservados para o fluxo padrão.
// Como ainda não temos rotinas par ao fluxo padrão,
// pode ser que peguemos os índices reservados.
// Para evitar, começaremos depois deles.
// Reserva um slot.
// 31 is also reserved. The server is reading 
// only in this fd.
    __slot = -1; //fail
    //for ( i=3; i<NUMBER_OF_FILES; i++ )
    for ( i=3; i<31; i++ )
    {
        if (Process->Objects[i] == 0)
        { 
            __slot = (int) i;
            break; 
        }
    };

// Check slot validation. 
    if (__slot == -1){
        printk ("socket_gramado: [FAIL] No free slots\n");
        goto fail;
    }

// Buffer

    //buff = (char *) newPage ();
    buff = (char *) kmalloc(BUFSIZ);
    if ((void *) buff == NULL){
        //Process->Objects[__slot] = (unsigned long) 0;
        debug_print ("socket_gramado: buff fail\n");
        printk      ("socket_gramado: buff fail\n");
        goto fail;
    }

// File
    _file = (void *) kmalloc ( sizeof(file) );
    if ((void *) _file == NULL){
        //Process->Objects[__slot] = (unsigned long) 0;
        debug_print ("socket_gramado: [FAIL] _file fail\n");
        printk      ("socket_gramado: [FAIL] _file fail\n");
        goto fail;
    }
    memset( _file, 0, sizeof(struct file_d) );

// Object type
    _file->____object = ObjectTypeSocket;

// #todo: Call the methods.
    _file->pid = (pid_t) current_process;
    _file->uid = (uid_t) current_user;
    _file->gid = (gid_t) current_group;

// sync
// #todo: Maybe we can have a method for this routine.

    _file->sync.sender_pid = (pid_t) -1;
    _file->sync.receiver_pid = (pid_t) -1;
    _file->sync.action = ACTION_NULL;
    _file->sync.can_read    = TRUE;
    _file->sync.can_write   = TRUE;
    _file->sync.can_execute = FALSE;
    _file->sync.can_accept  = TRUE;
    _file->sync.can_connect = TRUE;

    _file->sync.block_on_read = FALSE;
    _file->sync.block_on_read_empty = TRUE;

    _file->sync.block_on_write = TRUE;
    _file->sync.block_on_write_full = TRUE;

// Is it blocked?
    _file->sync.is_blocked = FALSE;
// flags
    _file->_flags = __SWR;  // OK to write.

// No name for now.
    _file->_tmpfname = NULL;
    //_file->_tmpfname = "socket";

// Buffer

    _file->_base = buff;
    _file->_p    = buff;
    _file->_lbfsize = BUFSIZ;
// Quanto falta
    _file->_cnt = _file->_lbfsize;
 // Offsets
    _file->_r = 0;
    _file->_w = 0;
// Status do buffer do socket.
    _file->socket_buffer_full = 0;

//
// Socket
//

// Socket pointer
// Associando a estrutura de socket à estrutura de arquivo.
    _file->socket = sock;
// O arquivo do soquete, o buffer ?
    sock->private_file = (file *) _file; 
// Socket pointer.
// Salvamos o ponteira para estrutura de soquete
// na estrutura de processo do processo atual.
    Process->priv = (void *) sock;


// fd
    _file->_file = __slot;
// Colocando na lista de arquivos abertos no processo.
    Process->Objects[__slot] = (unsigned long) _file;
    _file->used = TRUE;
    _file->magic = 1234;
// ok.
// Retornamos o fd na lista de arquivos abertos pelo processo.
    return (int) __slot;

fail:
    debug_print ("socket_gramado: [FAIL]\n");
    //refresh_screen();
    return (int) (-1);
}

// Configura a estrutura de socket para um socket da
// família AF_UNIX or AF_LOCAL.
// Precisa ser uma rotina parecida com a do outro tipo criado.
int 
socket_unix ( 
    struct socket_d *sock, 
    int family, 
    int type, 
    int protocol )
{
// Called by sys_socket().

    struct process_d *Process;
    pid_t current_process = -1;
    file *_file;
    char *buff;
    register int i=0;
    int __slot = -1;

    //debug_print ("socket_unix:\n");

// Parameters
    if ((void*) sock == NULL){
        debug_print ("socket_unix: sock\n");
        goto fail;
    }
    if (sock->magic != 1234){
        debug_print ("socket_unix: sock validation\n");
        goto fail;
    }
    if (family != AF_UNIX){
        debug_print ("socket_unix: family\n");
        goto fail;
    }

    sock->addr.sa_family = family;
    sock->addr.sa_data[0] = 'x'; 
    sock->addr.sa_data[1] = 'x';

// #test
// GRAMADO_PROTOCOL is a valid protocol in AF_UNIX.

// Process
    current_process = (pid_t) get_current_process();
    if ( current_process < 0 || current_process >= PROCESS_COUNT_MAX ){
        printk ("socket_unix: current_process\n");
        goto fail;
    }
    Process = (void *) processList[current_process];
    if ((void *) Process == NULL){
        printk("socket_unix: Process\n");
        goto fail;

    }
    if ( Process->used != TRUE || Process->magic != 1234 ){
        printk("socket_unix: Process validation\n");
        goto fail;
    }

// #todo
// Temos que criar uma rotina que procure slots em Process->Streams[]
// e colocarmos em process.c
// essa é afunção que estamos criando.
// process_find_empty_stream_slot ( struct process_d *process );

// #improvisando
// 0, 1, 2 são reservados para o fluxo padrão.
// Como ainda não temos rotinas par ao fluxo padrão,
// pode ser que peguemos os índices reservados.
// Para evitar, começaremos depois deles.

    __slot = -1;
    // Reserva um slot.
    //for ( i=3; i< NUMBER_OF_FILES; i++ )
    for ( i=3; i< 31; i++ )
    {
        if ( Process->Objects[i] == 0 )
        { 
            __slot = (int) i; 
            break; 
        }
    };

// Check slot validation. 
    if ( __slot == -1 ){
        printk ("socket_unix: No free slots\n");
        goto fail;
    }

// Buffer
    //buff = (char *) newPage ();
    buff = (char *) kmalloc(BUFSIZ);
    if ((void *) buff == NULL){
        //Process->Objects[__slot] = (unsigned long) 0;
        debug_print ("socket_unix: [FAIL] Buffer allocation fail\n");
        printk      ("socket_unix: [FAIL] Buffer allocation fail\n");
        goto fail;
    }

// File
    _file = (void *) kmalloc( sizeof(file) );
    if ((void *) _file == NULL){
        //Process->Objects[__slot] = (unsigned long) 0;
        printk ("socket_unix: _file fail\n");
        goto fail;
    }
    memset( _file, 0, sizeof(struct file_d) );

// #todo: 
// Podemos adiar isso para o fim da rotina.
    _file->used = TRUE;
    _file->magic = 1234;

// #todo: Use methods.
    _file->pid = (pid_t) current_process;
    _file->uid = (uid_t) current_user;
    _file->gid = (gid_t) current_group;

    _file->____object = ObjectTypeSocket;

// sync
// #todo: Maybe we can have a method for this routine.

    _file->sync.sender_pid = (pid_t) -1;
    _file->sync.receiver_pid = (pid_t) -1;
    _file->sync.action = ACTION_NULL;
    _file->sync.can_read = TRUE;
    _file->sync.can_write = TRUE;
    _file->sync.can_execute = FALSE;
    _file->sync.can_accept = TRUE;
    _file->sync.can_connect = TRUE;

    _file->sync.block_on_read = FALSE;
    _file->sync.block_on_read_empty = TRUE;

    _file->sync.block_on_write = TRUE;
    _file->sync.block_on_write_full = TRUE;

// Is it blocked?
    _file->sync.is_blocked = FALSE;
// Flags
    _file->_flags = __SWR;  // OK to write.

// No name for now.
// #bugbug: 
// unix sockets has names.
// it's like a normal fs pathname.
    _file->_tmpfname = NULL;
    //_file->_tmpfname = "/tmp/socketXXX";    

// Buffer

    _file->_base = buff;
    _file->_p    = buff;
    _file->_lbfsize = BUFSIZ; 
// Quanto falta
    _file->_cnt = _file->_lbfsize;
    _file->_r = 0;
    _file->_w = 0;
    _file->socket_buffer_full = 0;  

//
// Socket
//

    _file->socket = sock;
// O arquivo do soquete, o buffer?
    sock->private_file = (file *) _file; 
// Salvamos o ponteira para estrutura de soquete
// na estrutura de processo do processo atual.
    Process->priv = (void *) sock;

// fd
    _file->_file = __slot;
//Colocando na lista de arquivos abertos no processo.
    Process->Objects[__slot] = (unsigned long) _file;
// ok.
// Retornamos o fd na lista de arquivos abertos pelo processo.
    return (int) __slot;

fail:
    debug_print ("socket_unix: fail\n");
    //refresh_screen();
    return (int) (-1);
}

// Configura a estrutura de socket para um socket da
// família AF_INET.
// Precisa ser uma rotina parecida com a do outro tipo criado.
// Esse domínio lida com tcp/ip ?

// #todo
// Based on the socket type when the domain is AF_INET.
// + SOCK_STREAM = tcp socket.
// + SOCK_DGRAM  = udp socket.
// + SOCK_RAW    = raw socket.

int 
socket_inet ( 
    struct socket_d *sock, 
    int family,
    int type, 
    int protocol )
{
// Called by sys_socket().

    struct process_d *Process;
    pid_t current_process = -1;
    file *_file;
    char *buff;
    register int i=0;
    int __slot = -1;

// Parameters
    if ((void*) sock == NULL){
        debug_print ("socket_inet: sock\n");
        goto fail;
    }
    if (sock->magic != 1234){
        debug_print ("socket_inet: sock validation\n");
        goto fail;
    }
    if (family != AF_INET){
        debug_print ("socket_inet: family\n");
        goto fail;
    }

    sock->addr_in.sin_family      = AF_INET;
    sock->addr_in.sin_addr.s_addr = SYS_SOCKET_IP(127,0,0,1);
    sock->addr_in.sin_port        = 11369;
    // #todo:
    // This is a default port number ... we need to pick 
    // a valid one for each process created.

// #todo
// Based on the socket type when the domain is AF_INET.
// + SOCK_STREAM = tcp socket.
// + SOCK_DGRAM  = udp socket.
// + SOCK_RAW    = raw socket.

    // Already done when the socket was created.
    // sock->type = type;
    // sock->type = protocol;

// #test
// GRAMADO_PROTOCOL is a valid protocol in AF_INET.

// Process
    current_process = (pid_t) get_current_process();
    if ( current_process < 0 || current_process >= PROCESS_COUNT_MAX ){
        printk ("socket_inet: current_process\n");
        goto fail;
    }
    Process = (void *) processList[current_process];
    if ((void *) Process == NULL){
        printk("socket_inet: Process\n");
        goto fail;
    }
    if ( Process->used != TRUE || Process->magic != 1234 ){
        printk("socket_inet: Process validation\n");
        goto fail;
    }

//#todo
//temos que criar uma rotina que procure slots em Process->Streams[]
//e colocarmos em process.c
//essa é afunção que estamos criando.

    // process_find_empty_stream_slot ( struct process_d *process );

// #improvisando
// 0, 1, 2 são reservados para o fluxo padrão.
// Como ainda não temos rotinas par ao fluxo padrão,
// pode ser que peguemos os índices reservados.
// Para evitar, começaremos depois deles.
    
    __slot=-1;
    //for ( i=3; i< NUMBER_OF_FILES; i++ )
    for ( i=3; i< 31; i++ )
    {
        if ( Process->Objects[i] == 0 )
        { 
            __slot = (int) i; 
            break; 
        }
    };

// Fail
    if ( __slot == -1 ){
        printk ("socket_inet: No free slots\n");
        goto fail;
    }

// Buffer
    //buff = (char *) newPage ();
    buff = (char *) kmalloc(BUFSIZ);
    if ((void *) buff == NULL){
        //Process->Objects[__slot] = (unsigned long) 0;
        debug_print ("socket_inet: [FAIL] Buffer allocation fail\n");
        printk      ("socket_inet: [FAIL] Buffer allocation fail\n");
        goto fail;
    }

// File
// Create a new file structure and clear the structure.
    _file = (void *) kmalloc( sizeof(file) );
    if ((void *) _file == NULL){
        //Process->Objects[__slot] = (unsigned long) 0;
        printk ("socket_inet: _file fail\n");
        goto fail;
    }
    memset( _file, 0, sizeof(struct file_d) );

// #todo: Use methods to grab these values.
    _file->pid = (pid_t) current_process;
    _file->uid = (uid_t) current_user;
    _file->gid = (gid_t) current_group;

// Object type
    _file->____object = ObjectTypeSocket;

// sync
// #todo: Maybe we can use methods for this routine.
    _file->sync.sender_pid = (pid_t) -1;
    _file->sync.receiver_pid = (pid_t) -1;
    _file->sync.action = ACTION_NULL;
    _file->sync.can_read = TRUE;
    _file->sync.can_write = TRUE;
    _file->sync.can_execute = FALSE;
    _file->sync.can_accept = TRUE;
    _file->sync.can_connect = TRUE;

    _file->sync.block_on_read = FALSE;
    _file->sync.block_on_read_empty = TRUE;

    _file->sync.block_on_write = TRUE;
    _file->sync.block_on_write_full = TRUE;

// Is it blocked?
    _file->sync.is_blocked = FALSE;
// flags 
    _file->_flags = __SWR;  // OK to write.

// No name for now.
    _file->_tmpfname = NULL;
    //_file->_tmpfname = "socket";

// Buffer support:
    _file->_base    = buff;
    _file->_p       = buff;
    _file->_lbfsize = BUFSIZ;
// Quanto falta
    _file->_cnt = _file->_lbfsize;
    _file->_r = 0;
    _file->_w = 0;
    _file->socket_buffer_full = 0;

//
// Socket
//

    _file->socket = sock;
// O arquivo do soquete, o buffer ?
    sock->private_file = (file *) _file;
// Salvamos o ponteira para estrutura de soquete
// na estrutura de processo do processo atual.
    Process->priv = (void *) sock;

// fd
    _file->_file = __slot;
// Validation
    _file->used = TRUE;
    _file->magic = 1234;
// Colocando na lista de arquivos abertos no processo.
    Process->Objects[__slot] = (unsigned long) _file;
// OK, return the fd.
    return (int) __slot;

fail:
    debug_print ("socket_inet: [FAIL]\n");
    //refresh_screen();
    return (int) (-1);
}

// Initialize socket list.
int socket_init(void)
{
    register int i=0;

    for (i=0; i<32; i++){
        socketList[i] = (unsigned long) 0;
    };
    __socket_initialize_gramado_ports();
    // ...
    return 0;
}

