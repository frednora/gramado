// pages.h
// Heder for supporting the creation of the 
// main system's page tables by pages.c
// Created by Fred Nora.

#ifndef __X86_64_PAGES_H
#define __X86_64_PAGES_H    1

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

