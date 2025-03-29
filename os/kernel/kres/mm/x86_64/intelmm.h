
// intelmm.h
// x86_64 only.

#ifndef __MM_INTELMM_H
#define __MM_INTELMM_H    1

//
// tables support
//

// Level 4
// PML4E - PML4 Entry.
struct intel_pml4e_d
{
// #todo
// ...
};

// Level 3
// PDPTE - Page directory Pointer Table Entry. 
struct intel_pdpte_d
{
// #todo
// ...
};

// Level 2
// PDE - Page Directory Entry.
struct intel_pde_d
{
// #todo
    unsigned long Present              :1;
    unsigned long ReadWrite            :1;
    unsigned long UserSupervisor       :1;
    unsigned long WriteThrough         :1;
    unsigned long CacheDisabled        :1;
    unsigned long Accessed             :1;
    unsigned long Reserved             :1;
    unsigned long PageSize             :1;
    unsigned long GlobalPage           :1;
    unsigned long Available1           :1;
    unsigned long Available2           :1;
    unsigned long Available3           :1;
    unsigned long PageTableBaseAddress :20;
};

// Level 1
// PTE - Page Table Entry.
struct intel_pte_d
{
// #todo
    unsigned long Present         :1;
    unsigned long ReadWrite       :1;
    unsigned long UserSupervisor  :1;
    unsigned long WriteThrough    :1;
    unsigned long CacheDisabled   :1;
    unsigned long Accessed        :1;
    unsigned long Reserved        :1;
    unsigned long PAT             :1;  // Page Table Attribute Index.
    unsigned long GlobalPage      :1;
    unsigned long Available1      :1;
    unsigned long Available2      :1;
    unsigned long Available3      :1;
    unsigned long PageBaseAddress :20;
}; 

#endif    




