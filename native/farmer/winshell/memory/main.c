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

// #test
// The client-side library
#include <libgui.h>


#include "memory.h" // Optional: define colors or prototypes if you like

// Global display pointer
struct gws_display_d *Display;

struct dccanvas_d *dc00;  // shared dc

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
static struct button_info_d  MyButton_Refresh;
static struct button_info_d  MyButton_Close;

static int __hover_button_id = -1; // Invalidate.



// Window and control IDs
static int main_window      = -1;
//static int refresh_button   = -1;
//static int close_button     = -1;

// Global default responder 
//static int default_responder = -1;

// Cached frame/chrome area
static unsigned long frame_left   = 0;
static unsigned long frame_top    = 0;
static unsigned long frame_width  = 0;
static unsigned long frame_height = 0;

// Cached client area
static unsigned long cr_left   = 0;
static unsigned long cr_top    = 0;
static unsigned long cr_width  = 0;
static unsigned long cr_height = 0;

static void update_children(int fd);
static void set_default_responder(int wid);
static void switch_responder(int fd);
static void trigger_default_responder(int fd);
static void on_button_clicked(int fd, int id);
static int __hit_test_button(unsigned long rel_mx, unsigned long rel_my);

// ----------------------------------------------------
// Helpers
// ----------------------------------------------------

// Handle click events for components
static void on_button_clicked(int fd, int id)
{
    if (id < 0)
        return;

    switch (id)
    {
        case 1:  // MyButton_Refresh.icon_id
            //printf("Button %d clicked!\n", id);
            update_children(fd);
            break;

        case 2:  // MyButton_Close.icon_id
            //printf("Button %d clicked!\n", id);
            //gws_destroy_window(fd, refresh_button);
            //gws_destroy_window(fd, close_button);
            gws_destroy_window(fd, main_window);
            exit(0);
            break;

        default:
            printf("Unknown button clicked: %d\n", id);
            break;
    };
}

// Hit-test for our fake button in PowerApp
static int __hit_test_button(unsigned long rel_mx, unsigned long rel_my) 
{

// Button Restart
    if ( rel_mx >= MyButton_Refresh.left && 
         rel_mx <= MyButton_Refresh.left + MyButton_Refresh.width &&
         rel_my >= MyButton_Refresh.top  && 
         rel_my <= MyButton_Refresh.top + MyButton_Refresh.height )
    {
        return (int) MyButton_Refresh.button_id;
    }

// Button Shutdown
    if ( rel_mx >= MyButton_Close.left && 
         rel_mx <= MyButton_Close.left + MyButton_Close.width &&
         rel_my >= MyButton_Close.top  && 
         rel_my <= MyButton_Close.top + MyButton_Close.height )
    {
        return (int) MyButton_Close.button_id;
    }

    // ...

    return (int) -1;
}


static void query_client_area(int fd)
{
    struct gws_window_info_d wi;
    gws_get_window_info(fd, main_window, &wi);

// Cached frame/chrome area
    frame_left   = wi.left;
    frame_top    = wi.top;
    frame_width  = wi.width;
    frame_height = wi.height;

// Cached client area
    //cr_left = 0;
    //cr_top = 0;
    cr_left   = wi.cr_left;
    cr_top    = wi.cr_top;
    cr_width  = wi.cr_width;
    cr_height = wi.cr_height;
}

static void draw_label(int fd, int x, int y, const char *text)
{
    if ((void*) text == NULL)
        return;

    // gws_draw_text(fd, main_window, x, y, COLOR_BLACK, (char *)text);

    // Draw the label string inside
    libgui_drawstring(
        frame_left + cr_left + x,  //20, 
        frame_top  + cr_top  + y,  //20, 
        text,
        COLOR_BLACK, COLOR_GRAY, 0
    );
}

