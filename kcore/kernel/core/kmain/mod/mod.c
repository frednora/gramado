// mod.c
// see: mod.h
// see: kernel.h
// Created by Fred Nora.

#include <kernel.h>

// Modulo 0.
// see: kernel.h
struct kernel_module_d  *kernel_mod0;

unsigned long kmList[KMODULE_MAX];

// -------------------
// Ring0 Module.
// The name of the kernel of the root partition.
// see: gramp/oskernel/

//const char *mod0_image_name = "MOD0    BIN";
const char *mod0_image_name = "HVMOD0  BIN";


static int __mod_initialize_first_module(void);
static int __load_mod_image(char *image_name);

// ==================================

// mod0: Call the entrypoint of the module.
// mod0.bin entry point.
// When this module was loaded? <<<
// see: I_x64CreateKernelProcess in x64init.c
// see: control/newm0/
// Vamos testar um modulo que ja foi carregado previamente?

void test_mod0(void)
{
// Called by input.c

    unsigned long return_value=0;
    unsigned long fn_table_base=0;
    unsigned long mod_sci=0;

    printk ("test_mod0:\n");

    if ((void*) kernel_mod0 == NULL)
        return;
    if (kernel_mod0->magic != 1234)
        return;
    if (kernel_mod0->initialized != TRUE)
        return;

// Entry point validation
    if ((void*) kernel_mod0->entry_point == NULL)
        return;

// #test
// Calling the virtual function, and
// getting the return value.

// --------------------
// Reason 1000: Initializing the module.
    return_value = 
        (unsigned long) kernel_mod0->entry_point(
            0xFF,
            1000,1234,0,0);
    printk ("RETURN: %d\n",return_value);

// --------------------
// Reason 1001: Testing printk function.
    return_value = 
        (unsigned long) kernel_mod0->entry_point(
            0xFF,
            1001,1234,0,0);    
    printk ("RETURN: %d\n",return_value);

// --------------------
// Reason 1002:
// Exporting the pointer for the function table.
    fn_table_base = (unsigned long) &kernel_mod0->fn_table[0];
    return_value = 
        (unsigned long) kernel_mod0->entry_point(
            0xFF,
            1002, 1234, fn_table_base, fn_table_base );    
    printk ("RETURN: %d\n",return_value);

// --------------------
// Reason 1003:
// Exporting the pointer for the module sci.
// This way the module can call routines inside the kernel.
    mod_sci = (unsigned long) &ring0_module_sci;
    return_value = 
        (unsigned long) kernel_mod0->entry_point(
            0xFF,
            1003, 1234, mod_sci, mod_sci );    
    printk ("RETURN: %d\n",return_value);

// Done:
    printk ("test_mod0: Done\n");
}

// Handler for ring0 module syscall.
// #warning:
// The module can't use some kind of data, just like
// file descriptors.
// They will have a set of rules to follow.
void *ring0_module_sci( 
    unsigned long number, 
    unsigned long arg2, 
    unsigned long arg3, 
    unsigned long arg4 )
{
    printk("ring0_module_sci:\n");

// #todo:
// Populate the switch with some good services for modules.

    unsigned long Value = 1234;

    //return NULL;
    return (void*) Value;
}

