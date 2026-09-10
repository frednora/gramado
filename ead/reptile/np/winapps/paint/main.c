// main.c
// Gramado OS client-side GUI app: Paint (simple sketchpad)
// Freehand drawing with mouse + colour toolbar.
// Respects the exact same event loop, DC, shared-flags and
// interaction pattern used by the Power and Launch examples.

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

#include "paint.h"

static int isTimeToQuit = FALSE;

// Global display pointer
struct gws_display_d *Display;

struct dccanvas_d *dc00;  // shared dc

static unsigned long __sh_flags = 0;

// Toolbar buttons
struct ui_component_d *uic_btn_clear;
struct ui_component_d *uic_btn_black;
struct ui_component_d *uic_btn_red;
struct ui_component_d *uic_btn_green;
struct ui_component_d *uic_btn_blue;
struct ui_component_d *uic_footer;

struct button_info_d
{
    int button_id;
    int wid;
    unsigned long absolute_left;
    unsigned long absolute_top;
    unsigned long width;
    unsigned long height;
    unsigned long left;
    unsigned long top;
    int state;
};
static struct button_info_d  MyBtn_Clear;
static struct button_info_d  MyBtn_Black;
static struct button_info_d  MyBtn_Red;
static struct button_info_d  MyBtn_Green;
static struct button_info_d  MyBtn_Blue;

static int __hover_button_id = -1;

// Drawing state
static int is_drawing = FALSE;
static unsigned long last_x = 0;
static unsigned long last_y = 0;
static unsigned long current_color = COLOR_BLACK;  // default pen

// Toolbar height
#define TOOLBAR_H  80

// Window
static int main_window = -1;

static void on_button_clicked(int id);
static int  __hit_test_button(unsigned long rel_mx, unsigned long rel_my);
static void update_children(int fd);
static void draw_pixel(unsigned long x, unsigned long y);
static void clear_canvas(void);

// =====================================================

/*
unsigned long
syscall3 (
    unsigned long num,
    unsigned long arg1,
    unsigned long arg2,
    unsigned long arg3 )
{
    unsigned long ret=0;
    asm volatile (
        "syscall \n"
        : "=a" (ret)
        : "a" (num), "D" (arg1), "S" (arg2), "d" (arg3)
        : "rcx", "r11", "memory"
    );
    return (unsigned long) ret;
}
*/

// ----------------------------------------------------
static void clear_canvas(void)
{
    struct gws_window_info_d wi;
    if ((void*)dc00 == NULL) return;

    gws_get_window_info(Display->fd, main_window, &wi);

    // Only the drawing area (above toolbar)
    lingui_draw_rectangle0_dc (
        dc00,
        0, 0,
        wi.cr_width,
        wi.cr_height - TOOLBAR_H,
        COLOR_WHITE,
        0 );
}

// Draw a small “pixel” (3×3 rect) at relative coordinates
static void draw_pixel(unsigned long x, unsigned long y)
{
    if ((void*)dc00 == NULL) return;

    // Keep the pixel inside the canvas (above toolbar)
    struct gws_window_info_d wi;
    gws_get_window_info(Display->fd, main_window, &wi);
    if (y >= (wi.cr_height - TOOLBAR_H - 2))
        return;

    lingui_draw_rectangle0_dc (
        dc00,
        x, y,
        3, 3,               // size of the “brush”
        current_color,
        0 );
}

// ----------------------------------------------------
static void on_button_clicked(int id)
{
    if (id < 0) return;

    switch (id)
    {
        case 1:  // Clear
            printf("paint: Clear canvas\n");
            clear_canvas();
            break;

        case 2:  // Black
            current_color = COLOR_BLACK;
            printf("paint: Pen = Black\n");
            break;

        case 3:  // Red
            current_color = COLOR_RED;
            printf("paint: Pen = Red\n");
            break;

        case 4:  // Green
            current_color = COLOR_GREEN;
            printf("paint: Pen = Green\n");
            break;

        case 5:  // Blue
            current_color = COLOR_BLUE;
            printf("paint: Pen = Blue\n");
            break;

        default:
            break;
    };
}

