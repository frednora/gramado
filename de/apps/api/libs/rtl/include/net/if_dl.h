
// File: if_dl.h
// #todo: porting from openbsd.


#ifndef _NET_IF_DL_H_
#define _NET_IF_DL_H_


#include <types.h>

/*
 * Structure of a Link-Level sockaddr:
 */
 
struct sockaddr_dl {

    u_char     sdl_len;     /* Total length of sockaddr */
    u_char     sdl_family;  /* AF_LINK */
    u_int16_t  sdl_index;   /* if != 0, system given index for interface */
    u_char     sdl_type;    /* interface type */
    u_char     sdl_nlen;    /* interface name length, no trailing 0 reqd. */
    u_char     sdl_alen;    /* link level address length */
    u_char     sdl_slen;    /* link layer selector length, mostly 0 */

    /* minimum work area, can be larger;
       contains both if name and ll address;
       big enough for IFNAMSIZ plus 8byte ll addr */
    char       sdl_data[24];  
};

#define LLADDR(s) ((caddr_t)((s)->sdl_data + (s)->sdl_nlen))



void link_addr(const char *addr, struct	sockaddr_dl *sdl);
char *link_ntoa(const struct sockaddr_dl	*sdl);


#endif   



//
// End.
//


