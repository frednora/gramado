// x64mm.c
// Memory management for x64 arch.
// Created by Fred Nora.

#include <kernel.h>

// See: x64mm.h
// Kernel process.
struct mm_data_d  kernel_mm_data;
// Init process.
struct mm_data_d  init_mm_data;
// A new process been created.
struct mm_data_d  newprocess_mm_data;
// ...


// ========================================================

// Local worker
static unsigned long 
__virtual_to_physical ( 
    unsigned long virtual_address, 
    unsigned long pml4_va );

// ========================================================


// PAE and PGE
// inline?
void __enable_pae(void)
{
    // PAE and PGE.
    asm volatile ( " movq %%cr4, %%rax; "
                   " orl $0x20,  %%eax; "
                   " movq %%rax, %%cr4  " :: );
}

// inline?
void x64mm_enable_paging(void)
{
    // Only paging.
    asm volatile (" movq %%cr0, %%rax;      "
                  " orl $0x80000000, %%eax; "
                  " movq %%rax, %%cr0       " :: );
}



// Precisa ser um endereço físico.
// 64bit?
void load_pml4_table(void *phy_addr)
{
    asm volatile ("movq %0, %%cr3"::"r"(phy_addr));
}

void x64mm_load_pml4_table(unsigned long phy_addr)
{
    asm volatile ("movq %0, %%cr3"::"r"(phy_addr));
}

void x64mm_refresh_cr3(void)
{
    // Refresh
    asm volatile ("movq %cr3, %rax");
    asm volatile ("movq %rax, %cr3");
}


// Worker
static unsigned long 
__virtual_to_physical ( 
    unsigned long virtual_address, 
    unsigned long pml4_va ) 
{

    //debug_print ("__virtual_to_physical: [TESTING] \n");

    //#debug
    //printk("virtual_address = %x \n",virtual_address);
    //printk("pml4_va = %x \n",pml4_va);
    //refresh_screen();
    //while(1){}

    if (virtual_address == 0){
        debug_print ("__virtual_to_physical: [?] virtual_address == 0 \n");
    }

// Why is it limited to 'int' size?

    unsigned int a = (unsigned int) ((virtual_address >> 39) & 0x1FF);  // 9 bits de pml4
    unsigned int b = (unsigned int) ((virtual_address >> 30) & 0x1FF);  // 9 bits de pdpt
    unsigned int d = (unsigned int) ((virtual_address >> 21) & 0x1FF);  // 9 bits de page directory
    unsigned int t = (unsigned int) ((virtual_address >> 12) & 0x1FF);  // 9 bits de page table. 
    unsigned int o = (unsigned int) (virtual_address         & 0xFFF);  // 12 bits de offset

    unsigned long tmp=0;
    unsigned long address=0;

    //printk ("a=%d b=%d d=%d t=%d o=%d \n",a,b,d,t,o);
    //refresh_screen();
    //while(1){}

    //offset limits
    //1111 1111 1111
    //FFF

// #debug
// In 64bit each table has only 512 entries.
// With 64KB pages, an offset can't have more than 4096 bytes.
    if ( a >= 512 || b >= 512 || d >= 512 || t >= 512 || 
         o >= 4096  )
    {
        printk("__virtual_to_physical: entry limits\n");
        printk("a=%d\n",a);
        printk("b=%d\n",b);
        printk("d=%d\n",d); //directory
        printk("t=%d\n",t); //page table
        printk("o=%d\n",o); //offset 4096 bytes limit
        refresh_screen();
        while (1){
        };
    }

// #todo
// Por enquanto estamos usando apenas as entradas '0'
// de pml4 e pdpt ... mas depois vamos usar todas.

    // #hackhack
    if (a != 0){
        printk ("__virtual_to_physical: [TODO] a != 0 \n");
        refresh_screen();
        while (1){
        };
        //return;
    }

    // #hackhack
    if (b != 0){
        printk ("__virtual_to_physical: [TODO] b != 0 \n");
        refresh_screen();
        while (1){
        };
        //return;
    }

    if (pml4_va == 0){
        debug_print ("__virtual_to_physical: [?] pml4_va == 0 \n");
        panic       ("__virtual_to_physical: [FAIL] Invalid pml4_va\n");
    }

// ==============================
// pml4
    //debug_print ("virtual_to_physical2: [pml4]\n");
    unsigned long *pml4VA = (unsigned long *) pml4_va;

    //printk (">> pml4VA[a] %x\n", pml4VA[a]);
    //refresh_screen();

    // Temos o pdpt junto com suas flags.
    tmp = (unsigned long) pml4VA[a];

// ==============================
// page directory pointer table.
    //debug_print ("virtual_to_physical2: [ptpt]\n");
    unsigned long *ptpt = (unsigned long *) (tmp & 0xFFFFFFFFF000);

    //printk (">> ptpt[d] %x\n", ptpt[b]);
    //refresh_screen();

    // Temos o pd junto com suas flags.
    tmp = (unsigned long) ptpt[b];

// ==============================
// page diretory
    //debug_print ("virtual_to_physical2: [dir]\n");
    unsigned long *dir = (unsigned long *) (tmp & 0xFFFFFFFFF000);

    //printk ("dir[d] %x\n", dir[d]);
    //refresh_screen();
    
    // Temos o endereço da pt junto com as flags.
    tmp = (unsigned long) dir[d];

    // Page table.
    //debug_print ("virtual_to_physical2: [pt]\n");
    unsigned long *pt = (unsigned long *) (tmp & 0xFFFFFFFFF000);

    // Encontramos o endereço base do page frame.
    tmp = (unsigned long) pt[t];

    address = (tmp & 0xFFFFFFFFF000);

    //debug_print ("virtual_to_physical2: done\n");

// OUT:
// Physical address.
    return (unsigned long) (address + o);
}

