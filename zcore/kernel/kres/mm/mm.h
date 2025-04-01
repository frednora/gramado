
// mm/mm.h
// Memory manager
// Created by Fred Nora.

#ifndef __MM_MM_H
#define __MM_MM_H    1

// Aligns a value on a boundary.
//#define ALIGN(x, a)    (((x) + ((a) - 1)) & ~((a) - 1))

// =========================

int mmInitialize(int phase);

#endif    


