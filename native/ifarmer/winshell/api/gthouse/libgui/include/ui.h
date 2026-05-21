// ui.h
// UI components

#ifndef __LIBGUI_UI_H
#define __LIBGUI_UI_H    1


// Generic ui component for the apps.
// (it's like a window, but for client side inside the client area)
// The plain is reuse this kind of structure for webpages components.
struct ui_component_d 
{
    int used;
    int magic;
    int id;
    int type;

// Relative with the viewport 
// (client area or screen of its in fullscreen mode)
    unsigned long left;
    unsigned long top;
    unsigned long width;
    unsigned long height;

    // ...

    struct ui_component *next;
};

#endif  

