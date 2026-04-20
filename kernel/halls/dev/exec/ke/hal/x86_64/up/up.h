/*
 * File: up.h 
 *    Uni-processor (UP) support.
 */

// See:
// https://en.wikipedia.org/wiki/Uniprocessor_system
// ...

#ifndef __KE_UP_H
#define __KE_UP_H    1

// Obs: 
// Não tem ponteiro. Apenas para UniProcessor.
// #bugbug: up.h is only for Intel.
// See: cpu.h and hal.c 
extern struct UPProcessorBlock_d  UPProcessorBlock;

#endif   

