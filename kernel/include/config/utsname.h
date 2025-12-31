// utsname.h
// 'UNIX Time-Sharing' standard variables.
// Created by Fred Nora.

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

struct utsname_d { 
    char sysname[_UTSNAME_LENGTH]; 
    char nodename[_UTSNAME_LENGTH]; 
    char release[_UTSNAME_LENGTH]; 
    char version[_UTSNAME_LENGTH]; 
    char machine[_UTSNAME_LENGTH]; 
    char domainname[_UTSNAME_LENGTH]; 
};
#define utsname  utsname_d

// see: kmain.c
extern struct utsname  kernel_utsname;

//The length of the arrays in a struct utsname is unspecified (see
//NOTES); the fields are terminated by a null byte ('\0').

//uname() returns system information in the structure pointed to by
//buf.  The utsname struct is defined in <sys/utsname.h>:

#endif    

