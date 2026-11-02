// sync.c
// It's a place for multi-threading/multi-process synchronization.
// Created by Fred Nora.

#include <kernel.h>

// Global spinlock.
// The process does not block, it spins in a loop.
// see: sync.h
int __spinlock_ipc=0;
//...



