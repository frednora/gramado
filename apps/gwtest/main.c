// main.c
// Example application using libgwt (Gramado Window Toolkit)
// Created for testing the new toolkit.

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
#include <gwt.h>   // Your new toolkit


// ------------------------------------------------------------
// Button callbacks
// ------------------------------------------------------------
static void on_hello_clicked(struct gwt_button *btn, void *user_data)
{
    printf("Hello button clicked!\n");
}

static void on_exit_clicked(struct gwt_button *btn, void *user_data)
{
    printf("Exit button clicked\n");
    exit(0);
}

// ------------------------------------------------------------
// Paint callback
// ------------------------------------------------------------
static void on_paint(struct gwt_window *win, void *user_data)
{
    // Always refresh client area info before drawing
    gwt_window_query_client_area(win);

    // Draw a title
    gwt_draw_text(win, 20, 20, COLOR_BLACK, "Welcome to libgwt!");

    // Draw a subtitle
    gwt_draw_text(win, 20, 50, COLOR_BLUE, "This window is drawn using the new toolkit.");
}

// ------------------------------------------------------------
// Close callback
// ------------------------------------------------------------
static void on_close(struct gwt_window *win, void *user_data)
{
    printf("Window closed\n");
    exit(0);
}


int main(int argc, char *argv[])
{
    // Initialize toolkit (connect to display server)
    if (gwt_init("display:name.0") < 0) {
        printf("Failed to initialize libgwt\n");
        return 1;
    }

    // Create a centered window
    struct gwt_window *win =
        gwt_window_create_centered("GWT Demo", 400, 250);

    if (!win) {
        printf("Failed to create window\n");
        return 1;
    }

    // Set callbacks
    gwt_window_set_paint_handler(win, on_paint, NULL);
    gwt_window_set_close_handler(win, on_close, NULL);

    // Query client area for layout
    gwt_window_query_client_area(win);

    // ------------------------------------------------------------
    // Button sizes and final positions
    // ------------------------------------------------------------
    unsigned long bw = win->cr_width / 4;
    unsigned long bh = 40;

    // Bottom Y coordinate
    unsigned long y = win->cr_height - (bh + 20);

    // X positions (30% and 70% of client width)
    unsigned long x_hello = (win->cr_width * 30) / 100 - (bw / 2);
    unsigned long x_exit  = (win->cr_width * 70) / 100 - (bw / 2);

    // ------------------------------------------------------------
    // Create buttons *directly* at their final positions
    // ------------------------------------------------------------
    struct gwt_button *btn_hello =
        gwt_button_create(win, "Hello", x_hello, y, bw, bh);

    struct gwt_button *btn_exit =
        gwt_button_create(win, "Exit",  x_exit,  y, bw, bh);

    // Assign callbacks
    gwt_button_set_on_click(btn_hello, on_hello_clicked, NULL);
    gwt_button_set_on_click(btn_exit,  on_exit_clicked,  NULL);

    // ------------------------------------------------------------
    // Enter main event loop
    // ------------------------------------------------------------
    gwt_mainloop();

    return 0;
}

