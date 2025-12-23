// honey.h
// Color theme for the color scheme.
// Created by Fred Nora.

#ifndef __HONEY_H
#define __HONEY_H    1


// Backgrounds (global surfaces)
#define HONEY_COLOR_BACKGROUND                 0x00505050 
#define HONEY_COLOR_WINDOW_BACKGROUND          COLOR_GRAY
#define HONEY_COLOR_MESSAGEBOX                 0x00404040 
#define HONEY_COLOR_DIALOGBOX                  0x00404040 
// #define HONEY_COLOR_EDITBOX                    WHITE

// Window Elements (frames & borders)
#define HONEY_COLOR_WINDOW                     0x00d4d0c8
#define HONEY_COLOR_WINDOW_BORDER              COLOR_INACTIVEBORDER
#define HONEY_COLOR_WWF_BORDER                 COLOR_BLUE
#define HONEY_COLOR_ACTIVE_WINDOW_BORDER       0x006e2209
#define HONEY_COLOR_INACTIVE_WINDOW_BORDER     0x00808080
#define HONEY_COLOR_SHADOW                     0x00303030   // dark gray shadow
#define HONEY_COLOR_RESIZE_HANDLE              0x00999999   // medium gray for grips

// Bars (UI components)
//#define HONEY_COLOR_TITLEBAR                   0x00808080  //# check the one we are already using
#define HONEY_COLOR_MENUBAR                    0x00808080 
#define HONEY_COLOR_TOOLBAR                    0x00C3C3C3 
#define HONEY_COLOR_SCROLLBAR                  0x00FFF5EE 
#define HONEY_COLOR_STATUSBAR                  0x0083FCFF
#define HONEY_COLOR_TASKBAR                    0x00C3C3C3 

// Titlebars
#define HONEY_COLOR_ACTIVE_WINDOW_TITLEBAR     0x00A9CCE3
#define HONEY_COLOR_INACTIVE_WINDOW_TITLEBAR   0x00606060 
#define HONEY_COLOR_TITLEBAR_TEXT              COLOR_BLACK

// Fonts
#define HONEY_COLOR_SYSTEMFONT      0x00000000 
#define HONEY_COLOR_TERMINALFONT    0x00FFFFFF 
#define HONEY_COLOR_HIGHLIGHTFONT   0x00FFD580   // honey gold highlight text
#define HONEY_COLOR_DISABLED_FONT   0x00707070   // muted gray for disabled text

// Controls
#define HONEY_COLOR_BUTTON          0x00d4d0c8 
#define HONEY_COLOR_CHECKBOX        0x00E6E6E6   // light gray box
#define HONEY_COLOR_RADIOBUTTON     0x00E6E6E6   // same light gray for consistency
#define HONEY_COLOR_SLIDER          0x00C3C3C3   // neutral gray track


// Mouse Hover Effects
#define HONEY_COLOR_BG_ONMOUSEHOVER              xCOLOR_GRAY3
#define HONEY_COLOR_BG_ONMOUSEHOVER_MIN_CONTROL  0x0073A9C2   // steel blue
#define HONEY_COLOR_BG_ONMOUSEHOVER_MAX_CONTROL  0x0073A9C2   // steel blue
#define HONEY_COLOR_BG_ONMOUSEHOVER_CLO_CONTROL  0x00B22222   // firebrick red
#define HONEY_COLOR_BG_ONMOUSEHOVER_MENUITEM     0x00FFD580   // warm honey gold
#define HONEY_COLOR_BG_ONMOUSEHOVER_ICON         0x00FFD580   // warm honey gold

// Icons
#define HONEY_COLOR_DESKTOPICON     0x00FFD580   // honey gold accent
#define HONEY_COLOR_TRAYICON        0x00808080   // neutral gray tray icons

// Selections
#define HONEY_COLOR_SELECTION_BG      0x00A9CCE3   // light blue (same as active titlebar)
#define HONEY_COLOR_SELECTION_BORDER  0x006e2209  // deep brown border (matches active border)

// Alerts
#define HONEY_COLOR_WARNING         0x00FFA500   // orange warning
#define HONEY_COLOR_ERROR           0x00FF0000   // bright red error
#define HONEY_COLOR_SUCCESS         0x0000CC00   // deep green success

// Focus Indicators
#define HONEY_COLOR_FOCUS_RING      0x00FFD580   // honey gold ring
#define HONEY_COLOR_FOCUS_SHADOW    0x00404040   // subtle dark shadow

#endif   

