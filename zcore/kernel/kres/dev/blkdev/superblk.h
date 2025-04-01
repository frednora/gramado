
// superblk.h
// Superblock support.

#ifndef __SUPERBLK_H
#define __SUPERBLK_H    1

// Superbloco de um disco.
// tera uma estrutura de volume;
struct superblock_d
{
    int id;
    int used;
    int magic;
    // ...
    struct superblock_d *next;
};

#endif    


