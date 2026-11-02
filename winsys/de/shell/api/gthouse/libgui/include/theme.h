// theme.h

#ifndef __LIBGUI_THEME_H
#define __LIBGUI_THEME_H    1


//
// libgui theme
// Colors are 0x00RRGGBB.
// The client-side library has its own theme, separate from the
// colors the display server uses for the window frame/chrome.
//

// ---------------------------------------------
// Palette (raw values, neutral names)
// ---------------------------------------------
#define LIBGUI_PAL_BLACK        0x00101010
#define LIBGUI_PAL_GRAY_DARK    0x00505050
#define LIBGUI_PAL_GRAY_MID     0x00989898
#define LIBGUI_PAL_GRAY         0x00C0C0C0
#define LIBGUI_PAL_GRAY_LIGHT   0x00E0E0E0
#define LIBGUI_PAL_WHITE        0x00FAFAFA
#define LIBGUI_PAL_GRAY_DISABLED 0x00808080

// ---------------------------------------------
// Roles: surfaces
// ---------------------------------------------

//#ifndef LIBGUI_COLOR_BUTTON_FACE
#define LIBGUI_COLOR_BUTTON_FACE   LIBGUI_PAL_GRAY        // button, icon
//#endif

//#ifndef LIBGUI_COLOR_PANEL_FACE
#define LIBGUI_COLOR_PANEL_FACE    LIBGUI_PAL_GRAY_LIGHT  // label, header, footer
//#endif

// ---------------------------------------------
// Roles: text
// ---------------------------------------------
//#ifndef LIBGUI_COLOR_TEXT
#define LIBGUI_COLOR_TEXT          LIBGUI_PAL_BLACK
//#endif
//#ifndef LIBGUI_COLOR_TEXT_DISABLED
#define LIBGUI_COLOR_TEXT_DISABLED LIBGUI_PAL_GRAY_DISABLED
//#endif

// ---------------------------------------------
// Roles: button bevel
//   BEVEL_TL_1 = most inner, top/left
//   BEVEL_TL_2 = inner,      top/left
//   BEVEL_BR_1 = most inner, bottom/right
//   BEVEL_BR_2 = inner,      bottom/right
// ---------------------------------------------
#define LIBGUI_COLOR_BEVEL_OUTER        LIBGUI_PAL_BLACK

// Raised (released, focus, hover, ...)
#define LIBGUI_COLOR_BEVEL_RAISED_TL_1  LIBGUI_PAL_WHITE
#define LIBGUI_COLOR_BEVEL_RAISED_TL_2  LIBGUI_PAL_GRAY_LIGHT
#define LIBGUI_COLOR_BEVEL_RAISED_BR_1  LIBGUI_PAL_GRAY_MID
#define LIBGUI_COLOR_BEVEL_RAISED_BR_2  LIBGUI_PAL_GRAY_DARK

// Sunken (pressed): the same colors, with the light and dark sides swapped
#define LIBGUI_COLOR_BEVEL_SUNKEN_TL_1  LIBGUI_PAL_GRAY_DARK
#define LIBGUI_COLOR_BEVEL_SUNKEN_TL_2  LIBGUI_PAL_GRAY_MID
#define LIBGUI_COLOR_BEVEL_SUNKEN_BR_1  LIBGUI_PAL_GRAY_LIGHT
#define LIBGUI_COLOR_BEVEL_SUNKEN_BR_2  LIBGUI_PAL_WHITE

// Default state: placeholder, same as raised for now.
// This is the one you plan to change.
#define LIBGUI_COLOR_BEVEL_DEFAULT_TL_1 LIBGUI_COLOR_BEVEL_RAISED_TL_1
#define LIBGUI_COLOR_BEVEL_DEFAULT_TL_2 LIBGUI_COLOR_BEVEL_RAISED_TL_2
#define LIBGUI_COLOR_BEVEL_DEFAULT_BR_1 LIBGUI_COLOR_BEVEL_RAISED_BR_1
#define LIBGUI_COLOR_BEVEL_DEFAULT_BR_2 LIBGUI_COLOR_BEVEL_RAISED_BR_2



#endif   



