// main.c - sysinfo.bin application (DC-aware)
// Gramado OS client-side GUI app showing system information.

#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <rtl/gramado.h>

#include <gws.h>
#include <libgui.h>

// Globals
struct gws_display_d *Display;
struct dccanvas_d *dc00;

struct ui_component_d *uic_button_refresh;
struct ui_component_d *uic_button_close;


static int main_window    = -1;
static int refresh_button = -1;
static int close_button   = -1;
static int default_responder = -1;

// Cached client area
static unsigned long cr_left   = 0;
static unsigned long cr_top    = 0;
static unsigned long cr_width  = 0;
static unsigned long cr_height = 0;

// ----------------------------------------------------
// Helpers
// ----------------------------------------------------

static void query_client_area(int fd)
{
    struct gws_window_info_d wi;
    gws_get_window_info(fd, main_window, &wi);

    cr_left   = wi.cr_left;
    cr_top    = wi.cr_top;
    cr_width  = wi.cr_width;
    cr_height = wi.cr_height;
}

static void draw_label_dc(
    struct dccanvas_d *dc,
    int x, int y,
    const char *label,
    const char *value )
{
    char line[256];
    sprintf(line, "%s: %s", label, value);
    libgui_drawstring_dc(dc, x, y, COLOR_BLACK, COLOR_WHITE, 0, line);
}


static void draw_system_info_dc(struct dccanvas_d *dc, int base_x, int base_y, int line_h)
{
    struct utsname un;
    uname(&un);

    unsigned long screen_w = gws_get_system_metrics(1);
    unsigned long screen_h = gws_get_system_metrics(2);
    unsigned long total_mem = gws_get_system_metrics(33);
    unsigned long used_mem  = gws_get_system_metrics(34);
    unsigned long free_mem  = gws_get_system_metrics(35);

    int y = base_y;

    draw_label_dc(dc, base_x, y, "System",   un.sysname);   y += line_h;
    draw_label_dc(dc, base_x, y, "Release",  un.release);   y += line_h;
    draw_label_dc(dc, base_x, y, "Version",  un.version);   y += line_h;
    draw_label_dc(dc, base_x, y, "Machine",  un.machine);   y += line_h;
    draw_label_dc(dc, base_x, y, "Node",     un.nodename);  y += line_h;
    draw_label_dc(dc, base_x, y, "Domain",   un.domainname);y += line_h;

    char buf[64];
    sprintf(buf, "%lux%lu", screen_w, screen_h);
    draw_label_dc(dc, base_x, y, "Screen", buf); y += line_h;

    sprintf(buf, "%lu KB", total_mem);
    draw_label_dc(dc, base_x, y, "Total Memory", buf); y += line_h;
    sprintf(buf, "%lu KB", used_mem);
    draw_label_dc(dc, base_x, y, "Used Memory", buf); y += line_h;
    sprintf(buf, "%lu KB", free_mem);
    draw_label_dc(dc, base_x, y, "Free Memory", buf); y += line_h;
}

// ----------------------------------------------------
// Worker: update children on paint
// ----------------------------------------------------

static void update_children(int fd)
{
    // Only server call here
    query_client_area(fd);

    unsigned long button_w = cr_width / 5;
    unsigned long button_h = cr_height / 10;
    if (button_h < 32) button_h = 32;

    unsigned long label_y   = 20;
    unsigned long metrics_x = 20;
    unsigned long metrics_y = label_y + 30;
    unsigned long line_h    = 20;

    unsigned long refresh_x = (cr_width / 4) - (button_w / 2);
    unsigned long close_x   = (3 * cr_width / 4) - (button_w / 2);
    unsigned long buttons_y = cr_height - (button_h*2);

    // Local DC drawing only
    lingui_draw_rectangle0_dc(dc00, 0, 0, cr_width, cr_height, COLOR_WHITE, 0);

    libgui_drawstring_dc(dc00, 20, label_y, COLOR_BLACK, COLOR_WHITE, 0,
                         "System Information:");

    draw_system_info_dc(dc00, metrics_x, metrics_y, line_h);

    // Buttons: redraw locally
    libgui_set_ui_component_position(uic_button_refresh, refresh_x, buttons_y);
    libgui_set_ui_component_dimension(uic_button_refresh, button_w, button_h);
    libgui_redraw_ui_component(uic_button_refresh, dc00);

    libgui_set_ui_component_position(uic_button_close, close_x, buttons_y);
    libgui_set_ui_component_dimension(uic_button_close, button_w, button_h);
    libgui_redraw_ui_component(uic_button_close, dc00);
}

