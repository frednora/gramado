// ibroker.h

#ifndef __GDI_IBROKER_H
#define __GDI_IBROKER_H    1


int ksys_shell_parse_cmdline(char *cmdline_address, size_t buffer_size);

// Process input
unsigned long 
ksys_console_process_keyboard_input ( 
    int msg, 
    unsigned long long1, 
    unsigned long long2 );

int input_set_input_targets(int stdin_target, int queue_target);

int input_process_cad_combination(unsigned long flags);

void input_enter_kernel_console(void);
void input_exit_kernel_console(void);

//
// Input events:
//

int 
wmRawKeyEvent(
    unsigned char raw_byte,
    int prefix );

int wmMouseEvent(int event_id,long long1, long long2);
int wmKeyboardEvent(int event_id,long long1, long long2);
int wmTimerEvent(int signature);


#endif 


