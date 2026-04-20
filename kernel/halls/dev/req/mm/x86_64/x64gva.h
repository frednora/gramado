// x64gva.h
// Virtual memory overview.
// Global virtual addresses.
// Created by Fred Nora.

#ifndef __MM_GVA_H
#define __MM_GVA_H    1

// #todo
// Tem um monte de endereços usados no início da 
// memoria virtual do kernel. Mas acontece que esses
// endereços são os mesmos para físico e virtual.
// então precisamos estar concordância com os endereços
// encontrados em gpa.h 

// mbr and vbr
#define MBR_ADDRESS_VA    0x600  
#define VOLUME1_VBR_ADDRESS_VA   (MBR_ADDRESS_VA + 0x200) 
#define VOLUME2_VBR_ADDRESS_VA   (MBR_ADDRESS_VA + 0x200) 

// fat
//Podemos alocar memória para isso, liberando esse espaço?
#define VOLUME1_FAT_ADDRESS_VA   0x00030000 
#define VOLUME2_FAT_ADDRESS_VA   0x00030000

// rootdir
//Podemos alocar memória para isso, liberando esse espaço?
#define VOLUME1_ROOTDIR_ADDRESS_VA  0x00070000 
#define VOLUME2_ROOTDIR_ADDRESS_VA  0x00070000 


// #
// There is no default address for the data area.


// Isso porque endereços físicos e virtuais são igual para
// baixo de 1 mb.
#define KERNEL_PML4_VA 0x000000000009C000


// ==============================================================


//
// Main regions
//

//
// Kernel area -----------------------------------------
//

// See: pages.c
#define RING0AREA_VA    0

//
// User area -------------------------------------------
//

// O processo init foi carregado na marca de 
// 32MB fisico e 2MB virtual, entao devemos deixar 
// esse espaço fisico para o processo init.
#define RING3AREA_VA    0x0000000000200000
// 0x00201000 Entrypoint
// 0x003FFFF0 Stack 

// Isso será usado por todos os processos em ring3
// no ambiente do sistema principal.
#define FLOWERTHREAD_BASE        0x00200000  // base
#define FLOWERTHREAD_ENTRYPOINT  0x00201000  // entry point
// Stack do processo em ring3.
// Lembre-se que o processo vai precisa de um heap.
#define FLOWERTHREAD_STACK       0x003FFFF0  // stack 

//
// Warning:
// Here we have big not used espace of virtual address.
//

//
// Kernel image -----------------------------------------
//

// 768MB mark (VA)
// The start of the kernel image virtual address.
#define KERNELIMAGE_VA  0x0000000030000000

// Entry point: Start of kernel text.
#define KERNELIMAGE_ENTRYPOINT_VA  (KERNELIMAGE_VA + 0x1000)

//
// heap and stack  (va)
//

//
// Kernel heap -----------------------------------------
//

// #obs:
// Estamos usando uma área dentro dos 2MB alocados
// pelo boot loader para o kernel.
// No momento da configuração do heap e da stack
// ainda estamos usando o mapeamento feito pelo boot loader. 

// heap
// 768MB + 1MB mark (VA)
// size = 0xD0000
#define KERNEL_HEAP_START  0x30100000 
#define KERNEL_HEAP_END    0x301D0000
#define KERNEL_HEAP_SIZE   (KERNEL_HEAP_END - KERNEL_HEAP_START)

// Tem um espaço perdido para evitar colisão?

//
// Kernel stack -----------------------------------------
//

// stack
// 768MB + 1MB + E0000 mark (VA)
#define KERNEL_STACK_END    0x301E0000
#define KERNEL_STACK_START  0x301FFFF0 
#define KERNEL_STACK_SIZE   (KERNEL_STACK_START-KERNEL_STACK_END)


//
// Front buffer (LFB) -------------------------------------
//

