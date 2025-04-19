
// mmsize.c

#include <kernel.h>

//
// Memory size support.
//

// see: mmsize.h
//struct last_mb_footer_d  last_mb_footer;


//base     = base memory retornada pelo cmos
//other    = (1MB - base). (Shadow memory = 384 KB)
//extended = retornada pelo cmos.
//total    = base + other + extended.
// alias
unsigned long memorysizeBaseMemoryViaCMOS=0;

unsigned long memorysizeBaseMemory=0;
unsigned long memorysizeOtherMemory=0;
unsigned long memorysizeExtendedMemory=0;
unsigned long memorysizeTotal=0;

unsigned long memorysizeInstalledPhysicalMemory=0;
unsigned long memorysizeTotalPhysicalMemory=0;
unsigned long memorysizeAvailablePhysicalMemory=0;
// Used
unsigned long memorysizeUsed=0;
// Free
unsigned long memorysizeFree=0;

struct memory_size_info_d MemorySizeInfo;


// Salva o tipo de sistema baseado no tamanho da memória.
// see: x64mm.h
int g_mm_system_type = stNull;


// ==================================================

// Initialize the size of the physical memory
// and the size of the system based on the memory size.
// It needs to be before the pagetables initialization.
int mmsize_initialize(void)
{
// Called by mmInitialize() in mm.c.

    MemorySizeInfo.initialized = FALSE;

//
// MEMORY SIZES
//

// -------------------
// Total memory in KB.
// Herdamos esse valor do boot loader.
// #todo
// New we have a new value from boot.
// We're gonna use this new value instead the one from cmos.
// #warning:
// Actually this address represents the start of the
// last MB of the physical memory.
    unsigned long __total_memory_in_kb = 
        (unsigned long) (blSavedLastValidAddress/0x400);

// -------------------
// CMOS
// Get memory sizes via RTC. (KB)
// base, other, extended.
// RTC só consegue perceber 64MB de memória.

    memorysizeBaseMemoryViaCMOS = (unsigned long) rtcGetBaseMemory();
    memorysizeBaseMemory = 
        (unsigned long) memorysizeBaseMemoryViaCMOS;
    memorysizeOtherMemory = 
        (unsigned long) (1024 - memorysizeBaseMemory);

// -------------------------
// Extended memory from cmos.
// memorysizeExtendedMemory = (unsigned long) rtcGetExtendedMemory(); 
    memorysizeExtendedMemory =  
            (unsigned long) ( 
            __total_memory_in_kb - 
            memorysizeBaseMemory - 
            memorysizeOtherMemory 
            );

// -------------------------
// Size in KB.
    memorysizeTotal = 
        (unsigned long) ( 
        memorysizeBaseMemory + 
        memorysizeOtherMemory + 
        memorysizeExtendedMemory 
        );

// --------------------------------------------
// __total_memory_in_kb: From boot loader.
// memorysizeTotal:      Calculate here.


// -------------------------------------------
// System type - Based on the memory size.
// #important
// Determinar o tipo de sistema de memória.
// small   pelo menos 32mb
// medium  pelo menos 64mb
// large   pelo menos 128mb

// -------------------------
// 0MB
// #atenção 
// Nesse caso devemos prosseguir e testar as outras opções.
    if (memorysizeTotal >= 0){
        g_mm_system_type = stNull;
        debug_print("mmsize_initialize: stNull\n");
    }

// -------------------------
// Small. (32MB).
// #bugbug: 256 is the minimum.
    if (memorysizeTotal >= SMALLSYSTEM_SIZE_KB){
        g_mm_system_type = stSmallSystem;
        debug_print("mmsize_initialize: stSmallSystem\n");
    }

// -------------------------
// Medium. (64MB).
// #bugbug: 256 is the minimum.
    if (memorysizeTotal >= MEDIUMSYSTEM_SIZE_KB){
        g_mm_system_type = stMediumSystem;
        debug_print("mmsize_initialize: stMediumSystem\n");
    }

// -------------------------
// Large. (128MB).
// #bugbug: 256 is the minimum.
    if (memorysizeTotal >= LARGESYSTEM_SIZE_KB){
        g_mm_system_type = stLargeSystem;
        debug_print("mmsize_initialize: stLargeSystem\n");
    }

// -------------------------
// #test
    if (memorysizeTotal == 0)
    {
        g_mm_system_type = stNull;
        debug_print("mmsize_initialize: Initialization fail\n");
        MemorySizeInfo.initialized = FALSE;
        return FALSE;
    }

// -------------------------
    MemorySizeInfo.initialized = TRUE;
    return TRUE;
}

