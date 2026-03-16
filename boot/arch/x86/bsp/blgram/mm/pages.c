// pages.c
// Pre-mapping the some memory regions.
// This is gonna be changed by the base kernel.
// We need this to compile the base kernel 
// against the 0x30000000 virtual address address.
// 2015 - Created by Fred Nora.

#include "../bl.h"  

// --------------------------
// PAE and PGE
// In essence, PAE allows for larger physical memory, 
// while PGE optimizes TLB management by selectively flushing entries

// --------------------------
// PAE (Physical Address Extensions): (Bit 5 in cr4).
// When PAE is enabled, the page table entries are extended 
// from 32 bits to 64 bits, allowing for a 36-bit physical address space. 
// Allows 32-bit operating systems to utilize more physical memory, 
// a feature commonly found in x86-64 architectures

// --------------------------
// PGE (Paging Global Enable):
// Determines how the TLB (Translation Lookaside Buffer) is flushed 
// when CR3 is modified or during a task switch.
// When PGE is set, moves to CR3 (or task switches) 
// will only invalidate entries in the TLB that have 
// the global bit (G-bit) in the corresponding page table entry set to 0. 
// If PGE is not set, all TLB entries will be invalidated.
// Reduces TLB flush overhead, especially when dealing 
// with global page mappings, as global mappings typically 
// don't need to be flushed. 

// ======================================================

extern void BlTransferTo64bitKernel(void);

// ======================================================

// Internal helpers
static void load_pml4_table(void *phy_addr);
static void enable_PAE(void);
static void enable_paging(void);

// ======================================================

// Load CR3 with the physical address of the PML4 table.
static void load_pml4_table(void *phy_addr)
{
    asm volatile (" movl %0, %%cr3 " :: "r" (phy_addr) );
}

// Enable PAE (Physical Address Extension).
static void enable_PAE(void)
{
    asm volatile ( " movl %%cr4, %%eax  \n"
                   " orl $0x20, %%eax   \n"     // Set bit 5 (PAE)
                   " movl %%eax, %%cr4  " :: );
}

// Enable paging (CR0.PG). 
// Assumes we are already in protected mode.
static void enable_paging(void)
{
    asm volatile ( " movl %%cr0, %%eax  \n"
                   " orl $0x80000000, %%eax  \n"  // Set bit 31 (PG)
                   " movl %%eax, %%cr0  " :: );
}

// ------------------------------------------------------ 
// SetUpPaging: 
// - Builds initial page tables for kernel and basic regions. 
//   We are building table to the used by the 64bit kernel. So
//   we're using 64bit addresses.
// - Enables PAE and loads CR3 with PML4. 
// - Sets EFER.LME via MSR to prepare for long mode. 
// - Transfers execution to 64-bit kernel entry. 
//   This routine does not return.
// Called by bl_main in main.c
// ------------------------------------------------------

