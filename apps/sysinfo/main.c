// main.c - sysinfo.bin application
// Gramado OS client-side GUI app showing system information.
// Similar architecture to memory_app.c

#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <rtl/gramado.h>
#include <gws.h>

// Globals
struct gws_display_d *Display;

static int main_window    = -1;
static int refresh_button = -1;
static int close_button   = -1;
static int default_responder = -1;


// Cached client area
static unsigned long cr_left   = 0;
static unsigned long cr_top    = 0;
static unsigned long cr_width  = 0;
static unsigned long cr_height = 0;


static void query_client_area(int fd);
static void draw_label(int fd, int x, int y, const char *label, const char *value);
static void draw_system_info(int fd, int base_x, int base_y, int line_h);
static void update_children(int fd);
static void set_default_responder(int wid);
static void trigger_default_responder(int fd);
static void switch_responder(int fd);

static void exitProgram(int fd);

static int 
systemProcedure(
    int fd, 
    int event_window, 
    int event_type,
    unsigned long long1, 
    unsigned long long2 );

static void pump(int fd);

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

static void draw_label(int fd, int x, int y, const char *label, const char *value) 
{
    char line[256];
    sprintf(line, "%s: %s", label, value);
    gws_draw_text(fd, main_window, x, y, COLOR_BLACK, line);
}

static void draw_system_info(int fd, int base_x, int base_y, int line_h) 
{
    struct utsname un;
    uname(&un);

    unsigned long screen_w = gws_get_system_metrics(1);
    unsigned long screen_h = gws_get_system_metrics(2);
    unsigned long total_mem = gws_get_system_metrics(33);
    unsigned long used_mem  = gws_get_system_metrics(34);
    unsigned long free_mem  = gws_get_system_metrics(35);

    int y = base_y;

    draw_label(fd, base_x, y, "System",   un.sysname);   y += line_h;
    draw_label(fd, base_x, y, "Release",  un.release);   y += line_h;
    draw_label(fd, base_x, y, "Version",  un.version);   y += line_h;
    draw_label(fd, base_x, y, "Machine",  un.machine);   y += line_h;
    draw_label(fd, base_x, y, "Node",     un.nodename);  y += line_h;
    draw_label(fd, base_x, y, "Domain",   un.domainname);y += line_h;

    char buf[64];
    sprintf(buf, "%lux%lu", screen_w, screen_h);
    draw_label(fd, base_x, y, "Screen", buf); y += line_h;

    sprintf(buf, "%lu KB", total_mem);
    draw_label(fd, base_x, y, "Total Memory", buf); y += line_h;
    sprintf(buf, "%lu KB", used_mem);
    draw_label(fd, base_x, y, "Used Memory", buf); y += line_h;
    sprintf(buf, "%lu KB", free_mem);
    draw_label(fd, base_x, y, "Free Memory", buf); y += line_h;
}

// ----------------------------------------------------
// Worker: update children on paint
// ----------------------------------------------------

static void update_children(int fd) 
{
    query_client_area(fd);

    unsigned long button_w = cr_width / 5;
    unsigned long button_h = cr_height / 10;
    if (button_h < 32)
        button_h = 32;

    unsigned long label_y   = 20;
    unsigned long metrics_x = 20;
    unsigned long metrics_y = label_y + 30;
    unsigned long line_h    = 20;

    unsigned long bottom_band_y = cr_height - (button_h + 20);
    unsigned long refresh_x     = (cr_width / 4) - (button_w / 2);
    unsigned long close_x       = (3 * cr_width / 4) - (button_w / 2);
    unsigned long buttons_y     = bottom_band_y;


// Redraw raw main window (first time)
    gws_redraw_window(fd, main_window, TRUE);

// Redraw main label
    gws_draw_text(
        fd, 
        main_window, 
        20, label_y, 
        COLOR_BLACK, "System Information:");

// ?
    draw_system_info(fd, metrics_x, metrics_y, line_h);

// Change refresh button
    gws_change_window_position(fd, refresh_button, refresh_x, buttons_y);
    gws_resize_window(fd, refresh_button, button_w, button_h);
    gws_redraw_window(fd, refresh_button, FALSE);
    //gws_redraw_window(fd, refresh_button, TRUE);

//  Change close button
    gws_change_window_position(fd, close_button, close_x, buttons_y);
    gws_resize_window(fd, close_button, button_w, button_h);
    gws_redraw_window(fd, close_button, FALSE);
    //gws_redraw_window(fd, close_button, TRUE);

// Refresh main window (again)
    gws_refresh_window(fd, main_window);
}

static void set_default_responder(int wid)
{
    if (wid < 0)
        return;
    default_responder = wid;
}

static void trigger_default_responder(int fd) 
{
    if (default_responder == refresh_button) {
        update_children(fd);
    } else if (default_responder == close_button) {

        exitProgram(fd);
        //gws_destroy_window(fd, refresh_button);
        //gws_destroy_window(fd, close_button);
        //gws_destroy_window(fd, main_window);
        //exit(0);
    }
}

// Toggle between refresh_button and close_button
static void switch_responder(int fd)
{
    if (default_responder == refresh_button) {
        set_default_responder(close_button);
        gws_set_focus(fd, close_button);
    } else {
        set_default_responder(refresh_button);
        gws_set_focus(fd, refresh_button);
    }
}

