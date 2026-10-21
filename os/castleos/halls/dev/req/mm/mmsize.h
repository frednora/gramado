// mmsize.h
// Memory size support for the kernel.
// Created by Fred Nora.

#ifndef __MM_MMSIZE_H
#define __MM_MMSIZE_H    1


/*
 * Bootloader footer structure
 * ---------------------------
 * Expected to be located at the start of the last MB of memory.
 * Initialized by the boot loader; the kernel only reads this structure.
 * Used to validate bootloader signature and detect memory layout.
 */
/*
struct last_mb_footer_d
{
    int used;
    int magic;
    unsigned long bootloader_signature;
};
extern struct last_mb_footer_d  last_mb_footer;
*/


/*
 * Unit definitions
 * ----------------
 * Convenience macros for expressing memory sizes in KB, MB, GB.
 * #ps: slow, because of the calculation
 */
#define KB  (1024)
#define MB  (1024 * 1024)
#define GB  (1024 * 1024 * 1024)


/*
 * System size thresholds
 * ----------------------
 * Defines classification boundaries for system types.
 * These values are used to categorize the system as:
 *   - Small   (>= 256 MB)
 *   - Medium  (>= 512 MB)
 *   - Large   (>= 1024 MB)
 *
 * Thresholds are expressed in multiple units:
 *   - Bytes
 *   - KB
 *   - MB
 *   - 4KB pages
 */

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

/*
 * System memory type classification
 * ---------------------------------
 * Enum used to categorize the system based on detected memory size.
 */
typedef enum {
    stNull,
    stSmallSystem,
    stMediumSystem,
    stLargeSystem,
}mm_system_type_t;

// Global system type classification (set during initialization)
// see: mm.c
extern int g_mm_system_type;


/*
 * Memory size information structure
 * ---------------------------------
 * Central structure holding all memory size data detected by the kernel.
 * Populated by mmsize_initialize() in mmsize.c.
 *
 * Fields:
 *   - BaseMemoryViaCMOS: Base memory reported by CMOS (KB)
 *   - BaseMemory:        Base memory (KB)
 *   - OtherMemory:       Shadow memory (≈384 KB)
 *   - ExtendedMemory:    Extended memory above 1 MB (KB)
 *   - Total:             Total physical memory (KB)
 *   - Used:              Memory currently used (KB)
 *   - Free:              Memory currently free (KB)
 *   - InstalledPhysicalMemory: Placeholder for installed RAM (KB)
 *   - TotalPhysicalMemory:     Placeholder for total RAM (KB)
 *   - AvailablePhysicalMemory: Placeholder for free RAM (KB)
 *   - initialized:       Flag indicating if structure is valid
 */
// In KB?
struct memory_size_info_d
{
    int initialized;

    unsigned long BaseMemoryViaCMOS;
    //unsigned long ExtendedMemoryViaCMOS;  //rtc

    unsigned long BaseMemory;
    unsigned long OtherMemory;
    unsigned long ExtendedMemory;
    unsigned long Total;

    unsigned long Used;
    unsigned long Free;

    unsigned long InstalledPhysicalMemory;
    unsigned long TotalPhysicalMemory;
    unsigned long AvailablePhysicalMemory;
};

// Global instance of memory size info
// see: mmsize.c
extern struct memory_size_info_d  MemorySizeInfo;

// ==========================

/*
 * Accessor functions
 * ------------------
 * Return memory statistics in KB.
 */

unsigned long mmsize_get_total_memory(void);
unsigned long mmsize_get_used_memory(void);
unsigned long mmsize_get_free_memory(void);

/*
 * Initialization
 * --------------
 * Detects and calculates the amount of physical memory installed,
 * then classifies the system type (small, medium, large).
 * Must be called before page table initialization.
 */

int mmsize_initialize(void);

#endif   

