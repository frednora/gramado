/*
 * theme.h
 * Themes for the Gramado Taskbar / User Shell (taskbar.bin)
 *
 * This file builds on top of the standard colors defined in:
 *   libgws / colors.h
 *
 * Select only ONE theme below.
 */

#ifndef __THEME_H
#define __THEME_H

// =====================================================
// Theme selection (uncomment exactly ONE)
// =====================================================

#define THEME_HONEY     1       // Classic light gray (Win9x / current look)
//#define THEME_DARK      1       // Modern dark theme
//#define THEME_GREEN     1       // Soft green accent
//#define THEME_BLUE      1       // Classic blue accent


// =====================================================
// Common aliases (from colors.h)
// =====================================================

// These already exist in colors.h, we just make them easier to use.
// If your main.c already includes <gws.h> or the colors header,
// these will resolve correctly.


// =====================================================
// THEME: HONEY  (classic light taskbar)
// =====================================================
#ifdef THEME_HONEY

    // Taskbar
    #define THEME_TASKBAR_BG             COLOR_LTGRAY           // 0xC0C0C0
    #define THEME_TASKBAR_CLIENT_BG      COLOR_WINDOW           // white
    #define THEME_TASKBAR_BORDER         COLOR_GRAY             // 0x808080
    #define THEME_TASKBAR_TEXT           COLOR_TEXT

    // Start / buttons
    #define THEME_BUTTON_BG              COLOR_BUTTONFACE       // 0xF0F0F0
    #define THEME_BUTTON_BG_HOVER        COLOR_BUTTONFACE2      // 0xE0E0E0
    #define THEME_BUTTON_BG_PRESSED      COLOR_BUTTONFACE3      // 0xD0D0D0
    #define THEME_BUTTON_TEXT            COLOR_BUTTONTEXT
    #define THEME_BUTTON_BORDER          COLOR_BUTTONSHADOW
    #define THEME_BUTTON_HIGHLIGHT       COLOR_BUTTONHIGHLIGHT

    // Desktop
    #define THEME_DESKTOP_BG             COLOR_BACKGROUND       // 0x008080 (teal)
    //#define THEME_DESKTOP_BG           COLOR_DESKTOP          // pure black (alternative)
    #define THEME_DESKTOP_ICON_BG        COLOR_LTGRAY
    #define THEME_DESKTOP_ICON_TEXT      COLOR_WHITE

    // Accents / highlights
    #define THEME_ACCENT                 COLOR_HOTTRACK         // nice blue
    #define THEME_HIGHLIGHT              COLOR_HIGHLIGHT
    #define THEME_HIGHLIGHT_TEXT         COLOR_HIGHLIGHTTEXT

    // Compatibility with old names used in taskbar.c
    #define HONEY_COLOR_TASKBAR          THEME_TASKBAR_BG
    #define DEFAULT_TB_BACKGROUND_COLOR  THEME_TASKBAR_CLIENT_BG
    #define MYGREEN                      0x0B6623               // keep original green

#endif


// =====================================================
// THEME: DARK
// =====================================================
#ifdef THEME_DARK

    // Taskbar
    #define THEME_TASKBAR_BG             xCOLOR_GRAY2           // 0x404040
    #define THEME_TASKBAR_CLIENT_BG      xCOLOR_GRAY1           // 0x202020
    #define THEME_TASKBAR_BORDER         xCOLOR_GRAY3           // 0x606060
    #define THEME_TASKBAR_TEXT           COLOR_WHITE

    // Buttons
    #define THEME_BUTTON_BG              xCOLOR_GRAY3
    #define THEME_BUTTON_BG_HOVER        xCOLOR_GRAY4
    #define THEME_BUTTON_BG_PRESSED      xCOLOR_GRAY2
    #define THEME_BUTTON_TEXT            COLOR_WHITE
    #define THEME_BUTTON_BORDER          xCOLOR_GRAY5
    #define THEME_BUTTON_HIGHLIGHT       xCOLOR_GRAY5

    // Desktop
    #define THEME_DESKTOP_BG             COLOR_BLACK
    #define THEME_DESKTOP_ICON_BG        xCOLOR_GRAY2
    #define THEME_DESKTOP_ICON_TEXT      COLOR_WHITE

    // Accents
    #define THEME_ACCENT                 COLOR_DEEPSKYBLUE      // 0x00BFFF
    #define THEME_HIGHLIGHT              COLOR_HIGHLIGHT
    #define THEME_HIGHLIGHT_TEXT         COLOR_HIGHLIGHTTEXT

    // Compatibility
    #define HONEY_COLOR_TASKBAR          THEME_TASKBAR_BG
    #define DEFAULT_TB_BACKGROUND_COLOR  THEME_TASKBAR_CLIENT_BG
    #define MYGREEN                      THEME_ACCENT

