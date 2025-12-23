// dark.h
// Dark color theme for the color scheme.
// by Copilot

#ifndef __HONEY_H
#define __HONEY_H    1

#define HONEY_COLOR_BACKGROUND                 0x00101010   // near black background
#define HONEY_COLOR_WINDOW                     0x00202020   // dark gray window
#define HONEY_COLOR_WINDOW_BACKGROUND          COLOR_BLACK
#define HONEY_COLOR_ACTIVE_WINDOW_BORDER       0x00404040   // medium gray border
#define HONEY_COLOR_INACTIVE_WINDOW_BORDER     0x00303030   // darker gray border

// Titlebar for active window.
// Deep charcoal with accent
#define HONEY_COLOR_ACTIVE_WINDOW_TITLEBAR     0x00252525   // dark charcoal
#define HONEY_COLOR_INACTIVE_WINDOW_TITLEBAR   0x00303030   // muted gray

#define HONEY_COLOR_MENUBAR                    0x00202020   // dark gray menubar
#define HONEY_COLOR_SCROLLBAR                  0x00404040   // medium gray scrollbar
#define HONEY_COLOR_STATUSBAR                  0x00202020   // dark status bar

// Taskbar for the window manager.
#define HONEY_COLOR_TASKBAR                    0x00202020   // dark gray taskbar
#define HONEY_COLOR_MESSAGEBOX                 0x00202020   // dark gray messagebox
#define HONEY_COLOR_SYSTEMFONT                 0x00FFFFFF   // white text
#define HONEY_COLOR_TERMINALFONT               0x00FFFFFF   // white text
#define HONEY_COLOR_BUTTON                     0x00303030   // dark gray button

// Window border
#define HONEY_COLOR_WINDOW_BORDER              COLOR_INACTIVEBORDER
// Window with focus border
#define HONEY_COLOR_WWF_BORDER                 COLOR_HIGHLIGHT  // bright blue accent

// Titlebar text
#define HONEY_COLOR_TITLEBAR_TEXT              COLOR_WHITE

// When mouse hover.
#define HONEY_COLOR_BG_ONMOUSEHOVER            xCOLOR_GRAY4
#define HONEY_COLOR_BG_ONMOUSEHOVER_MIN_CONTROL COLOR_HIGHLIGHT
#define HONEY_COLOR_BG_ONMOUSEHOVER_MAX_CONTROL COLOR_HIGHLIGHT
#define HONEY_COLOR_BG_ONMOUSEHOVER_CLO_CONTROL COLOR_RED

#endif
