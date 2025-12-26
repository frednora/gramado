// honey.h
// Color theme for the color scheme.
// Created by Fred Nora.

#ifndef __HONEY_H
#define __HONEY_H    1

// Background
#define HONEY_COLOR_BACKGROUND                0x00505050 
#define HONEY_COLOR_WINDOW_BACKGROUND         COLOR_GRAY  //0x00202020 

// Window
#define HONEY_COLOR_WINDOW                    0x00d4d0c8
#define HONEY_COLOR_WINDOW_BORDER             COLOR_INACTIVEBORDER
#define HONEY_COLOR_WWF_BORDER                COLOR_BLUE
#define HONEY_COLOR_ACTIVE_WINDOW_BORDER   0x005A8AC6   // vibrant Win95 blue
#define HONEY_COLOR_INACTIVE_WINDOW_BORDER 0x00A0A0A0   // muted gray

// Borders (beveled effect)
// Active window: bright highlight + deep shadow
#define HONEY_COLOR_BORDER_LIGHT_ACTIVE    0x00F0F0F0   // almost white (top/left)
#define HONEY_COLOR_BORDER_DARK_ACTIVE     0x00101010   // almost black (bottom/right)

// Inactive window: softer highlight + muted shadow
#define HONEY_COLOR_BORDER_LIGHT_INACTIVE  0x00D0D0D0   // light gray (top/left)
#define HONEY_COLOR_BORDER_DARK_INACTIVE   0x00303030   // charcoal gray (bottom/right)

// #test
//#define HONEY_COLOR_BORDER_LIGHT_WWF     COLOR_YELLOW   // silver highlight (top/left)
//#define HONEY_COLOR_BORDER_DARK_WWF      COLOR_RED   // deep gray shadow (bottom/right)

// WWF (keyboard owner, but not active)
#define HONEY_COLOR_BORDER_LIGHT_WWF     0x00F4F4F4  //0x00E8E8E8   // silver highlight (top/left)
#define HONEY_COLOR_BORDER_DARK_WWF      0x00101010  //0x00484848   // deep gray shadow (bottom/right)

// Inactive (no focus, not WWF)
#define HONEY_COLOR_BORDER_LIGHT_NOFOCUS 0x00A4A4A4   // muted light gray (top/left)
#define HONEY_COLOR_BORDER_DARK_NOFOCUS  0x00707070   // dark charcoal (bottom/right)


// Controls
#define HONEY_COLOR_BUTTON  0x00d4d0c8 

// Box
#define HONEY_COLOR_MESSAGEBOX  0x00404040 
#define HONEY_COLOR_DIALOGBOX   0x00404040 

// Bars
#define HONEY_COLOR_MENUBAR    0x00808080 
#define HONEY_COLOR_SCROLLBAR  0x00FFF5EE 
#define HONEY_COLOR_STATUSBAR  0x0083FCFF
#define HONEY_COLOR_TASKBAR    0x00C3C3C3 

// Titlebar
#define HONEY_COLOR_ACTIVE_WINDOW_TITLEBAR  0x00A9CCE3  //COLOR_BLUE1
#define HONEY_COLOR_INACTIVE_WINDOW_TITLEBAR   0x00606060 
#define HONEY_COLOR_TITLEBAR_TEXT  COLOR_BLACK

// Fonts
#define HONEY_COLOR_SYSTEMFONT      0x00000000 
#define HONEY_COLOR_TERMINALFONT    0x00FFFFFF 

// When mouse hover
#define HONEY_COLOR_BG_ONMOUSEHOVER             xCOLOR_GRAY3
#define HONEY_COLOR_BG_ONMOUSEHOVER_MIN_CONTROL 0x0073A9C2   // steel blue
#define HONEY_COLOR_BG_ONMOUSEHOVER_MAX_CONTROL 0x0073A9C2   // steel blue
#define HONEY_COLOR_BG_ONMOUSEHOVER_CLO_CONTROL 0x00B22222   // firebrick red
#define HONEY_COLOR_BG_ONMOUSEHOVER_MENUITEM    0x00FFD580   // warm honey gold

#endif   

