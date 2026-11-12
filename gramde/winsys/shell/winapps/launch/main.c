// main.c
// Gramado OS client-side GUI app: Launch (v2)
// Launches #power.bin, #memory.bin or #sysinfo.bin.
//
// Rebuilt on the same routines as the Power Manager example:
//  - on_button_pressed / on_button_released / on_button_clicked
//  - hover tracking on MSG_MOUSEMOVE
//  - press effect on MSG_MOUSEPRESSED, action on MSG_MOUSERELEASED
//  - keyboard: press effect on MSG_KEYDOWN, action on MSG_KEYUP
//  - the same event loop: shared-flags blit check, then rtl_get_event()
//    handing the event to the procedure. No display-server pump.
//
// What is new compared to the old Launch:
//  - Mouse effects: the button looks pressed while the mouse button is down.
//  - A click only launches if the pointer is still over the pressed button.
//  - Hover frame around the button under the pointer.
//  - Keyboard focus (arrows move it, Enter launches) drawn as a black frame.
//  - Keys P/M/I show the pressed effect on key down and launch on key up.
//  - Footer shows what was launched.
//  - Table-driven buttons: add a program by adding one line to ItemList[].
//  - No printf in the event path (enable with LAUNCH_DEBUG).
//  - The title text is scaled down when it does not fit in the window.

// rtl
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <sys/socket.h>
#include <rtl/gramado.h>

// The client-side library
#include <gws.h>

// #test
// The client-side library
#include <libgui.h>

#include "launch.h"

// Set to 1 to get messages on the console.
#define LAUNCH_DEBUG  0
#if LAUNCH_DEBUG
#define LAUNCH_LOG(...)  printf(__VA_ARGS__)
#else
#define LAUNCH_LOG(...)  ((void)0)
#endif

// Frame colors (drawn just outside the button)
#define FRAME_THICKNESS   2
#define FRAME_HOVER_COLOR  0x000060C0
#define FRAME_FOCUS_COLOR  0x00000000
#define FRAME_NONE_COLOR   COLOR_WHITE   // same as the window background

static int isTimeToQuit = FALSE;

// Global display pointer
struct gws_display_d *Display;

struct dccanvas_d *dc00;  // shared dc

static unsigned long __sh_flags = 0;

struct ui_component_d *uic_footer;

struct button_info_d
{
    int button_id;

// This is the window id that represents the icon.
    int wid;

// Absolute values
    unsigned long absolute_left;
    unsigned long absolute_top;
    unsigned long width; 
    unsigned long height;

// Relative values
    unsigned long left;
    unsigned long top;

// The state of the icon, it also represents
// the state of the client application.
// (running, minimized, etc.).
    int state;
};

// ----------------------------------------------------
// The buttons
// ----------------------------------------------------
// button_id = index + 1. 
// To add a program, add a line here. The layout adapts.
struct launch_item_d
{
    struct button_info_d info;
    struct ui_component_d *uic;
    const char *label;     // text on the button
    const char *program;   // program image
    char key;              // shortcut (lowercase)
};

static struct launch_item_d  ItemList[] = {
    { {0}, NULL, "Power",   "#power.bin",   'p' },
    { {0}, NULL, "Memory",  "#memory.bin",  'm' },
    { {0}, NULL, "Sysinfo", "#sysinfo.bin", 'i' }
};
#define ITEM_COUNT  (int)(sizeof(ItemList)/sizeof(ItemList[0]))

static const char *TitleText = "p=Power m=Memory i=Sysinfo q=Quit";

// ----------------------------------------------------
// State
// ----------------------------------------------------
static int __hover_button_id = -1;   // Invalidate.
static int __mouse_pressed_id = -1;  // Pressed with the mouse
static int __key_pressed_id = -1;    // Pressed with the keyboard
static int __focus_button_id = -1;   // Keyboard focus

// Position of the pointer at the time of the event.
// Filled by the input loop from the event buffer.
static unsigned long last_x = 0;
static unsigned long last_y = 0;

