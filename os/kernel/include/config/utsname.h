// File: utsname.h

#ifndef __GRAMADO_UTSNAME_H
#define __GRAMADO_UTSNAME_H    1


#ifndef UTS_SYSNAME
#define UTS_SYSNAME  PRODUCT_NAME_SHORT
#endif

#ifndef UTS_NODENAME
#define UTS_NODENAME  PRODUCT_NAME_SHORT
#endif

#ifndef UTS_DOMAINNAME
// Domain Controller?
#define UTS_DOMAINNAME  "DOMAIN-NAME"
#endif


#define _UTSNAME_LENGTH  65

/* Structure describing the system and machine.  */

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

// see: kmain.c
extern struct utsname  kernel_utsname;

//The length of the arrays in a struct utsname is unspecified (see
//NOTES); the fields are terminated by a null byte ('\0').

//uname() returns system information in the structure pointed to by
//buf.  The utsname struct is defined in <sys/utsname.h>:

#endif    