static void draw_value_line(int fd, int x, int y, const char *label, unsigned long value_kb)
{
    char line[128];
    sprintf(line, "%s: %lu KB", label, value_kb);

    //gws_draw_text(fd, main_window, x, y, COLOR_BLACK, line);

    // Draw the label string inside
    const char *label_chose = "System Memory Status: ";
    libgui_drawstring(
        frame_left + cr_left + x, 
        frame_top  + cr_top  + y, 
        line,
        COLOR_BLACK, COLOR_GRAY, 0
    );
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
    // gws_redraw_window(fd, main_window, TRUE);

    // Title/label
    // draw_label(fd, 20, label_y, "System Memory Status:");

    // Draw the label string inside
    const char *label_chose = "System Memory Status: ";

    /*
    libgui_drawstring(
        frame_left + cr_left + 20, 
        frame_top  + cr_top  + label_y,
        label_chose,
        COLOR_BLACK, COLOR_GRAY, 0
    );
    */

/*
    libgui_drawstringblock(
        frame_left + cr_left + 20,
        frame_top  + cr_top  + label_y,
        COLOR_BLACK, //0xFF888888,
        label_chose,
        2 );
*/

    // Metrics block
    draw_memory_metrics(fd, metrics_x, metrics_y, line_h);

// Move/resize/redraw buttons


// =====================================

// Refresh button

    //MyButton_Refresh.button_id = 1;   // arbitrary ID
    // MyButton_Refresh.wid     = main_window; // parent window ID

    // Relative values
    MyButton_Refresh.left = refresh_x; //4;
    MyButton_Refresh.top  = buttons_y; //4;

    // Absolute coordinates (relative to screen)
    MyButton_Refresh.absolute_left = frame_left + cr_left + MyButton_Refresh.left;
    MyButton_Refresh.absolute_top  = frame_top  + cr_top + MyButton_Refresh.top;
    MyButton_Refresh.width         = button_w; //32;
    MyButton_Refresh.height        = button_h; //24;

    //gws_change_window_position(fd, refresh_button, refresh_x, buttons_y);
    //gws_resize_window(fd, refresh_button, button_w, button_h);
    //gws_redraw_window(fd, refresh_button, TRUE);


// -------------------------------------------------------
// Refresh button

/*
    libgui_backbuffer_draw_rectangle0(
        MyButton_Refresh.absolute_left, 
        MyButton_Refresh.absolute_top, 
        MyButton_Refresh.width, 
        MyButton_Refresh.height,
        xCOLOR_GRAY2, 
        1, 0, FALSE
    );
    // Draw the label string inside
    const char *label_refresh = "REFRESH";
    libgui_drawstring(
        MyButton_Refresh.absolute_left +4, 
        MyButton_Refresh.absolute_top +4, 
        label_refresh,
        COLOR_BLACK, COLOR_GRAY, 0
    );
// Refresh to show it
    libgui_refresh_rectangle_via_kernel(
        MyButton_Refresh.absolute_left, 
        MyButton_Refresh.absolute_top, 
        MyButton_Refresh.width, 
        MyButton_Refresh.height
    );
*/

// Drawing a fake button
    if ((void*)dc00 != NULL)
    {
       lingui_draw_rectangle0_dc (
            dc00,
            MyButton_Refresh.left, 
            MyButton_Refresh.top, 
            MyButton_Refresh.width, 
            MyButton_Refresh.height,
            COLOR_GRAY,
            0  // ROP
        );
        libgui_drawchar_dc ( 
            dc00, 
            MyButton_Refresh.left +2, 
            MyButton_Refresh.top +2,
            'R', COLOR_WHITE, COLOR_GRAY, 0 );
    }


// =====================================
// Close button

    //MyButton_Close.button_id = 2;   // arbitrary ID
    // MyButton_Close.wid     = main_window; // parent window ID

    // Relative values
    MyButton_Close.left = close_x; //4;
    MyButton_Close.top  = buttons_y; //4;

    // Absolute coordinates (relative to screen)
    MyButton_Close.absolute_left = frame_left + cr_left + MyButton_Close.left;
    MyButton_Close.absolute_top  = frame_top  + cr_top + MyButton_Close.top;
    MyButton_Close.width         = button_w; //32;
    MyButton_Close.height        = button_h; //24;

    //gws_change_window_position(fd, close_button, close_x, buttons_y);
    //gws_resize_window(fd, close_button, button_w, button_h);
    //gws_redraw_window(fd, close_button, TRUE);


/*
    libgui_backbuffer_draw_rectangle0(
        MyButton_Close.absolute_left, 
        MyButton_Close.absolute_top, 
        MyButton_Close.width, 
        MyButton_Close.height,
        xCOLOR_GRAY2, 
        1, 0, FALSE
    );
    // Draw the label string inside
    const char *label_close = "CLOSE";
    libgui_drawstring(
        MyButton_Close.absolute_left +4, 
        MyButton_Close.absolute_top +4, 
        label_close,
        COLOR_BLACK, COLOR_GRAY, 0
    );
// Refresh to show it
    libgui_refresh_rectangle_via_kernel(
        MyButton_Close.absolute_left, 
        MyButton_Close.absolute_top, 
        MyButton_Close.width, 
        MyButton_Close.height
    );
*/

// Drawing a fake button
    if ((void*)dc00 != NULL)
    {
       lingui_draw_rectangle0_dc (
            dc00,
            MyButton_Close.left, 
            MyButton_Close.top, 
            MyButton_Close.width, 
            MyButton_Close.height,
            COLOR_GRAY,
            0  // ROP
        );
        libgui_drawchar_dc ( 
            dc00, 
            MyButton_Close.left +2, 
            MyButton_Close.top +2,
            'R', COLOR_WHITE, COLOR_GRAY, 0 );
    }


    // Optional: ensure final composite
    // #todo: use kernel service
    //gws_refresh_window(fd, main_window);

// Refresh to show it
    libgui_refresh_rectangle_via_kernel(
        frame_left, 
        frame_top, 
        frame_width, 
        frame_height
    );

}