static void exitProgram(int fd)
{
    if (fd<0)
        return;
    gws_destroy_window(fd, refresh_button);
    gws_destroy_window(fd, close_button);
    gws_destroy_window(fd, main_window);
    exit(0);
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
    if (fd < 0 || event_window < 0 || event_type < 0)
    {
        return -1;
    }

    switch (event_type) 
    {

    case MSG_KEYDOWN:
        switch (long1) {

        // First responder
        case VK_RETURN: 
            trigger_default_responder(fd); 
            break;

        // Refresh
        case 'R': 
        case 'r': 
            update_children(fd); 
            break;

        // Cancel
        case 'C': 
        case 'c':
            exitProgram(fd);
            break;

        // #test
        case VK_TAB: 
            printf("app: TAB received\n");
            //switch_responder(fd);
            break;
        }
        break;

    case MSG_SYSKEYDOWN:
        switch (long1){
        case VK_F5:  update_children(fd);  break; 
        case VK_F12:  printf("system_info_app: Debug info\n");  break;
        //
        case VK_ARROW_RIGHT:  printf("Editor: VK_ARROW_RIGHT \n"); break;
        case VK_ARROW_UP:     printf("Editor: VK_ARROW_UP \n");    break;
        case VK_ARROW_DOWN:   printf("Editor: VK_ARROW_DOWN \n");  break;
        case VK_ARROW_LEFT:
            printf("Editor: VK_ARROW_LEFT \n"); 
            switch_responder(fd);
            break;
        };
        break;

    case GWS_MouseClicked:
        if ((int)long1 == refresh_button)
            update_children(fd);
        if ((int)long1 == close_button){
            exitProgram(fd);
        }
        break;

    case MSG_CLOSE:
        exitProgram(fd);
        break;

    case MSG_PAINT:
        update_children(fd);
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

    // Display
    Display = gws_open_display("display:name.0");
    if (!Display) return EXIT_FAILURE;
    int fd = Display->fd;

    // Screen
    unsigned long screen_w = gws_get_system_metrics(1);
    unsigned long screen_h = gws_get_system_metrics(2);
    //unsigned long win_w = screen_w / 2;
    //unsigned long win_h = screen_h / 2;
    unsigned long win_w = (screen_w * 7) / 10;
    unsigned long win_h = (screen_h * 7) / 10;
    unsigned long win_x = (screen_w - win_w) / 2;
    unsigned long win_y = (screen_h - win_h) / 2;

// Main window

    main_window = 
    (int) gws_create_window(
        fd, 
        WT_OVERLAPPED,         // Window type
        WINDOW_STATUS_ACTIVE,  // Window status (active,inactive) / button state
        WINDOW_STATE_NORMAL,   // Window state (min,max, normal)
        "System Information", 
        win_x, win_y, win_w, win_h,
        0,        // Parent window (wid)
        WS_APP,   // Window style
        COLOR_WINDOW,  // Client area color (unused for overlapped) 
        COLOR_WINDOW   // bg color (unused for overlapped)
    );

    if (main_window < 0){
        printf("on main_window\n");
        exit(0);
    }
    gws_refresh_window(fd, main_window);

//-----------------------------

    query_client_area(fd);


//
// Buttons
//

    // Buttons
    unsigned long button_w = cr_width / 5;
    unsigned long button_h = cr_height / 10;
    if (button_h < 32) button_h = 32;
    unsigned long refresh_x = (cr_width / 4) - (button_w / 2);
    unsigned long close_x   = (3 * cr_width / 4) - (button_w / 2);
    unsigned long buttons_y = cr_height - (button_h + 20);


// Create Refresh button
    refresh_button = 
        gws_create_window(
            fd, 
            WT_BUTTON,    // Window type
            BS_DEFAULT,   // Window status / Button state
            1,            // Window state
            "Refresh", 
            refresh_x, buttons_y, button_w, button_h,
            main_window, 
            0,  // No style 
            COLOR_GRAY, COLOR_GRAY);
    //gws_refresh_window(fd, refresh_button);

// Create Close button
    close_button = gws_create_window(
        fd,
        WT_BUTTON,
        BS_DEFAULT,
        1,
        "Close",
        close_x, buttons_y, button_w, button_h,
        main_window, 0, COLOR_GRAY, COLOR_GRAY );
    //gws_refresh_window(fd, close_button);

// Default responder
    set_default_responder(refresh_button);
    //set_default_responder(close_button);

// Main window active
    gws_set_active(fd, main_window);
   gws_set_focus(fd, refresh_button);

// Refresh. (again)
    gws_refresh_window(fd, main_window);

/*
// ================================
// Lets say to the kernel that we want to receive the TAB event.
    sc80(
        912,    // syscall number
        2000,   // option
        2000,   // extra values
        0  );   // not used
*/
// Lets say to the kernel that we want to receive the TAB event.
    // rtl_msgctl(2000,2000);

// ================================
// Event loop

    while (1) {
        // 1. Pump events from Display Server
        pump(fd);

        // 2. Pump events from Input Broker (system events)
        if (rtl_get_event() == TRUE) {
            systemProcedure(
                fd,
                (int) RTLEventBuffer[0],   // window id
                (int) RTLEventBuffer[1],   // event type
                (unsigned long) RTLEventBuffer[2], // VK code
                (unsigned long) RTLEventBuffer[3]  // scancode
            );
            RTLEventBuffer[1] = 0; // clear after dispatch
        }
    }

    return EXIT_SUCCESS;
}
