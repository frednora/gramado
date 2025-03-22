// apictim.h
// APIC timer support.
// Created by Fred Nora.

#ifndef __HAL_APIC_TIM_H
#define __HAL_APIC_TIM_H    1

void apic_initial_count_timer(int value);
void apic_timer_umasked(void);
void apic_timer_masked(void);
int apic_timer(void);


#endif   


