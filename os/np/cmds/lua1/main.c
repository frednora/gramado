// main.c
// Lua 1.0 for Gramado OS


// rtl
#include <types.h>
#include <ctype.h>
#include <heap.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

// Lua
#include "lua.h"


// The main function
int main(int argc, char *argv[])
{
    int rv = EXIT_FAILURE;

    // See: lua.c
    rv = (int) lua_main(argc, argv);

    //#debug
    printf("lua1: Exit with %d\n", rv);

    return (int) rv;
}

