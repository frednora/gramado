// mm/aspace.h
// Address space support.
// Created by Fred Nora.

#ifndef __MM_ASPACE_H
#define __MM_ASPACE_H  1

// aspace types
typedef enum {
    aspacetypeShared,
    aspacetypePersonal,
}aspace_type_t;

// aspace_d
// This structure can be used to represent any region of physical memory. 
// We have another structure for blocks of memory called mm_block, but that 
// memory is used only to represent memory blocks inside the Kernel's heap.
struct aspace_d
{
	object_type_t objectType;
	object_class_t objectClass;
	int used;
	int magic;

	aspace_type_t type;
	
	char *name;  //Qual é o nome desse Address Apace.
	//file *_file;   //Qual arquivo é referente à esse espaço.
	//metafile??

	unsigned long virtual_address;
	unsigned long physical_address;
	unsigned long size;  //Tamanho em bytes.
	
	//...
	
	// @todo: Informações sobre os processos que estão utilizando
	//        essa área de memória.
	
	//Lista de processos utilizando esse objeto.
	struct process_d *processListHead;
	//int processCount;
	
	// propriedades da região.
	// permissões de leitura e escrita.
	//informações sobre todos os arquivos associados com a região.

	// Deve-se encadear as regições de memória em um árvore,
	// de forma a facilitar o acesso.
	// Obs: Acho que a estrutura de contas em bancos
    // cumpre esse papel.	

	//?? @questionamento: Uma conta poderia pertencer à mais de um banco
	// ou devemos indicar aqui à que banco essa conta pertence.
	//struct bank_d *bank;
	
	//navegação.
    struct aspace_d *next;
};

#endif   

