// main.c - sysinfo.bin application (DC-aware) (v2)
// Gramado OS client-side GUI app showing system information.
// Created by Fred Nora.
//
// Rebuilt on the same routines as the Power Manager / Launch examples:
//  - on_button_pressed / on_button_released / on_button_clicked
//  - hover tracking on MSG_MOUSEMOVE
//  - press effect on MSG_MOUSEPRESSED, action on MSG_MOUSERELEASED
//  - keyboard: press effect on MSG_KEYDOWN, action on MSG_KEYUP
//  - the same event loop: shared-flags blit check, then rtl_get_event()
//    handing the event to the procedure. No display-server pump.
//
// What is new compared to the old sysinfo:
//  - The buttons work. The old version had no mouse handling at all.
//  - The information is cached in a snapshot. MSG_PAINT only draws it
//    (no syscalls). Refresh (button, R, F5, Enter) is what queries again.
//  - The wproxy update (sc80 48) is done, like the other apps.
//    Without it the mouse events do not reach the app correctly.
//  - Text is clipped. libgui_drawstring_dc() does not clip, so a long
//    line used to write past the right edge of the buffer.
//  - Close quits the main loop. The old 'C' key called exit(0).
//  - Hover frame (blue) and keyboard focus frame (black).
//  - More information: bits per pixel and the percentage of memory in use.
//  - No printf in the event path (enable with SYSINFO_DEBUG).

#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <rtl/gramado.h>

#include <gws.h>
#include <libgui.h>

// Set to 1 to get messages on the console.
#define SYSINFO_DEBUG  0
#if SYSINFO_DEBUG
#define SYSINFO_LOG(...)  printf(__VA_ARGS__)
#else
#define SYSINFO_LOG(...)  ((void)0)
#endif

// Frame colors (drawn just outside the button)
#define FRAME_THICKNESS   2
#define FRAME_HOVER_COLOR  0x000060C0
#define FRAME_FOCUS_COLOR  0x00000000
#define FRAME_NONE_COLOR   COLOR_WHITE   // same as the client background

#define CHAR_WIDTH   8
#define CHAR_HEIGHT  8

static int isTimeToQuit = FALSE;

// Globals
struct gws_display_d *Display;
struct dccanvas_d *dc00;

static int client_fd   = -1;
static int main_window = -1;

static unsigned long __sh_flags = 0;

// ----------------------------------------------------
// The buttons
// ----------------------------------------------------
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

// button_id = index + 1
#define BTN_REFRESH  1
#define BTN_CLOSE    2

struct sysinfo_item_d
{
    struct button_info_d info;
    struct ui_component_d *uic;
    const char *label;
    char key;              // shortcut (lowercase)
};

static struct sysinfo_item_d  ItemList[] = {
    { {0}, NULL, "Refresh", 'r' },
    { {0}, NULL, "Close",   'c' }
};
#define ITEM_COUNT  (int)(sizeof(ItemList)/sizeof(ItemList[0]))

// ----------------------------------------------------
// State
// ----------------------------------------------------
static int __hover_button_id  = -1;   // Invalidate.
static int __mouse_pressed_id = -1;   // Pressed with the mouse
static int __key_pressed_id   = -1;   // Pressed with the keyboard
static int __focus_button_id  = BTN_REFRESH;   // Keyboard focus (Enter)

// Position of the pointer at the time of the event.
// Filled by the input loop from the event buffer.
static unsigned long last_x = 0;
static unsigned long last_y = 0;

