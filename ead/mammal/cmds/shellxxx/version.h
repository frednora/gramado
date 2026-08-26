// version.h

#ifndef __SHELL00_VERSION_H
#define __SHELL00_VERSION_H    1

/* 
Version control for the shell.  
This file gets changed when you say
`make newversion' to the Makefile.  
It is created by newversion.aux. 
*/

//format: 0.1.0001

/* The distribution version number of this shell. */
#define DISTVERSION "0.1"

/* The last built version of this shell. */
/*revision*/
#define BUILDVERSION "0001"

// =============================================

void show_shell_version(void);

#endif   

