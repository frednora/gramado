
#ifndef __CLIENT_H
#define __CLIENT_H    1


struct gns_client_profile_d
{
// Packets
    unsigned long received_count;
    unsigned long sent_count;

};

#define DEFAULT_CLIENT_NAME_SIZE  128

struct gns_client_d
{
    int used;
    int magic;

    int id;
    char name[DEFAULT_CLIENT_NAME_SIZE];

    // Socket for this client.
    int fd;
    int is_connected;

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


    struct gns_client_profile_d  profile;

    struct gns_client_d *next;
};

extern struct gns_client_d  *serverClient;
extern struct gns_client_d  *currentClient;
// ...

// Linked list
extern struct gns_client_d  *first_client;

//
// Client list
//

#define SERVER_CLIENT_INDEX  0
#define CLIENT_COUNT_MAX  32
extern unsigned long clientList[CLIENT_COUNT_MAX];



#endif    