#endif


// =====================================================
// THEME: GREEN  (soft green)
// =====================================================
#ifdef THEME_GREEN

    #define THEME_TASKBAR_BG             COLOR_HONEYDEW         // 0xF0FFF0
    #define THEME_TASKBAR_CLIENT_BG      COLOR_MINTCREAM
    #define THEME_TASKBAR_BORDER         COLOR_DARKSEAGREEN
    #define THEME_TASKBAR_TEXT           COLOR_DARKGREEN

    #define THEME_BUTTON_BG              COLOR_PALEGREEN
    #define THEME_BUTTON_BG_HOVER        COLOR_LIGHTGREEN
    #define THEME_BUTTON_BG_PRESSED      COLOR_MEDIUMSEAGREEN
    #define THEME_BUTTON_TEXT            COLOR_DARKGREEN
    #define THEME_BUTTON_BORDER          COLOR_SEAGREEN
    #define THEME_BUTTON_HIGHLIGHT       COLOR_WHITE

    #define THEME_DESKTOP_BG             COLOR_DARKOLIVEGREEN
    #define THEME_DESKTOP_ICON_BG        COLOR_PALEGREEN
    #define THEME_DESKTOP_ICON_TEXT      COLOR_WHITE

    #define THEME_ACCENT                 COLOR_FORESTGREEN
    #define THEME_HIGHLIGHT              COLOR_LIMEGREEN
    #define THEME_HIGHLIGHT_TEXT         COLOR_WHITE

    #define HONEY_COLOR_TASKBAR          THEME_TASKBAR_BG
    #define DEFAULT_TB_BACKGROUND_COLOR  THEME_TASKBAR_CLIENT_BG
    #define MYGREEN                      THEME_ACCENT

#endif


// =====================================================
// THEME: BLUE  (classic blue)
// =====================================================
#ifdef THEME_BLUE

    #define THEME_TASKBAR_BG             COLOR_LIGHTSTEELBLUE
    #define THEME_TASKBAR_CLIENT_BG      COLOR_ALICEBLUE
    #define THEME_TASKBAR_BORDER         COLOR_STEELBLUE
    #define THEME_TASKBAR_TEXT           COLOR_DARKBLUE

    #define THEME_BUTTON_BG              COLOR_LIGHTSKYBLUE
    #define THEME_BUTTON_BG_HOVER        COLOR_SKYBLUE
    #define THEME_BUTTON_BG_PRESSED      COLOR_CORNFLOWERBLUE
    #define THEME_BUTTON_TEXT            COLOR_DARKBLUE
    #define THEME_BUTTON_BORDER          COLOR_STEELBLUE
    #define THEME_BUTTON_HIGHLIGHT       COLOR_WHITE

    #define THEME_DESKTOP_BG             COLOR_MIDNIGHTBLUE
    #define THEME_DESKTOP_ICON_BG        COLOR_LIGHTSTEELBLUE
    #define THEME_DESKTOP_ICON_TEXT      COLOR_WHITE

    #define THEME_ACCENT                 COLOR_DODGERBLUE
    #define THEME_HIGHLIGHT              COLOR_HIGHLIGHT
    #define THEME_HIGHLIGHT_TEXT         COLOR_HIGHLIGHTTEXT

    #define HONEY_COLOR_TASKBAR          THEME_TASKBAR_BG
    #define DEFAULT_TB_BACKGROUND_COLOR  THEME_TASKBAR_CLIENT_BG
    #define MYGREEN                      THEME_ACCENT

#endif


// =====================================================
// Safety: make sure at least one theme is selected
// =====================================================
#if !defined(THEME_HONEY) && !defined(THEME_DARK) && \
    !defined(THEME_GREEN) && !defined(THEME_BLUE)
    #error "theme.h: No theme selected! Please define one of THEME_HONEY, THEME_DARK, THEME_GREEN or THEME_BLUE"
#endif

#endif // __THEME_H

