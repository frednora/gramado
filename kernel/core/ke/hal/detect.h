// detect.h
// Detech hardware components for HAL layer.
// Created by Fred Nora.

#ifndef __KE_DETECT_H
#define __KE_DETECT_H    1


//
// syscall instruction support
//

extern int syscall_is_supported;

int check_syscall_support(void);
int probe_if_cpu_has_support_to_syscall(void);
void syscall_enable(void);
void syscall_set_entry_point(uint64_t ent);
void syscall_set_mask(uint32_t mask);
void initialize_syscall(void);

//
// Probe cpu type
//

int hal_probe_cpu(void);
int hal_probe_processor_type(void);

int hal_hardware_detect(void);

#endif    