void SetUpPaging(void)
{
    register unsigned int i=0;

    // Physical base addresses
    unsigned long kernel_address = 0;            // Start of RAM 
    unsigned long kernel_base    = KERNEL_BASE;  // Kernel image base (0x100000)
    unsigned long user_address   = USER_BASE;    // User space base (0x00400000)
    unsigned long vga_address    = VM_BASE;      // Legacy VGA text buffer (0x000B8000)
    unsigned long lfb_address    = (unsigned long) g_lbf_pa;    // VESA LFB
    unsigned long buff_address   = (unsigned long) 0x01000000;  // Backbuffer (16 MB mark)

// =====================================
// Page table hierarchy
//
// PML4, PDPT, PD, PT
// PML4 - Page Map Level 4
// PDPT - Page Directory Pointer Table
// PD   - Page Directory
// PT   - Page Table
//

    unsigned long *boot_pml4 = (unsigned long *) (0x01000000 - 0x00700000);  // level 4
    unsigned long *boot_pdpt = (unsigned long *) (0x01000000 - 0x00800000);  // level 3
    unsigned long *boot_pd0  = (unsigned long *) (0x01000000 - 0x00900000);  // level 2
    // level 1: A lot of page tables in the level 1.

    // Individual page tables (level 1)
    unsigned long *ptKM =         (unsigned long *) 0x0008F000;  // First 2 MB
    unsigned long *ptKM2 =        (unsigned long *) 0x0008E000;  // Kernel image
    unsigned long *ptUM =         (unsigned long *) 0x0008D000;  // User space
    unsigned long *ptVGA =        (unsigned long *) 0x0008C000;  // VGA text buffer
    unsigned long *ptLFB =        (unsigned long *) 0x0008B000;  // Linear Frame Buffer
    unsigned long *ptBACKBUFFER = (unsigned long *) 0x0008A000;  // Backbuffer
    // ...

//
// ==================================================
//

// Clear levels 4, 3 and 2.

// Clear level 4.
    for ( i=0; i < 512; i++ )
    {
        boot_pml4[i*2   ] = (unsigned long) 0 | 2;
        boot_pml4[i*2 +1] = (unsigned long) 0;
    };
// Clear level 3.
    for ( i=0; i < 512; i++ )
    {
        boot_pdpt[i*2   ] = (unsigned long) 0 | 2;
        boot_pdpt[i*2 +1] = (unsigned long) 0;
    };
// Clear level 2.
    for ( i=0; i < 512; i++ )
    {
        boot_pd0[i*2   ] = (unsigned long) 0 | 2;
        boot_pd0[i*2 +1] = (unsigned long) 0;
    };

// ============================

// Link PD into PDPT
// Pointing the 'page directory' address 
// at the first entry in the 'page directory pointer table'.
    boot_pdpt[0*2]   = (unsigned long) &boot_pd0[0];
    boot_pdpt[0*2]   = (unsigned long) boot_pdpt[0*2] | 3;
    boot_pdpt[0*2+1] = 0; 

// Link PDPT into PML4
// Pointing the 'page directory pointer table' address 
// at the first entry in the 'boot_pml4'.
    boot_pml4[0*2]   = (unsigned long) &boot_pdpt[0];
    boot_pml4[0*2]   = (unsigned long) boot_pml4[0*2] | 3; 
    boot_pml4[0*2+1] = 0; 

//
// PAGE TABLES.
//

// Vamos criar algumas pagetables e apont�-las
// como entradas no diret�rio 'boot_pd0'.

// =======================================
// Primeiros 2MB.
// 0virt
Entry_0:

/*
 kernel mode pages 
 =================
 (kernel mode)(0fis = 0virt).
 Configurando o in�cio da mem�ria RAM
 como �rea em kernel mode.
 + Mapeando os primeiros 2MB da mem�ria.  
 + Preenchendo a tabela km_page_table.
 + A entrada '0' do diret�rio aponta para
   uma tabela que mapeia os primeiros 4 mega 
   de endere�o virtual.
 'kernel_address' � o in�cio da mem�ria RAM.
  Cada pagina tem 4KB.
 */

// #importante
// Essa primeira entrada esta funcionando.
// Conseguimos usar essa identidade 1:1,
// tanto aqui no bl, quanto no kernel.

    for ( i=0; i < 512; i++ )
    {
        ptKM[i*2]   = kernel_address | 3; 
        ptKM[i*2+1] = 0;
        kernel_address = kernel_address + 4096;
    };
    // Criando a primeira entrada do diret�rio.
    // Isso mapeia os primeiros 2MB da mem�ria RAM.
    boot_pd0[0*2] = (unsigned long) &ptKM[0];
    boot_pd0[0*2] = boot_pd0[0*2] | 3;    //Configurando atributos.
    boot_pd0[0*2+1] = 0;

// =======================================
// Uma �rea em user mode.
// 0x00200000vir - Come�a na marca de 32mb.
Entry_1:

/*
 user mode pages 
 ===============
 (400000fis = 400000virt).
 Configurando uma �rea de mem�ria em user mode,
 usada por processos em user mode.
 Mapear 2MB da mem�ria come�ando em 400000fis. 
 (user mode).
 user_address = (400000fis = 400000virt).
 */

    for (i=0; i < 512; i++)
    {
        ptUM[i*2]   = user_address | 7;    //0111 em bin�rio.
        ptUM[i*2+1] = 0;
        user_address = user_address + 4096;     //4096 = 4KB.
    };
    //Criando a entrada n�mero 1 do diret�rio.
    boot_pd0[1*2]   = (unsigned long) &ptUM[0];
    boot_pd0[1*2]   = (unsigned long) boot_pd0[1*2] | 7;  //Configurando atributos.
    boot_pd0[1*2+1] = 0; 

// ============================
// vga
Entry_2:

/*
 User Mode VGA pages: 
 ===================
 (0xB8000fis = 800000virt).
 Mapeando a �rea de mem�ria usada pela mem�ria
 de v�deo em modo texto, 0xB8000.
 Mapear 2MB da mem�ria come�ando em B8000fis.
 Obs:
     Aqui, na verdade n�o precisaria configurar 4 megas, 
     apenas o tamanho da mem�ria de v�deo presente em 0xb8000.
 vga_address = 0xB8000.
 */

    for ( i=0; i < 512; i++ )
    {
        ptVGA[i*2]   = vga_address | 7;    //0111 em bin�rio.
        ptVGA[i*2+1] = 0;
        vga_address  = vga_address + 4096;       //4KB.
    };
    //Criando a entrada n�mero 2 no diret�rio.
    boot_pd0[2*2] = (unsigned long) &ptVGA[0];
    boot_pd0[2*2] = boot_pd0[2*2] | 7;    //Configurando atributos.
    boot_pd0[2*2+1] = 0; 


// =====================================
// A imagem do kernel.
// 0x30000000virt - Come�a na marca de 1MB da mem�ria f�sica.
Entry_384:

// #bugbug
// Essa n�o � possivel pois temos o limite de 512 entradas.

/*
 kernel mode pages 
 =================
 (0x100000fis = 0xc0000000virt).
      possivelmente 0x30000000
      mas precisamos checar, pois isso n�o esta funcionando.
      precisamos encontrar esse endere�o virtual.
 Configurando a �rea de mem�ria onde ficar� a imagem do kernel.
 Isso mapeia 2MB come�ando do primeiro mega. 
 (kernel mode).
 Preenchendo a tabela km_page_table.
 'kernel_base' � o endere�o f�sico da imagem do kernel.
 */

    for ( i=0; i < 512; i++ )
    {
        ptKM2[i*2]   = kernel_base | 3;     // 0011b
        ptKM2[i*2+1] = 0;
        kernel_base  = kernel_base + 4096;  // 4KB
    };
//Criando a entrada de n�mero 768 do diret�rio.
    boot_pd0[384*2] = (unsigned long) &ptKM2[0];
    boot_pd0[384*2] = (unsigned long) boot_pd0[384*2] | 3;    //Configurando atributos.
    boot_pd0[384*2+1] = 0;

    //   1 1000 0 000
    // | 0 0000 0 000 | 0 0000   0000  | 0000 0000 0000
    //   00|000 0|000   0 0000 | 0000    0000 0000 0000
    //   30000000
    // ent�o a entrada 384 aponta para 0x30000000      

//===========================================
// frontbuffer - LFB
// 0x30200000virt
Entry_385:

/*
 user mode LFB pages 
 ===================
 (0X??fis = 0xC0400000virt).
 O endere�o linear do lfb �  agora. ?? 0x30200000
 Mapear 2MB � partir do endere�o configurado
 como in�cio do LFB.
 O Boot Manager configurou VESA e obteve o endere�o do LFB.
 O Boot Manager passou para o Boot Loader esse endere�o.
 Mapeando 2MB da mem�ria fisica come�ando no 
 endere�o passado pelo Boot Manager.
 O endere�o de mem�ria virtual utilizada � 0xC0400000.
 lfb_address = Endere�o do LFB, passado pelo Boot Manager.
 */

    for ( i=0; i < 512; i++ )
    {
        ptLFB[i*2]   = lfb_address | 7;     // 0111b
        ptLFB[i*2+1] = 0;
        lfb_address  = lfb_address + 4096;  // 4KB
    };
    //Criando a entrada n�mero 769 do diret�rio.
    boot_pd0[385*2] = (unsigned long) &ptLFB[0];
    boot_pd0[385*2] = boot_pd0[385*2] | 7;    //Configurando atributos.	
    boot_pd0[385*2+1] = 0; 

// ===================================================
// backbuffer
// 0x30400000virt
Entry_386:

/*
 user mode BUFFER1 pages 
 =======================
 (0X??fis = 0xC0800000virt) (BackBuffer). 0x30800000
 Esse � o backbuffer para a mem�ria de v�deo.
 O conte�do desse buffer � transferido para o LFB. 
 O endere�o de mem�ria virtual utilizada � 0xC0800000.
 buff_address = 0x01000000. (16MB), provis�rio.
 //16mb. (16*1024*1024) = 0x01000000.
 */

    for ( i=0; i < 512; i++ )
    {
        ptBACKBUFFER[i*2] = buff_address | 7;    //0111 em bin�rio.
        ptBACKBUFFER[i*2+1] = 0;
        buff_address    = buff_address + 4096;       //4KB.
    };
    //Criando a entrada n�mero 770 do diret�rio.
    boot_pd0[386*2] = (unsigned long) &ptBACKBUFFER[0];
    boot_pd0[386*2] = (unsigned long) boot_pd0[386*2] | 7;    //Configurando atributos.
    boot_pd0[386*2+1] = 0;

//breakpoint
    //printf ("SetUpPaging: tables done\n");
    //refresh_screen();
    //while(1){}

// ==============================================

    // __asm__ __volatile__("cli");

//
// PAE
//

    // Enable PAE
    enable_PAE();

//
// CR3
//

    // Load CR3 with PML4
    // Point cr3 to boot_pml4.
    unsigned long pml4_address = (unsigned long) &boot_pml4[0];
    load_pml4_table( (unsigned long) pml4_address );

//
// MSR
//

    //#debug
    //printf ("SetUpPaging: cpuSetMSR\n");
    //refresh_screen();

    //int msrStatus = FALSE;
    //msrStatus = cpuHasMSR();

    // Enable Long Mode via EFER.LME
    //IN: MSR, LO, HI
    cpuSetMSR( 0xC0000080, 0x100, 0 );

// Nesse caso habilitamos SYSCALL/SYSRET
// #ok: Testado no qemu
    //cpuSetMSR( 0xC0000080, 0x101, 0 );

    //refresh_screen();
    //while(1){}

// Enable paging.
    //printf ("SetUpPaging: enable_paging\n");
    //refresh_screen();

// #important:
// Adiamos isso, o assembly vai fazer isso.
// Pois no momento em que configurarmos isso, o long mode
// estar� habilitado.
    // enable_paging();

// #bugbug
// N�o podemos chamar rotina alguma antes de configurarmos
// os registradores.

// vamos tentar fazer isso quando saltarmos para o kernel
    //unsigned long *fb = (unsigned long *) 0xC0400000; 
    //fb[0] = 0xFF00FF;

// nao podemos chamar rotina alguma aqui,
// somente retornar.
// os registradores estao bagun�ados.

    //#debug
    //printf ("SetUpPaging: [breakpoint] DONE \n");
    //printf ("SetUpPaging: DONE, returning to assembly in head.s \n");
    //refresh_screen();
    //while(1){}

// Go to kernel
// Transfer control to 64-bit kernel entry
// See: transfer.inc

    BlTransferTo64bitKernel();

// Hang:
// Not reached
    while (1){
        asm ("cli ");
        asm ("hlt ");
    };
}

//
// End
//

