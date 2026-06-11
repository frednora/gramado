// lua.c
// Main file for lua programming language
// TeCGraf - PUC-Rio
// 28 Apr 93


// rtl
#include <stdlib.h>
#include <stdio.h>

// Lua
#include "opcode.h"
#include "lualib.h"
#include "lua.h"

//=======================================

static void callfunc(void)
{
    lua_Object obj = lua_getparam(1);

    if (lua_isstring(obj))
        lua_call(lua_getstring(obj), 0);
}

static void execstr(void)
{
    lua_Object obj = lua_getparam (1);
    if (lua_isstring(obj))
        lua_dostring(lua_getstring(obj));
}

void test(void)
{
    lua_pushobject( lua_getparam(1) );
    lua_call("c", 1);
}

//
// #
// INITIALIZATION
//

// Called by main() in main.c
int lua_main(int argc, char *argv[])
{
    int i=0;

    if (argc < 2){
        puts ("usage: lua filename [functionnames]\n");
        return EXIT_FAILURE;
    }

    // Register functions
    lua_register ("callfunc", callfunc);
    lua_register ("execstr", execstr);
    lua_register ("test", test);
    // ...

    // ?
    iolib_open();
    strlib_open();
    mathlib_open();

// Handle io files
// See: opcode.c
// IN: input file
    lua_dofile( argv[1] );

    // Call the fuctions provided by the user
    for (i=2; i<argc; i++)
    {
        //if ((void*) argv[i] != NULL)
            lua_call(argv[i], 0);
    };

    return EXIT_SUCCESS;
}

