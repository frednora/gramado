// x64gpa.h 
// Physical memory overview.
// Global declaration of static physical address used by the kernel.
// Created by Fred Nora.

#ifndef __MM_GPA_H
#define __MM_GPA_H  1

//
// == 0 KB ==================================================
//

/*
0x00000000 - 0x000003FF - Real Mode Interrupt Vector Table
0x00000400 - 0x000004FF - BIOS Data Area
0x00000500 - 0x00007BFF - Unused
0x00007C00 - 0x00007DFF - Our Bootloader
0x00007E00 - 0x0009FFFF - Unused
0x000A0000 - 0x000BFFFF - Video RAM (VRAM) Memory
0x000B0000 - 0x000B7777 - Monochrome Video Memory
0x000B8000 - 0x000BFFFF - Color Video Memory
0x000C0000 - 0x000C7FFF - Video ROM BIOS
0x000C8000 - 0x000EFFFF - BIOS Shadow Area
0x000F0000 - 0x000FFFFF - System BIOS
*/

// First 2 MB.
#define SYSTEM_ORIGIN  0
#define SMALLSYSTEM_ORIGIN_ADDRESS   SYSTEM_ORIGIN
#define MEDIUMSYSTEM_ORIGIN_ADDRESS  SYSTEM_ORIGIN
#define LARGESYSTEM_ORIGIN_ADDRESS   SYSTEM_ORIGIN
// Size = 1KB
#define REALMODE_IVT  SYSTEM_ORIGIN

// Size?
// 0x00000400 - 0x000004FF - BIOS Data Area
#define REALMODE_BIOSDATAAREA  0x400
#define BDA  0x400

// Size?
#define REALMODE_FREEAREA  0x500

// mbr and vbr
// A place for the kernel 
// to put the MBR on the fly.
#define MBR_ADDRESS  0x600
#define VOLUME1_VBR_ADDRESS  (MBR_ADDRESS + 0x200) 
#define VOLUME2_VBR_ADDRESS  (MBR_ADDRESS + 0x200) 
// ...


// ...

//
// == 4 KB ==================================================
//

// ===================================================

// #importante: PERIGO !!!
// >>>> 0x1000
// O sistema está usando esse endereço como início de um heap
// onde pegamos páginas de memória para criarmos diretórios de páginas.
// Isso porque precisamos de endereços que terminem com pelo menos 
// 12bits zerados.
// #todo: 
// Podemos fazer uma contagem de alocaçao possivel nesse heap
// #todo: 
// Precisamos encontrar outro lugar para esse heap, tendo em vista que
// o número de diretórios criados será grande e o heap invadirá 
// outras áreas.
// me parece que o próximo endereço usado é o MBR em 0x00020000. 
// #todo: Possivelmente podemos mudar o MBR de lugar usando o alocador.
// #OBS: Os endereços físico e virtual são iguais.
// See: mm/pages.c

// #todo
// Define min and max.
// Quantos diretórios podemos alocar aqui?
// 31KB. (31 alocações)

// This is the base address of a pre-alocated
// heap of pagetables.
// 31KB. (31 alocações)
#define ____DANGER_TABLE_POINTER_HEAP_BASE  0x1000

// Essa tabela é bem longa
// Vai até o MBR.
// #bugbug
// We're gonna use this area to put the AP trampoline code.

//
// == 128 KB ==================================================
//

//#define __128KBMARK  0x00020000

//
// == 192 KB ==================================================
//

// #bugbug
// Both are using the same address.

#define FAT_ADDRESS  0x00030000
#define VOLUME1_FAT_ADDRESS  (FAT_ADDRESS + 0)  // Size?
//#define VOLUME2_FAT_ADDRESS  (FAT_ADDRESS + 0)  // Size?

//
// == 448 KB ==================================================
//

// #bugbug
// Both are using the same address.

// Rootdir size = (512*32) = 16 KB.
#define ROOTDIR_ADDRESS  0x00070000 
#define VOLUME1_ROOTDIR_ADDRESS  (ROOTDIR_ADDRESS + 0) 
#define VOLUME2_ROOTDIR_ADDRESS  (ROOTDIR_ADDRESS + 0)