// Frontbuffer
// Linear Frame Buffer (LFB) provided by the VESA BIOS Extensions (VBE).
// FRONTBUFFER_VA is the Linear Frame Buffer (LFB)
// provided by VESA BIOS Extensions (VBE).
// This is the virtual alias for the physical LFB base
// reported in the VBE mode info block.
// FRONTBUFFER_VA (VESA LFB) is intentionally user-accessible.
// Trade-off: ring3 apps can scribble on the screen (visual glitches only).
// Future: restrict to display server or move to kernel-only compositing.
// Map with Write-Combining (WC) if supported, else Uncacheable (UC).
// Always NX (non-executable)
// 768MB + 2MB mark (VA)
#define FRONTBUFFER_VA  0x0000000030200000
#define DEFAULT_LFB_VIRTUALADDRESS  FRONTBUFFER_VA 
#define FRONTBUFFER_ADDRESS         FRONTBUFFER_VA

//
// Backbuffer (RAM) ----------------------------------
//

// 768MB + 4MB mark (VA)
// Alias
// Backbuffer
// Endereço virtual padrão para o BackBuffer. (buffer1)
// 768MB + 4MB mark (VA)
#define BACKBUFFER_VA   0x0000000030400000
#define DEFAULT_BACKBUFFER_VIRTUALADDRESS  BACKBUFFER_VA
#define BACKBUFFER_ADDRESS                 BACKBUFFER_VA

//
// Paged pool --------------------------------
//


// 768MB + 6MB mark (VA)
// 2mb só da pra 512 páginas.
#define PAGEDPOOL_VA    0x0000000030600000

//
// Heap pool -------------------------------------
//

// 768MB + 8MB mark (VA)
// pequeno pool usado pra alocar heaps.
#define HEAPPOOL_VA     0x0000000030800000

//
// Extra heaps 1,2 and 3. ---------------------------
//

// 768MB + 10MB mark (VA)
// Ring 0 kernel module. (MOD0.BIN)
// see: __initialize_extraheap1() in pages.c.
#define EXTRAHEAP1_VA   0x0000000030A00000

// 768MB + 12MB mark (VA)
// First part of a slab allocator.
// see: __initialize_extraheap2() in pages.c.
#define EXTRAHEAP2_VA   0x0000000030C00000

// 768MB + 14MB mark (VA)
// Second part of a slab allocator.
// see: __initialize_extraheap3() in pages.c.
#define EXTRAHEAP3_VA   0x0000000030E00000

//-------------

// Alias
// The firt ring0 module. mod0.bin
#define MOD0_IMAGE_VA  EXTRAHEAP1_VA
#define MOD0_ENTRYPOINT_VA  0x0000000030A01000

// -----------------------------------
// free :)
// 768MB + 16MB mark (VA)
// 0x0000000031000000


//
// =============================================================
// Device regions (driver‑reserved virtual slots)
// =============================================================
//

// Notes:
// Device slots (BAR mappings): 
// RW, NX, UC or WC depending on device 
// (NIC BARs typically UC; framebuffers often WC).

// -----------------------------------
// Intel E1000 NIC BAR mapping
// “Driver‑reserved VA; 
// mapped at runtime to PCI BAR physical address. Use UC, NX.”
// #todo: Maybe NIC INTEL to another place
// with all the other devices.
// 768MB + 18MB mark (VA)
#define NIC_INTEL_E1000_VA  0x0000000031200000


//
// =============================================================
// MMIO regions (hardware registers mapped uncached, NX)
// =============================================================
//

// MMIO registers: RW, NX, UC, 4 KB pages (no large pages).

// -----------------------------------
// Local APIC registers
// MMIO registers: RW, NX, UC, 4 KB pages (no large pages).
// 768MB + 20MB mark (VA)
#define LAPIC_VA            0x0000000031400000

// -----------------------------------
// I/O APIC registers
// MMIO registers: RW, NX, UC, 4 KB pages (no large pages).
// 768MB + 22MB mark (VA)
#define IOAPIC_VA           0x0000000031600000

// -----------------------------------
// ACPI tables (RSDT/XSDT, MADT, FADT, etc.)
// 768MB + 24MB mark (VA)
// see: x64smp.c and acpi.c
#define RSDT_VA             0x0000000031800000
#define XSDT_VA             0x0000000031800000
// #todo
// We have more tables for ACPI support.
// MADT FADT

//--------------------------------------------

#endif    

