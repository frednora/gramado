// main.c
// Gramado OS client-side GUI app showing system memory information.
// Created by Fred Nora (example by Copilot).

// rtl
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <rtl/gramado.h>
// libgws - The client-side library.
#include <gws.h>

#include "memory.h" // Optional: define colors or prototypes if you like

// Global display pointer
struct gws_display_d *Display;

// Window and control IDs
static int main_window      = -1;
static int refresh_button   = -1;
static int close_button     = -1;

// Global default responder 
static int default_responder = -1;

// Cached layout info
static unsigned long cr_left   = 0;
static unsigned long cr_top    = 0;
static unsigned long cr_width  = 0;
static unsigned long cr_height = 0;


static void set_default_responder(int wid);
static void switch_responder(int fd);
static void trigger_default_responder(int fd);

// ----------------------------------------------------
// Helpers
// ----------------------------------------------------


static void query_client_area(int fd)
{
    struct gws_window_info_d wi;
    gws_get_window_info(fd, main_window, &wi);

    // These values are relatives
    // Lets use 0,0 here to avoid confusion
    //cr_left   = wi.cr_left;
    //cr_top    = wi.cr_top;
    cr_left   = 0;
    cr_top    = 0;

    cr_width  = wi.cr_width;
    cr_height = wi.cr_height;
}

static void draw_label(int fd, int x, int y, const char *text)
{
    gws_draw_text(fd, main_window, x, y, COLOR_BLACK, (char *)text);
}

static void draw_value_line(int fd, int x, int y, const char *label, unsigned long value_kb)
{
    char line[128];
    sprintf(line, "%s: %lu KB", label, value_kb);
    gws_draw_text(fd, main_window, x, y, COLOR_BLACK, line);
}

// ----------------------------------------------------
// Memory metrics
// ----------------------------------------------------

static void draw_memory_metrics(int fd, int base_x, int base_y, int line_h)
{
    unsigned long base   = gws_get_system_metrics(30);
    unsigned long other  = gws_get_system_metrics(31);
    unsigned long ext    = gws_get_system_metrics(32);
    unsigned long total  = gws_get_system_metrics(33);
    unsigned long used   = gws_get_system_metrics(34);
    unsigned long free_  = gws_get_system_metrics(35);

    int y = base_y;

    draw_value_line(fd, base_x, y,              "Base",     base);   y += line_h;
    draw_value_line(fd, base_x, y,              "Other",    other);  y += line_h;
    draw_value_line(fd, base_x, y,              "Extended", ext);    y += line_h;
    draw_value_line(fd, base_x, y,              "Total",    total);  y += line_h;
    draw_value_line(fd, base_x, y,              "Used",     used);   y += line_h;
    draw_value_line(fd, base_x, y,              "Free",     free_);  y += line_h;
}

// ----------------------------------------------------
// Worker: update children on paint
// ----------------------------------------------------

static void update_children(int fd)
{
    query_client_area(fd);

    // Compute responsive control sizes
    unsigned long button_w = cr_width / 5;
    unsigned long button_h = cr_height / 10;
    // minimum height
    if (button_h < 32) 
        button_h = 32;

    unsigned long label_y  = 20;
    unsigned long metrics_x = 20;
    unsigned long metrics_y = label_y + 30;
    unsigned long line_h    = 20;

    // Place buttons at bottom-left and bottom-right, centered vertically in bottom band
    unsigned long refresh_x     = (cr_width / 4) - (button_w / 2);
    unsigned long close_x       = (3 * cr_width / 4) - (button_w / 2);

    unsigned long bottom_band_y = cr_height - (button_h*2);
    unsigned long buttons_y     = bottom_band_y;

    // Redraw parent background
    gws_redraw_window(fd, main_window, TRUE);

    // Title/label
    draw_label(fd, 20, label_y, "System Memory Status:");

    // Metrics block
    draw_memory_metrics(fd, metrics_x, metrics_y, line_h);

    // Move/resize/redraw buttons
    gws_change_window_position(fd, refresh_button, refresh_x, buttons_y);
    gws_resize_window(fd, refresh_button, button_w, button_h);
    gws_redraw_window(fd, refresh_button, TRUE);

    gws_change_window_position(fd, close_button, close_x, buttons_y);
    gws_resize_window(fd, close_button, button_w, button_h);
    gws_redraw_window(fd, close_button, TRUE);

    // Optional: ensure final composite
    gws_refresh_window(fd, main_window);
}

