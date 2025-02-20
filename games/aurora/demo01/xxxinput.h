

#ifndef XXXINPUT_H
#define XXXINPUT_H    1


//
// == prototypes ===================================
//

void update_mouse (void);
void parse_data_packet ( char data, char x, char y);
int top_at ( int x, int y );
int mouse_scan_windows (void);
void input_dummy(void);


#endif    


