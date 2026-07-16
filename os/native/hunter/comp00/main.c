
#include "ds.h"



//
// $$ 
// MAIN
//

// main: 
// Entry point.
// Called by crt0().

int main (int argc, char **argv)
{
    int rv = EXIT_FAILURE;

// Initialize the compositor/server loop
// See: comp/comploop.c

    rv = (int) comploop_main(argc, argv);
    if (rv == EXIT_FAILURE){
        server_debug_print("COMP00.BIN: EXIT_FAILURE\n");
        printf            ("COMP00.BIN: EXIT_FAILURE\n");
    }

    return EXIT_SUCCESS; 
}