// Hit-test for toolbar buttons
static int __hit_test_button(unsigned long rel_mx, unsigned long rel_my)
{
    if ( rel_mx >= MyBtn_Clear.left &&
         rel_mx <= MyBtn_Clear.left + MyBtn_Clear.width &&
         rel_my >= MyBtn_Clear.top  &&
         rel_my <= MyBtn_Clear.top  + MyBtn_Clear.height )
        return MyBtn_Clear.button_id;

    if ( rel_mx >= MyBtn_Black.left &&
         rel_mx <= MyBtn_Black.left + MyBtn_Black.width &&
         rel_my >= MyBtn_Black.top  &&
         rel_my <= MyBtn_Black.top  + MyBtn_Black.height )
        return MyBtn_Black.button_id;

    if ( rel_mx >= MyBtn_Red.left &&
         rel_mx <= MyBtn_Red.left + MyBtn_Red.width &&
         rel_my >= MyBtn_Red.top  &&
         rel_my <= MyBtn_Red.top  + MyBtn_Red.height )
        return MyBtn_Red.button_id;

    if ( rel_mx >= MyBtn_Green.left &&
         rel_mx <= MyBtn_Green.left + MyBtn_Green.width &&
         rel_my >= MyBtn_Green.top  &&
         rel_my <= MyBtn_Green.top  + MyBtn_Green.height )
        return MyBtn_Green.button_id;

    if ( rel_mx >= MyBtn_Blue.left &&
         rel_mx <= MyBtn_Blue.left + MyBtn_Blue.width &&
         rel_my >= MyBtn_Blue.top  &&
         rel_my <= MyBtn_Blue.top  + MyBtn_Blue.height )
        return MyBtn_Blue.button_id;

    return -1;
}

// ----------------------------------------------------
// Only redraws the toolbar + footer.  The canvas is left intact
// so the user’s drawing survives MSG_PAINT.
static void update_children(int fd)
{
    struct gws_window_info_d wi;
    gws_get_window_info(fd, main_window, &wi);

    if ((void*)dc00 == NULL) return;

    unsigned long btn_w = (wi.cr_width - 40) / 5;
    unsigned long btn_h = 28;
    unsigned long btn_y = wi.cr_height - TOOLBAR_H + 6;
    unsigned long gap   = 8;

    // Toolbar background strip
    lingui_draw_rectangle0_dc (
        dc00,
        0, wi.cr_height - TOOLBAR_H,
        wi.cr_width, TOOLBAR_H,
        COLOR_GRAY,
        0 );

    // ---- Clear ----
    MyBtn_Clear.left = gap;
    MyBtn_Clear.top  = btn_y;
    MyBtn_Clear.width  = btn_w;
    MyBtn_Clear.height = btn_h;
    MyBtn_Clear.absolute_left = wi.left + wi.cr_left + MyBtn_Clear.left;
    MyBtn_Clear.absolute_top  = wi.top  + wi.cr_top  + MyBtn_Clear.top;

    libgui_set_ui_component_position(uic_btn_clear, MyBtn_Clear.left, MyBtn_Clear.top);
    libgui_set_ui_component_dimension(uic_btn_clear, btn_w, btn_h);
    libgui_set_ui_component_flags(uic_btn_clear, (0x0001 | 0x0002));
    libgui_redraw_ui_component(uic_btn_clear, dc00);

    // ---- Black ----
    MyBtn_Black.left = gap + (btn_w + gap)*1;
    MyBtn_Black.top  = btn_y;
    MyBtn_Black.width  = btn_w;
    MyBtn_Black.height = btn_h;
    MyBtn_Black.absolute_left = wi.left + wi.cr_left + MyBtn_Black.left;
    MyBtn_Black.absolute_top  = wi.top  + wi.cr_top  + MyBtn_Black.top;

    libgui_set_ui_component_position(uic_btn_black, MyBtn_Black.left, MyBtn_Black.top);
    libgui_set_ui_component_dimension(uic_btn_black, btn_w, btn_h);
    libgui_set_ui_component_flags(uic_btn_black, (0x0001 | 0x0002));
    libgui_redraw_ui_component(uic_btn_black, dc00);

    // ---- Red ----
    MyBtn_Red.left = gap + (btn_w + gap)*2;
    MyBtn_Red.top  = btn_y;
    MyBtn_Red.width  = btn_w;
    MyBtn_Red.height = btn_h;
    MyBtn_Red.absolute_left = wi.left + wi.cr_left + MyBtn_Red.left;
    MyBtn_Red.absolute_top  = wi.top  + wi.cr_top  + MyBtn_Red.top;

    libgui_set_ui_component_position(uic_btn_red, MyBtn_Red.left, MyBtn_Red.top);
    libgui_set_ui_component_dimension(uic_btn_red, btn_w, btn_h);
    libgui_set_ui_component_flags(uic_btn_red, (0x0001 | 0x0002));
    libgui_redraw_ui_component(uic_btn_red, dc00);

    // ---- Green ----
    MyBtn_Green.left = gap + (btn_w + gap)*3;
    MyBtn_Green.top  = btn_y;
    MyBtn_Green.width  = btn_w;
    MyBtn_Green.height = btn_h;
    MyBtn_Green.absolute_left = wi.left + wi.cr_left + MyBtn_Green.left;
    MyBtn_Green.absolute_top  = wi.top  + wi.cr_top  + MyBtn_Green.top;

    libgui_set_ui_component_position(uic_btn_green, MyBtn_Green.left, MyBtn_Green.top);
    libgui_set_ui_component_dimension(uic_btn_green, btn_w, btn_h);
    libgui_set_ui_component_flags(uic_btn_green, (0x0001 | 0x0002));
    libgui_redraw_ui_component(uic_btn_green, dc00);

    // ---- Blue ----
    MyBtn_Blue.left = gap + (btn_w + gap)*4;
    MyBtn_Blue.top  = btn_y;
    MyBtn_Blue.width  = btn_w;
    MyBtn_Blue.height = btn_h;
    MyBtn_Blue.absolute_left = wi.left + wi.cr_left + MyBtn_Blue.left;
    MyBtn_Blue.absolute_top  = wi.top  + wi.cr_top  + MyBtn_Blue.top;

    libgui_set_ui_component_position(uic_btn_blue, MyBtn_Blue.left, MyBtn_Blue.top);
    libgui_set_ui_component_dimension(uic_btn_blue, btn_w, btn_h);
    libgui_set_ui_component_flags(uic_btn_blue, (0x0001 | 0x0002));
    libgui_redraw_ui_component(uic_btn_blue, dc00);

    // Footer (very bottom)
    libgui_set_ui_component_position(uic_footer, 0, wi.cr_height - 18);
    libgui_set_ui_component_dimension(uic_footer, wi.cr_width, 18);
    libgui_set_ui_component_flags(uic_footer, (0x0001 | 0x0002));
    libgui_redraw_ui_component(uic_footer, dc00);
}