struct bmp_cache_d *icon00_cache;

// Window IDs
static int main_window = -1;

// -----------------
static void on_button_pressed(int button_id);
static void on_button_released(int button_id);
static void on_button_clicked(int button_id);

static int  __hit_test_button(unsigned long rel_mx, unsigned long rel_my);
static void layout_items(struct gws_window_info_d *wi);
static void update_children(int fd);

// =====================================================
// Drawing helper
// =====================================================

// Fill a rectangle, clipped against the dc. ROP_COPY only.
static void 
launch_fill_rect(
    long x, long y, long w, long h,
    unsigned int color )
{
    long x2, y2, yy, xx;
    long dw, dh;
    unsigned char *row;
    unsigned int c = (color & 0x00FFFFFF);
    unsigned char b = (unsigned char)( c        & 0xFF);
    unsigned char g = (unsigned char)((c >> 8)  & 0xFF);
    unsigned char r = (unsigned char)((c >> 16) & 0xFF);

    if ((void*) dc00 == NULL || (void*) dc00->data == NULL)
        return;
    if (w <= 0 || h <= 0)
        return;

    dw = (long) dc00->device_width;
    dh = (long) dc00->device_height;

    x2 = x + w;
    y2 = y + h;
    if (x  < 0)  x  = 0;
    if (y  < 0)  y  = 0;
    if (x2 > dw) x2 = dw;
    if (y2 > dh) y2 = dh;
    if (x >= x2 || y >= y2)
        return;

    for (yy = y; yy < y2; yy++)
    {
        row = (unsigned char *) dc00->data + ((unsigned long) yy * dc00->pitch);

        if (dc00->bpp == 32)
        {
            unsigned int *p = (unsigned int *) (row + ((unsigned long) x << 2));
            for (xx = x; xx < x2; xx++)
                *p++ = c;
        }
        else if (dc00->bpp == 24)
        {
            unsigned char *p = row + ((unsigned long) x * 3);
            for (xx = x; xx < x2; xx++)
            {
                p[0] = b;
                p[1] = g;
                p[2] = r;
                p += 3;
            }
        }
    }
}

// =====================================================
// Frames: hover (blue) and keyboard focus (black)
// =====================================================

// Redraw the frame around one button, given its id.
// The color depends on the state: hover wins over focus.
static void draw_item_frame(int button_id)
{
    struct launch_item_d *it;
    unsigned int color = FRAME_NONE_COLOR;
    long x, y, w, h, t = FRAME_THICKNESS;

    if (button_id < 1 || button_id > ITEM_COUNT)
        return;
    if ((void*) dc00 == NULL)
        return;

    it = &ItemList[button_id - 1];

    if (button_id == __focus_button_id)
        color = FRAME_FOCUS_COLOR;
    if (button_id == __hover_button_id)
        color = FRAME_HOVER_COLOR;

    x = (long) it->info.left   - t;
    y = (long) it->info.top    - t;
    w = (long) it->info.width  + (t * 2);
    h = (long) it->info.height + (t * 2);

    launch_fill_rect(x,         y,         w, t,         color);  // top
    launch_fill_rect(x,         y + h - t, w, t,         color);  // bottom
    launch_fill_rect(x,         y + t,     t, h - (t*2), color);  // left
    launch_fill_rect(x + w - t, y + t,     t, h - (t*2), color);  // right
}

static void set_hover(int button_id)
{
    int old = __hover_button_id;

    if (button_id == old)
        return;

    __hover_button_id = button_id;
    draw_item_frame(old);
    draw_item_frame(button_id);
}

static void move_focus(int delta)
{
    int old = __focus_button_id;
    int n;

    if (old < 1)
        n = (delta > 0) ? 1 : ITEM_COUNT;
    else
        n = old + delta;

    if (n < 1) n = ITEM_COUNT;
    if (n > ITEM_COUNT) n = 1;

    __focus_button_id = n;
    draw_item_frame(old);
    draw_item_frame(n);
}

