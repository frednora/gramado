// bootmenu_app.c
// Gramado OS client-side GUI app with two buttons
// Calls syscall 294 with values 1000 and 1001.
// Created by Fred Nora (example by Copilot).

#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <rtl/gramado.h>
#include <gws.h>

// Global display pointer
struct gws_display_d *Display;

// Window IDs
static int main_window     = -1;
static int show_button     = -1;
static int skip_button     = -1;

// Default responder
static int default_responder = -1;

static void set_default_responder(int wid) {
    if (wid >= 0) default_responder = wid;
}

static void switch_responder(int fd) {
    if (default_responder == show_button) {
        set_default_responder(skip_button);
        gws_set_focus(fd, skip_button);
    } else {
        set_default_responder(show_button);
        gws_set_focus(fd, show_button);
    }
}

static void trigger_default_responder(int fd) {
    if (default_responder == show_button) {
        printf("BootMenuApp: Enter >> Show Menu\n");
        sc80(294, 1000, 0, 0);
        exit(0);
    } else if (default_responder == skip_button) {
        printf("BootMenuApp: Enter >> Skip Menu\n");
        sc80(294, 1001, 0, 0);
        exit(0);
    }
}

// ----------------------------------------------------
// Procedure: handles events sent by the display server
// ----------------------------------------------------
static int bootmenuProcedure(
    int fd, int event_window, int event_type,
    unsigned long long1, unsigned long long2 )
{
    if (fd < 0 || event_window < 0 || event_type < 0)
        return -1;

    switch (event_type) {
    case MSG_KEYDOWN:
        switch (long1) {
        case VK_RETURN:
            trigger_default_responder(fd);
            break;
        case 'S': case 's':
            sc80(294, 1001, 0, 0);
            exit(0);
            break;
        case 'M': case 'm':
            sc80(294, 1000, 0, 0);
            exit(0);
            break;
        };
        break;

    case MSG_SYSKEYDOWN:
        switch (long1) {
        case VK_F1:
            sc80(294, 1000, 0, 0);
            exit(0);
            break;
        case VK_F2:
            sc80(294, 1001, 0, 0);
            exit(0);
            break;
        case VK_ARROW_LEFT:
        case VK_ARROW_RIGHT:
            switch_responder(fd);
            break;
        }
        break;

    case GWS_MouseClicked:
        if ((int)long1 == show_button) {
            printf("Show Menu button clicked\n");
            sc80(294, 1000, 0, 0);
            exit(0);
        }
        if ((int)long1 == skip_button) {
            printf("Skip Menu button clicked\n");
            sc80(294, 1001, 0, 0);
            exit(0);
        }
        break;

    case MSG_CLOSE:
        gws_destroy_window(fd, show_button);
        gws_destroy_window(fd, skip_button);
        gws_destroy_window(fd, main_window);
        printf("BootMenuApp: Window closed\n");
        exit(0);
        break;

    case MSG_PAINT:
        gws_draw_text(fd, main_window, 20, 20, COLOR_BLACK,
                      "Choose boot option:");
        return 0;
    };

    return -1;
}

// ----------------------------------------------------
// Pump events
// ----------------------------------------------------
static void pump(int fd)
{
    struct gws_event_d event;
    struct gws_event_d *e;

    e = gws_get_next_event(fd, main_window, &event);
    if ((void*) e == NULL) return;
    if (e->magic != 1234 || e->used != TRUE) return;
    if (e->type <= 0) return;

    bootmenuProcedure(fd, e->window, e->type, e->long1, e->long2);
}

// ----------------------------------------------------
// Main
// ----------------------------------------------------
int main(int argc, char *argv[])
{
    const char *display_name = "display:name.0";
    Display = gws_open_display(display_name);
    if ((void*) Display == NULL) {
        printf("bootmenu_app: Could not open display\n");
        return EXIT_FAILURE;
    }

    int client_fd = Display->fd;
    if (client_fd <= 0) {
        printf("bootmenu_app: Invalid fd\n");
        return EXIT_FAILURE;
    }

    unsigned long screen_w = gws_get_system_metrics(1);
    unsigned long screen_h = gws_get_system_metrics(2);

    unsigned long win_w = screen_w / 2;
    unsigned long win_h = screen_h / 2;

    // #hack
    if (screen_w == 320 && screen_h == 200)
    {
        win_w = screen_w -8;
        win_h = screen_h -36;
    }

    unsigned long win_x = (screen_w - win_w) / 2;
    unsigned long win_y = (screen_h - win_h) / 2;

    // #hack
    if (screen_w == 320 && screen_h == 200)
    {
        win_x = 4;
        win_y = 4;
    }

    main_window = gws_create_window(
        client_fd, WT_OVERLAPPED,
        WINDOW_STATUS_ACTIVE, WINDOW_STATE_NULL,
        "Boot Menu Manager",
        win_x, win_y, win_w, win_h,
        0, 0x0000, COLOR_WHITE, COLOR_GRAY );

    if (main_window < 0) {
        printf("bootmenu_app: Failed to create main window\n");
        return EXIT_FAILURE;
    }

    gws_draw_text(client_fd, main_window, 20, 20,
                  COLOR_BLACK, "Choose boot option:");

    unsigned long button_w = win_w / 4;
    unsigned long button_h = win_h / 8;
    unsigned long button_y = (win_h - button_h) / 2;

    unsigned long show_x = (win_w / 4) - (button_w / 2);
    unsigned long skip_x = (3 * win_w / 4) - (button_w / 2);

    show_button = gws_create_window(
        client_fd, WT_BUTTON, BS_DEFAULT, 1,
        "Show Menu",
        show_x, button_y, button_w, button_h,
        main_window, 0, COLOR_GRAY, COLOR_GRAY );

    skip_button = gws_create_window(
        client_fd, WT_BUTTON, BS_DEFAULT, 1,
        "Skip Menu",
        skip_x, button_y, button_w, button_h,
        main_window, 0, COLOR_GRAY, COLOR_GRAY );

    default_responder = show_button;

    gws_set_active(client_fd, main_window);
    gws_refresh_window(client_fd, main_window);

    while (1) {
        pump(client_fd);
        if (rtl_get_event() == TRUE) {
            bootmenuProcedure(
                client_fd,
                (int) RTLEventBuffer[0],
                (int) RTLEventBuffer[1],
                (unsigned long) RTLEventBuffer[2],
                (unsigned long) RTLEventBuffer[3] );
            RTLEventBuffer[1] = 0;
        }
    }

    return EXIT_SUCCESS;
}
