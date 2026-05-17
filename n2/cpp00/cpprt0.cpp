// cpprt0.cpp
// This is the C++ runtime for Gramado OS.
// It's gonna call the main() function for a c++ program.
// This is a c++ ring 3 program for Gramado OS.

extern "C" void cpprt0_main(unsigned long data)
{
    asm (" int $3 ");
}

