#ifndef GWT_H
#define GWT_H

#include <gws.h>

// Forward declarations
struct gwt_window;
struct gwt_button;

// Callback types
typedef void (*gwt_button_callback_t)(struct gwt_button *btn, void *user_data);
typedef void (*gwt_paint_callback_t)(struct gwt_window *win, void *user_data);
typedef void (*gwt_close_callback_t)(struct gwt_window *win, void *user_data);

// ------------------------------
// Window structure
// ------------------------------
struct gwt_window {
    int wid;
    int fd;

    unsigned long cr_left;
    unsigned long cr_top;
    unsigned long cr_width;
    unsigned long cr_height;

    gwt_paint_callback_t on_paint;
    gwt_close_callback_t on_close;
    void *user_data;
};

// ------------------------------
// Button structure
// ------------------------------
struct gwt_button {
    int wid;
    struct gwt_window *parent;

    gwt_button_callback_t on_click;
    void *user_data;
};

int gwt_get_main_wid(void);

// ------------------------------
// Initialization
// ------------------------------
int gwt_init(const char *display_name);
void gwt_mainloop(void);

// ------------------------------
// Window API
// ------------------------------
struct gwt_window *
gwt_window_create_centered(const char *title,
                           unsigned long w,
                           unsigned long h);

void gwt_window_set_paint_handler(struct gwt_window *win,
                                  gwt_paint_callback_t cb,
                                  void *user_data);

void gwt_window_set_close_handler(struct gwt_window *win,
                                  gwt_close_callback_t cb,
                                  void *user_data);

void gwt_window_refresh(struct gwt_window *win);
void gwt_window_query_client_area(struct gwt_window *win);

// ------------------------------
// Button API
// ------------------------------
struct gwt_button *
gwt_button_create(struct gwt_window *win,
                  const char *label,
                  unsigned long x,
                  unsigned long y,
                  unsigned long w,
                  unsigned long h);

void gwt_button_set_on_click(struct gwt_button *btn,
                             gwt_button_callback_t cb,
                             void *user_data);

// ------------------------------
// Layout helpers
// ------------------------------
void gwt_button_place_bottom_band(struct gwt_window *win,
                                  struct gwt_button *btn,
                                  unsigned long x,
                                  unsigned long button_w,
                                  unsigned long button_h,
                                  unsigned long margin_y);

// ------------------------------
// Text helpers
// ------------------------------
void gwt_draw_text(struct gwt_window *win,
                   unsigned long x,
                   unsigned long y,
                   unsigned int color,
                   const char *text);

#endif
