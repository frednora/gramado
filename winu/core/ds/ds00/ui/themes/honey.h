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


// Controls
#define HONEY_COLOR_BUTTON  0x00d4d0c8 

// Button states
#define HONEY_COLOR_BUTTON_DEFAULT   HONEY_COLOR_BUTTON
#define HONEY_COLOR_BUTTON_HOVER     HONEY_COLOR_BG_ONMOUSEHOVER
#define HONEY_COLOR_BUTTON_PRESSED   0x005A8AC6   // vibrant blue (matches active border)
#define HONEY_COLOR_BUTTON_DISABLED  0x00A0A0A0   // muted gray
#define HONEY_COLOR_BUTTON_FOCUS     COLOR_BLUE   // focus highlight
#define HONEY_COLOR_BUTTON_PROGRESS  0x00808080   // neutral gray (progress indicator)

// Light theme (Honey)
#define HONEY_COLOR_LABEL_BASELINE_LIGHT   0x00000000   // black text, high contrast on light button
#define HONEY_COLOR_LABEL_SELECTED_LIGHT  0x008B4513  // saddle brown

// Dark theme (Honey)
#define HONEY_COLOR_LABEL_BASELINE_DARK    0x00FFFFFF   // white text, readable on dark background
#define HONEY_COLOR_LABEL_SELECTED_DARK    0x00A9CCE3   // soft sky blue highlight (matches active titlebar)

// Button with focus
#define HONEY_COLOR_BUTTON_FOCUS_BG     0x00A9CCE3   // soft sky blue (matches active titlebar)
#define HONEY_COLOR_BUTTON_FOCUS_BORDER 0x005A8AC6   // vibrant blue border

// Progress state (button is busy/working)
#define HONEY_COLOR_BUTTON_PROGRESS_BG        0x00808080   // medium neutral gray background
#define HONEY_COLOR_BUTTON_PROGRESS_BORDER 0x00808080   // same neutral gray border
#define HONEY_COLOR_BUTTON_PROGRESS_BORDER_1  0x00A0A0A0   // lighter gray top/left
#define HONEY_COLOR_BUTTON_PROGRESS_BORDER_2  0x00404040   // darker gray bottom/right
#define HONEY_COLOR_BUTTON_PROGRESS_BORDER_LIGHT 0x00C0C0C0 // soft highlight
#define HONEY_COLOR_BUTTON_PROGRESS_BORDER_OUTER 0x00606060 // outer neutral gray


#define HONEY_COLOR_BUTTON_FOCUS_BORDER_1  0x007AA9E6  // light sky blue
#define HONEY_COLOR_BUTTON_FOCUS_BORDER_2_light 0x007AA9E6
#define HONEY_COLOR_BUTTON_FOCUS_BORDER_2 COLOR_BLUE

//#define HONEY_COLOR_BUTTON_BORDER_OUTER_XPSTYLE  0x00316AC5  // XP marine blue
//#define HONEY_COLOR_BUTTON_BORDER_OUTER_XPSTYLE  0x002A5CAA  // deep marine blue
#define HONEY_COLOR_BUTTON_BORDER_OUTER_XPSTYLE  0x00191970  // Midnight Blue



// Button border colors using tl/br notation
// tl_2 = top/left inner highlight
// tl_1 = top/left most inner highlight
// br_2 = bottom/right inner shadow
// br_1 = bottom/right most inner highlight
// outer = outer frame

// Default state (raised button)
// The same as BS_RELEASED
#define HONEY_COLOR_BUTTON_DEFAULT_TL2     COLOR_WHITE
#define HONEY_COLOR_BUTTON_DEFAULT_TL1     COLOR_WHITE
#define HONEY_COLOR_BUTTON_DEFAULT_BR2     xCOLOR_GRAY1
#define HONEY_COLOR_BUTTON_DEFAULT_BR1     xCOLOR_GRAY5
#define HONEY_COLOR_BUTTON_DEFAULT_OUTER   COLOR_BLACK

// Hover state
#define HONEY_COLOR_BUTTON_HOVER_TL2       COLOR_WHITE
#define HONEY_COLOR_BUTTON_HOVER_TL1       COLOR_WHITE
#define HONEY_COLOR_BUTTON_HOVER_BR2       xCOLOR_GRAY1
#define HONEY_COLOR_BUTTON_HOVER_BR1       xCOLOR_GRAY5
//#define HONEY_COLOR_BUTTON_HOVER_OUTER     COLOR_BLACK
#define HONEY_COLOR_BUTTON_HOVER_OUTER     0x00191970

// Pressed state (sunken look)
#define HONEY_COLOR_BUTTON_PRESSED_TL2     xCOLOR_GRAY2
#define HONEY_COLOR_BUTTON_PRESSED_TL1     xCOLOR_GRAY2
#define HONEY_COLOR_BUTTON_PRESSED_BR2     COLOR_WHITE
#define HONEY_COLOR_BUTTON_PRESSED_BR1     xCOLOR_GRAY5
#define HONEY_COLOR_BUTTON_PRESSED_OUTER   COLOR_BLACK

// Disabled state
#define HONEY_COLOR_BUTTON_DISABLED_TL2    COLOR_GRAY
#define HONEY_COLOR_BUTTON_DISABLED_TL1    COLOR_GRAY
#define HONEY_COLOR_BUTTON_DISABLED_BR2    COLOR_GRAY
#define HONEY_COLOR_BUTTON_DISABLED_BR1    xCOLOR_GRAY5
#define HONEY_COLOR_BUTTON_DISABLED_OUTER  xCOLOR_GRAY2

// Focus state (keyboard focus / first responder)
// Focused button (shine effect)
#define HONEY_COLOR_BUTTON_FOCUS_TL2       0x005A8AC6 
#define HONEY_COLOR_BUTTON_FOCUS_TL1       0x005A8AC6 
#define HONEY_COLOR_BUTTON_FOCUS_BR2       0x005A8AC6 
#define HONEY_COLOR_BUTTON_FOCUS_BR1       xCOLOR_GRAY5 
#define HONEY_COLOR_BUTTON_FOCUS_OUTER     0x00191970 


// Progress state (busy/working)
#define HONEY_COLOR_BUTTON_PROGRESS_TL2    HONEY_COLOR_BUTTON_PROGRESS_BORDER_1   // lighter gray
#define HONEY_COLOR_BUTTON_PROGRESS_TL1    HONEY_COLOR_BUTTON_PROGRESS_BORDER_LIGHT // soft highlight
#define HONEY_COLOR_BUTTON_PROGRESS_BR2    HONEY_COLOR_BUTTON_PROGRESS_BORDER_2   // darker gray
#define HONEY_COLOR_BUTTON_PROGRESS_BR1    HONEY_COLOR_BUTTON_PROGRESS_BORDER_LIGHT // soft highlight
#define HONEY_COLOR_BUTTON_PROGRESS_OUTER  HONEY_COLOR_BUTTON_PROGRESS_BORDER_OUTER


#endif   

