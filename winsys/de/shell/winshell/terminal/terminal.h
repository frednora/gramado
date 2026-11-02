// terminal.h
// The main header for the terminal.bin

// rtl
#include <types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <sys/socket.h>
#include <rtl/gramado.h>

//
// Shared
//

#include "shared/globals.h"
#include "shared/alias.h" 
#include "shared/general.h"
#include "shared/ndir.h"


//
// Core
//

#include "core/packet.h"
#include "core/variables.h"
#include "core/compiler.h"
#include "core/flags.h"


#include "term0.h"

//
// UI
//

#include "ui/font00.h"


extern struct gws_display_d *Display;

int terminal_init(unsigned short flags);

