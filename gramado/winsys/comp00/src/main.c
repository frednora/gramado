
#include "ds.h"



//
// $$ 
// MAIN
//

// main: 
// Entry point.
// Called by crt0().

int main(int argc, char **argv)
{
    int rv = EXIT_FAILURE;

// Initialize the compositor/server loop
// See: comp/comploop.c

    rv = (int) comploop_main(argc, argv);
    if (rv == EXIT_FAILURE){
        server_debug_print("COMP00.BIN: EXIT_FAILURE\n");
        printf            ("COMP00.BIN: EXIT_FAILURE\n");
    }

// IPC:
// Telling the Init thread that the display server is shutting down.
// IN: tid, msgcode, signature, signature.

    rtl_post_to_tid( 0, 44901, 1234, 5678 );

    return EXIT_SUCCESS; 
}