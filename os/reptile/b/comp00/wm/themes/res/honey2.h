// dark.h
// Dark color theme for the color scheme.
// by Copilot

#ifndef __HONEY_H
#define __HONEY_H    1

// Background
#define HONEY_COLOR_BACKGROUND         0x00102030   // almost black with a blue tint
#define HONEY_COLOR_WINDOW_BACKGROUND    0x00303030   // soft dark gray

// Window
#define HONEY_COLOR_WINDOW                    0x00404040   // medium dark gray
#define HONEY_COLOR_WINDOW_BORDER             COLOR_INACTIVEBORDER
#define HONEY_COLOR_WWF_BORDER                COLOR_HIGHLIGHT  // bright blue accent
#define HONEY_COLOR_ACTIVE_WINDOW_BORDER      0x00404040   // medium gray border
#define HONEY_COLOR_INACTIVE_WINDOW_BORDER    0x00303030   // darker gray border

// Controls
#define HONEY_COLOR_BUTTON                     0x00303030   // dark gray button

// Bars
#define HONEY_COLOR_MENUBAR                    0x00202020   // dark gray menubar
#define HONEY_COLOR_SCROLLBAR                  0x00404040   // medium gray scrollbar
#define HONEY_COLOR_STATUSBAR                  0x00202020   // dark status bar
#define HONEY_COLOR_TASKBAR                    0x00202020   // dark gray taskbar

// Titlebar for active window.
// Deep charcoal with accent
#define HONEY_COLOR_ACTIVE_WINDOW_TITLEBAR     0x00252525   // dark charcoal
#define HONEY_COLOR_INACTIVE_WINDOW_TITLEBAR   0x00303030   // muted gray
#define HONEY_COLOR_TITLEBAR_TEXT              COLOR_WHITE

// Box
#define HONEY_COLOR_MESSAGEBOX                 0x00202020   // dark gray messagebox
#define HONEY_COLOR_DIALOGBOX                  0x00202020   // dark gray messagebox

// Fonts
#define HONEY_COLOR_SYSTEMFONT                 0x00FFFFFF   // white text
#define HONEY_COLOR_TERMINALFONT               0x00FFFFFF   // white text

// When mouse hover
#define HONEY_COLOR_BG_ONMOUSEHOVER             xCOLOR_GRAY3
// Minimize & Maximize Controls (same blue tone)
#define HONEY_COLOR_BG_ONMOUSEHOVER_MIN_CONTROL 0x005A7D9A   // slate blue-gray
#define HONEY_COLOR_BG_ONMOUSEHOVER_MAX_CONTROL 0x005A7D9A   // slate blue-gray
// Close Control (keep strong red)
#define HONEY_COLOR_BG_ONMOUSEHOVER_CLO_CONTROL 0x00B22222   // firebrick red
// Menu Item Hover (blue accent for consistency)
#define HONEY_COLOR_BG_ONMOUSEHOVER_MENUITEM    0x005A7D9A   // slate blue-gray

#endif
