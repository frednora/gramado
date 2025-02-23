// client.c
// Created by Fred Nora.

#include "../gwsint.h"

// "... and the house of God will overflow ..."
unsigned long clientList[CLIENT_COUNT_MAX];

struct gws_client_d  *serverClient;
struct gws_client_d  *currentClient;
// ...

// Linked list
struct gws_client_d  *first_client;


void initClientStruct(struct gws_client_d *c)
{
    register int i=0;

    if ((void *) c == NULL){
        //server_debug_print("initClientStruct: [FAIL] c\n");
        return;
    }

// ID
// #todo
    c->id = -1;  //fail

// Clear name
    for (i=0; i<DEFAULT_CLIENT_NAME_SIZE; i++){
        c->name[i] = 0;
    };

    // host
    c->host.id = 0;
    c->host.used = TRUE;
    c->host.magic = 1234;
    // ...
    // display
    c->display.id = 0;
    c->display.used = TRUE;
    c->display.magic = 1234;
    // ...
    // screen
    c->screen.id = 0;
    c->screen.used = TRUE;
    c->screen.magic = 1234;

// Client's PID and GID.
    c->pid = (pid_t) -1;
    c->gid = (gid_t) -1;

// Main window WID.
    c->window = -1;

// Geometry
    c->l = 0;
    c->t = 0;
    c->w = 50;
    c->h = 50;

// Connection
    c->is_connected = FALSE;
    c->fd  = -1;

    c->is_visible = FALSE;
// No tags yet.
    for (i=0; i<4; i++){
       c->tags[i] = FALSE;
    };

    // Profiler
    c->profile.received_count = 0;
    c->profile.sent_count = 0;

// Validation
    c->used = TRUE;
    c->magic = 1234;
}

// Initialize the client list.
// This is an array of connections.
// See: clients.h
void initClientSupport(void)
{
    register int i=0;

    //server_debug_print ("initClientSupport:\n");

// Clear the client list.
    for (i=0; i<CLIENT_COUNT_MAX; i++){
        clientList[i] = 0;
    };

// Major clients
    serverClient = NULL;
    currentClient = NULL;
// The list of clients
    first_client = NULL;

// -----------------------
// The current client
// #todo
// We need the information about the current client
// And we need a list of the connected clientes.

    currentClient = NULL;

// -----------------------
// The server client

    serverClient = 
        (struct gws_client_d *) malloc( sizeof(struct gws_client_d) );
    if ((void *) serverClient == NULL)
    {
        //server_debug_print("initClientSupport: [FATAL] Couldn't create serverClient\n");
        printf            ("initClientSupport: [FATAL] Couldn't create serverClient\n");
        exit(1);
    }
    memset ( serverClient, 0, sizeof(struct gws_client_d) );

// ID = 0.
    serverClient->id = 0;
    serverClient->is_connected = FALSE;
// The fd of the server.
// Nothing for now.
    serverClient->fd = -1;
// Clear name
    for (i=0; i<DEFAULT_CLIENT_NAME_SIZE; i++){
        serverClient->name[i] = 0;
    };

// Server's PID and GID.
    serverClient->pid = (pid_t) getpid();
    serverClient->gid = (gid_t) getgid();
// The server is not visible ... 
    serverClient->is_visible = FALSE;
    //serverClient->window = ?;

    // host
    serverClient->host.id = 0;
    serverClient->host.used = TRUE;
    serverClient->host.magic = 1234;
    // ...
    // display
    serverClient->display.id = 0;
    serverClient->display.used = TRUE;
    serverClient->display.magic = 1234;
    // ...
    // screen
    serverClient->screen.id = 0;
    serverClient->screen.used = TRUE;
    serverClient->screen.magic = 1234;

    // Profiler
    serverClient->profile.received_count = 0;
    serverClient->profile.sent_count = 0;

    // ...

    // ...
    serverClient->used = TRUE;
    serverClient->magic = 1234;

// The first client and the next.
    first_client = (struct gws_client_d *) serverClient;
    first_client->next = NULL;

// Save us in the list of connections.
    clientList[SERVER_CLIENT_INDEX] = (unsigned long) serverClient;
}







