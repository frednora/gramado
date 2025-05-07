// wminput.c
// Input support.
// Created by Fred Nora.


#include "../gwsint.h"


// #todo
// We can carefully move some routine from wm.c to this file.


int wminputGetAndProcessSystemEvents(void)
{
    return (int) wmInputReader();
}


