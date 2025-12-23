// window.h
// This header should be included by including "gws.h".
// Created by Fred Nora.

#ifndef  __LIBGWS_WINDOW_H
#define  __LIBGWS_WINDOW_H    1

//
// Window Flags
//

// >> Style: design-time identity. (unsigned long)
// Defines window type and decorations/features.

// >> Status: interaction/activation. (int)
// Indicates focus, active/inactive, and user engagement.

// >> State: runtime condition. (int)
// Tracks current behavior (minimized, maximized, fullscreen, etc).


// ==============================================================
// >> Style: design-time identity. (unsigned long)
// Defines window type and decorations/features.

#define WS_NULL  0x0000

//----------------------
// Style (design-time components)
//----------------------

// #todo: Reserved values
#define WS_RES1    0x0001
#define WS_RES2    0x0002
#define WS_RES3    0x0004
#define WS_RES4    0x0008

//----------------------
// Style (design-time components)
//----------------------
#define WS_TITLEBAR            0x0100
#define WS_TITLEBARICON        0x0200
#define WS_TRANSPARENT         0x0400
#define WS_STATUSBAR           0x0800  // Bottom bar
#define WS_HSCROLLBAR          0x1000
#define WS_VSCROLLBAR          0x2000
#define WS_RESERVED00          0x4000
#define WS_CLIP_IN_CLIENTAREA  0x8000

//----------------------
// Style (design-time components)
// Role / semantic identity (misplaced in WS_)
//----------------------
#define WS_APP        0x10000   // wrong: should be window type
#define WS_DIALOG     0x20000   // wrong: should be window type
#define WS_TERMINAL   0x40000   // wrong: should be window type
#define WS_TASKBAR    0x80000   // wrong: should be window type
#define WS_CHILD     0x100000   // really a type, not a style
// ...

// ==============================================================
// >> Status: interaction/activation. (int)
// Indicates focus, active/inactive, and user engagement.
#define WINDOW_STATUS_ACTIVE       1
#define WINDOW_STATUS_INACTIVE     0

// ==============================================================
// >> State: runtime condition. (int)
// Tracks current behavior (minimized, maximized, fullscreen, etc).
#define WINDOW_STATE_NULL       0
#define WINDOW_STATE_FULL       1000
#define WINDOW_STATE_MAXIMIZED  1001
#define WINDOW_STATE_MINIMIZED  1002
#define WINDOW_STATE_NORMAL     1003  //Normal (restaurada)

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

