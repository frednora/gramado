// sync.h
// It's a place for multi-threading/multi-process synchronization.
// Created by Fred Nora.

#ifndef __INTAKE_SYNC_H
#define __INTAKE_SYNC_H    1

// Global spinlock.
// Global:
// #todo: It needs to be atomic.
// see: process.c
extern int __spinlock_ipc;
//...


#endif   


