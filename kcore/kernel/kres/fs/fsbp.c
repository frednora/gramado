// fsbp.c
// File System Boot Partition.
// Created by Fred Nora.

#include <kernel.h>

// Canonical:
// Folders in boot partition.
struct system_directory_d  sdROOT;          // '/'
// We do not touch the EFI folder for now.  // '/EFI/
struct system_directory_d  sdGRAMADO;       // '/GRAMADO/'
struct system_directory_d  sdDE;            // '/DE/''

// FAT for boot partition
struct system_fat_d  bpFAT;

#define __DIRNAME_SIZE_MAX    16
const char *root_name = "/";

static void __do_initialize_sdROOT(void);
static void __do_initialize_sdGRAMADO(void);
static void __do_initialize_sdDE(void);


// =============================================================

// ------------------
// ROOT
// Load the root dir into the memory and 
// initialize the sdROOT structure.
static void __do_initialize_sdROOT(void)
{
// Called by initialize_FAT_and_main_directories().

    size_t name_size=0;

    sdROOT.initialized = FALSE;

    if (bpFAT.initialized != TRUE){
        x_panic("__do_initialize_sdROOT: bpFAT");
    }
// Load the root dir into the canonical virtual address.
    fs_load_rootdir ( 
        VOLUME1_ROOTDIR_ADDRESS, 
        VOLUME1_ROOTDIR_LBA, 
        32 );

// Register the va and the number of entries.
    sdROOT.address = (unsigned long) VOLUME1_ROOTDIR_ADDRESS;
    sdROOT.number_of_entries = FAT16_ROOT_ENTRIES;

// Name support.
// #todo: We need a definition for this size.

    name_size = strlen(root_name);
    if (name_size <= 0 || name_size >= __DIRNAME_SIZE_MAX)
    {
        x_panic("name_size");
    }
    sdROOT.name_size = (size_t) name_size; 
    memset(sdROOT.name, 0, __DIRNAME_SIZE_MAX);
    ksprintf( sdROOT.name, root_name );

    sdROOT.initialized = TRUE;
}

// GRAMADO
// Load the directory GRAMADO/ that belongs 
// to the root dir into the memory and 
// initialize the sdGRAMADO structure.
static void __do_initialize_sdGRAMADO(void)
{
// Called by initialize_FAT_and_main_directories().
    size_t name_size=0;
    const char *DirectoryName = "GRAMADO";
    unsigned long DirectoryBufferAddress=0;
    unsigned long DirectoryBufferSizeInBytes=0;

    sdGRAMADO.initialized = FALSE;

    if (sdROOT.initialized != TRUE)
        x_panic("__do_initialize_sdGRAMADO: No sdROOT");

    // #bugbug
    // se carregarmos mais arquivos lá do que isso,
    // então teremos problemas para ler.
    // # isso é so um teste por enquanto.
    // Podemos melhorar essa rotina no futuro.
    // 32 setores, igual o root.
    // 512 * 32 = 16384 bytes
    // 16384/4096 = 4 páginas.
    //#todo: A lot bigger when possible.
    DirectoryBufferAddress = (unsigned long) allocPages(8);
    if ((void*) DirectoryBufferAddress == NULL){
        x_panic("__do_initialize_sdGRAMADO: buf");
    }
    // Size in bytes = (32 setores)
    //(32*512) = 16384.
    DirectoryBufferSizeInBytes = 16384;  // 16KB, 4 pages
    //DirectoryBufferSizeInBytes = ?;      // 32KB, 8 pages

    // #bugbug
    // We alocated 8 pages,
    // but we are using 4 pages.

    unsigned long Ret = 1;  // 1 fail | 0 ok.
    Ret = fsLoadFile ( 
              (unsigned long) VOLUME1_FAT_ADDRESS,      // fat address
              (unsigned long) VOLUME1_ROOTDIR_ADDRESS,  // dir address. onde procurar.
              (int) FAT16_DEFAULT_NUMBER_OF_ENTRIES,    // Number of entries.  
              (const char *) DirectoryName,
              (unsigned long) DirectoryBufferAddress,  // onde carregar. 
              (unsigned long) DirectoryBufferSizeInBytes );                            

    if (Ret!=0){
        x_panic("__do_initialize_sdGRAMADO: Load");
    }

    sdGRAMADO.address = (unsigned long) DirectoryBufferAddress;
    sdGRAMADO.number_of_entries = FAT16_DEFAULT_NUMBER_OF_ENTRIES;
// name
    name_size = strlen(DirectoryName);
    if (name_size <= 0 || name_size >= __DIRNAME_SIZE_MAX)
    {
        x_panic("name_size");
    }
    sdGRAMADO.name_size = (size_t) name_size; 
    memset(sdGRAMADO.name, 0, __DIRNAME_SIZE_MAX);
    ksprintf( sdGRAMADO.name, DirectoryName );

// Set the flags as initialized.
    sdGRAMADO.initialized = TRUE;
}