// =====================================================
// Footer
// =====================================================

// "Launching <name>"
static void set_footer_launching(const char *name)
{
    const char *prefix = "Launching ";
    char text[32];
    int n = 0;
    const char *s;

    if ((void*) uic_footer == NULL || (void*) dc00 == NULL)
        return;
    if (uic_footer->magic != 1234)
        return;
    if (sizeof(uic_footer->label) < sizeof(text))
        return;

    for (s = prefix; *s && n < 31; s++)
        text[n++] = *s;
    for (s = name; s && *s && n < 31; s++)
        text[n++] = *s;
    text[n] = '\0';

    memcpy(uic_footer->label, text, sizeof(text));
    uic_footer->label_size = n;

    libgui_redraw_ui_component(uic_footer, dc00);
}

// =====================================================
// Button handlers (same roles as in the Power example)
// =====================================================

// For visual effects
// #todo: We need to implement this kind of worker inside the library
static void on_button_pressed(int button_id)
{
    if (button_id < 1 || button_id > ITEM_COUNT)
        return;

    libgui_on_button_pressed(ItemList[button_id - 1].uic, dc00);
}

// For visual effects
// #todo: We need to implement this kind of worker inside the library
static void on_button_released(int button_id)
{
    if (button_id < 1 || button_id > ITEM_COUNT)
        return;

    libgui_on_button_released(ItemList[button_id - 1].uic, dc00);
}

// Handle click events for components
static void on_button_clicked(int button_id)
{
    struct launch_item_d *it;

    if (button_id < 1 || button_id > ITEM_COUNT)
        return;

    it = &ItemList[button_id - 1];

    LAUNCH_LOG("launch: Launching %s\n", it->program);
    rtl_clone_and_execute(it->program);

    // Do not exit — keep the launcher alive so the user can
    // launch more programs if desired.
    set_footer_launching(it->label);
}

// Hit-test for the buttons of the Launch app
static int __hit_test_button(unsigned long rel_mx, unsigned long rel_my) 
{
    int i;
    struct button_info_d *b;

    for (i = 0; i < ITEM_COUNT; i++)
    {
        b = &ItemList[i].info;

        if ( rel_mx >= b->left && 
             rel_mx <  b->left + b->width &&
             rel_my >= b->top  && 
             rel_my <  b->top  + b->height )
        {
            return (int) b->button_id;
        }
    }

    return -1;
}

// Which button does this key activate? (-1 = none)
static int __key_to_button_id(unsigned long key)
{
    int i;
    char c = (char) key;

    if (key == VK_RETURN)
        return __focus_button_id;   // may be -1

    if (c >= 'A' && c <= 'Z')
        c = (char)(c + 32);

    for (i = 0; i < ITEM_COUNT; i++)
    {
        if (ItemList[i].key == c)
            return ItemList[i].info.button_id;
    }

    return -1;
}

// =====================================================
// Layout and painting
// =====================================================

// Compute the position and dimension of every button
// from the client area. Evenly spaced on the middle of the window.
static void layout_items(struct gws_window_info_d *wi)
{
    int i;
    unsigned long button_w = wi->cr_width / 5;
    unsigned long button_h = wi->cr_height / 8;
    unsigned long button_y = (wi->cr_height - button_h) / 2;
    unsigned long center_x;

    for (i = 0; i < ITEM_COUNT; i++)
    {
        // Centers at 1/6, 3/6, 5/6 ... of the width (for three buttons)
        center_x = (((2 * i) + 1) * wi->cr_width) / (2 * ITEM_COUNT);

        ItemList[i].info.button_id = i + 1;

        // Relative values
        ItemList[i].info.left   = center_x - (button_w / 2);
        ItemList[i].info.top    = button_y;
        ItemList[i].info.width  = button_w;
        ItemList[i].info.height = button_h;

        // Absolute values
        ItemList[i].info.absolute_left = 
            wi->left + wi->cr_left + ItemList[i].info.left;
        ItemList[i].info.absolute_top = 
            wi->top  + wi->cr_top  + ItemList[i].info.top;
    }
}