#define VOLUME1_ROOTDIR_PA  VOLUME1_ROOTDIR_ADDRESS
#define VOLUME2_ROOTDIR_PA  VOLUME2_ROOTDIR_ADDRESS


/*
#bugbug
0x00007E00 - 0x0009FFFF - Unused" 
This is not entirely true. 
The range that is guaranteed to be free is 0x00007E00 - 0x0007FFFF. 
Above that range you will have the 
EBDA (usually at 0x0009FC00 - 0x0009FFFF, 
but I've seen it at 0x00096C00 and other locations as well) and 
potentially some BIOS code. 
Some BIOSes have PXE boot code in this range. 
A conservative approach is to avoid everything above 0x00080000. 
*/

//
// == 512 KB ==================================================
//


//
// == 576 KB ==================================================
//

// #bugbug
// Em 0x00090000 costumava ter uma pilha?
// Se o boot ou o kernel usou essa área para pilha,
// então poderemos ter perdido algum valor 
// colocado aí pelo BIOS.

// 'Canonical'
// O endereço físico e virtual do boot block são o mesmo.
// Boot block size?
#define BOOTBLOCK_PA  0x0000000000090000


// 'Canonical?'
// Page Map Level N.
// pd, pdpt, pml4 for the kernel process.
// Lembrando que a parte das flags precisa ser '000'
#define KERNEL_PD_PA    0x000000000009A000  //pml2
#define KERNEL_PDPT_PA  0x000000000009B000  //pml3
#define KERNEL_PML4_PA  0x000000000009C000  //pml4

/*
 Older computers typically uses 1 KiB from 0x9FC00 - 0x9FFFF, 
 modern firmware can be found using significantly more. 
 You can determine the size of the EBDA by 
 using BIOS function INT 12h, or by examining 
 the word at 0x413 in the BDA (see below). 
 Both of those methods will tell you how much conventional memory 
 is usable before the EBDA.
 Credits: https://wiki.osdev.org/Memory_Map_(x86)
 */

// #bugbug
// EBDA address is found here.
// 0x9FC00

// #bugbug
// Temos uma pilha aqui?
// O boot loader sujou essa área colocando uma pilha aí?
// 0x0009FFF0

// #bugbug
// BM.BIN is using the range 0x00090000 ~ 0x0009FFF0.
// So, the kernel simply can't trust in values found here.
// See: bm/sysvar32.inc

//
// == 640 KB ==================================================
//

// Base memory limit.

// - 0x000A0000 - 0x000BFFFF - Video RAM (VRAM) Memory
// - 0x000B0000 - 0x000B7777 - Monochrome Video Memory
// - 0x000B8000 - 0x000BFFFF - Color Video Memory
// - 0x000C0000 - 0x000C7FFF - Video ROM BIOS
// - 0x000C8000 - 0x000EFFFF - BIOS Shadow Area
// - 0x000F0000 - 0x000FFFFF - System BIOS

//==============
// vga
// The address of the VGA buffer.
//EGA	A0000 for 128K byte *
//VGA	A0000 for 128K byte *

#define VGA_PA  0x000A0000
#define EGA_PA  0x000A0000

//
// == 704 KB ==================================================
//

//==============
// mda
// The address of the MDA buffer.
// Monocrome.
//MDA	B0000 for 4K byte
//HGC (Hercules)	B0000 for 64K byte

#define MDA_PA  0x000B0000
#define HGC_PA  0x000B0000


//
// == 736 KB ==================================================
//


//==============
// cga
// The address of the CGA buffer.
// colors.
//CGA	B8000 for 16K byte

#define CGA_PA            0x000B8000
#define SMALLSYSTEM_CGA   CGA_PA
#define MEDIUMSYSTEM_CGA  CGA_PA
#define LARGESYSTEM_CGA   CGA_PA


//
// == 768 KB ==================================================
//

// 16-bit devices, expansion ROMs
#define BIOS_STUFF  0x000C0000


