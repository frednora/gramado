// gramado.c
// Created by Fred Nora.


#include "../../gwsint.h"

static gramado_initialized = FALSE;
// #todo:
// Version ...
// ...

int GramadoInitialize(void)
{
    printf ("Initializing on Gramado OS\n");

// #todo
// Here we can call the Gramado Kernel to get some system metrics.
// We're already doing this at the graphics initialization.

    gramado_initialized = TRUE;
    return 0;  //OK
}

