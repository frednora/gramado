// irql.c
// It manages the value into the register cr8 used for hw irql support.
// #bugbug: 
// We gotta know if the current processor has support for this register.
// Created by Fred Nora

#include <kernel.h>

void irql_load_cr8(unsigned long value)
{
    x64_load_cr8(value);
}

