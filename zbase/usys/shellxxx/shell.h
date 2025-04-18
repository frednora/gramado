// shell.h 
// 2016 - Created by Fred Nora.

/*
 * Header principal do Shell.
 * Para atender os comandos do usuário o Shell chamará vários programas, assim 
 * como terá rotinas internas que atendem os comandos. Para isso, esse 
 * diretório deve ter várias pastas, uma para cada um dos comandos principais, 
 * como shutdown. Cda pasta é um programa que o Shell vai chamar.
 * O kernel que gerencia a estrutura de processo deve criar o fluxo padrão ?
 */

// ??
// Setup current compiler 
#include "compiler.h"

#include <types.h>
#include <stddef.h>
#include <stdarg.h>   
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>

#include <unistd.h>

//shell stuff 
#include "globals.h"
#include "builtins.h"

// #deprecated
//#include <api.h>

#include "desktop.h"

//# bash 1.05 stuff #
#include "variables.h"
#include "general.h"
#include "flags.h"
#include "alias.h"
#include "ndir.h"
#include "version.h"
//...

// #test
#include "vk.h"
#include "msgcode.h"
#include "syscall.h"
#include "color.h"
#include <rtl/gramado.h>

#include "shellinc.h"

