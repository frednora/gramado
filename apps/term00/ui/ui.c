// ui.c
// Frontend part for the terminal application.

#include <types.h>
//#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


// Shared
#include "../shared/globals.h"

// Backend
#include "../core/core.h"
#include "../core/shell.h"

// Frontend
#include "ui.h"

#include "../terminal.h"

// Client-side library.
#include <gws.h>


// Cursor
int cursor_x=0;
int cursor_y=0;


// see: font00.h
struct font_info_d  FontInfo;





