// main.c
// Gramado OS client-side GUI app with Restart and Shutdown buttons.
// Created by Fred Nora (example by Copilot).

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

#include "power.h"


// Global display pointer
struct gws_display_d *Display;


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
static struct button_info_d  MyButton_Restart;
static struct button_info_d  MyButton_Shutdown;

static int __hover_button_id = -1; // Invalidate.

// Window IDs
static int main_window     = -1;
// static int restart_button  = -1;
// static int shutdown_button = -1;

// Default responder (button to trigger on Enter)
static int default_responder = -1;

static void set_default_responder(int wid);
static void switch_responder(int fd);
static void trigger_default_responder(int fd);

static void on_button_clicked(int id);
static int __hit_test_button(unsigned long rel_mx, unsigned long rel_my);
static void update_children(int fd);

// =====================================================


// Handle click events for components
static void on_button_clicked(int id)
{
    if (id < 0)
        return;

    switch (id)
    {
        case 1:  // MyButton_Restart.icon_id
            printf("Button %d clicked!\n", id);
            rtl_clone_and_execute("reboot.bin");
            exit(0);
            break;

        case 2:  // MyButton_Shutdown.icon_id
            printf("Button %d clicked!\n", id);
            rtl_clone_and_execute("shutdown.bin");
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
    if ( rel_mx >= MyButton_Restart.left && 
         rel_mx <= MyButton_Restart.left + MyButton_Restart.width &&
         rel_my >= MyButton_Restart.top  && 
         rel_my <= MyButton_Restart.top + MyButton_Restart.height )
    {
        return (int) MyButton_Restart.button_id;
    }

// Button Shutdown
    if ( rel_mx >= MyButton_Shutdown.left && 
         rel_mx <= MyButton_Shutdown.left + MyButton_Shutdown.width &&
         rel_my >= MyButton_Shutdown.top  && 
         rel_my <= MyButton_Shutdown.top + MyButton_Shutdown.height )
    {
        return (int) MyButton_Shutdown.button_id;
    }

    // ...

    return -1;
}


static void update_children(int fd)
{
    struct gws_window_info_d  wi;

    // Get window info
    gws_get_window_info(fd, main_window, &wi);

    unsigned long button_w = wi.cr_width / 4;
    unsigned long button_h = wi.cr_height / 8;

    unsigned long button_y = (wi.cr_height - button_h) / 2;

    unsigned long restart_x  = (wi.cr_width / 4) - (button_w / 2);
    unsigned long shutdown_x = (3 * wi.cr_width / 4) - (button_w / 2);


// Move and redraw buttons

/*
    gws_change_window_position(fd, restart_button, restart_x, button_y);
    gws_resize_window(fd, restart_button, button_w, button_h);
    gws_redraw_window(fd, restart_button, TRUE);
*/

/*
    gws_change_window_position(fd, shutdown_button, shutdown_x, button_y);
    gws_resize_window(fd, shutdown_button, button_w, button_h);
    gws_redraw_window(fd, shutdown_button, TRUE);
*/

// Refresh main window
    // gws_refresh_window (fd, main_window);

//------------------------------------------
// #test

/*
    // Redraw label text
    gws_draw_text(
        fd, main_window, 20, 20, COLOR_BLACK,
        "Choose an action:"
    );
*/

    // Draw the label string inside
    const char *label_chose = "Choose an action: ";
    libgui_drawstring(
        wi.left + wi.cr_left +20, 
        wi.top + wi.cr_top +20, 
        label_chose,
        COLOR_BLACK, COLOR_GRAY, 0
    );

//
// Support for button positions and dimensions
//

// -------------------------------------------------
// Draw restart button

    // Relative values
    MyButton_Restart.left = restart_x; // 4;
    MyButton_Restart.top  = button_y;  // 4;

    // Update absolute values.
    MyButton_Restart.absolute_left = wi.left + wi.cr_left + MyButton_Restart.left;
    MyButton_Restart.absolute_top = wi.top + wi.cr_top + MyButton_Restart.top;

// Draw the fake button
    libgui_backbuffer_draw_rectangle0(
        MyButton_Restart.absolute_left, 
        MyButton_Restart.absolute_top, 
        MyButton_Restart.width, 
        MyButton_Restart.height,
        xCOLOR_GRAY2, 
        1, 0, FALSE
    );

    // Draw the label string inside
    const char *label_restart = "RESTART";
    libgui_drawstring(
        MyButton_Restart.absolute_left +4, 
        MyButton_Restart.absolute_top +4, 
        label_restart,
        COLOR_BLACK, COLOR_GRAY, 0
    );


// Refresh to show it
    libgui_refresh_rectangle_via_kernel(
        MyButton_Restart.absolute_left, 
        MyButton_Restart.absolute_top, 
        MyButton_Restart.width, 
        MyButton_Restart.height
    );


// -------------------------------------------------
// Draw shutdown button

    // Relative values
    MyButton_Shutdown.left = shutdown_x; // 4;
    MyButton_Shutdown.top  = button_y;  // 4;

    // Update absolute values.
    MyButton_Shutdown.absolute_left = wi.left + wi.cr_left + MyButton_Shutdown.left;
    MyButton_Shutdown.absolute_top = wi.top + wi.cr_top + MyButton_Shutdown.top;

// Draw the fake button
    libgui_backbuffer_draw_rectangle0(
        MyButton_Shutdown.absolute_left, 
        MyButton_Shutdown.absolute_top, 
        MyButton_Shutdown.width, 
        MyButton_Shutdown.height,
        xCOLOR_GRAY2,
        1, 0, FALSE
    );

    // Draw the label string inside
    const char *label_shutdown = "SHUTDOWN";
    libgui_drawstring(
        MyButton_Shutdown.absolute_left +4, 
        MyButton_Shutdown.absolute_top +4, 
        label_shutdown,
        COLOR_BLACK, COLOR_GRAY, 0
    );

// Refresh to show it
    libgui_refresh_rectangle_via_kernel(
        MyButton_Shutdown.absolute_left, 
        MyButton_Shutdown.absolute_top, 
        MyButton_Shutdown.width, 
        MyButton_Shutdown.height
    );

// #test
// Refresh the whole client window.
    libgui_refresh_rectangle_via_kernel(
        wi.left + wi.cr_left, 
        wi.top  + wi.cr_top, 
        wi.cr_width, 
        wi.cr_height
    );
}

static void set_default_responder(int wid)
{
    if (wid >= 0)
        default_responder = wid;
}

static void switch_responder(int fd)
{
/*
    if (default_responder == restart_button) {
        set_default_responder(shutdown_button);
        gws_set_focus(fd, shutdown_button);
    } else {
        set_default_responder(restart_button);
        gws_set_focus(fd, restart_button);
    }
*/
}

static void trigger_default_responder(int fd) 
{
/*
    if (default_responder == restart_button) {
        printf("PowerApp: Enter >> Restart\n");
        rtl_clone_and_execute("reboot.bin");
        exit(0);
    } else if (default_responder == shutdown_button) {
        printf("PowerApp: Enter >> Shutdown\n");
        rtl_clone_and_execute("shutdown.bin");
        exit(0);
    }
*/
}

// ----------------------------------------------------
// Procedure: handles events sent by the display server
// ----------------------------------------------------
static int 
powerProcedure(
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

    case MSG_KEYDOWN:
        switch (long1){

        case VK_RETURN:
            //trigger_default_responder(fd);
            break;

        case 'R':
        case 'r':
            printf("PowerApp: VK_F1 >> Restart\n");
            rtl_clone_and_execute("reboot.bin");
            // #todo: Close our windows.
            exit(0);
            break;

        case 'S':
        case 's':
            printf("PowerApp: VK_F2 >> Shutdown\n");
            rtl_clone_and_execute("shutdown.bin");
            // #todo: Close our windows.
            exit(0);
            break;
        };
        break;

    case MSG_SYSKEYDOWN:
        switch (long1) {
            case VK_F1:
                printf("PowerApp: VK_F1 >> Restart\n");
                rtl_clone_and_execute("reboot.bin");
                // #todo: Close our windows.
                exit(0);
                return 0;
            case VK_F2:
                printf("PowerApp: VK_F2 >> Shutdown\n");
                rtl_clone_and_execute("shutdown.bin");
                // #todo: Close our windows.
                exit(0);
                return 0;
            case VK_F11:
                // Should not appear — broker intercepts fullscreen toggle
                printf("PowerApp: VK_F11 (unexpected)\n");
                return 0;

            case VK_ARROW_LEFT: 
            case VK_ARROW_RIGHT: 
                switch_responder(fd); 
                break;

            // reserved for future
            case VK_ARROW_UP: 
            case VK_ARROW_DOWN: 
                printf("PowerApp: Arrow up/down pressed (no action yet)\n"); 
                break;
        };
        break;

    case GWS_MouseClicked:
        //printf("GWS_MouseClicked:\n");
        /*    
        printf("GWS_MouseClicked:\n");
        //printf("MouseClicked event_window = %d\n", event_window);
        // Debug dump of all parameters
        printf("[DEBUG] Event received:\n");
        printf("  fd          = %d\n", fd);
        printf("  event_window= %d\n", event_window);
        printf("  event_type  = %d\n", event_type);
        printf("  long1       = %lu\n", long1);
        printf("  long2       = %lu\n", long2);
        */

        /*
        // Use long1 as the clicked child window ID
        if ((int)long1 == restart_button)
        {
            printf("Restart button clicked\n");
            rtl_clone_and_execute("reboot.bin");
            return 0;
        }
        */

        /*
        if ((int)long1 == shutdown_button)
        {
            printf("Shutdown button clicked\n");
            rtl_clone_and_execute("shutdown.bin");
            return 0;
        }
        */

        //printf("GWS_MouseClicked: done\n");
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

        case MSG_MOUSEPRESSED:
            //printf("power: MSG_MOUSEPRESSED:\n");
            break;

        case MSG_MOUSERELEASED:
            printf("power: Button released: %d\n", __hover_button_id);
            //printf("power: MSG_MOUSERELEASED:\n");
            on_button_clicked(__hover_button_id);
            break;

    case MSG_CLOSE:
        //gws_destroy_window(fd, restart_button);
        //gws_destroy_window(fd, shutdown_button);
        gws_destroy_window(fd, main_window);
        printf("PowerApp: Window closed\n");
        exit(0);
        break;


    // Unknown event
    default:
        return -1;
        break;
    };

// Fail
    return (int) -1;
}