// ------------------
// DE/
// Load the directory DE/ that belongs 
// to the root dir into the memory and 
// initialize the sdDE structure.
static void __do_initialize_sdDE(void)
{
// Called by initialize_FAT_and_main_directories().
    size_t name_size=0;
    const char *DirectoryName = "DE";
    unsigned long DirectoryBufferAddress=0;
    unsigned long DirectoryBufferSizeInBytes=0;

    sdDE.initialized = FALSE;

    if (sdROOT.initialized != TRUE)
        x_panic("__do_initialize_sdDE: No sdROOT");


    // #bugbug
    // se carregarmos mais arquivos lá do que isso,
    // então teremos problemas para ler.
    // # isso é so um teste por enquanto.
    // Podemos melhorar essa rotina no futuro.
    // 32 setores, igual o root.
    // 512 * 32 = 16384 bytes
    // 16384/4096 = 4 páginas.
    //#todo: A lot bigger when possible.
    DirectoryBufferAddress = (unsigned long) allocPages(8);
    if ((void*) DirectoryBufferAddress == NULL){
        x_panic("__do_initialize_sdDE: buf");
    }
    // Size in bytes = (32 setores)
    //(32*512) = 16384.
    DirectoryBufferSizeInBytes = 16384;

    unsigned long Ret = 1;  // 1 fail | 0 ok.
    Ret = fsLoadFile ( 
              (unsigned long) VOLUME1_FAT_ADDRESS,      // fat address
              (unsigned long) VOLUME1_ROOTDIR_ADDRESS,  // dir address. onde procurar.
              (int) FAT16_DEFAULT_NUMBER_OF_ENTRIES,    // Number of entries.  
              (const char *) DirectoryName,
              (unsigned long) DirectoryBufferAddress,  // onde carregar. 
              (unsigned long) DirectoryBufferSizeInBytes );                            

    if (Ret!=0){
        x_panic("__do_initialize_sdDE: Load");
    }

    sdDE.address = (unsigned long) DirectoryBufferAddress;
    sdDE.number_of_entries = FAT16_DEFAULT_NUMBER_OF_ENTRIES;
// name
    name_size = strlen(DirectoryName);
    if (name_size <= 0 || name_size >= __DIRNAME_SIZE_MAX)
    {
        x_panic("name_size");
    }
    sdDE.name_size = (size_t) name_size;
    memset(sdDE.name, 0, __DIRNAME_SIZE_MAX);
    ksprintf( sdDE.name, DirectoryName );

// Set the flags as initialized.
    sdDE.initialized = TRUE;
}


// >> So podemos chamar isso depois de termos
// inicialiado o hardware de disco ...
// see: I_init in x64init.c
// Load FAT for the first time
// Load root dir for the first time.
// Load the other main directories for the first time.
void fsbp_initialize_fat(void)
{
// Called by x64init.c

// ------------------
// FAT
// Load the FAT into the memory and setup the flag.
    fs_load_fat( VOLUME1_FAT_ADDRESS, VOLUME1_FAT_LBA, 246 );
    bpFAT.initialized = TRUE;
}


// Initialize boot partitions canonical directories.
int fsbp_initialize_bp_directories(void)
{
// Called by x64init.c

// ------------------
// ROOT
// Load the root dir into the memory and 
// initialize the sdROOT structure.
    __do_initialize_sdROOT();

// ------------------
// GRAMADO
// Load the directory GRAMADO/ that belongs 
// to the root dir into the memory and 
// initialize the sdGRAMADO structure.
    __do_initialize_sdGRAMADO();

// ------------------
// DE/
// Load the directory DE/ that belongs 
// to the root dir into the memory and 
// initialize the sdDE structure.
    __do_initialize_sdDE();

    return 0;
}