// Called by I_x64CreateKernelProcess() in x64init.c
static int __mod_initialize_first_module(void)
{
// Setup the first kernel module.
// It is not a dynlinked module.
// This is just a loadable ring0 code with shared symbols.

    struct kernel_module_d *m;
    int module_id = KMODULE_MOD0;
    unsigned long module_entry_point_va = (unsigned long) XP_MOD0;
    const char *name = "km::mod0";
    size_t s=0;

    m = (struct kernel_module_d *) kmalloc( sizeof(struct kernel_module_d) );
    if ((void*) m == NULL){
        printk ("__mod_initialize_first_module: m\n");
        return FALSE;
    }
    memset( m, 0, sizeof(struct kernel_module_d) );

// Name
    memset(m->name,0,64);
    mysprintf(m->name,name);
    s = strlen(name);
    m->name_size = s;
    m->name[s] = 0;   // finalize

//#debug
    //printk ("String: {%s}\n",m->name);
    //refresh_screen();
    //while(1){}

// See:
// I_x64CreateWSControlThread()

    //m->thread = (struct thread_d *) tid0_thread;

// info
    //m->info.entry_point = (unsigned long) tid0_thread->initial_rip;
    m->info.dialog_address = 0;
    m->info.function_table_address = 0;

// Put it into the list.
// #todo: Change this name
    m->id = module_id;
    kmList[module_id] = (unsigned long) m;

//
// Entry point
//

// #test
// Virtual function
// The entry point for the ring0 kernel module.
// Using the kernel's address espace.
// See: mod0.h, mod0.c.
    m->entry_point = (unsigned long) module_entry_point_va;

// Finalize the module structure.
    m->used = TRUE;
    m->magic = 1234;
    m->initialized = TRUE;

// See: 
// kernel.h, mod.c
    kernel_mod0 = (struct kernel_module_d *) m; 

    return 0;
}



// #todo
// Check the information in the elf header.
// Save some of this information in the process structure. 
// see: exec_elf.h and process.h

static int __load_mod_image(char *image_name)
{

// The virtual address for the module image.
// #warning
// This is a static address. Why not?
// Hack me!
    unsigned long ImageAddress = (unsigned long) 0x30A00000;
// #bugbug
// We have a limit for the image size.
    unsigned long BUGBUG_IMAGE_SIZE_LIMIT = (512 * 4096);

    unsigned long fileret=1;

// Check the validation of the name.
    if ((void*) image_name == NULL){
        panic("__load_mod_image: image_name\n");
    }
    if (*image_name == 0)
        panic("__load_mod_image: image_name\n");

// ---------------------
// It loads a file into the memory.
// IN:
//     fat_address  = FAT address.
//     dir_addresss = Directory address.
//     dir_entries  = Number of entries in the given directory.
//     file_name    = File name.
//     buffer = Where to load the file. The pre-allocated buffer.
//     buffer_size_in_bytes = Maximum buffer size.
// -----------------
// OUT: 
//    1 = fail 
//    0 = ok

    fileret = 
        (unsigned long) fsLoadFile( 
                            VOLUME1_FAT_ADDRESS, 
                            VOLUME1_ROOTDIR_ADDRESS, 
                            FAT16_ROOT_ENTRIES,    //#bugbug: number of entries.
                            image_name, 
                            (unsigned long) ImageAddress,
                            BUGBUG_IMAGE_SIZE_LIMIT ); 

    if (fileret != 0){
        printk("__load_mod_image: on fsLoadFile()\n");
        goto fail;
    }

// OUT: 
//    1 = fail 
//    0 = ok
    return 0;

fail:
    return (int) -1;
}

//
// #
// INITIALIZATION
//

// Initialize support for loadable kernel modules.
// Called by x64init.c
int mod_initialize(void)
{
// Called by I_kmain(). Needs to return TRUE.

    register int i=0;
    int Status = -1;

// Initialize the kernel module list.
    for (i=0; i<KMODULE_MAX; i++){
        kmList[i] = 0;
    };

//
// The first Kernel module
//

// Load the image for the first loadable kernel module.
// #todo
// Check the information in the elf header.
// Save some of this information in the process structure. 
// see: exec_elf.h and process.h
    Status = (int) __load_mod_image(mod0_image_name);
    if (Status<0){
        panic ("__load_mod_image: Couldn't load\n");
    }

// ====================
// Setup the first kernel module.
// It is not a dynlinked module.
// This is just a loadable ring0 thread with shared symbols.

// see: mod.c
    Status = (int) __mod_initialize_first_module();
    if (Status < 0)
        panic ("__load_mod_image: Couldn't initialize first module\n");


    return TRUE;
}

