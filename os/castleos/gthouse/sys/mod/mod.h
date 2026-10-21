// mod.h
// see: mod.c
// Created by Fred Nora.

#ifndef __MOD_MOD_H
#define __MOD_MOD_H    1

#define KMODULE_MOD0  0   // first of the list.
#define KMODULE_MAX   8

//
// mod0 addresses
//

// New address
#define MOD0_BASE_ADDRESS  0x10000000
#define MOD0_ENTRYPOINT    0x10001000


// Handling information between kernel and a given module.
struct km_shared_info_d
{
    unsigned long entry_point;  // The entry point
    unsigned long dialog_address;  // Procedure

// The pointer for a table of symbols exported by this module.
    unsigned long function_table_address;
};

struct kernel_module_d
{
    int used;
    int magic;
    int id;

    int initialized;

    char name[64];
    size_t name_size;

// Shared info
    struct km_shared_info_d info;
    struct thread_d  *thread;

// #test
// Virtual function
// The entry point for the ring0 kernel module.
// Using the kernel's address espace.
// IN: 4 parameters.
// OUT: unsigned long.
   unsigned long (*entry_point)( 
       unsigned char,
       unsigned long, 
       unsigned long, 
       unsigned long, 
       unsigned long );

// Table of function pointers.
    unsigned long fn_table[32];

// Navigation
    struct kernel_module_d  *next;
};

// see: mod.c, x64init.c
extern struct kernel_module_d  *kernel_mod0;

// see: mod.c
// Not exported anymore.
//extern unsigned long kmList[KMODULE_MAX];

//----------------------------------------------

struct mod_fn_d 
{
    int used;
    int magic;
    int initialized;

// This is a module procedure for multiple 
// services that are NOT sensitive to cache locality latency.
    int (*fn_module_procedure)(unsigned long);
    // ...
};
extern struct mod_fn_d  mod0_modfn;


// ===================================

void test_mod0(void);

void *ring0_module_sci( 
    unsigned long number, 
    unsigned long arg2, 
    unsigned long arg3, 
    unsigned long arg4 );

//
// #
// INITIALIZATION
//

int mod_initialize(void);

#endif   


