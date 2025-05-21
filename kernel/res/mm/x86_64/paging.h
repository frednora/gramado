// paging.h
// Paging for x86_64 machines.
// Created by Fred Nora.

// Heder for supporting the creation of the 
// main system's page tables by paging.c


#ifndef __X86_64_PAGING_H
#define __X86_64_PAGING_H    1


// Bits for intel x86_64 paging.


// Purpose: This flag (bit 0) indicates whether the page is present (i.e., loaded) in physical memory.
// Usage: If not set, any access to that page triggers a page fault.
#define PAGE_PRESENT  0x001

// Purpose: This flag (bit 1) makes the page writable.
// Usage: When combined with PAGE_PRESENT, it ensures the page can be both accessed and modified.
#define PAGE_WRITE    0x002

// Purpose: This flag (bit 2) means that the page is accessible from user mode in addition to kernel mode.
// Usage: When not set, only kernel-mode code can access the page, providing a layer of memory protection.
#define PAGE_USER     0x004

// Purpose: This flag (bit 3) sets a write-through caching policy for the page.
// Usage: With write-through caching, write operations are immediately propagated 
// to the lower levels of the memory hierarchy, ensuring consistency at the expense of performance.
#define PAGE_WRITE_THROUGH  0x008

// Purpose: This flag (bit 4) disables caching for the page.
// Usage: Useful in scenarios where caching might lead to stale data or 
// when dealing with special memory-mapped I/O regions.
#define PAGE_NOCACHE   0x010

// Purpose: This flag (bit 5) is set by the CPU when the page is read or written to.
// Usage: Operating systems use this flag to implement features like page replacement, 
// knowing which pages have been recently used.
#define PAGE_ACCESSED  0x020

// Purpose: This flag (bit 6) is set by the CPU when a write operation modifies the page.
// Usage: The OS uses this to decide if a page needs to be written back to disk or 
// if it has been altered since being loaded.
#define PAGE_DIRTY     0x040

// In 4K page entries, bit 7 is used as the PAT flag. 
// In some cases for large pages (like 2 MB pages), this same bit might be 
// repurposed as the Page Size (PS) flag.
#define PAGE_PAT  0x080

// Purpose: This flag (bit 8) designates a page as “global”.
// Usage: When set, the page is not flushed from the Translation Lookaside Buffer (TLB) 
// on a context switch, which can improve performance for 
// frequently accessed pages shared across processes.
#define PAGE_GLOBAL  0x100

// Purpose: This value (0xE00, corresponding to bits 9–11 set) represents bits that are 
// available for use by the operating system.
// Usage: These bits are “free” in that the hardware doesn’t use them, 
// so OS developers can store additional metadata or use them in custom ways.
#define PAGE_AVAIL   0xE00

// Meaning: This combination ensures the page is present and writable.
#define PAGE_PRESENT_WRITE       ( PAGE_WRITE | PAGE_PRESENT )

// Meaning: Here, the page is marked as present, writable, and accessible from user space.
#define PAGE_PRESENT_WRITE_USER  ( PAGE_USER | PAGE_WRITE | PAGE_PRESENT )

// Meaning: This likely represents the default protection for 
// kernel pages—present and writable but 
// intentionally not marked as accessible to user mode (since it lacks PAGE_USER).
#define PAGE_KERNEL_PGPROT       (PAGE_PRESENT_WRITE)


// Physical Address Bits (Bits 12–51): 
// Although not a flag, these bits in the page table entry hold the physical base address of the mapped page.

// Reserved or Implementation-Specific Bits (Bits 52–62): 
// These bits are often reserved or defined for specific CPUs/extensions.

// No-Execute (NX) Bit – Bit 63: On modern x86_64 systems, an additional important flag 
// is the NX (No-Execute) flag. This flag, if set, marks the page as non-executable. 
// Since x86_64 page table entries are 64 bits wide, the NX flag is usually defined as the highest bit.
// #define PAGE_NX    (1ULL << 63)

// ================================================

void pages_print_info(int system_type);
void pages_print_video_info(void);

void *CreateAndIntallPageTable (
    unsigned long pml4_va,   // page map level 4
    unsigned long pml4_index,
    unsigned long pdpt_va,   // page directory pointer table
    unsigned long pdpt_index,
    unsigned long pd_va,     // page directory 
    int pd_index,            // Install the pagetable into this entry of the page directory. 
    unsigned long region_pa );

unsigned long get_new_frame(void);
unsigned long alloc_frame(void);

// #danger
unsigned long get_table_pointer_va(void);

void *CloneKernelPDPT0(void);
void *CloneKernelPD0(void);
void *CloneKernelPML4 (void);
void *clone_pml4 ( unsigned long pml4_va );

int isValidPageStruct(struct page_d *p);

void freePage (struct page_d *p);
void notfreePage (struct page_d *p);

int mm_is_page_aligned_va(unsigned long va);

// #todo
void pages_calc_mem (void);

int 
mm_map_2mb_region(
    unsigned long pa,
    unsigned long va);

//
// #
// INITIALIZATION
//

// Memory initialization.
// This routine initializes the paging infrastructure.
int pagesInitializePaging(void);

#endif 

