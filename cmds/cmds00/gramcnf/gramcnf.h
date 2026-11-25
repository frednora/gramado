// gramcnf.h
// This is the main header included into 
// all source files of the gramcnf project.
// 2018 - Created by Fred Nora.

// rtl
#include <types.h>
#include <ctype.h>
#include <heap.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

// Local
#include "include/config.h"
#include "include/debug.h"
#include "include/globals.h"
#include "include/gdef.h"
#include "include/token.h"

// used by lexer and parser
#include "include/object.h"

//
// Core
//

#include "include/core/lexer.h"
#include "include/core/parser.h"
#include "include/core/vm.h"

#include "include/tree.h" 
#include "include/run.h"

