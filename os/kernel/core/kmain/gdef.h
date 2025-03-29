/*
  * File: gdef.h
  *     Defini��es globais. 
  *     Ser� utilizado por todos os m�dulos. Deve ficar no in�cio.
  *     Quando uma defini��o deve ser acessada por todos os m�dulos
  *     do kernel base ela deve ficar aqui.
  */

/*
 Sobre o banco de dados:
 =======================
Tipos de contas:
    aspace e dspace, pessoais e conjuntas.
aspace:
    Um Address Space � uma conta de banco de dados em mem�ria. Uma conta 
pode ser do tipo conjunta(Shared) ou  pessoal(Personal). 
dspace:
    Um Disk Space � uma conta de banco de dados em disco. Uma conta pode 
ser do tipo conjunta(Shared) ou  pessoal(Personal). 
Donos das contas:
    Os processos s�o os clientes dos bancos de dados e s�o os donos das 
contas de bancos de dados.
Tipos de bancos:
    Kernel Data Base, Global Data Base e Local Data Base.
KDB:
    Banco de dados do tipo kernel. � o banco de maior privil�gio, poucos
processos poder�o usar esse banco e as contas presentes nele.
GDB:
    Bancos de dados do tipo global. � o banco das contas conjuntas, para
que os processos possam compartilhar as contas e os dados que elas acessam.
LDB:
    Bancos de dados do tipo pessoal. � o banco das contas pessoais. Um 
processo pode ser dono de uma conta dessas para proteger seus dados
e revogar qualquer tipo de acesso e somente ele ter� acesso aos dados
dessa conta.
*/

#ifndef __KE_GDEF_H
#define __KE_GDEF_H    1

//using gui flags.
#define GUI_ON   1
#define GUI_OFF  0

//layers
#define KERNEL      0
#define GRAMADO     1   //1 kgws
#define EXECUTIVE   2   //2
#define MICROKERNEL 3   //3
#define HAL         4   //4

// IOPL constants.
// Intel/AMD
#define KernelMode  0
#define UserMode   3
#define RING0  0
#define RING1  1
#define RING2  2
#define RING3  3


//#define LOBYTE(w) ((char)(((unsigned long )(w)) & 0xff))
//#define HIBYTE(w) ((char)((((unsigned long)(w)) >> 8) & 0xff))


// ===================================================

//keyboard suppport 
//abnt2 flag.
//#todo: Move this to another file.
//maybe gdef.h
extern int abnt2;
//...



/*
 * Os Type:
 *     O sistema operacional pode ser usado como servidor.
 * Radando em uma m�quina real e acessado por um terminal
 * via rede LAN, ou porta serial. TTY.
 *     Qualquer sistema operacional em uma m�quina cliente pode
 * acessar o sistema operacional na maquina servidora. Pra isso
 * tem que ter um programa na m�quina cliente que envie comandos 
 * para a m�quina servidora atrav�s da placa de rede ou da porta 
 * serial.
 *     O sistema pode atuar como cliente.
 */

typedef enum {
    OsTypeClient,
    OsTypeServer,
    OsTypeHeadlessServer,
    // ...
}os_type_t;


//
// The boot block structure.
//

/*
// #todo
// Actually the kmain.c is using another structure for this purpose.
struct boot_block_d
{
    unsigned long bootblock_address;  //pa ?? va ??
    unsigned long lfb;
    unsigned long x;
    unsigned long y;
    unsigned long bpp;
    
    unsigned long last_valid_address;
    unsigned long physical_memory_in_kb;

    unsigned long metafile_address;
    unsigned long disk_number;  // Gigen by the BIOS.
    unsigned long heads; 
    unsigned long spt; 
    unsigned long cylinders; 
    unsigned long boot_mode; 
    unsigned long gramado_mode;
    //...

    // #test
    // The IDE port number given by the 32bit boot loader.
    unsigned long ide_port_number;

    int initialized;
};
// see: globals.c
extern struct boot_block_d  BootBlock;
*/


// Regions
// see: globals.c
extern unsigned long g_ring0area_va;
extern unsigned long g_ring3area_va;
extern unsigned long g_kernelimage_va;
extern unsigned long g_frontbuffer_va;   
extern unsigned long g_backbuffer_va;
extern unsigned long g_pagedpool_va;  //pagedpool virtual address
extern unsigned long g_heappool_va;
extern unsigned long g_extraheap1_va;
extern unsigned long g_extraheap2_va;
extern unsigned long g_extraheap3_va;


// frontbuffer and backbuffer.
// see: video.c
extern unsigned long g_frontbuffer_pa; 
extern unsigned long g_backbuffer_pa;


// Endere�o virtual do pool de heaps.
// os heaps nessa �rea ser�o dados para os processos.
// see: globals.c
extern int g_heap_count;
extern int g_heap_count_max;
extern unsigned long g_heap_size;


// extra heap 1
extern unsigned long g_extraheap1_size;
extern int g_extraheap1_initialized;
// extra heap 2
extern unsigned long g_extraheap2_size;
extern int g_extraheap2_initialized;
// extra heap 3
extern unsigned long g_extraheap3_size;
extern int g_extraheap3_initialized;


// see: globals.c
extern int has_booted;

#endif   

//
// End
//

