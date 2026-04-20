
// mm/mmsize.h
// Memory size support.
// Created by Fred Nora.

#ifndef __MM_MMSIZE_H
#define __MM_MMSIZE_H    1

/*
// #test
// It's expected to find this structure at the end of the memory
// in the start of the last MB.
// This structure was initialized by the boot loader
// and the kernel will just read, not write in the structure.
struct last_mb_footer_d
{
    int used;
    int magic;
    unsigned long bootloader_signature;
};
extern struct last_mb_footer_d  last_mb_footer;
*/

// Size support.
#define KB  (1024)
#define MB  (1024 * 1024)
#define GB  (1024 * 1024 * 1024)

//
// System size
//

// --------------
// System size in MB.
#define __SMALL   ( 256 -1)
#define __MEDIUM  ( 512 -1)
#define __LARGE   (1024 -1)
// System size in bytes.
#define SMALLSYSTEM_SIZE  ( __SMALL  *1024*1024)
#define MEDIUMSYSTEM_SIZE ( __MEDIUM *1024*1024)
#define LARGESYSTEM_SIZE  ( __LARGE  *1024*1024)
// System size in KB.
#define SMALLSYSTEM_SIZE_KB  (__SMALL  * 1024)
#define MEDIUMSYSTEM_SIZE_KB (__MEDIUM * 1024)
#define LARGESYSTEM_SIZE_KB  (__LARGE  * 1024)
// System size in MB.
#define SMALLSYSTEM_SIZE_MB  (__SMALL)
#define MEDIUMSYSTEM_SIZE_MB (__MEDIUM)
#define LARGESYSTEM_SIZE_MB  (__LARGE)
// System size in 4KB pages.
#define SMALLSYSTEM_SIZE_PAGES  ( (__SMALL  *1024*1024) / 4096 )
#define MEDIUMSYSTEM_SIZE_PAGES ( (__MEDIUM *1024*1024) / 4096 )
#define LARGESYSTEM_SIZE_PAGES  ( (__LARGE  *1024*1024) / 4096 )
// --------------

//
// Memory size support.
//

//base     = base memory retornada pelo cmos
//other    = (1MB - base). (Shadow memory = 384 KB)
//extended = retornada pelo cmos.
//total    = base + other + extended.
// alias
extern unsigned long memorysizeBaseMemoryViaCMOS;

extern unsigned long memorysizeBaseMemory;
extern unsigned long memorysizeOtherMemory;
extern unsigned long memorysizeExtendedMemory;
extern unsigned long memorysizeTotal;

extern unsigned long memorysizeInstalledPhysicalMemory;
extern unsigned long memorysizeTotalPhysicalMemory;
extern unsigned long memorysizeAvailablePhysicalMemory;
// Used
extern unsigned long memorysizeUsed;
// Free
extern unsigned long memorysizeFree;


//
// System memory type.
//

// Tipo de sistema baseado no tamanho da memoria.
typedef enum {
    stNull,
    stSmallSystem,
    stMediumSystem,
    stLargeSystem,
}mm_system_type_t;

// Salva o tipo de sistema baseado no tamanho da memória.
// see: mm.c
extern int g_mm_system_type;


//
// Memory size
//

struct memory_size_info_d
{
// If this structure was initialized or not.
// Initialized by mmsize_initialize() in mmsize.c
    int initialized;
    // ...
};
// see: mmsize.c
extern struct memory_size_info_d MemorySizeInfo;

// ==========================

int mmsize_initialize(void);

#endif   

