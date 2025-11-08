
// File: wt.h
// This header should be included by including "gws.h".

#ifndef __LIBGWS_WT_H
#define __LIBGWS_WT_H    1

// 
// Window types
//

#define WT_NULL          0 
#define WT_SIMPLE        1
// Input box.
// Just like the simple window, but with a border.
#define WT_EDITBOX       2
// Application window.
// (titlebar + board + client area);
#define WT_OVERLAPPED    3
#define WT_APPLICATION    WT_OVERLAPPED
//#define WT_MESSAGEBOX2  WT_POPUP
//#define WT_DIALOGBOX2   WT_POPUP
// Popup window.
// Special tipe of overlapped. Sometimes with no frame.
// Used for dialog box and message box.
#define WT_POPUP         4
//#define WT_MESSAGEBOX  WT_POPUP
//#define WT_DIALOGBOX   WT_POPUP
// Selection box, verification box,
#define WT_CHECKBOX      5
// Scroll bar window.
#define WT_SCROLLBAR     6
// Edit box with multiple lines.
#define WT_EDITBOX_MULTIPLE_LINES  7
#define WT_BUTTON     8
#define WT_STATUSBAR  9
#define WT_ICON       10
// #define WT_SURFACE    11
// #define WT_MAX (WT_SURFACE+1)

#endif    

