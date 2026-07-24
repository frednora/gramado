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
#include "config.h"
#include "debug.h"
#include "globals.h"
#include "gdef.h"
#include "token.h"

// used by lexer and parser
#include "object.h"

#include "common/lexer.h"
#include "common/parser.h"

#include "js/jslex.h"
#include "js/jspar.h"
#include "js/jsvm.h"


#include "tree.h" 
#include "compiler.h"
