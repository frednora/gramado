// search.h
// Created by Fred Nora

#ifndef __FS_SEARCH_H
#define __FS_SEARCH_H    1

//
// == Search path ================================
//

// See:
// https://en.wikipedia.org/wiki/PATH_(variable)
// ...
// #importante
// + Quando uma função para eecutar uma programa é chamada
// o kernel deve tomar suas decisões sobre qual diretório
// procurar. Não é seguro obedecer o search path indicado pelo
// processo que chamou a função.
// Então o kernel pode começar procurando em /BIN/ e /USER/BIN/.
// O kernel só vai procurar no cwd se o pathname desejado começar
// explicitamente com "./". 
// Multics originated the idea of a search path. 
// The early Unix shell only looked for program names in /bin, 
// but by Version 3 Unix the directory was too large and /usr/bin, 
// and a search path, became part of the operating system.

extern unsigned long search_path_dir_address;
extern unsigned long search_path_dir_entries;


// List of clusters. 
// Usado na rotina de carregamento de arquivo.
// #bugbug
//#define MAX_CLUSTERS   1024   // The limit is a 512KB file
#define MAX_CLUSTERS     2048   // The limit is a 1MB file
// ...

extern unsigned short file_cluster_list[MAX_CLUSTERS]; 

// =================================================

int 
search_in_dir ( 
    const char *file_name, 
    unsigned long dir_address );

int search_in_root ( const char *file_name );
unsigned short fs_find_n_empty_entries(int n);


int 
findEmptyDirectoryEntry ( 
    unsigned long dir_address, 
    int number_of_entries );

int fsSearchFileInRoot(const char *file_name);
unsigned short fs_find_empty_entry_in_fat(char *fat_address);
int search_path_in_the_inode_table(const char *path);


#endif   