static void set_default_responder(int wid)
{
    //if (wid >= 0)
        //default_responder = wid;
}

static void switch_responder(int fd)
{
/*
    if (default_responder == refresh_button) {
        set_default_responder(close_button);
        gws_set_focus(fd, close_button);
    } else {
        set_default_responder(refresh_button);
        gws_set_focus(fd, refresh_button);
    }
*/
}

static void trigger_default_responder(int fd) 
{
/*
    if (default_responder == refresh_button) {
        update_children(fd);
    } else if (default_responder == close_button) {
        gws_destroy_window(fd, refresh_button);
        gws_destroy_window(fd, close_button);
        gws_destroy_window(fd, main_window);
        exit(0);
    }
*/
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
    int ButtonId = -1;

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
        //gws_destroy_window(fd, refresh_button);
        //gws_destroy_window(fd, close_button);
        //gws_destroy_window(fd, main_window);
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
        /*
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
        */
        break;

    // #test
    case MSG_MOUSEMOVE:
        // #bugbug
        // Kernel is sending us absolute values
        // instead of relative values.
        //printf("%d %d\n", long1, long2);
        ButtonId = (int) __hit_test_button(long1, long2);
        if (ButtonId > 0)
            __hover_button_id = ButtonId;
        if (ButtonId <= 0)
            __hover_button_id = -1;
        break;

    case MSG_MOUSERELEASED:
        printf("memory: Button released: %d\n", __hover_button_id);
        //printf("power: MSG_MOUSERELEASED:\n");
        on_button_clicked(fd, __hover_button_id);

        // #test: Testing the activation
        // We gotta do this only when the window is not active
        //gws_set_active( fd, main_window );
        //gws_refresh_window (fd, main_window);

        break;

    case MSG_CLOSE:
        //gws_destroy_window(fd, refresh_button);
        //gws_destroy_window(fd, close_button);
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


// =========================================
// Library initialization

    int status = -1;
    status = (int) libgui_initialize();
    if (status < 0){
        printf("memory: libgui_initialize fail\n");
        exit(1);
    }


    unsigned long mw_style = WS_APP;

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
            mw_style,  // style
            COLOR_WHITE, 
            COLOR_GRAY );

    if (main_window < 0) {
        printf("memory_app: Failed to create main window\n");
        return EXIT_FAILURE;
    }

    //gws_refresh_window(client_fd, main_window);


