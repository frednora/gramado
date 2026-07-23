// ui.h
// UI components

#ifndef __LIBGUI_UI_H
#define __LIBGUI_UI_H    1

// Char
struct libgui_char_initialization_d
{
    int initialized;

    int width;
    int height;
};

// Rectangule
// Structure for a rectangle.
// A rectangle belongs to a window.
struct libgui_rect_d 
{
    int used;
    int magic;

    // struct libdisp_window_d *window;

    unsigned long left;
    unsigned long top;
    unsigned long width;
    unsigned long height;

    unsigned int bg_color;  // color
    int is_filled;          // filled or not
    unsigned long rop;      // raster operation

    //unsigned int flags;      // Reserved for future use

    int dirty;  // Validation
	int is_solid; // Is it a solid color rectangle?

    struct libgui_rect_d *next;
};


// The view:
// UI element / UI component.
struct libgui_view_d 
{
    int used;
	int magic;

// If we want to draw the component in an offscreen buffer, 
// we can use this field to store the buffer's address.
// Offscreen buffer for the component's content
	//char *offscreen_buffer;

// Type of the component (e.g., button, checkbox, text field)
// it also can be a container for other components, like a panel or a window.
// maybe called viewgroup or something like that.
	int type;

// Text label for the component (e.g., button text, checkbox label)
    char *label;

    unsigned int fg;  // foreground (text, icon, border)
    unsigned int bg;  // background (fill)
	unsigned long rop; // raster operation for drawing the component

// Geometry
// The values here are relative to the window's client area.
    unsigned long left;
    unsigned long top;
    unsigned long width;
    unsigned long height;

// State of the component (e.g., normal, hovered, pressed, disabled)
	int state;

//
// Input Pointer support (keyboard)
//

// The state of the input ponter.
// Used to blink the cursor.
    int ip_on;

    unsigned long ip_x;
    unsigned long ip_y;
    unsigned int ip_color;
    unsigned long width_in_chars;
    unsigned long height_in_chars;

    //unsigned long ip_type; //?? algum estilo especifico de cursor?
    //unsigned long ip_style;
    // ...

// para input do tipo teclado
    unsigned long ip_pixel_x;
    unsigned long ip_pixel_y;

// Navigation
    struct libgui_view_d *next;
};

struct libgui_node_d 
{
	int used;
	int magic;

	struct libgui_view_d *component;
	struct libgui_node_d *next;
};


// UI component types
#define UI_COMPONENT_BUTTON   1
#define UI_COMPONENT_LABEL    2
#define UI_COMPONENT_HEADER   3
#define UI_COMPONENT_FOOTER   4


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

    char label[64];
    size_t label_size;

    // ...

    struct ui_component *next;
};

#endif  

