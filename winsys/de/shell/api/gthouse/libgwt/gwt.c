// gwt.c
// Gramado Window Toolkit (libgwt)
// Minimal fix: reset widget registry on init to avoid stale buttons.

// rtl
#include <types.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <heap.h>
#include <sys/socket.h>
#include <rtl/gramado.h>

// libgws
#include <gws.h>

// libgwt
#include "include/gwt.h"

// Global display
static struct gws_display_d *GWT_Display = NULL;
static int GWT_fd = -1;
static int GWT_main_wid = -1;

// Simple registry for widgets
#define GWT_MAX_WINDOWS  64
#define GWT_MAX_BUTTONS 128

static struct gwt_window *gwt_windows[GWT_MAX_WINDOWS];
static struct gwt_button *gwt_buttons[GWT_MAX_BUTTONS];

static int gwt_window_count = 0;
static int gwt_button_count = 0;


int gwt_get_main_wid(void)
{
    return (int) GWT_main_wid;
}

// --------------------------------------------------------
// Initialization
// --------------------------------------------------------
int gwt_init(const char *display_name)
{
    int i = 0;

// Reset registry to avoid stale entries between runs
    for (i = 0; i < GWT_MAX_WINDOWS; i++)
        gwt_windows[i] = NULL;
    for (i = 0; i < GWT_MAX_BUTTONS; i++)
        gwt_buttons[i] = NULL;

    gwt_window_count = 0;
    gwt_button_count = 0;

// Connect to display server
    GWT_Display = gws_open_display(display_name);
    if (!GWT_Display)
        return -1;

    GWT_fd = GWT_Display->fd;
    return (GWT_fd > 0) ? 0 : -1;
}

// --------------------------------------------------------
// Window creation
// --------------------------------------------------------
struct gwt_window *
gwt_window_create_centered(const char *title,
                           unsigned long w,
                           unsigned long h)
{
    unsigned long sw = gws_get_system_metrics(1);
    unsigned long sh = gws_get_system_metrics(2);

    unsigned long x = (sw - w) / 2;
    unsigned long y = (sh - h) / 2;

    int wid = gws_create_window(
        GWT_fd,
        WT_OVERLAPPED,
        WINDOW_STATUS_ACTIVE,
        WINDOW_STATE_NULL,
        title,
        x, y, w, h,
        0,
        0x0000,
        COLOR_WHITE,
        COLOR_GRAY);

    if (wid < 0)
        return NULL;

    GWT_main_wid = wid;

    struct gwt_window *win = malloc(sizeof(struct gwt_window));
    if (!win)
        return NULL;

    memset(win, 0, sizeof(*win));

    win->wid = wid;
    win->fd  = GWT_fd;

    gwt_window_query_client_area(win);
    gws_set_active(GWT_fd, win->wid);
    gws_refresh_window(GWT_fd, win->wid);

    if (gwt_window_count < GWT_MAX_WINDOWS)
        gwt_windows[gwt_window_count++] = win;

    return win;
}

// --------------------------------------------------------
// Window helpers
// --------------------------------------------------------
void gwt_window_query_client_area(struct gwt_window *win)
{
    struct gws_window_info_d wi;
    gws_get_window_info(win->fd, win->wid, &wi);

    win->cr_left   = wi.cr_left;
    win->cr_top    = wi.cr_top;
    win->cr_width  = wi.cr_width;
    win->cr_height = wi.cr_height;
}

void gwt_window_refresh(struct gwt_window *win)
{
    gws_refresh_window(win->fd, win->wid);
}

void gwt_window_set_paint_handler(struct gwt_window *win,
                                  gwt_paint_callback_t cb,
                                  void *user_data)
{
    win->on_paint  = cb;
    win->user_data = user_data;
}

void gwt_window_set_close_handler(struct gwt_window *win,
                                  gwt_close_callback_t cb,
                                  void *user_data)
{
    win->on_close  = cb;
    win->user_data = user_data;
}

// --------------------------------------------------------
// Button creation
// --------------------------------------------------------
struct gwt_button *
gwt_button_create(struct gwt_window *win,
                  const char *label,
                  unsigned long x,
                  unsigned long y,
                  unsigned long w,
                  unsigned long h)
{
    int bwid = gws_create_window(
        win->fd,
        WT_BUTTON,
        BS_DEFAULT,
        1,
        label,
        x, y, w, h,
        win->wid,
        0,
        COLOR_GRAY,
        COLOR_GRAY);

    if (bwid < 0)
        return NULL;

    struct gwt_button *btn = malloc(sizeof(struct gwt_button));
    if (!btn)
        return NULL;

    memset(btn, 0, sizeof(*btn));

    btn->wid    = bwid;
    btn->parent = win;

    gws_refresh_window(win->fd, bwid);

    if (gwt_button_count < GWT_MAX_BUTTONS)
        gwt_buttons[gwt_button_count++] = btn;

    return btn;
}

void gwt_button_set_on_click(struct gwt_button *btn,
                             gwt_button_callback_t cb,
                             void *user_data)
{
    btn->on_click  = cb;
    btn->user_data = user_data;
}

// --------------------------------------------------------
// Layout helpers
// --------------------------------------------------------
void gwt_button_place_bottom_band(struct gwt_window *win,
                                  struct gwt_button *btn,
                                  unsigned long x,
                                  unsigned long button_w,
                                  unsigned long button_h,
                                  unsigned long margin_y)
{
    // Position relative to client area
    unsigned long y =
        win->cr_height - (button_h + margin_y);

    gws_change_window_position(win->fd, btn->wid, x, y);
    gws_resize_window(win->fd, btn->wid, button_w, button_h);

    // No forced redraw here — let the parent window repaint
    gws_redraw_window(win->fd, btn->wid, FALSE);
}

// --------------------------------------------------------
// Text helpers
// --------------------------------------------------------
void gwt_draw_text(struct gwt_window *win,
                   unsigned long x,
                   unsigned long y,
                   unsigned int color,
                   const char *text)
{
    gws_draw_text(win->fd, win->wid, x, y, color, text);
}

// --------------------------------------------------------
// Main event loop
// --------------------------------------------------------
void gwt_mainloop(void)
{
    int i = 0;

    while (1)
    {
        struct gws_event_d event;
        event.used  = FALSE;
        event.magic = 0;

        struct gws_event_d *e =
            gws_get_next_event(GWT_fd, GWT_main_wid, &event);

        if (!e || e->magic != 1234 || e->used != TRUE)
            continue;

        int wid  = e->window;
        int type = e->type;

        //printf("EVENT: type=%d window=%d long1=%d\n",
           // e->type, e->window, e->long1);

        // Dispatch to windows
        for (i = 0; i < gwt_window_count; i++) {
            struct gwt_window *win = gwt_windows[i];
            if (!win)
                continue;

            if (win->wid == wid) {
                if (type == MSG_PAINT && win->on_paint)
                    win->on_paint(win, win->user_data);

                if (type == MSG_CLOSE && win->on_close)
                    win->on_close(win, win->user_data);
            }
        }

        // Dispatch to buttons
        if (type == GWS_MouseClicked) {
            int clicked = (int) e->long1;

            for (i = 0; i < gwt_button_count; i++) {
                struct gwt_button *btn = gwt_buttons[i];
                if (!btn)
                    continue;

                if (btn->wid == clicked && btn->on_click) {
                    btn->on_click(btn, btn->user_data);
                }
            }
        }
    }
}