// IN:
//   + virtual_address
//   + pml4_va
// OUT:
//   + physical address
unsigned long 
virtual_to_physical ( 
    unsigned long virtual_address, 
    unsigned long pml4_va ) 
{
    return (unsigned long) __virtual_to_physical(virtual_address,pml4_va);
}


// ---------------------------
// mm_fill_page_table:
// Cria uma page table com 512 entradas
// para uma região de 2mb e configura uma
// determinada entrada no diretório de páginas indicado.
// IN:
// directory_va: 
//     Endereço virtual do diretório de páginas.
// directory_entry: 
//     Índice da entrada no diretório indicado.
// pt_va:
//     Endereço virtual da tabela de páginas.
// region_2mb_pa: 
//     Endereço físico da região de 2MB que queremos mapear.
// flags:
//     As flags usadas em todas as entradas da pagetable
//     e na entrada do diretório de páginas.

int 
mm_fill_page_table( 
    unsigned long directory_va, 
    int           directory_entry,
    unsigned long pt_va,
    unsigned long region_2mb_pa,
    unsigned long flags )
{
    register int i=0;
    unsigned long *dir = (unsigned long *) directory_va;
    unsigned long *pt  = (unsigned long *) pt_va;
    unsigned long pa   = (unsigned long) region_2mb_pa;

// #todo
// We need to validate some limits here.

// Validation
    if ( directory_entry < 0 || 
         directory_entry >= 512 )
    {
        return -1;
    }

// Fill the pagetable with 512 entries.
    for ( i=0; i<512; ++i )
    {
        pt[i] = (unsigned long) (pa | flags);
        pa    = (unsigned long) (pa + 4096);
    };

// Create a directory entry in the given index.
    dir[directory_entry] = 
        (unsigned long) pt_va;  //&pt[0];
    dir[directory_entry] = 
        (unsigned long) (dir[directory_entry] | flags);

    return 0;
}

