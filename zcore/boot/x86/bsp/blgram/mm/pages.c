// pages.c
// Pre-mapping the some memory regions.
// This is gonna be changed by the base kernel.
// We need this to compile the base kernel 
// against the 0x30000000 virtual address address.
// 2015 - Created by Fred Nora.

#include "../bl.h"  


extern void BlTransferToKernel(void);

// local
// inline?
void load_pml4_table(void *phy_addr)
{
    asm volatile (" movl %0, %%cr3 " :: "r" (phy_addr) );
}

// local
// inline?
// PAE and PGE
void enable_pae()
{
    asm volatile ( " movl %%cr4, %%eax; "
                   " orl $0x20, %%eax;  "
                   " movl %%eax, %%cr4  " :: );
}

// local
// inline?
void page_enable()
{
    asm volatile ( " movl %%cr0, %%eax;      "
                   " orl $0x80000000, %%eax; "
                   " movl %%eax, %%cr0       " :: );
}

/*
 * SetUpPaging:
 *     Mapping.
 * In this function:
 *     (Phase 1) Endere�os da mem�ria f�sicas acess�veis em Kernel Mode. 
 *     (Phase 2) Inicializando o diret�rio.
 *     (Phase 3) Cria tabelas de p�ginas e coloca o ponteiro de cada uma 
 * delas na sua respectiva entrada de diret�rio.
 *     (Phase 4) CRIANDO OUTROS DIRET�RIOS
 * @diretorio:
 *   boot_pd0 = 0x9C000.
 *   Page directory do kernel.
 * Obs: Esse diret�rio criado ser� usado pelas primeiros processos durante
 * essa fase de constru��o do sistema. O ideal � um diret�rio por processo.
 *      Toda vez que o Kernel inicia a execu��o de um processo, ele deve 
 * carregar o endere�o do diret�rio do processo em CR3. Por enquanto s� tem 
 * um diret�rio criado e todos os processos est�o definindo o diret�rio do
 * Kernel como sendo seu pr�prio. Ou seja, eles podem usar as mesmas tabelas
 * de p�ginas que o processo kernel usa. O problema � que com isso os 
 * processos precisam ter endere�o virtual diferentes. Quando cada processo
 * tiver seu pr�prio diret�rio ent�o eles poder�o ter o mesmo endere�o virtual.
 * @todo: Esses endere�os precisam ser registrados em vari�veis globais ou
 * dentro de uma estrutura para serem passados para o Kernel.
 * Obs: Essa deve ser uma interface que chama as rotinas de configura��o de 
 * pagina��o. (Mapeamento). 
 * Obs: 
 * Essa configura��o inicial feita no Boot Loader, n�o impede o Kernel de 
 * refazer as configura��es b�sicas de pagina��o. O fato � que o kernel ser� o 
 * gerenciador de p�ginas de mem�ria. O que n�o impede de haver 
 * gerenciamento um user mode.
 * History:
 *     2015 - Created by Fred Nora.
 */
// Called by bl_main in main.c