// -- CLient Area ---------------------

// Query client area for initial layout
    query_client_area(client_fd);


// ============================================================
// #test
// Update the wproxy structure that belongs to this thread.

    unsigned long m[10];
    int mytid = gettid();
    m[0] = (unsigned long) (mytid & 0xFFFFFFFF);

    // Frame/chrome rectangle
    m[1] = frame_left;
    m[2] = frame_top;
    m[3] = frame_width;
    m[4] = frame_height;

    // Client area rectangle
    m[5] = cr_left;
    m[6] = cr_top;
    m[7] = cr_width;
    m[8] = cr_height;

    sc80( 48, &m[0], &m[0], &m[0] );


// ================================================

// After creating main_window (second time)
    struct gws_window_info_d wi;
    gws_get_window_info(
        client_fd,
        main_window,
        (struct gws_window_info_d *) &wi );


    dc00 = (struct dccanvas_d *) libgui_create_dc(
        wi.ca_canvas_base_address,
        wi.ca_canvas_width,
        wi.ca_canvas_height,
        wi.ca_canvas_bpp
    );

// ================================================


    // Initial label (will be redrawn in paint anyway)
    // gws_draw_text( client_fd, main_window, 
        // 20, 20, COLOR_BLACK, "System Memory Status:");
    

    // Draw the label string inside
    const char *label_chose = "System Memory Status: ";

    /*
    libgui_drawstring(
        frame_left + cr_left + 20, 
        frame_top  + cr_top  + 20, 
        label_chose,
        COLOR_BLACK, COLOR_GRAY, 0
    );
    */

/*
    libgui_drawstringblock(
        frame_left + cr_left + 20,
        frame_top  + cr_top  + 20,
        COLOR_BLACK, //0xFF888888,
        label_chose,
        2 );
*/


    // Button baseline sizes
    unsigned long button_w = cr_width / 5;
    unsigned long button_h = cr_height / 10;
    if (button_h < 32)
        button_h = 32;

    // Initial button positions (will be updated in paint)
    unsigned long refresh_x = (cr_width / 4) - (button_w / 2);
    unsigned long close_x   = (3 * cr_width / 4) - (button_w / 2);
    unsigned long buttons_y = cr_height - (button_h *2);

//
// Create buttons
//

// Refresh button

    MyButton_Refresh.button_id = 1;   // arbitrary ID
    // MyButton_Refresh.wid     = main_window; // parent window ID

    // Relative values
    MyButton_Refresh.left = refresh_x; //4;
    MyButton_Refresh.top  = buttons_y; //4;

    // Absolute coordinates (relative to screen)
    MyButton_Refresh.absolute_left = frame_left + cr_left + MyButton_Refresh.left;
    MyButton_Refresh.absolute_top  = frame_top  + cr_top + MyButton_Refresh.top;
    MyButton_Refresh.width         = button_w; //32;
    MyButton_Refresh.height        = button_h; //24;

/*
    refresh_button = 
        (int) gws_create_window(
            client_fd,
            WT_BUTTON,
            BS_DEFAULT,
            1,
            "Refresh",
            refresh_x, buttons_y, button_w, button_h,
            main_window, 
            WS_CHILD,
            COLOR_GRAY, COLOR_GRAY );

    gws_refresh_window(client_fd, refresh_button);
*/

