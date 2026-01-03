// painter.h
// Created by Fred Nora

#ifndef __UI_PAINTER_H
#define __UI_PAINTER_H    1


void validate_window(struct gws_window_d *window);
void validate_window_by_id(int wid);
void begin_paint(struct gws_window_d *window);


void invalidate_window (struct gws_window_d *window);
void invalidate_window_by_id(int wid);

void invalidate_root_window(void);
void invalidate_titlebar(struct gws_window_d *pwindow);
void invalidate_menubar(struct gws_window_d *pwindow);
void invalidate_toolbar(struct gws_window_d *pwindow);
void invalidate_scrollbar(struct gws_window_d *pwindow);
void invalidate_statusbar(struct gws_window_d *pwindow);
void invalidate_taskbar_window(void);

void end_paint(struct gws_window_d *window);

int 
painterFillWindowRectangle( 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned int color,
    unsigned long rop_flags );


void 
__draw_button_borders(
    struct gws_window_d *w,
    unsigned int tl_2,  // tl 2 inner (light)
    unsigned int tl_1,  // tl 1 most inner (lighter)
    unsigned int br_2,  // br 2 inner (dark)
    unsigned int br_1,  // br 1 most inner (light) 
    unsigned int outer_color );


// worker: no checks
void 
__draw_window_border( 
    struct gws_window_d *parent, 
    struct gws_window_d *window,
    unsigned long rop_top,
    unsigned long rop_left,
    unsigned long rop_right,
    unsigned long rop_bottom );

void redraw_text_for_editbox(struct gws_window_d *window);

int redraw_controls(struct gws_window_d *window);
int redraw_titlebar_window(struct gws_window_d *window);

int 
redraw_window (
    struct gws_window_d *window, 
    unsigned long flags ); 

int redraw_window_by_id(int wid, unsigned long flags);

int clear_window_by_id(int wid, unsigned long flags);

void wm_flush_rectangle(struct gws_rect_d *rect);
void wm_flush_window(struct gws_window_d *window);
void wm_flush_screen(void);

#endif    


