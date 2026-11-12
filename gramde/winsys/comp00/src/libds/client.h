// client.h
// Client structure to handle the connections.
// Created by Fred Nora.

#ifndef ____CLIENT_H
#define ____CLIENT_H    1

// The client behavior
struct gws_client_profile_d
{
// Packets
    unsigned long received_count;
    unsigned long sent_count;
    // ...
};

// Nesse arquivo fica a estrutura de cliente.
// Essa estrutura vai conter as informações
// e características dos clientes.
// nao eh dependente do os, pode sair de os/.

#define DEFAULT_CLIENT_NAME_SIZE  128

// #define MAX_TAGS  4

// Client info.
// We get this info with the application.
struct gws_client_d
{
    int used;
    int magic;

    int id;
    char name[DEFAULT_CLIENT_NAME_SIZE];

// WID
// The overlapped window for this client.
    int window;

// A area ocupada pela janela
// pode ser do tamanho da janela ou ainda maior.
    int l;
    int t;
    int w;
    int h;

// Quando uma tag foi acionada
// e esse cliente foi exibido na tela.
    int is_visible;

// As 4 tags possivies
// TRUE = estamos nessa tad
// FALSE = nao estamos nessa tag.
// #todo: MAX_TAGS
    int tags[4];

// #todo
    unsigned long flags;

//
// Connection.
//

    // Socket for this client.
    int fd;
    int is_connected;

// host, display, screen
// Not pointers.
    struct gws_host_d     host;
    struct gws_display_d  display;
    struct gws_screen_d   screen;
 
// Client's PID, GID and TID.
// In the case of local connections.
    pid_t pid;
    gid_t gid;
    int tid;

// Spin
// Lets count how many times this client
// is looking for the next event.
// It can be useful for changing the client's priority.

    unsigned long spin;

    // ...

// The client behavior
    struct gws_client_profile_d  profile;

// Um loop vai circular os clientes.
// se ele fizer parte de uma tag, entao ele sera exibido.
    struct gws_client_d *next;
};

extern struct gws_client_d  *serverClient;
extern struct gws_client_d  *currentClient;
// ...

// Linked list
extern struct gws_client_d  *first_client;

//
// Client list
//

#define SERVER_CLIENT_INDEX  0
#define CLIENT_COUNT_MAX  32
extern unsigned long clientList[CLIENT_COUNT_MAX];

//
// == prototypes =============================
//

void initClientSupport(void);
void initClientStruct(struct gws_client_d *c);

#endif    



