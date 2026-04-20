// pints.h
// Profiler, interrupts.
// Variáveis globais para contagem de interrupções.
// Created by Fred Nora.

#ifndef __HAL_PINTS_H
#define __HAL_PINTS_H    1

//
// System interrupt
//

//
// see: hal.c
//

extern unsigned long g_profiler_ints_syscall_counter;

//
// Legacy hardware interrupts (irqs) (legacy pic)
//

extern unsigned long g_profiler_ints_irq0;  // timer.c
extern unsigned long g_profiler_ints_irq1;  // keyboard.c
extern unsigned long g_profiler_ints_irq2;  // cascade
extern unsigned long g_profiler_ints_irq3;  //
extern unsigned long g_profiler_ints_irq4;  //
extern unsigned long g_profiler_ints_irq5;  //
extern unsigned long g_profiler_ints_irq6;  //
extern unsigned long g_profiler_ints_irq7;  //

extern unsigned long g_profiler_ints_irq8;   // rtc.c
extern unsigned long g_profiler_ints_irq9;   // nicintel.c
extern unsigned long g_profiler_ints_irq10;  //
extern unsigned long g_profiler_ints_irq11;  //
extern unsigned long g_profiler_ints_irq12;  // mouse.c
extern unsigned long g_profiler_ints_irq13;  //
extern unsigned long g_profiler_ints_irq14;  // atairq.c
extern unsigned long g_profiler_ints_irq15;  // atairq.c

// ...

#endif   

