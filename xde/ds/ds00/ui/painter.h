// painter.h


#ifndef __PAINTER_H
#define __PAINTER_H    1

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
    unsigned int color1,
    unsigned int color2_dark,
    unsigned int color2_light,
    unsigned int outer_color );

//worker: no checks
void 
__draw_window_border( 
    struct gws_window_d *parent, 
    struct gws_window_d *window );

void begin_paint(struct gws_window_d *window);
void end_paint(struct gws_window_d *window);

int clear_window_by_id(int wid, unsigned long flags);

void invalidate_root_window(void);
void invalidate_taskbar_window(void);
void invalidate_window (struct gws_window_d *window);
void invalidate_window_by_id(int wid);

void invalidate_titlebar(struct gws_window_d *pwindow);
void invalidate_menubar(struct gws_window_d *pwindow);
void invalidate_toolbar(struct gws_window_d *pwindow);
void invalidate_scrollbar(struct gws_window_d *pwindow);
void invalidate_statusbar(struct gws_window_d *pwindow);

int redraw_controls(struct gws_window_d *window);
int redraw_titlebar_window(struct gws_window_d *window);

void redraw_text_for_editbox(struct gws_window_d *window);
int 
redraw_window (
    struct gws_window_d *window, 
    unsigned long flags ); 
int redraw_window_by_id(int wid, unsigned long flags);

void validate_window(struct gws_window_d *window);
void validate_window_by_id(int wid);

void wm_flush_rectangle(struct gws_rect_d *rect);
void wm_flush_screen(void);
void wm_flush_window(struct gws_window_d *window);


#endif    


