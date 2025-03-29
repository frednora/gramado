
// pints.h
// profiler, interrupts.
// Variáveis globais para contagem de interrupções.
// Created by Fred Nora.

#ifndef __KE_PINTS_H
#define __KE_PINTS_H    1

//
// System interrupt
//

//
// see: hal.c
//

//See:  gde_serv.c 
extern unsigned long g_profiler_ints_gde_services;


//
// Legacy hardware interrupts (irqs) (legacy pic)
//

extern unsigned long g_profiler_ints_irq0;  // kdrivers/timer.c
extern unsigned long g_profiler_ints_irq1;  // x/i8042/keyboard.c
extern unsigned long g_profiler_ints_irq2;  // cascade.
extern unsigned long g_profiler_ints_irq3;  //
extern unsigned long g_profiler_ints_irq4;  //
extern unsigned long g_profiler_ints_irq5;  //
extern unsigned long g_profiler_ints_irq6;  //
extern unsigned long g_profiler_ints_irq7;  //

extern unsigned long g_profiler_ints_irq8;   // kdrivers/rtc.c
extern unsigned long g_profiler_ints_irq9;   // kdrivers/network/nicintel.c
extern unsigned long g_profiler_ints_irq10;  //
extern unsigned long g_profiler_ints_irq11;  //
extern unsigned long g_profiler_ints_irq12;  // x/i8042/mouse.c
extern unsigned long g_profiler_ints_irq13;  //
extern unsigned long g_profiler_ints_irq14;  // kdrivers/kdrivers/ide/atairq.c
extern unsigned long g_profiler_ints_irq15;  // kdrivers/kdrivers/ide/atairq.c

// continua ...

#endif   


