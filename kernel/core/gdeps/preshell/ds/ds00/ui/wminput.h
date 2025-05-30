// wminput.h
// Input support.
// Created by Fred Nora.

#ifndef __UI_WMINPUT_H
#define __UI_WMINPUT_H    1

// #test (#bugbug)
#define MSG_MOUSE_DOUBLECLICKED   60
#define MSG_MOUSE_DRAG            61
#define MSG_MOUSE_DROP            62

// Key combination
int wmProcessCombinationEvent(int msg_code);
inline int is_combination(int msg_code);

// Process mouse events.
void 
wmProcessMouseEvent(
    int event_type, 
    unsigned long x, 
    unsigned long y );

// Process keyboard events.
unsigned long 
wmProcessKeyboardEvent(
    int msg,
    unsigned long long1,
    unsigned long long2 );

void wmProcessTimerEvent(unsigned long long1, unsigned long long2);

//
// Input reader
//

int wmInputReader(void);
int wmInputReader2(void);
int wmSTDINInputReader(void);

// Get input
int wminputGetAndProcessSystemEvents(void);

#endif    