static void set_default_responder(int wid)
{
    if (wid >= 0)
        default_responder = wid;
}

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

static void trigger_default_responder(int fd) 
{
    if (default_responder == refresh_button) {
        update_children(fd);
    } else if (default_responder == close_button) {
        gws_destroy_window(fd, refresh_button);
        gws_destroy_window(fd, close_button);
        gws_destroy_window(fd, main_window);
        exit(0);
    }
}

// ----------------------------------------------------
// Procedure: handles events sent by the display server
// ----------------------------------------------------

static int 
memoryProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 )
{
    if (fd < 0) return -1;
    if (event_window < 0) return -1;
    if (event_type < 0) return -1;

    switch (event_type) {

    case 0:
        // Null/heartbeat event
        return 0;

case MSG_KEYDOWN:
    switch (long1) {
    case VK_RETURN:  // Enter key
        trigger_default_responder(fd);
        break;

    case 'R':
    case 'r':
        update_children(fd);
        break;

    case 'C':
    case 'c':
        gws_destroy_window(fd, refresh_button);
        gws_destroy_window(fd, close_button);
        gws_destroy_window(fd, main_window);
        exit(0);
        break;
    }
    break;

    case MSG_SYSKEYDOWN:
        switch (long1) {
        case VK_F5:
            printf("memory_app: VK_F5  Refresh metrics\n");
            update_children(fd);
            return 0;
            break;

        case VK_F12:
            printf("memory_app: VK_F12  Debug info\n");
            // maybe dump stats or redraw
            return 0;
            break;

        case VK_F11:
            // Should not appear — broker intercepts fullscreen toggle
            printf("memory_app: VK_F11 (unexpected)\n");
            return 0;
            break;
        //
        case VK_ARROW_RIGHT:  printf("memory: VK_ARROW_RIGHT \n"); break;
        case VK_ARROW_UP:     printf("memory: VK_ARROW_UP \n");    break;
        case VK_ARROW_DOWN:   printf("memory: VK_ARROW_DOWN \n");  break;
        case VK_ARROW_LEFT:
            printf("memory: VK_ARROW_LEFT \n"); 
            switch_responder(fd);
            break;
        };
        break;

    case GWS_MouseClicked:
        // Child ID comes in long1
        if ((int)long1 == refresh_button) {
            // Just trigger a repaint to refresh metrics
            update_children(fd);
            return 0;
        }
        if ( (int)long1 == close_button ) {
            gws_destroy_window(fd, refresh_button);
            gws_destroy_window(fd, close_button);
            gws_destroy_window(fd, main_window);
            exit(0);
        }
        break;

    case MSG_CLOSE:
        gws_destroy_window(fd, refresh_button);
        gws_destroy_window(fd, close_button);
        gws_destroy_window(fd, main_window);
        exit(0);
        break;

    case MSG_PAINT:
        update_children(fd);
        return 0;

    default:
        // Unknown event
        return -1;
    };

    return 0;
}

// ----------------------------------------------------
// Pump: fetches events from the server and dispatches
// ----------------------------------------------------

static void pump(int fd)
{
    struct gws_event_d event;
    event.used  = FALSE;
    event.magic = 0;
    event.type  = 0;

    struct gws_event_d *e =
        (struct gws_event_d *) gws_get_next_event(
            fd, 
            (int) main_window, 
            (struct gws_event_d *) &event);

    if ((void*) e == NULL) return;
    if (e->magic != 1234 || e->used != TRUE) return;

    if (e->type <= 0) return;

    memoryProcedure(fd, e->window, e->type, e->long1, e->long2);
}

// ----------------------------------------------------
// Main
// ----------------------------------------------------

