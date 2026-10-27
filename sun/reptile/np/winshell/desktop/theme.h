/*
 * theme.h
 * Themes for desktop.bin (Explorer-like shell)
 * Desktop window + Taskbar
 *
 * Builds on top of the standard colors from libgws/colors.h
 */

#ifndef __THEME_H
#define __THEME_H

// =====================================================
// Theme selection  (uncomment exactly ONE)
// =====================================================

#define THEME_HONEY     1       // Classic light gray (current look)
//#define THEME_DARK      1       // Modern dark theme
//#define THEME_GREEN     1       // Soft green
//#define THEME_BLUE      1       // Classic blue


// =====================================================
// THEME: HONEY  (classic light)
// =====================================================
#ifdef THEME_HONEY

    // Desktop
    #define THEME_DESKTOP_BG             COLOR_BACKGROUND       // 0x008080 (teal)
    //#define THEME_DESKTOP_BG           COLOR_DESKTOP          // pure black alternative
    #define THEME_DESKTOP_ICON_BG        COLOR_LTGRAY
    #define THEME_DESKTOP_ICON_TEXT      COLOR_WHITE

    // Taskbar main
    #define THEME_TASKBAR_BG             COLOR_LTGRAY           // 0xC0C0C0
    #define THEME_TASKBAR_CLIENT_BG      COLOR_LTGRAY  //COLOR_WINDOW           // white
    #define THEME_TASKBAR_BORDER         COLOR_GRAY
    #define THEME_TASKBAR_TEXT           COLOR_TEXT

    // Taskbar areas (for debugging / visual separation)
    #define THEME_LAUNCH_AREA_BG         COLOR_BUTTONFACE       // soft gray
    #define THEME_ICON_AREA_BG           COLOR_BUTTONFACE2
    #define THEME_NOTIFICATION_AREA_BG   COLOR_BUTTONFACE3

/*
#define THEME_LAUNCH_AREA_BG         0x00D4D4D4             // slightly darker
#define THEME_ICON_AREA_BG           COLOR_LTGRAY           // same as main
#define THEME_NOTIFICATION_AREA_BG   0x00D4D4D4             // slightly darker
*/

    // Start button / buttons
    #define THEME_BUTTON_BG              COLOR_BUTTONFACE
    #define THEME_BUTTON_BG_HOVER        COLOR_BUTTONFACE2
    #define THEME_BUTTON_BG_PRESSED      COLOR_BUTTONFACE3
    #define THEME_BUTTON_TEXT            COLOR_BUTTONTEXT
    #define THEME_BUTTON_BORDER          COLOR_BUTTONSHADOW
    #define THEME_BUTTON_HIGHLIGHT       COLOR_BUTTONHIGHLIGHT

    // Clock / text
    #define THEME_CLOCK_TEXT             COLOR_BLACK
    #define THEME_CLOCK_BG               COLOR_WHITE

/*
#define THEME_CLOCK_TEXT             COLOR_BLACK
#define THEME_CLOCK_BG               COLOR_LTGRAY 
*/

    // Accents
    #define THEME_ACCENT                 COLOR_HOTTRACK
    #define THEME_HIGHLIGHT              COLOR_HIGHLIGHT
    #define THEME_HIGHLIGHT_TEXT         COLOR_HIGHLIGHTTEXT

    // Compatibility with existing defines in main.c
    #define HONEY_COLOR_TASKBAR          THEME_TASKBAR_BG
    #define DEFAULT_TB_BACKGROUND_COLOR  THEME_TASKBAR_CLIENT_BG
    #define MYGREEN                      0x0B6623

#endif


// =====================================================
// THEME: DARK
// =====================================================
#ifdef THEME_DARK

    // Desktop
    #define THEME_DESKTOP_BG             COLOR_BLACK
    #define THEME_DESKTOP_ICON_BG        xCOLOR_GRAY2
    #define THEME_DESKTOP_ICON_TEXT      COLOR_WHITE

    // Taskbar main
    #define THEME_TASKBAR_BG             xCOLOR_GRAY2           // 0x404040
    #define THEME_TASKBAR_CLIENT_BG      xCOLOR_GRAY1           // 0x202020
    #define THEME_TASKBAR_BORDER         xCOLOR_GRAY3
    #define THEME_TASKBAR_TEXT           COLOR_WHITE

    // Taskbar areas
    #define THEME_LAUNCH_AREA_BG         xCOLOR_GRAY3
    #define THEME_ICON_AREA_BG           xCOLOR_GRAY2
    #define THEME_NOTIFICATION_AREA_BG   xCOLOR_GRAY3

    // Buttons
    #define THEME_BUTTON_BG              xCOLOR_GRAY3
    #define THEME_BUTTON_BG_HOVER        xCOLOR_GRAY4
    #define THEME_BUTTON_BG_PRESSED      xCOLOR_GRAY2
    #define THEME_BUTTON_TEXT            COLOR_WHITE
    #define THEME_BUTTON_BORDER          xCOLOR_GRAY5
    #define THEME_BUTTON_HIGHLIGHT       xCOLOR_GRAY5

    // Clock
    #define THEME_CLOCK_TEXT             COLOR_WHITE
    #define THEME_CLOCK_BG               xCOLOR_GRAY1

    // Accents
    #define THEME_ACCENT                 COLOR_DEEPSKYBLUE
    #define THEME_HIGHLIGHT              COLOR_HIGHLIGHT
    #define THEME_HIGHLIGHT_TEXT         COLOR_HIGHLIGHTTEXT

    // Compatibility
    #define HONEY_COLOR_TASKBAR          THEME_TASKBAR_BG
    #define DEFAULT_TB_BACKGROUND_COLOR  THEME_TASKBAR_CLIENT_BG
    #define MYGREEN                      THEME_ACCENT