// The title uses scale 2 when it fits, otherwise scale 1.
static void draw_title(unsigned long cr_width)
{
    unsigned long len = (unsigned long) strlen(TitleText);
    unsigned long scale = 2;

    if ((len * 8 * scale) + 16 > cr_width)
        scale = 1;

    libgui_drawstringblock_dc(
        dc00, 8, 8, COLOR_BLACK, TitleText, scale );
}

static void update_children(int fd)
{
    struct gws_window_info_d  wi;
    int i;

    // Get window info
    gws_get_window_info(fd, main_window, &wi);

    if ((void*)dc00 == NULL)
        return;

// The background for the client area
    launch_fill_rect( 0, 0, wi.cr_width, wi.cr_height, COLOR_WHITE );

// String
    draw_title(wi.cr_width);

// Buttons: position, dimension and redraw.
// The component keeps its status, so a pressed button stays pressed.
    layout_items(&wi);

    for (i = 0; i < ITEM_COUNT; i++)
    {
        libgui_set_ui_component_position(
            ItemList[i].uic, 
            ItemList[i].info.left, 
            ItemList[i].info.top );
        libgui_set_ui_component_dimension(
            ItemList[i].uic,
            ItemList[i].info.width,
            ItemList[i].info.height );
        libgui_set_ui_component_flags(ItemList[i].uic, (0x0001 | 0x0002) );
        libgui_redraw_ui_component(ItemList[i].uic, dc00);
    }

// bmp
// zoom=2
    if (icon00_cache && icon00_cache->loaded) 
    {
        bmp_decode_bmp_image(icon00_cache, dc00, 40, 40, 2);
    }

// footer
    libgui_set_ui_component_position(
        uic_footer, 0, wi.cr_height - 24 );
    libgui_set_ui_component_dimension(
        uic_footer, wi.cr_width, 24 );
    libgui_set_ui_component_flags(uic_footer, (0x0001 | 0x0002));
    libgui_redraw_ui_component(uic_footer, dc00);

// Frames (hover and focus) go on top
    for (i = 0; i < ITEM_COUNT; i++)
        draw_item_frame(ItemList[i].info.button_id);
}

