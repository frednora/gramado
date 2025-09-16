// kernel.h
// Main header for the kernel module.

#ifndef __KERNEL_H
#define __KERNEL_H  1


#include "config.h"

#include "../x86_64/head.h"

#include "../crt/klimits.h"
#include "../crt/ktypes.h"
#include "../crt/ktypes2.h"
#include "../crt/kstddef.h"
#include "../crt/kstdarg.h"
#include "../crt/kmemory.h"
#include "../crt/kstring.h"
#include "../crt/kstdio.h"

#include "globals.h"

#include "../ke/ports64.h"
#include "../ke/hal.h"
#include "../ke/kal.h"

#include "../dev/display.h"

#endif  

