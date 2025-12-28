// ibroker.h
// Created by Fred Nora

#ifndef __EVI_IBROKER_H
#define __EVI_IBROKER_H    1


struct input_broker_info_d 
{
    int initialized;
    int shell_flag;
    // ...
};
extern struct input_broker_info_d  InputBrokerInfo;

// ==========================

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
    int prefix );

int wmMouseEvent(int event_id,long long1, long long2);
int wmKeyboardEvent(int event_id,long long1, long long2);
int wmTimerEvent(int signature);

int ibroker_initialize(void);

#endif 