void SetUpPaging(void)
{
// 64bit.
// + Setup pages.
// + Transfer execution to the kernel.
// This routine does not return.

    unsigned int i=0;

// 9 9 9 9 12
// Agora as tabelas possuem 512 entradas,
// pois � isso o que d� pra ter com apenas 9 bits.
// Agora o addr de cada tabela � de 52 bits, os outros 12 bits s�o reservados.
// Antes cada entrada era de 4 bytes, agora � de 8 bytes.
// Nesse caso a tabela continua do mesmo tamanho, mas agora com 512 entradas de 8

// (Phase 1) 
// Endere�os da mem�ria f�sicas acess�veis em Kernel Mode.

//
// Kernel.
//

// Address. para os 2 primeiros mega da mem�ria fisica.
// Base, 0x100000, para o kernel que come�a no primeiro mega.
    unsigned long kernel_address = 0;           //In�cio da mem�ria RAM.                
    unsigned long kernel_base = KERNEL_BASE;    //In�cio da imagem do kernel.    

//
// (Phase 2) 
// Endere�os da mem�ria f�sicas acess�veis em User Mode.
//

// User, 0x00400000.
    unsigned long user_address = USER_BASE;
// VGA, F�sico=0x000B8000.
    unsigned long vga_address = VM_BASE;   
// VESA LFB, foi passado pelo Boot Manager.
    unsigned long lfb_address  = (unsigned long) g_lbf_pa;
// backbuffer: endere�o f�sico provis�rio para o backbuffer
// BUFFER, provis�rio. @todo: Mudar. 
// (para sistemas com pouca mem�ria.)
// 16mb. (16*1024*1024) = 0x01000000.
    unsigned long buff_address  = (unsigned long) 0x01000000; 

// DIRECTORY:
// Diret�rios de p�ginas.
// Esse valor � salvo em cr3. Cada diret�rio tem seu endere�o.
// Esse diret�rio, configurado aqui no Boot Loader � o mesmo usado 
// pelo Kernel.
// Obs: 
// A id�ia � que os diret�rio sejam criados em ordem decrescente.
// Os espa�os entre os diret�rio ser�o preenchidos com p�ginas 
// avulsas. Depois, no kernel, haver� �rea de aloca��o para 
// diret�rios e p�ginas.
// Teste:
// Criando diret�rios para os primeiros processos do sistema. 
// (antes de 32MB).
// Obs: Essa �rea onde est�o os diret�rios � uma �rea desprotegiada ainda.
// Obs: O kernel est� reconfigurando e usando outro endere�o para o 
// diret�rio de p�ginas.
// @todo: Alertar o kernel sobre esses endere�os de diret�rios.


// =====================================
// PML4, PDPT, PD, PT
// PML4 - Page Map Level 4
// PDPT - Page Directory Pointer Table
// PD   - Page Directory
// PT   - Page Table

    // level 4
    unsigned long *boot_pml4  = (unsigned long *) (0x01000000 - 0x00700000);
    // level 3
    unsigned long *boot_pdpt  = (unsigned long *) (0x01000000 - 0x00800000);
    // level 2
    unsigned long *boot_pd0   = (unsigned long *) (0x01000000 - 0x00900000);
    // level 1
    // a lot of page tables in the level 1.

// TABLES: 
// Tabelas de p�ginas.
// Uma tabela para cada �rea de mem�ria que se deseja usar.
// Usando endere�os decrescentes.
// ptKM  - First 2MB.
// ptKM2 - The kernel image.
// ptUM  - User mode area for application. (Not used yet)
// ptVGA - Legacy VGA memory.
// ptLFB - Linear Frame Buffer. (Front buffer)
// ptBACKBUFFER - The backbuffer.

    unsigned long *ptKM  = (unsigned long *) 0x0008F000;
    unsigned long *ptKM2 = (unsigned long *) 0x0008E000;
    unsigned long *ptUM  = (unsigned long *) 0x0008D000;
    unsigned long *ptVGA = (unsigned long *) 0x0008C000;
    unsigned long *ptLFB = (unsigned long *) 0x0008B000;
    unsigned long *ptBACKBUFFER = (unsigned long *) 0x0008A000;
    // ...

//
// ==================================================
//

// Clear levels 4, 3 and 2.

// Clear level 4
    for ( i=0; i < 512; i++ )
    {
        boot_pml4[i*2   ] = (unsigned long) 0 | 2;
        boot_pml4[i*2+1 ] = (unsigned long) 0;
    };

// Clear level 3
    for ( i=0; i < 512; i++ )
    {
        boot_pdpt[i*2   ] = (unsigned long) 0 | 2;
        boot_pdpt[i*2+1 ] = (unsigned long) 0;
    };

// Clear level 2
    for ( i=0; i < 512; i++ )
    {
        boot_pd0[i*2   ] = (unsigned long) 0 | 2;
        boot_pd0[i*2+1 ] = (unsigned long) 0;
    };

//
// ============================
//

// Pointing the 'page directory' address 
// at the first entry in the 'page directory pointer table'.

    boot_pdpt[0*2]   = (unsigned long) &boot_pd0[0];
    boot_pdpt[0*2]   = (unsigned long) boot_pdpt[0*2] | 3;
    boot_pdpt[0*2+1] = 0; 

// Pointing the 'page directory pointer table' address 
// at the first entry in the 'boot_pml4'.

    boot_pml4[0*2]   = (unsigned long) &boot_pdpt[0];
    boot_pml4[0*2]   = (unsigned long) boot_pml4[0*2] | 3; 
    boot_pml4[0*2+1] = 0; 

//
// ============================
//

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
        ptKM2[i*2]   = kernel_base | 3;    //0011 bin�rio.
        ptKM2[i*2+1] = 0;
        kernel_base  = kernel_base + 4096;       //4KB.
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
        ptLFB[i*2]   = lfb_address | 7;    //0111 em bin�rio.
        ptLFB[i*2+1] = 0;
        lfb_address  = lfb_address + 4096;       //4KB.
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

    //#debug
    //printf ("SetUpPaging: enable_pae\n");
    //refresh_screen();

    enable_pae();

    //refresh_screen();
    //while(1){}

//
// CR3
//

// Point cr3 to boot_pml4.
    
    //#debug
    //printf ("SetUpPaging: load_pml4_table\n");
    //refresh_screen();

    unsigned long pml4_address = (unsigned long) &boot_pml4[0];

    load_pml4_table( (unsigned long) pml4_address );

//
// MSR
//

    //#debug
    //printf ("SetUpPaging: cpuSetMSR\n");
    //refresh_screen();

// #todo
// long mode exige uma configura��o usando msr.

    //int msrStatus = FALSE;
    //msrStatus = cpuHasMSR();

    //IN: MSR, LO, HI
    cpuSetMSR( 0xC0000080, 0x100, 0 );

// Nesse caso habilitamos SYSCALL/SYSRET
// #ok: Testado no qemu
    //cpuSetMSR( 0xC0000080, 0x101, 0 );

    //refresh_screen();
    //while(1){}

// Enable paging.
    //printf ("SetUpPaging: page_enable\n");
    //refresh_screen();

// #important:
// Adiamos isso, o assembly vai fazer isso.
// Pois no momento em que configurarmos isso, o long mode
// estar� habilitado.
    // page_enable();

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

// Go to kernel
// See: transfer.inc

    BlTransferToKernel();

// Not reached
Hang:
    while (1){
        asm ("cli");
        asm ("hlt");
    };
}

//
// End
//

