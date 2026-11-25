// cpprt0.cpp
// This is the C++ runtime for Gramado OS.
// It's gonna call the main() function for a c++ program.
// This is a c++ ring 3 program for Gramado OS.

#include "cpprt0.hpp"

// Forward declaration of main (defined in main.cpp)
extern "C" int main(int argc, char *argv[]);

// #define __null  ((void*)0)

char *my_argv[3] = { "", "" };

namespace rtl {

void CppRt0::start(unsigned long data) 
{
    // Call the user program's main
    int ret = main(0,my_argv);

    // Optionally handle return value (exit syscall, etc.)
    // For now, just ignore or loop forever
    (void)ret;
    while (true) 
    { 
        /* halt or yield */ 
    }
}

} // namespace rtl