// ----------------------------------------------------
static int
paintProcedure(
    int fd,
    int event_window,
    int event_type,
    unsigned long long1,
    unsigned long long2 )
{
    int ButtonId = -1;

    if (fd < 0) return -1;
    if (event_window < 0) return -1;
    if (event_type < 0) return -1;

    switch (event_type)
    {
    case 0:
        return 0;

    case MSG_PAINT:
        // Only refresh the toolbar – keep the drawing
        update_children(fd);
        return 0;

    case MSG_KEYDOWN:
        switch (long1)
        {
        case 'C':
        case 'c':
            clear_canvas();
            break;
        case 'K':
        case 'k':
            current_color = COLOR_BLACK;
            break;
        case 'R':
        case 'r':
            current_color = COLOR_RED;
            break;
        case 'G':
        case 'g':
            current_color = COLOR_GREEN;
            break;
        case 'B':
        case 'b':
            current_color = COLOR_BLUE;
            break;
        case 'Q':
        case 'q':
            gws_async_command(fd, 88, 0, 0);
            isTimeToQuit = TRUE;
            break;
        }
        break;

    case MSG_SYSKEYDOWN:
        // F1 = clear, F2-F5 = colours
        switch (long1)
        {
        case VK_F1: clear_canvas(); break;
        case VK_F2: current_color = COLOR_BLACK; break;
        case VK_F3: current_color = COLOR_RED;   break;
        case VK_F4: current_color = COLOR_GREEN; break;
        case VK_F5: current_color = COLOR_BLUE;  break;
        }
        break;

    case MSG_MOUSEMOVE:
        // long1/long2 are currently absolute – the hit-test still works
        // for the toolbar.  For drawing we use the relative values
        // that the procedure receives.
        ButtonId = __hit_test_button(long1, long2);
        if (ButtonId > 0)
            __hover_button_id = ButtonId;
        else
            __hover_button_id = -1;

        // Freehand drawing
        if (is_drawing)
        {
            draw_pixel(long1, long2);
            last_x = long1;
            last_y = long2;
        }
        break;

    case MSG_MOUSEPRESSED:
        // Start a stroke if we are over the canvas (not toolbar)
        {
            struct gws_window_info_d wi;
            gws_get_window_info(fd, main_window, &wi);
            if (long2 < (wi.cr_height - TOOLBAR_H))
            {
                is_drawing = TRUE;
                last_x = long1;
                last_y = long2;
                draw_pixel(long1, long2);
            }
        }
        break;

    case MSG_MOUSERELEASED:
        if (is_drawing)
        {
            is_drawing = FALSE;
        }
        else
        {
            // Click on a toolbar button
            printf("paint: Button released: %d\n", __hover_button_id);
            on_button_clicked(__hover_button_id);
        }
        break;

    case MSG_CLOSE:
        isTimeToQuit = TRUE;
        break;

    default:
        return -1;
    }

    return -1;
}

