/*
 * File: host.h
 * 
 *     This header should be included by including "gws.h".
 */


#ifndef __LIBGWS_HOST_H
#define __LIBGWS_HOST_H  1


/*
struct gws_host_address_d
{

// for example FamilyInternet
    int family;

// length of address, in bytes
    int length;

// pointer to where to find the bytes
    char *address;
};
*/

struct gws_host_d
{
    int id;
    int used;
    int magic;
    char name[256];
    // struct gws_host_address_d  *host_address;
    // ...
};


#endif    