#endif


// =====================================================
// THEME: GREEN
// =====================================================
#ifdef THEME_GREEN

    #define THEME_DESKTOP_BG             COLOR_DARKOLIVEGREEN
    #define THEME_DESKTOP_ICON_BG        COLOR_PALEGREEN
    #define THEME_DESKTOP_ICON_TEXT      COLOR_WHITE

    #define THEME_TASKBAR_BG             COLOR_HONEYDEW
    #define THEME_TASKBAR_CLIENT_BG      COLOR_MINTCREAM
    #define THEME_TASKBAR_BORDER         COLOR_DARKSEAGREEN
    #define THEME_TASKBAR_TEXT           COLOR_DARKGREEN

    #define THEME_LAUNCH_AREA_BG         COLOR_PALEGREEN
    #define THEME_ICON_AREA_BG           COLOR_LIGHTGREEN
    #define THEME_NOTIFICATION_AREA_BG   COLOR_MEDIUMSEAGREEN

    #define THEME_BUTTON_BG              COLOR_PALEGREEN
    #define THEME_BUTTON_BG_HOVER        COLOR_LIGHTGREEN
    #define THEME_BUTTON_BG_PRESSED      COLOR_MEDIUMSEAGREEN
    #define THEME_BUTTON_TEXT            COLOR_DARKGREEN
    #define THEME_BUTTON_BORDER          COLOR_SEAGREEN
    #define THEME_BUTTON_HIGHLIGHT       COLOR_WHITE

    #define THEME_CLOCK_TEXT             COLOR_DARKGREEN
    #define THEME_CLOCK_BG               COLOR_MINTCREAM

    #define THEME_ACCENT                 COLOR_FORESTGREEN
    #define THEME_HIGHLIGHT              COLOR_LIMEGREEN
    #define THEME_HIGHLIGHT_TEXT         COLOR_WHITE

    #define HONEY_COLOR_TASKBAR          THEME_TASKBAR_BG
    #define DEFAULT_TB_BACKGROUND_COLOR  THEME_TASKBAR_CLIENT_BG
    #define MYGREEN                      THEME_ACCENT

#endif


// =====================================================
// THEME: BLUE
// =====================================================
#ifdef THEME_BLUE

    #define THEME_DESKTOP_BG             COLOR_MIDNIGHTBLUE
    #define THEME_DESKTOP_ICON_BG        COLOR_LIGHTSTEELBLUE
    #define THEME_DESKTOP_ICON_TEXT      COLOR_WHITE

    #define THEME_TASKBAR_BG             COLOR_LIGHTSTEELBLUE
    #define THEME_TASKBAR_CLIENT_BG      COLOR_ALICEBLUE
    #define THEME_TASKBAR_BORDER         COLOR_STEELBLUE
    #define THEME_TASKBAR_TEXT           COLOR_DARKBLUE

    #define THEME_LAUNCH_AREA_BG         COLOR_LIGHTSKYBLUE
    #define THEME_ICON_AREA_BG           COLOR_SKYBLUE
    #define THEME_NOTIFICATION_AREA_BG   COLOR_CORNFLOWERBLUE

    #define THEME_BUTTON_BG              COLOR_LIGHTSKYBLUE
    #define THEME_BUTTON_BG_HOVER        COLOR_SKYBLUE
    #define THEME_BUTTON_BG_PRESSED      COLOR_CORNFLOWERBLUE
    #define THEME_BUTTON_TEXT            COLOR_DARKBLUE
    #define THEME_BUTTON_BORDER          COLOR_STEELBLUE
    #define THEME_BUTTON_HIGHLIGHT       COLOR_WHITE

    #define THEME_CLOCK_TEXT             COLOR_DARKBLUE
    #define THEME_CLOCK_BG               COLOR_ALICEBLUE

    #define THEME_ACCENT                 COLOR_DODGERBLUE
    #define THEME_HIGHLIGHT              COLOR_HIGHLIGHT
    #define THEME_HIGHLIGHT_TEXT         COLOR_HIGHLIGHTTEXT

    #define HONEY_COLOR_TASKBAR          THEME_TASKBAR_BG
    #define DEFAULT_TB_BACKGROUND_COLOR  THEME_TASKBAR_CLIENT_BG
    #define MYGREEN                      THEME_ACCENT

#endif


// =====================================================
// Safety check
// =====================================================
#if !defined(THEME_HONEY) && !defined(THEME_DARK) && \
    !defined(THEME_GREEN) && !defined(THEME_BLUE)
    #error "theme.h: No theme selected!"
#endif

#endif // __THEME_H

