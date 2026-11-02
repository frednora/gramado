// mmsize.c
// Memory size detection and classification.
// Created by Fred Nora.

#include <kernel.h>

/*
 * Memory size support
 * -------------------
 * This module is responsible for detecting the amount of physical memory
 * available in the system and classifying the system type (small, medium, large).
 * It centralizes all memory size information into the global structure
 * `MemorySizeInfo`, which is defined in mmsize.h.
 *
 * The values are detected during early kernel initialization,
 * before page tables are set up.
 */

//base     = base memory retornada pelo cmos
//other    = (1MB - base). (Shadow memory = 384 KB)
//extended = retornada pelo cmos.
//total    = base + other + extended.

// Global structure holding memory size information
struct memory_size_info_d  MemorySizeInfo;

// see: mmsize.h
//struct last_mb_footer_d  last_mb_footer;


// Global system type classification (set during initialization)
// see: x64mm.h
int g_mm_system_type = stNull;

// ==================================================

// Return total physical memory (KB)
// total = (base + other + extended)
unsigned long mmsize_get_total_memory(void)
{
    return (unsigned long) MemorySizeInfo.Total;
}

unsigned long mmsize_get_used_memory(void)
{
    return (unsigned long) MemorySizeInfo.Used;
}

unsigned long mmsize_get_free_memory(void)
{
    return (unsigned long) MemorySizeInfo.Free;
}

/* 
 * ==================================================
 * Initialization
 * ==================================================
 * mmsize_initialize()
 *
 * Detects and calculates the amount of physical memory installed,
 * then classifies the system type (small, medium, large) based on thresholds.
 *
 * Steps:
 *   1. Clear the MemorySizeInfo structure.
 *   2. Read total memory from bootloader (last valid address).
 *   3. Read base memory from CMOS.
 *   4. Calculate shadow memory ("other").
 *   5. Calculate extended memory.
 *   6. Compute total memory (base + other + extended).
 *   7. Classify system type based on thresholds.
 *   8. Mark structure as initialized.
 *
 * Called by mmInitialize() in mm.c.
 */

int mmsize_initialize(void)
{
    // Clear the structure
    memset(&MemorySizeInfo, 0, sizeof(struct memory_size_info_d));
    MemorySizeInfo.initialized = FALSE;

    // -------------------
    // Total memory in KB (from bootloader)
    // Note: blSavedLastValidAddress points to the start of the last MB.
    unsigned long __total_memory_in_kb = 
        (unsigned long) (blSavedLastValidAddress/0x400);

    // -------------------
    // Base memory from CMOS (KB)
    // RTC can only detect up to 64 MB.
    MemorySizeInfo.BaseMemoryViaCMOS = (unsigned long) rtcGetBaseMemory();
    MemorySizeInfo.BaseMemory = (unsigned long) MemorySizeInfo.BaseMemoryViaCMOS;
    MemorySizeInfo.OtherMemory = 
        (unsigned long) (1024 - MemorySizeInfo.BaseMemory);

// -------------------------
// Extended memory from cmos.
// memorysizeExtendedMemory = (unsigned long) rtcGetExtendedMemory(); 

    // -------------------
    // Extended memory (KB)
    MemorySizeInfo.ExtendedMemory =  
            (unsigned long) ( 
            __total_memory_in_kb - 
            MemorySizeInfo.BaseMemory - 
            MemorySizeInfo.OtherMemory 
            );

    // -------------------
    // Total memory (KB)
    MemorySizeInfo.Total = 
        (unsigned long) ( 
        MemorySizeInfo.BaseMemory + 
        MemorySizeInfo.OtherMemory + 
        MemorySizeInfo.ExtendedMemory 
        );

// --------------------------------------------
// __total_memory_in_kb: From boot loader.
// MemorySizeInfo.Total:      Calculated here.


    // -------------------
    // System type classification
    // Thresholds defined in mmsize.h

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
    if (MemorySizeInfo.Total >= 0){
        g_mm_system_type = stNull;
        debug_print("mmsize_initialize: stNull\n");
    }

// -------------------------
// Small. (32MB).
// #bugbug: 256 is the minimum.
    if (MemorySizeInfo.Total >= SMALLSYSTEM_SIZE_KB){
        g_mm_system_type = stSmallSystem;
        debug_print("mmsize_initialize: stSmallSystem\n");
    }

// -------------------------
// Medium. (64MB).
// #bugbug: 256 is the minimum.
    if (MemorySizeInfo.Total >= MEDIUMSYSTEM_SIZE_KB){
        g_mm_system_type = stMediumSystem;
        debug_print("mmsize_initialize: stMediumSystem\n");
    }

// -------------------------
// Large. (128MB).
// #bugbug: 256 is the minimum.
    if (MemorySizeInfo.Total >= LARGESYSTEM_SIZE_KB){
        g_mm_system_type = stLargeSystem;
        debug_print("mmsize_initialize: stLargeSystem\n");
    }

    // -------------------
    // Fail-safe: no memory detected
    if (MemorySizeInfo.Total == 0)
    {
        g_mm_system_type = stNull;
        debug_print("mmsize_initialize: Initialization fail\n");
        MemorySizeInfo.initialized = FALSE;
        return FALSE;
    }

    // -------------------
    // Success
    MemorySizeInfo.initialized = TRUE;
    return TRUE;
}