// ----------------------------------------------------
// Procedure: handles events
// ----------------------------------------------------

static int systemProcedure(int fd, int event_window, int event_type,
                           unsigned long long1, unsigned long long2)
{
    if (fd < 0 || event_window < 0 || event_type < 0) return -1;

    switch (event_type)
    {
    case MSG_KEYDOWN:
        switch (long1) {
        case VK_RETURN: update_children(fd); break;
        case 'R': case 'r': update_children(fd); break;
        case 'C': case 'c': gws_destroy_window(fd, main_window); exit(0); break;
        }
        break;

    case MSG_SYSKEYDOWN:
        switch (long1) {
        case VK_F5: update_children(fd); break;
        case VK_F12: printf("sysinfo: Debug info\n"); break;
        }
        break;

    case MSG_PAINT:
        update_children(fd);
        break;

    case MSG_CLOSE:
        gws_destroy_window(fd, main_window);
        exit(0);
        break;
    }
    return 0;
}

// ----------------------------------------------------
// Pump
// ----------------------------------------------------

static void pump(int fd)
{
    struct gws_event_d event;
    struct gws_event_d *e =
        (struct gws_event_d *) gws_get_next_event(fd, main_window, &event);

    if (!e || e->magic != 1234 || e->used != TRUE) return;
    if (e->type <= 0) return;

    systemProcedure(fd, e->window, e->type, e->long1, e->long2);
}

// ----------------------------------------------------
// Main
// ----------------------------------------------------

int main(int argc, char *argv[])
{
    // Connect to display server
    Display = gws_open_display("display:name.0");
    if (!Display) return EXIT_FAILURE;
    int fd = Display->fd;

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
    main_window = gws_create_window(fd, WT_OVERLAPPED,
        WINDOW_STATUS_ACTIVE, WINDOW_STATE_NORMAL,
        "System Information",
        win_x, win_y, win_w, win_h,
        0, WS_APP, COLOR_WINDOW, COLOR_WINDOW);

    if (main_window < 0) {
        printf("sysinfo: main_window fail\n");
        exit(0);
    }
    gws_refresh_window(fd, main_window);

    // Query client area
    query_client_area(fd);

    // Create DC for client area
    struct gws_window_info_d wi;
    gws_get_window_info(fd, main_window, &wi);
    dc00 = libgui_create_dc(wi.ca_canvas_base_address,
                            wi.ca_canvas_width,
                            wi.ca_canvas_height,
                            wi.ca_canvas_bpp);
    if (!dc00) {
        printf("sysinfo: dc00 fail\n");
        exit(1);
    }

    // Buttons
    unsigned long button_w = cr_width / 5;
    unsigned long button_h = cr_height / 10;
    if (button_h < 32) button_h = 32;

    unsigned long refresh_x = (cr_width / 4) - (button_w / 2);
    unsigned long close_x   = (3 * cr_width / 4) - (button_w / 2);
    unsigned long buttons_y = cr_height - (button_h*2);

    // Create Refresh button
    uic_button_refresh = libgui_create_ui_component(dc00, 1,  // type = button
        refresh_x, buttons_y, button_w, button_h, "Refresh");

    // Create Close button
    uic_button_close = libgui_create_ui_component(dc00, 1,    // type = button
        close_x, buttons_y, button_w, button_h, "Close");

    // Default responder
    default_responder = refresh_button;

    // Activate main window
    gws_set_active(fd, main_window);
    gws_set_focus(fd, refresh_button);

    // Event loop
    int nSysMsg = 0;
    while (1) {
        // Pump events from Display Server
        pump(fd);

        // Pump events from Input Broker (system events)
        for (nSysMsg = 0; nSysMsg < 32; nSysMsg++) {
            if (rtl_get_event() == TRUE) {
                systemProcedure(fd,
                    (int) RTLEventBuffer[0],
                    (int) RTLEventBuffer[1],
                    (unsigned long) RTLEventBuffer[2],
                    (unsigned long) RTLEventBuffer[3]);
                RTLEventBuffer[1] = 0; // clear after dispatch
            }
        }
    }

    return EXIT_SUCCESS;
}
