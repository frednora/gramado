
// UNAME.BIN
// 2022 - Fred Nora.

#include <types.h>
//#include <sys/types.h>
#include <stdlib.h>
//#include <stddef.h>
#include <stdio.h>
#include <sys/utsname.h>

/*
char  sysname[]  Name of this implementation of the operating system. 
char  release[]  Current release level of this implementation. 
char  version[]  Current version level of this release. 
char  nodename[] Name of this node within the communications 
                 network to which this node is attached, if any. 
char  machine[]  Name of the hardware type on which the system is running. 
#todo: domain name.
*/
// See:
// https://man7.org/linux/man-pages/man2/uname.2.html

// ==========================

static void usage(void);

/*
static int is_64_kernel(void);
static int is_64_kernel(void)
{
    struct utsname un;
    int res=FALSE;

    if (uname (&un) < 0){
        return -1;
    }
    if (strcmp (un.machine, "x86_64") == 0){
        res=TRUE;
    }
    return (int) res;
}
*/

/*
Flags
-a	Displays all information specified with the -m, -n, -r, -s, and -v flags. Cannot be used with the -x or -SName flag. If the -x flag is specified with the -a flag, the -x flag overrides it.
-l	Displays the LAN network number.
-m	Displays the machine ID number of the hardware running the system.
-M	Displays the system model name. If the model name attribute does not exist, a null string is displayed.
-n	Displays the name of the node. This may be a name the system is known by to a UUCP communications network.
-r	Displays the release number of the operating system.
-s	Displays the system name. This flag is on by default.
-S Name	Sets the name of the node. This can be the UUCP communications network name for the system.
-T Name	Sets the system name. This can be the UUCP communications network name for the system.
-u	Displays the system ID number. If this attribute is not defined, the output is the same as the output displayed by uname -m.
-v	Displays the operating system version.
-x	Displays the information specified with the -a flag as well as the LAN network number, as specified by the -l flag.
*/

static void usage(void)
{
    printf("usage: #todo\n");
}

// Main
int main(int argc, char **argv)
{
// The kernel will put the strings here.
    struct utsname  un;
    register int i=0;
// Flags
    int fAll=FALSE;
    int fSysName=FALSE;
    int fVersion=FALSE;
    int fRelease=FALSE;
    int fMachine=FALSE;
    int fNodeName=FALSE;

// Get parametes.
    // #debug
    // printf("argc: {%d}\n",argc);

// Nothing to parse
    if (argc == 1)
    {
        fSysName = TRUE;
        goto display_info;
    }

// Get flags
    for (i=0; i<argc; i++)
    {
        //printf("%d: %s\n",i, argv[i]);
        // Se em algum momento a flag for '-?'.
        if ( strncmp( argv[i], "-a", 2 ) == 0 ){ fAll = TRUE; }
        if ( strncmp( argv[i], "-s", 2 ) == 0 ){ fSysName = TRUE; }
        if ( strncmp( argv[i], "-r", 2 ) == 0 ){ fRelease = TRUE; }
        if ( strncmp( argv[i], "-v", 2 ) == 0 ){ fVersion = TRUE; }
        //if ( strncmp( argv[i], "-?", 2 ) == 0 ){ fMachine = TRUE; } //
        if ( strncmp( argv[i], "-n", 2 ) == 0 ){ fNodeName = TRUE; }
        //...
    };

display_info:

// Get kernel info
    uname(&un);

    if (fAll == TRUE)
    {
        fSysName=TRUE;
        fVersion=TRUE;
        fRelease=TRUE;
        fMachine=TRUE;
        fNodeName=TRUE;
    }

    if (fSysName==TRUE) { printf ("sysname:  %s\n", un.sysname  ); }
    if (fVersion==TRUE) { printf ("version:  %s\n", un.version  ); }
    if (fRelease==TRUE) { printf ("release:  %s\n", un.release  ); }
    if (fMachine==TRUE) { printf ("machine:  %s\n", un.machine  ); }
    // ...

// The node name
    if (fNodeName == TRUE){
        printf ("nodename: {%s}\n", un.nodename ); 
    }

//#todo: Domain name
    printf ("domainname: {%s}\n", un.domainname ); 

    return EXIT_SUCCESS;

fail:
    usage();
    return EXIT_FAILURE;
}

