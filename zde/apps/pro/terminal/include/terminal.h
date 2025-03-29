
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

#include "font00.h"

// terminal project includes.
#include "compiler.h"
#include "globals.h"
#include "variables.h"
#include "general.h"
#include "flags.h"
#include "alias.h" 
#include "ndir.h"
#include "packet.h"
#include "term0.h"

int terminal_init(unsigned short flags);