//
// == 960 KB ==================================================
//

// BIOS ROM
#define BIOS_STUFF2  0x000F0000
#define BIOS_ROM  BIOS_STUFF2

//
// == 1 MB ==================================================
//

// EXTENDED MEMORY

// 1MB físico.
// #importante: 
// Foram mapeados 2MB para 
//  + a 'imagem do kernel' e para 
//  + o 'heap' e 
//  + a 'stack'.

// 1
#define KERNEL_BASE_PA  0x00100000
#define SMALLSYSTEM_KERNELBASE   KERNEL_BASE_PA
#define MEDIUMSYSTEM_KERNELBASE  KERNEL_BASE_PA
#define LARGESYSTEM_KERNELBASE   KERNEL_BASE_PA

//
// == 2 MB ==================================================
//

// 2
// 0x00200000
// #warning
// Reserved to extend the area for the kernel image.


//
// == 4 MB ==================================================
//

// 4
// 0x00400000
// #warning
// Reserved to extend the area for the kernel image.


// 6
// 0x00600000
// #warning
// Reserved to extend the area for the kernel image.



//
// == 8 MB ==================================================
//

// 8
// 0x00800000
// #warning
// Reserved to extend the area for the kernel image.


// 10
// 0xA00000
// #warning
// Reserved to extend the area for the kernel image.


// 12
// 0xC00000
// #warning
// Reserved to extend the area for the kernel image.


// 14
// 0xE00000
// #warning
// Reserved to extend the area for the kernel image.


//
// == 16 MB =========================================================
//
// 0x01000000

// 16
// #bugbug
// The 'heap pool' has 2MB in size
// and it starts at 0x01000000. (16MB mark).
// 2MB = 2048 KB.
// (2048/64) = 32 KB.
// It has 64 processes with 32KB each.
// See: process.h
#define HEAPPOOL_PA  0x01000000
#define SMALLSYSTEM_HEAPPOLL_START   HEAPPOOL_PA
#define MEDIUMSYSTEM_HEAPPOLL_START  HEAPPOOL_PA
#define LARGESYSTEM_HEAPPOLL_START   HEAPPOOL_PA

//16+2 = 18 MB
#define EXTRAHEAP1_PA  (0x01000000 + 0x200000)
#define SMALLSYSTEM_EXTRAHEAP1_START     EXTRAHEAP1_PA
#define MEDIUMSYSTEM_EXTRAHEAP1_START    EXTRAHEAP1_PA 
#define LARGESYSTEM_EXTRAHEAP1_START     EXTRAHEAP1_PA 

//16+4 = 20 MB
#define EXTRAHEAP2_PA  (0x01000000 + 0x400000)
#define SMALLSYSTEM_EXTRAHEAP2_START    EXTRAHEAP2_PA
#define MEDIUMSYSTEM_EXTRAHEAP2_START   EXTRAHEAP2_PA 
#define LARGESYSTEM_EXTRAHEAP2_START    EXTRAHEAP2_PA 

//16+6 = 22 MB
#define EXTRAHEAP3_PA  (0x01000000 + 0x600000)
#define SMALLSYSTEM_EXTRAHEAP3_START     EXTRAHEAP3_PA
#define MEDIUMSYSTEM_EXTRAHEAP3_START    EXTRAHEAP3_PA 
#define LARGESYSTEM_EXTRAHEAP3_START     EXTRAHEAP3_PA 

// 24
#define PAGEDPOOL1_PA  (0x01000000 + 0x800000) 
#define SMALLSYSTEM_PAGEDPOLL_START   PAGEDPOOL1_PA
#define MEDIUMSYSTEM_PAGEDPOLL_START  PAGEDPOOL1_PA
#define LARGESYSTEM_PAGEDPOLL_START   PAGEDPOOL1_PA
#define PAGEDPOOL_PA  PAGEDPOOL1_PA

// 26
#define PAGEDPOOL2_PA  (0x01000000 + 0xA00000) 

// 28
#define PAGEDPOOL3_PA  (0x01000000 + 0xC00000) 

