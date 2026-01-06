// ibroker.h
// Created by Fred Nora

#ifndef __EVI_IBROKER_H
#define __EVI_IBROKER_H    1


struct keyboard_raw_data_d 
{
// Raw data
    unsigned char raw0;       // first raw byte (scancode or prefix)
    unsigned char raw1;       // second raw byte (optional: break marker 0xF0, or part of Pause)
    unsigned char raw2;       // third raw byte (optional: Pause/Break continuation)

// Facilitator
    unsigned char prefix;     // facilitator: 0x00, 0xE0, 0xE1 (real or injected)
};


struct input_broker_info_d 
{
    int initialized;
    int shell_flag;
    // ...
};
extern struct input_broker_info_d  InputBrokerInfo;

// ==========================

// Kernel-side syscall wrapper
unsigned long sys_change_boot_menu_option(unsigned long value);

void 
ibroker_set_keymap(
    unsigned char *p_normal,
    unsigned char *p_shift,
    unsigned char *p_ctrl,
    unsigned char *p_altgr,
    unsigned char *p_extended );

// Send ascii to the tty associated with stdin.
int ibroker_send_ascii_to_stdin(int ascii_code);

int ksys_shell_parse_cmdline(char *cmdline_address, size_t buffer_size);

// Process input
unsigned long 
ksys_console_process_keyboard_input ( 
    int msg, 
    unsigned long long1, 
    unsigned long long2 );

// Input targets
int input_enable_this_input_target(int this_one);
int input_disable_this_input_target(int this_one);


int input_process_cad_combination(unsigned long flags);

void input_enter_kernel_console(void);
void input_exit_kernel_console(void);

//
// Input events:
//

int 
wmRawKeyEvent( 
    unsigned char raw_byte_0,
    unsigned char raw_byte_1,
    unsigned char raw_byte_2,
    unsigned char raw_prefix );

int wmMouseEvent(int event_id,long long1, long long2);
int wmKeyboardEvent(int event_id,long long1, long long2);
int wmTimerEvent(int signature);

int ibroker_initialize(int phase);

#endif 


