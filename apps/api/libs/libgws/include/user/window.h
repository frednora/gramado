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

//----------------------
// Fundamental characteristics (lowest block)
//----------------------
// Universal traits that apply to any window.
#define WS_VISIBLE     0x0001   // window is visible
#define WS_ENABLED     0x0002   // window can receive input
#define WS_FOCUSABLE   0x0004   // window can take focus
#define WS_BORDERLESS  0x0008   // window has no border/frame

//----------------------
// Characteristics (design-time traits)
//----------------------
// Behavioral traits that define how the window interacts or renders.
#define WS_TRANSPARENT         0x0100   // window background transparency
#define WS_CLIP_IN_CLIENTAREA  0x0200   // clip drawing inside client rect
#define WS_RESIZABLE           0x0400   // user can resize window
#define WS_MOVABLE             0x0800   // user can drag/move window
#define WS_FOCUSABLE           0x1000   // can receive focus
#define WS_MODAL               0x2000   // blocks interaction with parent
#define WS_TOPMOST             0x4000   // stays above other windows
#define WS_FULLSCREENABLE      0x8000   // can toggle fullscreen


//----------------------
// Role / semantic identity (misplaced in WS_)
//----------------------
// Defines the semantic type of the window
#define WS_APP       0x10000  // Marks the main/root window of an app
#define WS_CHILD     0x20000  // Marks child windows (must have parent)
#define WS_DIALOG    0x40000  // Dialog windows (special popup style)
#define WS_TERMINAL  0x80000  // Terminal windows (special role)


// #define WS_res     0x100000
// #define WS_res     0x200000
// #define WS_res     0x400000
// #define WS_res     0x800000

// ----------------------
// Style (design-time components)
// ----------------------

// Flags for menu containers and items.

// Menu system
#define WS_MENU       0x1000000   // window is a menu container
#define WS_MENUITEM   0x2000000   // window is a menu item (child of a menu)
//#define WS_  0x4000000  // res
//#define WS_  0x8000000  // res

#define WS_MENUITEM_CHECKBOX  0x10000000
#define WS_MENUITEM_RADIO     0x20000000
//#define WS_  0x40000000  // res
//#define WS_  0x80000000  // res

// Bars (structural components)
#define WS_TITLEBAR       0x10000000   // Top bar with title + controls
#define WS_MENUBAR        0x20000000   // Horizontal bar with menus
#define WS_TOOLBAR        0x40000000   // Quick-access buttons/icons
#define WS_BArRes00_NEW   0x80000000 
#define WS_STATUSBAR    0x100000000  // Bottom info bar
#define WS_TASKBAR      0x200000000  // System-level bar at screen bottom
#define WS_HSCROLLBAR   0x400000000  // 
#define WS_VSCROLLBAR   0x800000000  // 

// Icons (contexts)
// Flags for icon placement contexts.
#define WS_DESKTOPICON  0x1000000000   // desktop-level icon
#define WS_BARICON      0x2000000000   // icons inside bars
#define WS_TRAYICON     0x4000000000   // system tray/notification icons
#define WS_BUTTONICON   0x8000000000   // icons inside buttons/controls

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