// 30
#define PAGEDPOOL4_PA  (0x01000000 + 0xE00000) 


//
// == 32 MB =========================================================
//

// O processo init foi carregado na marca de 
// 32MB fisico e 2MB virtual.
// 0x02000000
// Um sistema maior que 32MB já é considerado Small.
// #atenção:
// Essa é uma area em user mode.
// #todo
// Pra que essa area esta sendo usada?

// 32 MB mark
#define USER_BASE_PA  0x02000000 
#define SMALLSYSTEM_USERBASE     USER_BASE_PA
#define MEDIUMSYSTEM_USERBASE    USER_BASE_PA
#define LARGESYSTEM_USERBASE     USER_BASE_PA

// #important: 
// Segue-se bastaste espaço livre.
// Mas como o processo init foi carregado na marca de 
// 32MB fisico e 2MB virtual, entao devemos deixar 
// esse espaço fisico para o processo init.

// 34
// 36
// 38
// 40
// 42
// 44
// 46
// 48
// 50
// 52
// 54
// 56
// 58
// 60
// 62

//
// == 64 MB =========================================================
//

// 0x04000000
// #important: Segue-se bastaste espaço livre.
// ...
#define BACKBUFFER_PA  0x04000000

// #test
// Let's reserve a big space here for the backbuffer.
// Maybe we can have more than one backbuffer in this area,
// or even a place for some surfaces.



//
// 128MB and 256MB marks -----------------------------------------------
//

// ----------------------------------
// Frame Table


// Start
#define __128MB_MARK_PA      (0x08000000)
#define FRAMETABLE_START_PA  __128MB_MARK_PA

// End
// #ps: End at 256MB mark less 8MB.
#define __256MB_MARK_PA      (0x10000000)
#define FRAMETABLE_END_PA    (__256MB_MARK_PA - 0x800000)

// -----------------------------------------
// #warning
// When the system has 256MB installed
// the detection routine can detect only 255,
// because it is not checking the lask MB.
// #ps:
// 256 MB mark is free to use when we have 512mb of memory or more.
// -----------------------------------------

#define WINDOWS_POOL_START_PA  __256MB_MARK_PA

// Let's put some windows here.
// 256/2 = 128 application windows.


//
// == 512 MB =========================================================
//


#define __512MB_MARK_PA  (0x20000000)

//
// == 1GB =========================================================
//

// 0x40000000
// #available

#define __1GB_MARK_PA  (0x40000000)

// #todo
// Quando o sistema tiver memória o suficiente
// Então colocaremos um backbuffer bem grande aqui.
// Provavelmente no limite de 1gb de tamanho.
// #todo: 
// Já podemos tentar isso na máquina real.

#define BACKBUFFER_1GB_MARK_PA  __1GB_MARK_PA


//
// == 2GB =========================================================
//

// 0x80000000
// #available

#define __2GB_MARK_PA  (0x80000000)

//
// == 3GB =========================================================
//

// 0xC0000000
// #available

#define __3GB_MARK_PA  (0xC0000000)



// =================================================================

/*
    // endereços comuns
    
    0x01000000 =  16 MB
    0x02000000 =  32 MB
    0x04000000 =  64 MB
    0x08000000 = 128 MB
    
    0x10000000 = 256 MB 
    0x20000000 = 512 MB
    0x40000000 = 1    GB
    0x50000000 = 1.24 GB
    0x60000000 = 1.5  GB
    0x70000000 = 1.75 GB
    0x80000000 = 2    GB
*/


// [APIC I/O unit]
// i/o apic physical.
// The address space reserved for the local APIC 
// is used by each processor to access its own local APIC.
// Subsequent I/O APIC addresses are assigned in 4K increments.
// 0xFEC00000 + 0x1000
#define __IOAPIC_PA  0xFEC00000

// [APIC Local Unit]
// local apic physical address.
// The address space reserved for the I/O APIC 
// must be shareable by all processors 
// to permit dynamic reconfiguration
#define __LAPIC_PA   0xFEE00000

//--------------------------------------------------

//
// After 4GB ... 
//


#endif    