// ----------------------------------------------------
// The information (snapshot)
// ----------------------------------------------------
struct sysinfo_snapshot_d
{
    struct utsname un;
    unsigned long screen_w;
    unsigned long screen_h;
    unsigned long bpp;
    unsigned long total_mem;   // KB
    unsigned long used_mem;    // KB
    unsigned long free_mem;    // KB
    unsigned long refresh_count;
};
static struct sysinfo_snapshot_d  Info;

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
sysinfo_fill_rect(
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

// libgui_drawstring_dc() does not clip, so we cut the string here.
// It must fit in the width (max_x) and in the height (max_y).
static void 
draw_text_clipped(
    unsigned long x, 
    unsigned long y, 
    unsigned long max_x, 
    unsigned long max_y,
    const char *text )
{
    char buf[160];
    unsigned long max_chars;

    if ((void*) dc00 == NULL || (void*) text == NULL)
        return;
    if (x >= max_x)
        return;
    if (y + CHAR_HEIGHT > max_y)
        return;

    max_chars = (max_x - x) / CHAR_WIDTH;
    if (max_chars > (sizeof(buf) - 1))
        max_chars = (sizeof(buf) - 1);
    if (max_chars == 0)
        return;

    strncpy(buf, text, max_chars);
    buf[max_chars] = '\0';

    libgui_drawstring_dc(dc00, x, y, COLOR_BLACK, COLOR_WHITE, 0, buf);
}

// =====================================================
// Frames: hover (blue) and keyboard focus (black)
// =====================================================

// Redraw the frame around one button, given its id.
// The color depends on the state: hover wins over focus.
static void draw_item_frame(int button_id)
{
    struct sysinfo_item_d *it;
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

    sysinfo_fill_rect(x,         y,         w, t,         color);  // top
    sysinfo_fill_rect(x,         y + h - t, w, t,         color);  // bottom
    sysinfo_fill_rect(x,         y + t,     t, h - (t*2), color);  // left
    sysinfo_fill_rect(x + w - t, y + t,     t, h - (t*2), color);  // right
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
// The information
// =====================================================

// Query the system and store the result in the snapshot.
// This is the only place that makes these requests.
static void sysinfo_refresh(void)
{
    memset(&Info.un, 0, sizeof(Info.un));
    uname(&Info.un);

    Info.screen_w  = gws_get_system_metrics(1);
    Info.screen_h  = gws_get_system_metrics(2);
    Info.bpp       = gws_get_system_metrics(9);
    Info.total_mem = gws_get_system_metrics(33);
    Info.used_mem  = gws_get_system_metrics(34);
    Info.free_mem  = gws_get_system_metrics(35);

    Info.refresh_count++;
}

// One "Label: value" line. Advances *y.
static void 
draw_info_line(
    unsigned long x, 
    unsigned long *y, 
    unsigned long line_h,
    unsigned long max_x,
    unsigned long max_y,
    const char *label, 
    const char *value )
{
    char line[160];

    // The precision keeps a bad string from overflowing the line.
    sprintf(line, "%s: %s", label, value);
    draw_text_clipped(x, *y, max_x, max_y, line);
    *y += line_h;
}

// Draw the snapshot. No syscalls here.
static void 
draw_system_info(
    unsigned long base_x, 
    unsigned long base_y, 
    unsigned long line_h,
    unsigned long max_x,
    unsigned long max_y )
{
    char buf[64];
    unsigned long y = base_y;
    unsigned long percent = 0;

    draw_info_line(base_x, &y, line_h, max_x, max_y, "System",  Info.un.sysname);
    draw_info_line(base_x, &y, line_h, max_x, max_y, "Release", Info.un.release);
    draw_info_line(base_x, &y, line_h, max_x, max_y, "Version", Info.un.version);
    draw_info_line(base_x, &y, line_h, max_x, max_y, "Machine", Info.un.machine);
    draw_info_line(base_x, &y, line_h, max_x, max_y, "Node",    Info.un.nodename);
    draw_info_line(base_x, &y, line_h, max_x, max_y, "Domain",  Info.un.domainname);

    sprintf(buf, "%lux%lu, %lu bpp", 
        Info.screen_w, Info.screen_h, Info.bpp);
    draw_info_line(base_x, &y, line_h, max_x, max_y, "Screen", buf);

    sprintf(buf, "%lu KB", Info.total_mem);
    draw_info_line(base_x, &y, line_h, max_x, max_y, "Total Memory", buf);

    sprintf(buf, "%lu KB", Info.used_mem);
    draw_info_line(base_x, &y, line_h, max_x, max_y, "Used Memory", buf);

    sprintf(buf, "%lu KB", Info.free_mem);
    draw_info_line(base_x, &y, line_h, max_x, max_y, "Free Memory", buf);

    if (Info.total_mem > 0)
        percent = (Info.used_mem * 100) / Info.total_mem;
    sprintf(buf, "%lu percent", percent);
    draw_info_line(base_x, &y, line_h, max_x, max_y, "Memory In Use", buf);
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
    switch (button_id)
    {
        case BTN_REFRESH:
            SYSINFO_LOG("sysinfo: Refresh\n");
            sysinfo_refresh();
            update_children(client_fd);   // Full repaint with the new values
            break;

        case BTN_CLOSE:
            SYSINFO_LOG("sysinfo: Close\n");
            isTimeToQuit = TRUE;
            break;

        default:
            break;
    };
}

// Hit-test for the buttons of the sysinfo app
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
// from the client area. Evenly spaced near the bottom of the window.
static void layout_items(struct gws_window_info_d *wi)
{
    int i;
    unsigned long button_w = wi->cr_width / 5;
    unsigned long button_h = wi->cr_height / 10;
    unsigned long button_y;
    unsigned long center_x;

    if (button_h < 32) 
        button_h = 32;

    // Do not let it wrap around in a tiny window
    if (wi->cr_height > (button_h * 2))
        button_y = wi->cr_height - (button_h * 2);
    else
        button_y = 0;

    for (i = 0; i < ITEM_COUNT; i++)
    {
        // Centers at 1/4, 3/4 ... of the width (for two buttons)
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

// ----------------------------------------------------
// Worker: update children on paint
// ----------------------------------------------------
static void update_children(int fd)
{
    struct gws_window_info_d  wi;
    char title[64];
    int i;

    unsigned long label_y   = 20;
    unsigned long metrics_x = 20;
    unsigned long metrics_y = label_y + 30;
    unsigned long line_h    = 20;
    unsigned long text_limit_y;

    // Only server call here
    gws_get_window_info(fd, main_window, &wi);

    if ((void*)dc00 == NULL)
        return;

    layout_items(&wi);

    // The text must stay above the buttons (and their frames).
    text_limit_y = ItemList[0].info.top;
    if (text_limit_y > FRAME_THICKNESS)
        text_limit_y -= FRAME_THICKNESS;

    // Local DC drawing only
    sysinfo_fill_rect(0, 0, wi.cr_width, wi.cr_height, COLOR_WHITE);

    sprintf(title, "System Information (update %lu):", Info.refresh_count);
    draw_text_clipped(20, label_y, wi.cr_width, text_limit_y, title);

    draw_system_info(
        metrics_x, metrics_y, line_h, wi.cr_width, text_limit_y );

    // Buttons: redraw locally.
    // The component keeps its status, so a pressed button stays pressed.
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
        libgui_set_ui_component_flags(ItemList[i].uic, (0x0001 | 0x0002));
        libgui_redraw_ui_component(ItemList[i].uic, dc00);
    }

    // Frames (hover and focus) go on top
    for (i = 0; i < ITEM_COUNT; i++)
        draw_item_frame(ItemList[i].info.button_id);
}

// ----------------------------------------------------
// Procedure: handles events
// ----------------------------------------------------
static int 
systemProcedure(
    int fd, 
    int event_window, 
    int event_type,
    unsigned long long1, 
    unsigned long long2 )
{
    int ButtonId = -1;

    if (fd < 0 || event_window < 0 || event_type < 0) 
        return -1;

    switch (event_type)
    {
    // Null event
    case 0:
        return 0;
        break;

    // Only draws what we have. It does not query the system.
    case MSG_PAINT:
        update_children(fd);
        return 0;
        break;

    // Keyboard: the press effect on key down,
    // the action on key up. Just like the Power example.
    case MSG_KEYDOWN:
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
        case VK_F5:  
            on_button_clicked(BTN_REFRESH);  
            return 0;

        case VK_F12: 
            SYSINFO_LOG("sysinfo: Debug info\n"); 
            return 0;

        // Move the keyboard focus. Enter activates it.
        case VK_ARROW_LEFT: 
            move_focus(-1);
            return 0;
        case VK_ARROW_RIGHT: 
            move_focus(+1);
            return 0;
        }
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
        isTimeToQuit = TRUE;
        return 0;
        break;
    }

    return 0;
}

// ----------------------------------------------------
// Main
// ----------------------------------------------------

int main(int argc, char *argv[])
{
    int i;

    isTimeToQuit = FALSE;

    // Connect to display server
    Display = gws_open_display("display:name.0");
    if (!Display) 
        return EXIT_FAILURE;

    client_fd = Display->fd;
    if (client_fd <= 0)
        return EXIT_FAILURE;

    // Screen size
    unsigned long screen_w = gws_get_system_metrics(1);
    unsigned long screen_h = gws_get_system_metrics(2);

    // Initialize libgui
    if (libgui_initialize() < 0) {
        printf("sysinfo: libgui_initialize fail\n");
        exit(1);
    }

    // Window geometry
    unsigned long win_w = (screen_w * 7) / 10;
    unsigned long win_h = (screen_h * 7) / 10;
    unsigned long win_x = (screen_w - win_w) / 2;
    unsigned long win_y = (screen_h - win_h) / 2;

    // Create main window
    main_window = gws_create_window(client_fd, WT_OVERLAPPED,
        WINDOW_STATUS_ACTIVE, WINDOW_STATE_NORMAL,
        "System Information",
        win_x, win_y, win_w, win_h,
        0, WS_APP, COLOR_WINDOW, COLOR_WINDOW);

    if (main_window < 0) {
        printf("sysinfo: main_window fail\n");
        exit(0);
    }
    gws_refresh_window(client_fd, main_window);

    // Get information about the window
    struct gws_window_info_d wi;
    gws_get_window_info(client_fd, main_window, &wi);

// ============================================================
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
// Create DC for client area

    dc00 = libgui_create_dc(
        wi.ca_canvas_base_address,
        wi.ca_canvas_width,
        wi.ca_canvas_height,
        wi.ca_canvas_bpp );
    if (!dc00) {
        printf("sysinfo: dc00 fail\n");
        exit(1);
    }

// First snapshot of the information
    memset(&Info, 0, sizeof(Info));
    sysinfo_refresh();

// ============================================================
// Create the buttons.
// The layout gives the position and dimension of each one.

    layout_items(&wi);

    for (i = 0; i < ITEM_COUNT; i++)
    {
        ItemList[i].uic = libgui_create_ui_component(
            dc00, 
            UI_COMPONENT_BUTTON,  // type 1 = button
            ItemList[i].info.left, 
            ItemList[i].info.top, 
            ItemList[i].info.width, 
            ItemList[i].info.height, 
            ItemList[i].label, 
            (0x0001|0x0002) );
        if ((void*) ItemList[i].uic == NULL){
            printf("sysinfo: on button %d\n", i);
            exit(1);
        }
    }

    // Activate main window
    gws_set_active(client_fd, main_window);

    // First paint
    update_children(client_fd);

//
// Event loop
//

// #ps:
// The purpose of this loop is only get system events,
// not to get events from the display server.
// We do this for performance reasons.

    while (1) {

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
            systemProcedure(
                client_fd,
                (int) RTLEventBuffer[0],
                (int) RTLEventBuffer[1],
                (unsigned long) RTLEventBuffer[2],
                (unsigned long) RTLEventBuffer[3] );
            RTLEventBuffer[1] = 0; // clear after dispatch
        }
    };

    if (isTimeToQuit == TRUE){
        printf("sysinfo: Close window\n");
        gws_destroy_window(client_fd, main_window);
    }

    if (client_fd > 0)
        close(client_fd);

    return EXIT_SUCCESS;
}
