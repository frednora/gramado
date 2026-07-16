// apictim.h
// APIC timer support.
// Created by Fred Nora.

#ifndef __HAL_APIC_TIM_H
#define __HAL_APIC_TIM_H    1

void apic_timer_setup_periodic(unsigned int vector, int masked, int lapic_info_id);

void apic_initial_count_timer(int value, int lapic_info_id);

void apic_timer_umasked(int lapic_info_id);
void apic_timer_masked(int lapic_info_id);

int apictim_initialize(int lapic_info_id);


#endif   