// ----------------------------------------------------
// Pump: fetches events from the server and dispatches
// ----------------------------------------------------
static void pump(int fd)
{
    struct gws_event_d event;
    event.used = FALSE;
    event.magic = 0;
    event.type = 0;
    //event.long1 = 0;
    //event.long2 = 0;

    struct gws_event_d *e;

    e = 
    (struct gws_event_d *) gws_get_next_event(
        fd, 
        (int) main_window, 
        (struct gws_event_d *) &event);

    if ((void*) e == NULL) return;
    if (e->magic != 1234 || e->used != TRUE) 
        return;

    if (e->type <= 0)
        return;
    powerProcedure(fd, e->window, e->type, e->long1, e->long2);
}

// ----------------------------------------------------
// Main function
// ----------------------------------------------------
int main(int argc, char *argv[])
{
    const char *display_name = "display:name.0";
    int client_fd = -1;

    // Connect to display server
    Display = gws_open_display(display_name);
    if ((void*) Display == NULL) {
        printf("power_app: Could not open display\n");
        return EXIT_FAILURE;
    }

    client_fd = Display->fd;
    if (client_fd <= 0) {
        printf("power: Invalid fd\n");
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
        printf("power_app: libgui_initialize fail\n");
        exit(1);
    }

// =========================================
// Main window
    unsigned long win_w = screen_w / 2;
    unsigned long win_h = screen_h / 2;
    unsigned long win_x = (screen_w - win_w) / 2;
    unsigned long win_y = (screen_h - win_h) / 2;

    main_window = 
        (int) gws_create_window(
                client_fd,
                WT_OVERLAPPED,
                WINDOW_STATUS_ACTIVE,  //status
                WINDOW_STATE_NULL,  //state
                "Power Manager",
                win_x, win_y, win_w, win_h,
                0,
                0x0000,  // style
                COLOR_WHITE, COLOR_GRAY );

    if (main_window < 0) {
        printf("power_app: Failed to create main window\n");
        return EXIT_FAILURE;
    }

    //#debug
    gws_refresh_window(client_fd, main_window);

// =============================


    //#debug
    //gws_refresh_window(client_fd, main_window);

// After creating main_window
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

// Text
/*
    gws_draw_text(
        client_fd,
        main_window,
        20, 20, COLOR_BLACK, "Choose an action:" 
    );
*/

    // Draw the label string inside
    const char *label_chose = "Choose an action: ";
    libgui_drawstring(
        wi.left + wi.cr_left +20, 
        wi.top + wi.cr_top +20, 
        label_chose,
        COLOR_BLACK, COLOR_GRAY, 0
    );


// ============================================================

//
// Support for button positions and dimensions
//

    unsigned long button_w = wi.cr_width / 4;
    unsigned long button_h = wi.cr_height / 8;
    unsigned long button_y = (wi.cr_height - button_h) / 2;

    unsigned long restart_x  = (wi.cr_width / 4) - (button_w / 2);
    unsigned long shutdown_x = (3 * wi.cr_width / 4) - (button_w / 2);

// ============================================================
// Create restart button

    MyButton_Restart.button_id = 1;   // arbitrary ID
    // MyButton_Restart.wid     = main_window; // parent window ID

    // Relative values
    MyButton_Restart.left = restart_x; //4;
    MyButton_Restart.top  = button_y; //4;

    // Absolute coordinates (relative to screen)
    MyButton_Restart.absolute_left = wi.left + wi.cr_left + MyButton_Restart.left;
    MyButton_Restart.absolute_top  = wi.top + wi.cr_top + MyButton_Restart.top;
    MyButton_Restart.width         = button_w; //32;
    MyButton_Restart.height        = button_h; //24;

    // Initial state
    // MyButton_Restart.state = 0;

// Draw the fake button
    libgui_backbuffer_draw_rectangle0(
        MyButton_Restart.absolute_left, 
        MyButton_Restart.absolute_top, 
        MyButton_Restart.width, 
        MyButton_Restart.height,
        xCOLOR_GRAY2, 
        1, 0, FALSE
    );

    // Draw the label string inside
    const char *label_restart = "RESTART";
    libgui_drawstring(
        MyButton_Restart.absolute_left +4, 
        MyButton_Restart.absolute_top +4, 
        label_restart,
        COLOR_BLACK, COLOR_GRAY, 0
    );

// Refresh to show it
    libgui_refresh_rectangle_via_kernel(
        MyButton_Restart.absolute_left, 
        MyButton_Restart.absolute_top, 
        MyButton_Restart.width, 
        MyButton_Restart.height
    );

// ============================================================
// Create shutdown button

    MyButton_Shutdown.button_id = 2;   // arbitrary ID
    // MyButton_Shutdown.wid     = main_window; // parent window ID

    // Relative values
    MyButton_Shutdown.left = shutdown_x; //4;
    MyButton_Shutdown.top  = button_y; //4;

    // Absolute coordinates (relative to screen)
    MyButton_Shutdown.absolute_left = wi.left + wi.cr_left + MyButton_Shutdown.left;
    MyButton_Shutdown.absolute_top  = wi.top + wi.cr_top + MyButton_Shutdown.top;
    MyButton_Shutdown.width         = button_w; //32;
    MyButton_Shutdown.height        = button_h; //24;

    // Initial state
    // MyButton_Shutdown.state = 0;

// Draw the fake button
    libgui_backbuffer_draw_rectangle0(
        MyButton_Shutdown.absolute_left, 
        MyButton_Shutdown.absolute_top, 
        MyButton_Shutdown.width, 
        MyButton_Shutdown.height,
        xCOLOR_GRAY2, 
        1, 0, FALSE
    );

    // Draw the label string inside
    const char *label_shutdown = "SHUTDOWN";
    libgui_drawstring(
        MyButton_Shutdown.absolute_left +4, 
        MyButton_Shutdown.absolute_top +4, 
        label_shutdown,
        COLOR_BLACK, COLOR_GRAY, 0
    );

// Refresh to show it
    libgui_refresh_rectangle_via_kernel(
        MyButton_Shutdown.absolute_left, 
        MyButton_Shutdown.absolute_top, 
        MyButton_Shutdown.width, 
        MyButton_Shutdown.height
    );

// ---------------------

//
// Button (Restart)
//

// Now you have:
// wi.cr_left, wi.cr_top, wi.cr_width, wi.cr_height
// which describe the client area rectangle of the main window.

/*
    restart_button = 
        (int) gws_create_window(
            client_fd,
            WT_BUTTON,
            BS_DEFAULT,
            1,
            "Restart",
            restart_x, button_y,
            button_w, button_h,
            main_window, 
            WS_CHILD,
           COLOR_GRAY, COLOR_GRAY );

    gws_refresh_window (client_fd, restart_button);
*/

//
// Button (Shutdown)
//


/*
    shutdown_button = 
        (int) gws_create_window(
            client_fd,
            WT_BUTTON,
            BS_DEFAULT,
            1,
            "Shutdown",
            shutdown_x, button_y,
            button_w, button_h,
            main_window, 
            WS_CHILD,
            COLOR_GRAY, COLOR_GRAY );

    gws_refresh_window (client_fd, shutdown_button);
*/

// Set default responder (choose one) 
    //default_responder = restart_button; // Enter will trigger Restart

// Main window: Activate and show.
    gws_set_active( client_fd, main_window );
    gws_refresh_window(client_fd, main_window);

    //printf("Restart button id = %d\n", restart_button);
    //printf("Shutdown button id = %d\n", shutdown_button);

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

    while (1)
    {
        // 1. Pump events from Display Server
        // #bugbug:
        // This pump is very slow, affecting the responsivity
        // for the other pump that gets events from the system.
        pump(client_fd);

        // 2. Pump events from Input Broker (system events)
        for (nSysMsg=0; nSysMsg<32; nSysMsg++){
        if (rtl_get_event() == TRUE)
        {
            // IN: wid, event type, VK, scancode.
            powerProcedure(
                client_fd,
                (int) RTLEventBuffer[0],
                (int) RTLEventBuffer[1],
                (unsigned long) RTLEventBuffer[2],
                (unsigned long) RTLEventBuffer[3] );
            RTLEventBuffer[1] = 0; // clear after dispatch
        }
        };
    };

    return EXIT_SUCCESS;
}
