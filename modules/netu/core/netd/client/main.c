// main.c
// 2020 - Created by Fred Nora.

// NET.BIN
// The goal of this applications is testing the 
// connection with the server NETD.BIN
// So, first of all you gotta launch the server.


#include <gnsint.h>

// Ports
#define PORTS_DS  4040  // Display server 
#define PORTS_NS  4041  // Network server
// ...

#define IP(a, b, c, d) \
    (a << 24 | b << 16 | c << 8 | d)

// ----------------------

static int __client_fd = -1;

static void __initialize_globals(void);
static void appShutdown(int socket_fd);
static int appInitialization(void);

// ----------------------

/*
// #test
// gerar número aleatório dentro de uma faixa.
int gerar_numero(int lim_inf, int lim_sup)
{
    return (lim_inf + (rand() % lim_sup));
}
*/

/*
#define IP(a, b, c, d) (a << 24 | b << 16 | c << 8 | d)
struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_port   = 7548, 
    .sin_addr   = IP(192, 168, 1, 79),
};
*/


static void __initialize_globals(void)
{
    register int i=0;

// Clear the buffer.
    for (i=0; i<__BUFFER_SIZE; i++){
        buffer[i] = 0;
    };
}

static void appShutdown(int socket_fd)
{
    debug_print("net.bin: appShutdown\n"); 
    printf     ("net.bin: appShutdown\n");

// #test
// Testando async command
    gns_async_command (socket_fd, 0, 0, 0);
    gns_async_command2(socket_fd, 0, 0, 0, 0, 0, 0);

// Close the given socket.
    close(socket_fd);
}

static int appInitialization(void)
{
// Connecting to the network server in this machine.
// Using the localhost's IP.

//========================
    struct sockaddr_in  server_address;
    server_address.sin_family = AF_INET;  // Remote or local connections.
    server_address.sin_addr.s_addr = IP(127,0,0,1);  //inet_addr("127.0.0.1");
    server_address.sin_port = PORTS_NS;
    int addrlen=0;
    addrlen = sizeof(server_address);
//========================

    debug_print ("net.bin: Initializing ...\n");
    __initialize_globals();

// Socket
// #bugbug
// This connection needs to be RAW.
    __client_fd = 
        (int) socket( 
            AF_INET,   // Remote or local connections 
            SOCK_RAW,  // Type
            0          // Protocol
            );

    if (__client_fd < 0)
    {
       //gws_debug_print ("gnst: Couldn't create socket\n");
       printf ("net.bin: on socket()\n");
       goto fail;
    }

// Connect
// Nessa hora colocamos no accept um fd.
// Então o servidor escreverá em nosso arquivo.

    while (1){
        if (connect (__client_fd, (void *) &server_address, addrlen ) < 0)
        { 
            //gws_debug_print ("gnst: Connection Failed\n");
            printf ("net.bin: Connection Failed \n"); 
            //close(__client_fd);
            //exit(1);
        }else{ break; }; 
    };

    if (__client_fd < 0){
        printf("net.bin: fd fail\n");
        goto fail;
    }

// ------------------
// Send request and wait for response.
// Ok. Its working :)

    //printf("\n");
    printf("NET.BIN: Sending hello!\n");

    int hello_status = (int) gns_hello(__client_fd);
    if (hello_status < 0){
        printf("net.bin: hello_status\n");
        goto fail;
    }

// Loop
// See: 
// libgns/

// Here we're getting data from the kernel.
// Probably the kernel is able to put data into a given 
// buffer that this application has access.

// #todo
// Explain it better!

/*
    int data_status=-1;
    int service_status = -1;
    while (1)
    {
        //if( isTimeToQuit == TRUE )
            //break;

        // ------------------
        // Get data from kernel.
        data_status = (int) gns_get_packet( buffer, __BUFFER_SIZE );
        if (data_status > 0)
        {
           //#debug
           // We got data from kernel.
           printf("net.bin: Data={%s}\n",buffer);
           //exit(0);
        }

        // ------------------
        // Send request and wait for response.
        // Ok. Its working :)
        //service_status = (int) gns_hello(__client_fd);
        //if (service_status <= 0){
        //    printf("net.bin: Service failed\n");
        //}

        // Sleep
        gns_yield();
    };
*/

// Not reached
    appShutdown(__client_fd);

    return (int) EXIT_SUCCESS;
fail:
    return (int) EXIT_FAILURE;
}

int main(int argc, char *argv[])
{
    int Status = -1;

    // #todo
    // Let's parse the parameters.
    // Each parameter can indicate one type of test to make.

    Status = appInitialization();
    return (int) Status;
}

