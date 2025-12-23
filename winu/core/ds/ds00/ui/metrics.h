// metrics.h 
// Default values for window metrics to ensure 
// a consistent UI style across the system. 
// Created by Fred Nora

#ifndef __UI_METRICS_H
#define __UI_METRICS_H    1

// ====================================================== 
// General UI Metrics 
// ======================================================

// Icon padding inside the titlebar
#define METRICS_ICON_LEFTPAD 4
#define METRICS_ICON_TOPPAD 3
#define METRICS_ICON_LEFT    METRICS_ICON_LEFTPAD
#define METRICS_ICON_TOP     METRICS_ICON_TOPPAD

// Border thickness for overlapped windows 
// (applications can override if needed)
#define METRICS_BORDER_SIZE  3


// ====================================================== 
// Taskbar Metrics 
// ======================================================

// Default taskbar height 
// 32 = (20 + padding adjustments)
#define METRICS_TASKBAR_DEFAULT_HEIGHT  32


// ====================================================== 
// Titlebar Metrics 
// ======================================================


// Default titlebar height 
// Controls and ornament sizes are derived from this
#define METRICS_TITLEBAR_DEFAULT_HEIGHT  32
// "Super" titlebar height (used in special cases)
#define METRICS_SUPERTITLEBAR_DEFAULT_HEIGHT  40

// Ornament line thickness 
// Used to indicate active/inactive state and separate 
// titlebar from client area
#define METRICS_TITLEBAR_ORNAMENT_SIZE  3

// Gap between titlebar height and control size 
// Controls shrink relative to titlebar height
#define __gap_BT_TB  4

// ====================================================== 
// Titlebar Controls Metrics 
// ======================================================

// Padding for control buttons (minimize, maximize, close)
#define METRICS_TITLEBAR_CONTROLS_TOPPAD     2
#define METRICS_TITLEBAR_CONTROLS_RIGHTPAD   2
#define METRICS_TITLEBAR_CONTROLS_SEPARATOR_WIDTH  1

// Control button dimensions 
// Derived from titlebar height minus gap and ornament

#define __METRICS_TITLEBAR_CONTROLS_DEFAULT_WIDTH \
    (METRICS_TITLEBAR_DEFAULT_HEIGHT - __gap_BT_TB - METRICS_TITLEBAR_ORNAMENT_SIZE)

#define __METRICS_TITLEBAR_CONTROLS_DEFAULT_HEIGHT \
    (METRICS_TITLEBAR_DEFAULT_HEIGHT - __gap_BT_TB - METRICS_TITLEBAR_ORNAMENT_SIZE)

// Defaults

// width
//#define METRICS_TITLEBAR_CONTROLS_DEFAULT_WIDTH  \
//    __METRICS_TITLEBAR_CONTROLS_DEFAULT_WIDTH  
#define METRICS_TITLEBAR_CONTROLS_DEFAULT_WIDTH  \
    (__METRICS_TITLEBAR_CONTROLS_DEFAULT_WIDTH + 4)  

// height
#define METRICS_TITLEBAR_CONTROLS_DEFAULT_HEIGHT  \
    __METRICS_TITLEBAR_CONTROLS_DEFAULT_HEIGHT



// ====================================================== 
// Application Window Metrics 
// ======================================================

// Minimum window dimensions (not child windows) 
// Derived from control button size for consistency

#define METRICS_DEFAULT_MINIMUM_WINDOW_WIDTH \
    (METRICS_TITLEBAR_CONTROLS_DEFAULT_WIDTH*5)
#define METRICS_DEFAULT_MINIMUM_WINDOW_HEIGHT \
    (METRICS_TITLEBAR_CONTROLS_DEFAULT_HEIGHT*3)


// ====================================================== 
// Editbox Metrics 
// ======================================================

// Margins inside editboxes
#define METRICS_EDITBOX_MARGIN_LEFT  4
#define METRICS_EDITBOX_MARGIN_TOP  4

// Maximum characters per line
#define METRICS_MAX_CHARS_PER_LINE  256

// ...

// ====================================================== 
// End of Metrics 
// ======================================================

#endif    

