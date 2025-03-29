// mm/mmblock.h
// Blocks
// Created by Fred Nora.

#ifndef __MM_MMBLOCK_H
#define __MM_MMBLOCK_H    1

// Number of indexes into the list.
// see: mmblockList[i]
#define MMBLOCK_COUNT_MAX  (2*4096)


 /*
 * @todo: 
 * Os blocos precisam de alguma organiza��o. 
 * Por enquanto, o total � 256 heaps de tamanhos diferentes.
 * Os blocos formar�o uma lista encadeada que ser� percorrida para se 
 * encontrar um bloco vazio. (walk).
 * Importante: A mem�ria ser� organizada em bancos, que conter�o mem�ria 
 * privada e mem�ria compartilhada. Os blocos de mem�ria e os heaps 
 * precisam estar associadoas aos bancos, que conter�o informa��es sobre 
 * processos e usu�rios. @todo:
 * Obs: Os bancos est�o definidos em bank.h
 * Obs: Um heap de processo tem v�rios blocos dentro.
 * IMPORTANTE: 
 *     Talvez tenhamos algum limite para o tamanho dessa estrutura 
 * em especial. (N�o incluir nenhuma vari�vel por enquanto!).
 */ 

// Essa estrutura é para gerenciar áreas de memória 
// alocadas dinamicamente dentro do heap do processo kernel. 
// >>> Don't change this structure.
// It has a fixed size.

/*
 * mmblock_d:
 * This strcture is used to represent blocks of memory only 
 * inside the Kernel's heap.
 * We also have another structure used to represent blocks of 
 * physical memory called aspace_d.
 * The blocks of memory inside the heap are allocated dinamicaly.
 */

struct mmblock_d 
{
// This is the structure to handle each allocation into the kernel heap.
// The structure has three major components:
// + The 'header base'. 
//     This is the base of the struture.
// + The 'user area base'.
//    This is the start of the allocated area.
// + The 'footer address'.
//    This is the end of the allocated area.

//
// Header
//

// Here goes the address of the start of the structure.
    unsigned long Header;
// Header size in bytes.
// This is the size of the structure.
    unsigned long headerSize;

//
// User area
//

// The address of the start of the user area.
    unsigned long userArea;
    unsigned long userareaSize;

//
// Footer
//

// The address of the end of the user area.
    unsigned long Footer;

// ----------------

// Block identification.
    int Id;

// Structure validation
    int Used; 
    int Magic;

// This block is free or not.
    int Free;

// process
    pid_t pid;

// thread
    tid_t tid;

// Navigation
    struct mmblock_d  *Next;

// #crazy
// Debug info
    //char name[8];
};
extern struct mmblock_d  *current_mmblock;

// Lista de blocos. 
// Lista de blocos de memória dentro de um heap.
// #todo: Na verdade temos que usar lista encadeada. 
// see: mm.c
extern unsigned long mmblockList[MMBLOCK_COUNT_MAX];  



#endif    


