// ke.h
// Created by Fred Nora.

#ifndef __KE_KE_H
#define __KE_KE_H    1

void keDie(void);
void keSoftDie(void);
int keReboot(void);

unsigned long keGetSystemMetrics(int index);

int keIsQemu(void);

int keCloseInitProcess(void);

// Called by main to execute the first process
int ke_x64ExecuteInitialProcess(int cpu_id);

int keInitialize(int phase);

#endif  

