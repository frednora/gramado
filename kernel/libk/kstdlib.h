// kstdlib.h
// Created by Fred Nora.

#ifndef  __LIBK_KSTDLIB_H
#define __LIBK_KSTDLIB_H    1

// A rotina terminou normalmente. 
#define EXIT_SUCCESS  0
// A rotina terminou de maneira anormal. 
#define EXIT_FAILURE  1

// Números pseudo-aleatórios   
#define RAND_MAX  32767    //0x7FFF

//
// Prototypes ===========================
//

void srand(unsigned int seed);
int rand(void);

int abs(int j);
int k_atoi(const char *str);

int __ptsname(int fd, char *buf, size_t buflen);

void *kmalloc(size_t size);
void kfree(void *ptr);
void *kcalloc(size_t count, size_t size);

#endif    

