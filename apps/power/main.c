// power_app.c
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
// libgws - The client-side library.
#include <gws.h>

#include "power.h"


// Global display pointer
struct gws_display_d *Display;


// Window IDs
static int main_window     = -1;
static int restart_button  = -1;
static int shutdown_button = -1;

// Default responder (button to trigger on Enter)
static int default_responder = -1;

static void trigger_default_responder(int fd);


// =====================================================


static void update_children(int fd)
{
    struct gws_window_info_d wi;
    gws_get_window_info(fd, main_window, &wi);

    unsigned long button_w = wi.cr_width / 4;
    unsigned long button_h = wi.cr_height / 8;
    unsigned long button_y = (wi.cr_height - button_h) / 2;

    unsigned long restart_x  = (wi.cr_width / 4) - (button_w / 2);
    unsigned long shutdown_x = (3 * wi.cr_width / 4) - (button_w / 2);


    // Redraw label text
    gws_draw_text(
        fd,
        main_window,
        20, 20,
        COLOR_BLACK,
        "Choose an action:"
    );

    // Move and redraw buttons
    gws_change_window_position(fd, restart_button, restart_x, button_y);
    gws_resize_window(fd, restart_button, button_w, button_h);
    gws_redraw_window(fd, restart_button, TRUE);

    gws_change_window_position(fd, shutdown_button, shutdown_x, button_y);
    gws_resize_window(fd, shutdown_button, button_w, button_h);
    gws_redraw_window(fd, shutdown_button, TRUE);
    
    //#debug
    gws_refresh_window (fd, main_window);
}

static void trigger_default_responder(int fd) 
{
    if (default_responder == restart_button) {
        printf("PowerApp: Enter >> Restart\n");
        rtl_clone_and_execute("reboot.bin");
        exit(0);
    } else if (default_responder == shutdown_button) {
        printf("PowerApp: Enter >> Shutdown\n");
        rtl_clone_and_execute("shutdown.bin");
        exit(0);
    }
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
    if (fd < 0) return -1;
    if (event_window < 0) return -1;
    if (event_type < 0) return -1;

    switch (event_type) {

    case 0:
        // Null event
        return 0;
        break;

    case MSG_KEYDOWN:
        switch (long1){

        case VK_RETURN:
            trigger_default_responder(fd);
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
        };
        break;

    case GWS_MouseClicked:
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

    // Use long1 as the clicked child window ID
    if ((int)long1 == restart_button) {
        printf("Restart button clicked\n");
        rtl_clone_and_execute("reboot.bin");
        return 0;
    }

    if ((int)long1 == shutdown_button) {
        printf("Shutdown button clicked\n");
        rtl_clone_and_execute("shutdown.bin");
        return 0;
    }
    
        //printf("GWS_MouseClicked: done\n");
        break;

    case MSG_CLOSE:
        gws_destroy_window(fd, restart_button);
        gws_destroy_window(fd, shutdown_button);
        gws_destroy_window(fd, main_window);
        printf("PowerApp: Window closed\n");
        exit(0);
        break;

    case MSG_PAINT:
        update_children(fd);
        return 0;
        break;

    default:
        // Unknown event
        return -1;
        break;
    };

    return -1;
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
        printf("power_app: Invalid fd\n");
        return EXIT_FAILURE;
    }

    // Screen size
    unsigned long screen_w = gws_get_system_metrics(1);
    unsigned long screen_h = gws_get_system_metrics(2);

    // Main window
    unsigned long win_w = screen_w / 2;
    unsigned long win_h = screen_h / 2;
    unsigned long win_x = (screen_w - win_w) / 2;
    unsigned long win_y = (screen_h - win_h) / 2;

    main_window = gws_create_window(
        client_fd,
        WT_OVERLAPPED,
        WINDOW_STATUS_ACTIVE,  //status
        WINDOW_STATE_NULL,  //state
        "Power Manager",
        win_x, win_y, win_w, win_h,
        0,
        0x0000,  // style
        COLOR_WHITE,
        COLOR_GRAY );

    if (main_window < 0) {
        printf("power_app: Failed to create main window\n");
        return EXIT_FAILURE;
    }

    //#debug
    gws_refresh_window(client_fd, main_window);

//
// Text
//

    gws_draw_text(
        client_fd,
        main_window,
        20, 20,                // position
        COLOR_BLACK,
        "Choose an action:"    // label
    );

    //#debug
    //gws_refresh_window(client_fd, main_window);

// After creating main_window
    struct gws_window_info_d wi;
    gws_get_window_info(
        client_fd,
        main_window,
        (struct gws_window_info_d *) &wi );


//
// Support for button positions and dimensions
//

    unsigned long button_w = wi.cr_width / 4;
    unsigned long button_h = wi.cr_height / 8;
    unsigned long button_y = (wi.cr_height - button_h) / 2;

    unsigned long restart_x  = (wi.cr_width / 4) - (button_w / 2);
    unsigned long shutdown_x = (3 * wi.cr_width / 4) - (button_w / 2);


//
// Button (Restart)
//

// Now you have:
// wi.cr_left, wi.cr_top, wi.cr_width, wi.cr_height
// which describe the client area rectangle of the main window.

restart_button = gws_create_window(
    client_fd,
    WT_BUTTON,
    BS_DEFAULT,
    1,
    "Restart",
    restart_x, button_y,
    button_w, button_h,
    main_window, 0,
    COLOR_GRAY, COLOR_GRAY );

    //#debug
    gws_refresh_window (client_fd, restart_button);


//
// Button (Shutdown)
//


shutdown_button = gws_create_window(
    client_fd,
    WT_BUTTON,
    BS_DEFAULT,
    1,
    "Shutdown",
    shutdown_x, button_y,
    button_w, button_h,
    main_window, 0,
    COLOR_GRAY, COLOR_GRAY );

    //#debug
    gws_refresh_window (client_fd, shutdown_button);

// Set default responder (choose one) 
    default_responder = restart_button; // Enter will trigger Restart

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


    while (1)
    {
        // 1. Pump events from Display Server
        pump(client_fd);

        // 2. Pump events from Input Broker (system events)
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

    return EXIT_SUCCESS;
}
