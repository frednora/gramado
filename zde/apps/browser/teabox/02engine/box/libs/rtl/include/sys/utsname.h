
// File: utsname.h


#ifndef UTSNAME_H
#define UTSNAME_H    1


#define  _UTSNAME_LENGTH  65

/*
#ifndef _UTSNAME_SYSNAME_LENGTH
# define _UTSNAME_SYSNAME_LENGTH _UTSNAME_LENGTH
#endif
#ifndef _UTSNAME_NODENAME_LENGTH
# define _UTSNAME_NODENAME_LENGTH _UTSNAME_LENGTH
#endif
#ifndef _UTSNAME_RELEASE_LENGTH
# define _UTSNAME_RELEASE_LENGTH _UTSNAME_LENGTH
#endif
#ifndef _UTSNAME_VERSION_LENGTH
# define _UTSNAME_VERSION_LENGTH _UTSNAME_LENGTH
#endif
#ifndef _UTSNAME_MACHINE_LENGTH
# define _UTSNAME_MACHINE_LENGTH _UTSNAME_LENGTH
#endif
*/

/*
char  sysname[]  Name of this implementation of the operating system.
char  nodename[] Name of this node within an implementation-defined
                 communications network.
char  release[]  Current release level of this implementation.
char  version[]  Current version level of this release.
char  machine[]  Name of the hardware type on which the system is running.
*/


/* 
 * Structure describing the system and machine.
 */

struct utsname_d
{
    /* Name of the implementation of the operating system.  */
    char sysname[_UTSNAME_LENGTH];
    /* Current version level of this release.  */
    char version[_UTSNAME_LENGTH];  
    /* Current release level of this implementation.  */
    char release[_UTSNAME_LENGTH]; 

    /* Name of the hardware type the system is running on.  */
    char machine[_UTSNAME_LENGTH]; 
    /* Name of this node on the network.  */
    char nodename[_UTSNAME_LENGTH]; 

    /* Name of the domain of this node on the network.  */ 
    /* NIS or YP domain name */
    
    //#ifdef _GNU_SOURCE
    char domainname[_UTSNAME_LENGTH]; 
    //#endif
};
#define utsname  utsname_d




//The length of the arrays in a struct utsname is unspecified (see
//NOTES); the fields are terminated by a null byte ('\0').



//uname() returns system information in the structure pointed to by
//buf.  The utsname struct is defined in <sys/utsname.h>:

// Imp-lemented in unistd.c (Why not?)

int uname(struct utsname *buf);


#endif    





