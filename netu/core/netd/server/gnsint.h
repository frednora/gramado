// gnsint.h
// Internal definitions for NETD.BIN
// Created by Fred Nora.

// rtl
#include <types.h>
#include <sys/types.h>
#include <sys/cdefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
// rtl
#include <net/if_arp.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
// for sync
#include <rtl/gramado.h>

#include "config/config.h"
#include "osdep/gramado/gramado.h"

#include "gns.h"
#include "globals.h"
#include "vk.h"
#include "wm.h"
#include "protocol.h"
#include "client.h"
#include "osdep/gramado/connect.h"
#include "osdep/gramado/packet.h"
#include "decode.h"

#include "actions.h"
