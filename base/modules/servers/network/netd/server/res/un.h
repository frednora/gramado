/*
 * File: in.h 
 *     ring0. socket address for unix style.
 */

// un.h
// socket support for the base kernel.
// ring0
// 2020 - Created by Fred Nora.


#ifndef ____UN_H
#define ____UN_H


//#test
// #bugbug
// It needs to be 16bit.
// normally define in ansi.h
typedef __uint8_t  __sa_family_t;  /* socket address family */


//#test
// #bugbug: 
// It needs to be 16bit.
#ifndef sa_family_t
typedef __sa_family_t  sa_family_t;
#define sa_family_t  __sa_family_t
#endif


#define UNIX_PATH_MAX 108
struct sockaddr_un {

    //#bugbug
    //ps: It needs to be 16 bit.
    uint16_t sun_family; 
    char sun_path[UNIX_PATH_MAX];
};



#endif   

