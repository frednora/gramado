
// #test

// The purpose here is to handle the structure
// usersession, room, desktop.
// But we already have these structure inside the kernel
// this is a good place to protect them all.


// WINDOW -> DESKTOP -> ROOM -> USER SECTION.

#ifndef __SECURITY_H
#define __SECURITY_H    1


// user session
struct gwssrv_usession_d
{
    int used;
    int magic;
    
    char name[64];

    // ...

    struct gwssrv_usession_d *next;
};
struct gwssrv_usession_d *CurrentUserSession;


// room
struct gwssrc_room_d 
{
    int used;
    int magic;

    char name[64];

    // belongs to this user session.
    struct gwssrv_usession_d *usession;

    // ...
    
    struct gwssrc_room_d *next;
};
struct gwssrc_room_d *CurrentRoom;


// desktop
struct gwssrc_desktop_d 
{
    int used;
    int magic;

    char name[64];

    // belongs to this room.
    struct gwssrc_room_d *room;
    
    // ...
    
    struct gwssrc_desktop_d *next;
};
struct gwssrc_desktop_d *CurrentDesktop;



#endif    