int main(int argc, char *argv[])
{
    const char *display_name = "display:name.0";
    int client_fd = -1;

    // Connect to display server
    Display = gws_open_display(display_name);
    if ((void*) Display == NULL) {
        printf("memory_app: Could not open display\n");
        return EXIT_FAILURE;
    }

    client_fd = Display->fd;
    if (client_fd <= 0) {
        printf("memory_app: Invalid fd\n");
        return EXIT_FAILURE;
    }

    // Screen size
    unsigned long screen_w = gws_get_system_metrics(1);
    unsigned long screen_h = gws_get_system_metrics(2);

    // Main window geometry
    unsigned long win_w = screen_w / 2;
    unsigned long win_h = screen_h / 2;
    unsigned long win_x = (screen_w - win_w) / 2;
    unsigned long win_y = (screen_h - win_h) / 2;

    main_window = 
        (int) gws_create_window(
            client_fd,
            WT_OVERLAPPED,         // type
            WINDOW_STATUS_ACTIVE,  // status
            WINDOW_STATE_NULL,     // state
            "Memory Information",
            win_x, win_y, win_w, win_h,
            0,
            0x0000,  // style
            COLOR_WHITE, 
            COLOR_GRAY );

    if (main_window < 0) {
        printf("memory_app: Failed to create main window\n");
        return EXIT_FAILURE;
    }

    gws_refresh_window(client_fd, main_window);

// -- CLient Area ---------------------

// Query client area for initial layout
    query_client_area(client_fd);

    // Initial label (will be redrawn in paint anyway)
    gws_draw_text( client_fd, main_window, 
        20, 20, COLOR_BLACK, "System Memory Status:");

    // Button baseline sizes
    unsigned long button_w = cr_width / 5;
    unsigned long button_h = cr_height / 10;
    if (button_h < 32)
        button_h = 32;

    // Initial button positions (will be updated in paint)
    unsigned long refresh_x = (cr_width / 4) - (button_w / 2);
    unsigned long close_x   = (3 * cr_width / 4) - (button_w / 2);
    unsigned long buttons_y = cr_height - (button_h *2);

// Create buttons

    // Refresh button
    refresh_button = 
        (int) gws_create_window(
            client_fd,
            WT_BUTTON,
            BS_DEFAULT,
            1,
            "Refresh",
            refresh_x, buttons_y, button_w, button_h,
            main_window, 
            0,
            COLOR_GRAY, 
            COLOR_GRAY );

    gws_refresh_window(client_fd, refresh_button);

    // Close button
    close_button = 
        (int) gws_create_window(
            client_fd,
            WT_BUTTON,
            BS_DEFAULT,
            1,
            "Close",
            close_x, buttons_y, button_w, button_h,
            main_window, 
            0,
            COLOR_GRAY, 
            COLOR_GRAY );

    gws_refresh_window(client_fd, close_button);

// Default responder
    set_default_responder(refresh_button);

// Main window
    gws_set_active(client_fd, main_window);
    gws_refresh_window(client_fd, main_window);

// ================================
// Event loop

/*
// ================================
// #test
// Lets setup if we want to block on empty queue or not
// #todo: Create msgctl() api

    int rv = -1;
    rv = (int) sc80( 912, 1000, 1000, 1000 );  // Yes
    //rv = (int) sc80( 912, 1001, 1001, 1001 );  // No
    if (rv < 0){
        printf ("on sc80:912\n");
        exit(0);
    }
*/

    while (1){

    // 1. Pump events from Display Server
    pump(client_fd);

    // 2. Pump events from Input Broker (system events)
    if (rtl_get_event() == TRUE)
    {
        memoryProcedure(
            client_fd,
            (int) RTLEventBuffer[0],   // window id
            (int) RTLEventBuffer[1],   // event type (MSG_SYSKEYDOWN, MSG_SYSKEYUP, etc.)
            (unsigned long) RTLEventBuffer[2], // VK code
            (unsigned long) RTLEventBuffer[3]  // scancode
        );
        RTLEventBuffer[1] = 0; // clear after dispatch
    }
    };

    return EXIT_SUCCESS;
}
