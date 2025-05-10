// main.c
// Main file for the project Teabox, an experimental browser for Gramado OS.
// Created by Fred Nora.

#include "browser.h"


int main( int argc, char *argv[] )
{
// see globals.c for the parameters.

    register int i=0;

// #todo: 
// Parse parameters and save global flags for the options.

// #test
// OK!
    for (i=1; i<argc; i++)
    {
        if ( strncmp("-t", argv[i], 2) == 0 ){
            printf("flag: TEST\n");
            gfTest=TRUE;
        }
        if ( strncmp("-a", argv[i], 2) == 0 ){
            printf("flag: All\n");
            gfAll=TRUE;
        }
        //...
    };

    return (int) uiInitialize();
}

