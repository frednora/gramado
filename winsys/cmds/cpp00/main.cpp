// This is a c++ ring 3 program for Gramado OS.

#include "cpprt0.hpp"
#include "rtl.hpp"

int main(int argc, char *argv[])
{
    asm (" int $3 ");
    return 0;
}
