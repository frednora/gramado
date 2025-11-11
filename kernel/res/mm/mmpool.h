// mmpool.h
// Created by Fred Nora.

#ifndef __MM_MMPOOL_H
#define __MM_MMPOOL_H    1

// Allocators
// #todo: 
// Explain the purpose of each one of these routine here in this file.
void *newPage(void);
void *mm_alloc_single_page(void);
void *mm_alloc_contig_pages(size_t size);
void *allocPages(size_t size);
void *mmAllocPage(void);
void *mmAllocPages(size_t size);

// Initialize
void initializeFramesAlloc(void);

#endif    

