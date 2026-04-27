
#include "ds.h"



//
// $$ 
// MAIN
//

// main: 
// Entry point.
// Called by crt0() in 
// userland/libs/rtl/entrance/student/crt0.c.
// This application was launched by init.bin.
int main (int argc, char **argv)
{
    // Initialize the compositor/server loop
    // See: comp/comploop.c
    return (int) comploop_main(argc,argv);
}