// -------------------------------------------------------
// Refresh button

    /*
    libgui_backbuffer_draw_rectangle0(
        MyButton_Refresh.absolute_left, 
        MyButton_Refresh.absolute_top, 
        MyButton_Refresh.width, 
        MyButton_Refresh.height,
        xCOLOR_GRAY2, 
        1, 0, FALSE
    );
    // Draw the label string inside
    const char *label_refresh = "REFRESH";
    libgui_drawstring(
        MyButton_Refresh.absolute_left +4, 
        MyButton_Refresh.absolute_top +4, 
        label_refresh,
        COLOR_BLACK, COLOR_GRAY, 0
    );
    */

// Drawing a fake button
    if ((void*)dc00 != NULL)
    {
       lingui_draw_rectangle0_dc (
            dc00,
            MyButton_Refresh.left, 
            MyButton_Refresh.top, 
            MyButton_Refresh.width, 
            MyButton_Refresh.height,
            COLOR_GRAY,
            0  // ROP
        );
        libgui_drawchar_dc ( 
            dc00, 
            MyButton_Refresh.left +2, 
            MyButton_Refresh.top +2,
            'R', COLOR_WHITE, COLOR_GRAY, 0 );
    }


/*
// Refresh to show it
    libgui_refresh_rectangle_via_kernel(
        MyButton_Refresh.absolute_left, 
        MyButton_Refresh.absolute_top, 
        MyButton_Refresh.width, 
        MyButton_Refresh.height
    );
*/

// -------------------------------------------------------
// Close button

    MyButton_Close.button_id = 2;   // arbitrary ID
    // MyButton_Close.wid     = main_window; // parent window ID

    // Relative values
    MyButton_Close.left = close_x; //4;
    MyButton_Close.top  = buttons_y; //4;

    // Absolute coordinates (relative to screen)
    MyButton_Close.absolute_left = frame_left + cr_left + MyButton_Close.left;
    MyButton_Close.absolute_top  = frame_top  + cr_top + MyButton_Close.top;
    MyButton_Close.width         = button_w; //32;
    MyButton_Close.height        = button_h; //24;

/*
    close_button = 
        (int) gws_create_window(
            client_fd,
            WT_BUTTON,
            BS_DEFAULT,
            1,
            "Close",
            close_x, buttons_y, button_w, button_h,
            main_window, 
            WS_CHILD,
            COLOR_GRAY, COLOR_GRAY );
    gws_refresh_window(client_fd, close_button);
*/

/*
    libgui_backbuffer_draw_rectangle0(
        MyButton_Close.absolute_left, 
        MyButton_Close.absolute_top, 
        MyButton_Close.width, 
        MyButton_Close.height,
        xCOLOR_GRAY2, 
        1, 0, FALSE
    );

    // Draw the label string inside
    const char *label_close = "CLOSE";
    libgui_drawstring(
        MyButton_Close.absolute_left +4, 
        MyButton_Close.absolute_top +4, 
        label_close,
        COLOR_BLACK, COLOR_GRAY, 0
    );
// Refresh to show it
    libgui_refresh_rectangle_via_kernel(
        MyButton_Close.absolute_left, 
        MyButton_Close.absolute_top, 
        MyButton_Close.width, 
        MyButton_Close.height
    );
*/

// Drawing a fake button
    if ((void*)dc00 != NULL)
    {
       lingui_draw_rectangle0_dc (
            dc00,
            MyButton_Close.left, 
            MyButton_Close.top, 
            MyButton_Close.width, 
            MyButton_Close.height,
            COLOR_GRAY,
            0  // ROP
        );
        libgui_drawchar_dc ( 
            dc00, 
            MyButton_Close.left +2, 
            MyButton_Close.top +2,
            'R', COLOR_WHITE, COLOR_GRAY, 0 );
    }

// Default responder
    //set_default_responder(refresh_button);

// Main window
    gws_set_active(client_fd, main_window);
    //gws_refresh_window(client_fd, main_window);

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

    int nSysMsg = 0;

    while (1){

    // 1. Pump events from Display Server
    pump(client_fd);

    // 2. Pump events from Input Broker (system events)
    for (nSysMsg=0; nSysMsg<32; nSysMsg++){
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

    };

    return EXIT_SUCCESS;
}