// ----------------------------------------------------
static void pump(int fd)
{
    struct gws_event_d event;
    event.used  = FALSE;
    event.magic = 0;
    event.type  = 0;

    struct gws_event_d *e;
    e = (struct gws_event_d *) gws_get_next_event(
            fd, (int) main_window, (struct gws_event_d *) &event);

    if ((void*)e == NULL) return;
    if (e->magic != 1234 || e->used != TRUE) return;
    if (e->type <= 0) return;

    paintProcedure(fd, e->window, e->type, e->long1, e->long2);
}

// ----------------------------------------------------
int main(int argc, char *argv[])
{
    const char *display_name = "display:name.0";
    int client_fd = -1;

    isTimeToQuit = FALSE;

    Display = gws_open_display(display_name);
    if ((void*)Display == NULL) {
        printf("paint: Could not open display\n");
        return EXIT_FAILURE;
    }

    client_fd = Display->fd;
    if (client_fd <= 0) {
        printf("paint: Invalid fd\n");
        return EXIT_FAILURE;
    }

    unsigned long screen_w = gws_get_system_metrics(1);
    unsigned long screen_h = gws_get_system_metrics(2);

    // Library init
    if (libgui_initialize() < 0) {
        printf("paint: libgui_initialize failed\n");
        exit(1);
    }

    // Main window
    //unsigned long win_w = screen_w / 2;
    //unsigned long win_h = screen_h / 2;
    //unsigned long win_x = screen_w / 4;
    //unsigned long win_y = screen_h / 4;

    // Main window
    unsigned long win_w = screen_w -20;
    unsigned long win_h = screen_h -40;
    unsigned long win_x = 10;
    unsigned long win_y = 10;

    main_window = (int) gws_create_window(
            client_fd,
            WT_OVERLAPPED,
            WINDOW_STATUS_ACTIVE,
            WINDOW_STATE_NULL,
            "Paint",
            win_x, win_y, win_w, win_h,
            0,
            WS_APP,
            COLOR_WHITE, COLOR_GRAY);

    if (main_window < 0) {
        printf("paint: Failed to create main window\n");
        return EXIT_FAILURE;
    }

    // Window info
    struct gws_window_info_d wi;
    gws_get_window_info(client_fd, main_window, &wi);

    // wproxy update
    unsigned long m[10];
    int mytid = gettid();
    m[0] = (unsigned long)(mytid & 0xFFFFFFFF);
    m[1] = wi.left;  m[2] = wi.top;
    m[3] = wi.width; m[4] = wi.height;
    m[5] = wi.cr_left;  m[6] = wi.cr_top;
    m[7] = wi.cr_width; m[8] = wi.cr_height;
    sc80(48, &m[0], &m[0], &m[0]);

    __sh_flags = (unsigned long) wi.sh_flags;

    // Device context for the client area
    dc00 = (struct dccanvas_d *) libgui_create_dc(
            wi.ca_canvas_base_address,
            wi.ca_canvas_width,
            wi.ca_canvas_height,
            wi.ca_canvas_bpp);
    if ((void*)dc00 == NULL) {
        printf("paint: dc00 failed\n");
        exit(1);
    }

    // Initial white canvas
    lingui_draw_rectangle0_dc(
            dc00,
            0, 0, wi.cr_width, wi.cr_height,
            COLOR_WHITE, 0);

    // Help text at the top
    libgui_drawstringblock_dc(
            dc00,
            8, 8,
            COLOR_BLACK,
            "Drag mouse to draw.  C=Clear  K/R/G/B=Colour  Q=Quit",
            1);

    // ---- Create toolbar buttons ----
    unsigned long btn_w = (wi.cr_width - 40) / 5;
    unsigned long btn_h = 28;
    unsigned long btn_y = wi.cr_height - TOOLBAR_H + 6;
    unsigned long gap   = 8;

    MyBtn_Clear.button_id = 1;
    MyBtn_Clear.left = gap;  MyBtn_Clear.top = btn_y;
    MyBtn_Clear.width = btn_w; MyBtn_Clear.height = btn_h;
    uic_btn_clear = libgui_create_ui_component(
            dc00, 1,
            MyBtn_Clear.left, MyBtn_Clear.top,
            MyBtn_Clear.width, MyBtn_Clear.height,
            "Clear", (0x0001 | 0x0002));

    MyBtn_Black.button_id = 2;
    MyBtn_Black.left = gap + (btn_w+gap)*1; MyBtn_Black.top = btn_y;
    MyBtn_Black.width = btn_w; MyBtn_Black.height = btn_h;
    uic_btn_black = libgui_create_ui_component(
            dc00, 1,
            MyBtn_Black.left, MyBtn_Black.top,
            MyBtn_Black.width, MyBtn_Black.height,
            "Black", (0x0001 | 0x0002));

    MyBtn_Red.button_id = 3;
    MyBtn_Red.left = gap + (btn_w+gap)*2; MyBtn_Red.top = btn_y;
    MyBtn_Red.width = btn_w; MyBtn_Red.height = btn_h;
    uic_btn_red = libgui_create_ui_component(
            dc00, 1,
            MyBtn_Red.left, MyBtn_Red.top,
            MyBtn_Red.width, MyBtn_Red.height,
            "Red", (0x0001 | 0x0002));

    MyBtn_Green.button_id = 4;
    MyBtn_Green.left = gap + (btn_w+gap)*3; MyBtn_Green.top = btn_y;
    MyBtn_Green.width = btn_w; MyBtn_Green.height = btn_h;
    uic_btn_green = libgui_create_ui_component(
            dc00, 1,
            MyBtn_Green.left, MyBtn_Green.top,
            MyBtn_Green.width, MyBtn_Green.height,
            "Green", (0x0001 | 0x0002));

    MyBtn_Blue.button_id = 5;
    MyBtn_Blue.left = gap + (btn_w+gap)*4; MyBtn_Blue.top = btn_y;
    MyBtn_Blue.width = btn_w; MyBtn_Blue.height = btn_h;
    uic_btn_blue = libgui_create_ui_component(
            dc00, 1,
            MyBtn_Blue.left, MyBtn_Blue.top,
            MyBtn_Blue.width, MyBtn_Blue.height,
            "Blue", (0x0001 | 0x0002));

    // Footer
    uic_footer = libgui_create_ui_component(
            dc00,
            UI_COMPONENT_FOOTER,
            0, wi.cr_height - 18,
            wi.cr_width, 18,
            "-- paint --",
            (0x0001 | 0x0002));

    // First toolbar paint
    update_children(client_fd);

    gws_set_active(client_fd, main_window);

    // -------------------- Event loop --------------------
    int nSysMsg = 0;
    while (1)
    {
        if (isTimeToQuit == TRUE)
            break;

        // Shared-flags blit handling (identical to Power / Launch)
        if (__sh_flags != 0)
        {
            char *flags_ptr = (char *) __sh_flags;
            if (*flags_ptr & 0x0008)
            {
                *flags_ptr &= ~0x0008;
                update_children(client_fd);
            }
        }

        // 1. Display-server events
        pump(client_fd);

        // 2. Input-broker events
        for (nSysMsg = 0; nSysMsg < 32; nSysMsg++)
        {
            if (rtl_get_event() == TRUE)
            {
                paintProcedure(
                    client_fd,
                    (int) RTLEventBuffer[0],
                    (int) RTLEventBuffer[1],
                    (unsigned long) RTLEventBuffer[2],
                    (unsigned long) RTLEventBuffer[3]);
                RTLEventBuffer[1] = 0;
            }
        }
    }

    if (isTimeToQuit == TRUE)
    {
        printf("Paint: Close window\n");
        gws_destroy_window(client_fd, main_window);
    }

    if (client_fd > 0)
        close(client_fd);

    return EXIT_SUCCESS;
}