// ----------------------------------------------------
// Procedure: handles the events
// ----------------------------------------------------
static int 
launchProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 )
{
    int ButtonId = -1;

    if (fd < 0)
        return (int) -1;

    if (event_window < 0)
        return (int) -1;
    if (event_type < 0)
        return (int) -1;

    switch (event_type) {

    // Null event
    case 0:
        return 0;
        break;

    // Redraw child windows
    case MSG_PAINT:
        update_children(fd);
        return 0;
        break;

    // Keyboard: the press effect on key down,
    // the action on key up. Just like the Power example.
    case MSG_KEYDOWN:

        // Quit the launcher itself
        if (long1 == 'Q' || long1 == 'q')
        {
            isTimeToQuit = TRUE;   // Quit the application
            // #test: This call quit the server
            // gws_async_command(fd,88,0,0);
            return 0;
        }

        ButtonId = __key_to_button_id(long1);
        // Ignore the auto-repeat: only the first key down counts.
        if (ButtonId > 0 && __key_pressed_id != ButtonId)
        {
            __key_pressed_id = ButtonId;
            on_button_pressed(ButtonId);  // Visual effect
        }
        return 0;
        break;

    case MSG_KEYUP:
        ButtonId = __key_to_button_id(long1);
        if (ButtonId > 0 && __key_pressed_id == ButtonId)
        {
            __key_pressed_id = -1;
            on_button_released(ButtonId);  // Visual effect
            on_button_clicked(ButtonId);   // Action
        }
        return 0;
        break;

    case MSG_SYSKEYDOWN:
        switch (long1) {
            case VK_F1:  on_button_clicked(1);  return 0;
            case VK_F2:  on_button_clicked(2);  return 0;
            case VK_F3:  on_button_clicked(3);  return 0;

            case VK_F11:
                // Should not appear — broker intercepts fullscreen toggle
                return 0;

            // Move the keyboard focus. Enter activates it.
            case VK_ARROW_LEFT: 
                move_focus(-1);
                return 0;
            case VK_ARROW_RIGHT: 
                move_focus(+1);
                return 0;

            // reserved for future
            case VK_ARROW_UP: 
            case VK_ARROW_DOWN: 
                return 0;
        };
        break;

    case GWS_MouseClicked:
        break;

    case MSG_MOUSEMOVE:
        // long1/long2: pointer position, relative to the client area.
        ButtonId = (int) __hit_test_button(long1, long2);
        set_hover(ButtonId);   // Frame effect
        return 0;
        break;

    case MSG_MOUSEPRESSED:
        // The pointer position is in last_x/last_y
        // (saved by the input loop from RTLEventBuffer[4]/[5]).
        ButtonId = (int) __hit_test_button(last_x, last_y);
        if (ButtonId < 0)
            ButtonId = __hover_button_id;

        __mouse_pressed_id = ButtonId;
        if (ButtonId > 0)
            on_button_pressed(ButtonId);  // Visual effect
        return 0;
        break;

    case MSG_MOUSERELEASED:
        if (__mouse_pressed_id > 0)
        {
            on_button_released(__mouse_pressed_id);  // Visual effect

            // A click only counts if the pointer is still
            // over the button that was pressed.
            ButtonId = (int) __hit_test_button(last_x, last_y);
            if (ButtonId < 0)
                ButtonId = __hover_button_id;

            if (ButtonId == __mouse_pressed_id)
                on_button_clicked(ButtonId);  // Action

            __mouse_pressed_id = -1;
        }
        return 0;
        break;

    case MSG_CLOSE:
        isTimeToQuit = TRUE;  // #test
        return 0;
        break;

    // Unknown event
    default:
        return -1;
        break;
    };

// Fail
    return (int) -1;
}

