// window.h
// This header should be included by including "gws.h".
// Created by Fred Nora.

#ifndef  __LIBGWS_WINDOW_H
#define  __LIBGWS_WINDOW_H    1

//
// Window Style
//

#define WS_MAXIMIZED    0x0001
#define WS_MINIMIZED    0x0002  // Iconic
#define WS_FULLSCREEN   0x0004
#define WS_STATUSBAR    0x0008  // In the bottom

#define WS_LOCKED              0x0010  // No input
// 0x0020
#define WS_CLIP_IN_CLIENTAREA  0x0040
// 0x0080

#define WS_TITLEBAR      0x0100
#define WS_TITLEBARICON  0x0200
#define WS_TRANSPARENT   0x0400
// 0x0800

#define WS_HSCROLLBAR   0x1000
#define WS_VSCROLLBAR   0x2000
#define WS_CHILD        0x4000
// 0x8000

#define WS_APP       0x10000
#define WS_DIALOG    0x20000
#define WS_TERMINAL  0x40000
#define WS_TASKBAR   0x80000


//----------
// window status
#define WINDOW_STATUS_ACTIVE       1
#define WINDOW_STATUS_INACTIVE     0

//----------

// #test w->view
#define VIEW_NULL      0
#define VIEW_FULL      1000
#define VIEW_MAXIMIZED 1001
#define VIEW_MINIMIZED 1002
#define VIEW_NORMAL    1003 //Normal (restaurada)
//...

// Button styles (int)
#define BSTYLE_3D  0
#define BSTYLE_FLAT  1
// ...

// Button states (int)
#define BS_NULL      0 
#define BS_DEFAULT   1
#define BS_RELEASED  1
#define BS_FOCUS     2
#define BS_PRESS     3
#define BS_PRESSED   3
#define BS_HOVER     4
#define BS_DISABLED  5
#define BS_PROGRESS  6
// ...

// The window manager needs to get some information
// about the window.

struct gws_window_info_d
{
    int used;
    int magic;
    int wid;   // The window id.
    int pwid;  // The wid of the parent.
    int type;
// Relative to the parent.
    unsigned long left;
    unsigned long top;
    unsigned long width;
    unsigned long height;
// Relative to the parent.
    unsigned long right;
    unsigned long bottom;
// Client rectangle.
// Se a janela tem um client rect, 
// so we can save the values here.
// This when querying the window's info
// we also get all the client area info.
    unsigned long cr_left;
    unsigned long cr_top;
    unsigned long cr_width;
    unsigned long cr_height;
    unsigned long border_width;
    // ...
};

#endif    


//
// End
//

