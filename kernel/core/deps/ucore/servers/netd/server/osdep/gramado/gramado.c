// gramado.c
// Created by Fred Nora.

#include <gnsint.h>

static int __gramado_initialized = FALSE;


int GramadoInitialize(void)
{
    __gramado_initialized = TRUE;
    return TRUE;
}