int main(int argc, char *argv[])
{
    const char *display_name = "display:name.0";
    int client_fd = -1;
    int i;

    isTimeToQuit = FALSE;

    // Connect to display server
    Display = gws_open_display(display_name);
    if ((void*) Display == NULL) {
        printf("launch_app: Could not open display\n");
        return EXIT_FAILURE;
    }

    client_fd = Display->fd;
    if (client_fd <= 0) {
        printf("launch: Invalid fd\n");
        return EXIT_FAILURE;
    }

    // Screen size
    unsigned long screen_w = gws_get_system_metrics(1);
    unsigned long screen_h = gws_get_system_metrics(2);

// =========================================
// Library initialization

    int status = -1;
    status = (int) libgui_initialize();
    if (status < 0){
        printf("launch_app: on libgui_initialize()\n");
        exit(1);
    }

// =========================================
// Main window

    unsigned long mw_style = WS_APP;

    unsigned long win_w = screen_w / 2;
    unsigned long win_h = screen_h / 2;
    unsigned long win_x = screen_w/4;
    unsigned long win_y = screen_h/4;

    main_window = 
        (int) gws_create_window(
                client_fd,
                WT_OVERLAPPED,
                WINDOW_STATUS_ACTIVE,  //status
                WINDOW_STATE_NULL,  //state
                "Launch",
                win_x, win_y, win_w, win_h,
                0,
                mw_style,  // style
                COLOR_WHITE, COLOR_GRAY );

    if (main_window < 0) {
        printf("launch_app: Failed to create main window\n");
        return EXIT_FAILURE;
    }

// After creating main_window,
// get information about it.
    struct gws_window_info_d wi;
    gws_get_window_info(
        client_fd,
        main_window,
        (struct gws_window_info_d *) &wi );

// ============================================================
// #test
// Update the wproxy structure that belongs to this thread.

    unsigned long m[10];
    int mytid = gettid();
    m[0] = (unsigned long) (mytid & 0xFFFFFFFF);

    // Frame/chrome rectangle
    m[1] = wi.left;
    m[2] = wi.top;
    m[3] = wi.width;
    m[4] = wi.height;

    // Client area rectangle
    m[5] = wi.cr_left;
    m[6] = wi.cr_top;
    m[7] = wi.cr_width;
    m[8] = wi.cr_height;

    sc80( 48, &m[0], &m[0], &m[0] );

// ============================================================
// Getting the flag earlier. This way we can use it in the loop.

    __sh_flags = (unsigned long) wi.sh_flags;

// ============================================================
// Create a device context structure
// based on the information we got with the server.
// This is a dc to draw into the client area.

    dc00 = (struct dccanvas_d *) libgui_create_dc(
        wi.ca_canvas_base_address,
        wi.ca_canvas_width,
        wi.ca_canvas_height,
        wi.ca_canvas_bpp
    );
    if ((void*)dc00 == NULL){
        printf("launch: on dc00\n");
        exit(1);
    }

// bg for the client area
    launch_fill_rect( 0, 0, wi.cr_width, wi.cr_height, COLOR_WHITE );

// String
    draw_title(wi.cr_width);

// ============================================================
// Create the buttons.
// The layout gives the position and dimension of each one.

    layout_items(&wi);

    for (i = 0; i < ITEM_COUNT; i++)
    {
        ItemList[i].uic = libgui_create_ui_component (
            dc00, 
            UI_COMPONENT_BUTTON,   // type 1 = button 
            ItemList[i].info.left, 
            ItemList[i].info.top, 
            ItemList[i].info.width, 
            ItemList[i].info.height,
            ItemList[i].label,
            (0x0001 | 0x0002)
        );
        if ((void*) ItemList[i].uic == NULL){
            printf("launch: on button %d\n", i);
            exit(1);
        }
    }

// -----------------------------------------------------
// #test: BMP image
// zoom=2

    icon00_cache = (struct bmp_cache_d*) bmp_load_bmp_image("#folder.bmp");
    if (icon00_cache && icon00_cache->loaded) 
    {
        bmp_decode_bmp_image(icon00_cache, dc00, 40, 40, 2);
    }

// ================================================================================

// Create footer component
    uic_footer =
        libgui_create_ui_component(
            dc00,
            UI_COMPONENT_FOOTER,
            0, 
            wi.cr_height -24, 
            wi.cr_width, 
            24,
            "-- launch --", 
            (0x0001 | 0x0002)
        );

// ================================================================================

// Main window: Activate and show.
    gws_set_active( client_fd, main_window );

//
// Event loop
//

// #ps:
// The purpose of this loop is only get system events,
// not to get events from the display server.
// We do this for performance reasons.

    while (1){

        if (isTimeToQuit == TRUE)
            break;

        // Shared flags handling (same as Power app)
        if (__sh_flags != 0)
        {
            char *flags_ptr = (char *) __sh_flags;
            if (*flags_ptr & 0x0008)
            {
                // Clear BLIT bit
                *flags_ptr &= ~0x0008;
                // Redraw
                update_children(client_fd);
            }
        }

        // Pump events from Input Broker (system events)
        if (rtl_get_event() == TRUE)
        {
            last_x = (unsigned long) RTLEventBuffer[4];
            last_y = (unsigned long) RTLEventBuffer[5];

            // IN: wid, event type, VK, scancode.
            launchProcedure(
                client_fd,
                (int) RTLEventBuffer[0],
                (int) RTLEventBuffer[1],
                (unsigned long) RTLEventBuffer[2],
                (unsigned long) RTLEventBuffer[3] );
            RTLEventBuffer[1] = 0;  // Clear after dispatch
        }
    };

    if (isTimeToQuit == TRUE){
        printf("LaunchApp: Close window\n");
        gws_destroy_window(client_fd, main_window);
    }

    if (client_fd > 0)
        close(client_fd);

    return EXIT_SUCCESS;
}
