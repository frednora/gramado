// desktop.c

// gerenciamento do desktop,
// manipulação dos ícones no desktop.

#include "shell.h"

//desktop folders.

#ifndef PDESKTOPFOLDER
#define PDESKTOPFOLDER "root:/volume0"
#endif
char *primary_desktop_folder = PDESKTOPFOLDER;

#ifndef SDESKTOPFOLDER
#define SDESKTOPFOLDER "root:/volume1/users/fred/desktop"
#endif
char *secondary_desktop_folder = SDESKTOPFOLDER;


int desktopInitialize (void)
{	
	printf("Initializing desktop ...\n");
	printf("%s\n",primary_desktop_folder);
    printf("%s\n",secondary_desktop_folder);		
	return 0;
}

