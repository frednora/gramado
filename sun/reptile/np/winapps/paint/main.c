// main.c (Created by claude ai)
// Gramado OS client-side GUI app: Paint (v2)
//
// What is new compared to the first version:
//  - Strokes are continuous (Bresenham between mouse samples).
//  - Tools: Pen, Eraser, Fill (bucket).
//  - Brush sizes (table), with Size -/+ buttons and [ ] keys.
//  - 8-colour palette (swatches) plus the K/R/G/B shortcuts.
//  - Fast direct-write rectangle/span filler (no per-pixel ROP chain).
//  - Status line in the footer (tool, size, colour, shortcuts).
//  - Table-driven toolbar instead of copy-pasted code per button.
//  - Press/release logic fixed: the press position comes from last_x/last_y
//    (RTLEventBuffer[4]/[5]), and a click only happens if the button that was
//    pressed is the one under the pointer on release.
//
// The input loop is unchanged in spirit:
//   while(1) { shared-flags check; if (rtl_get_event()) paintProcedure(...); }
// Nothing heavy runs per iteration: no syscalls, no window-info queries,
// no allocations. Drawing happens only inside the procedure, only when an
// event arrives.

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

#include "paint.h"

static int isTimeToQuit = FALSE;

// Global display pointer
struct gws_display_d *Display;

struct dccanvas_d *dc00;  // shared dc

static unsigned long __sh_flags = 0;

// ----------------------------------------------------
// Layout
// ----------------------------------------------------
//
// [ canvas ........................................... ]
// [ row 1: Clear Pen Eraser Fill Size- Size+          ]  toolbar
// [ row 2: palette swatches                           ]
// [ footer / status line                              ]
//
#define TOOLBAR_H      88
#define FOOTER_H       18
#define ROW1_Y         6     // relative to toolbar top
#define ROW1_H         28
#define ROW2_Y         40    // relative to toolbar top
#define ROW2_H         22
#define GAP            8

// ----------------------------------------------------
// Tools, buttons and palette
// ----------------------------------------------------
#define TOOL_PEN     1
#define TOOL_ERASER  2
#define TOOL_FILL    3

// Toolbar button ids (index = id - 1)
#define BTN_CLEAR    1
#define BTN_PEN      2
#define BTN_ERASER   3
#define BTN_FILL     4
#define BTN_SIZE_DN  5
#define BTN_SIZE_UP  6
#define BTN_COUNT    6

// Palette swatch ids
#define SWATCH_BASE   100
#define SWATCH_COUNT  8

struct button_info_d
{
    int button_id;
    int wid;
    unsigned long absolute_left;
    unsigned long absolute_top;
    unsigned long width;
    unsigned long height;
    unsigned long left;
    unsigned long top;
    int state;
};

// Toolbar buttons (relative to the client area)
static struct button_info_d   Btn[BTN_COUNT];
static struct ui_component_d *Uic[BTN_COUNT];
static const char *BtnLabel[BTN_COUNT] = {
    "Clear", "Pen", "Eraser", "Fill", "Size -", "Size +"
};

// Palette swatches (relative to the client area)
static struct button_info_d  Swatch[SWATCH_COUNT];
static const unsigned int Palette[SWATCH_COUNT] = {
    0x00000000,  // black
    0x00808080,  // gray
    0x00FF0000,  // red
    0x00FF8000,  // orange
    0x00FFFF00,  // yellow
    0x0000A000,  // green
    0x000000FF,  // blue
    0x008000FF   // purple
};

// Brush sizes in pixels
static const int BrushSizes[] = { 1, 2, 3, 5, 8, 12, 16, 24 };
#define BRUSH_COUNT  (int)(sizeof(BrushSizes)/sizeof(BrushSizes[0]))

// ----------------------------------------------------
// State
// ----------------------------------------------------
static int __hover_button_id   = -1;
static int __pressed_button_id = -1;

static int is_drawing = FALSE;
static int has_prev   = FALSE;   // prev_x/prev_y are valid for this stroke
static long prev_x = 0;
static long prev_y = 0;

// last_x/last_y are filled by the input loop from the event buffer
// (positions of the pointer at the time of the event).
static unsigned long last_x = 0;
static unsigned long last_y = 0;

static int current_tool       = TOOL_PEN;
static int current_size_index = 2;                // 3 px, like the old brush
static unsigned int current_color = COLOR_BLACK;  // default pen

// The current height for the area where the
// pointer is allowed to paint.
unsigned long current_cr_height = 0;

// Window
static int main_window = -1;

unsigned long window_x;
unsigned long window_y;

// Used by all the routines
struct gws_window_info_d  wi;

// ----------------------------------------------------
// Prototypes
// ----------------------------------------------------
static void on_button_pressed(int button_id);
static void on_button_released(int button_id);
static void on_button_clicked(int id);

static int  __hit_test_button(unsigned long rel_mx, unsigned long rel_my);
static void update_children(int fd);
static void clear_canvas(void);
static void draw_status(void);
static void refresh_toolbar_state(void);

// ====================================================
// Fast drawing primitives (direct writes into the dc)
// ====================================================

// Fill a rectangle, clipped against the dc. ROP_COPY only.
// This is much cheaper than lingui_draw_rectangle0_dc(), which goes
// through the ROP chain for every single pixel.
static void 
paint_fill_rect(
    struct dccanvas_d *dc,
    long x, long y, long w, long h,
    unsigned int color )
{
    long x2, y2, yy, xx;
    long dw, dh;
    unsigned char *row;
    unsigned int c = (color & 0x00FFFFFF);
    unsigned char b = (unsigned char)( c        & 0xFF);
    unsigned char g = (unsigned char)((c >> 8)  & 0xFF);
    unsigned char r = (unsigned char)((c >> 16) & 0xFF);

    if ((void*) dc == NULL || (void*) dc->data == NULL)
        return;
    if (w <= 0 || h <= 0)
        return;

    dw = (long) dc->device_width;
    dh = (long) dc->device_height;

    x2 = x + w;
    y2 = y + h;
    if (x  < 0)  x  = 0;
    if (y  < 0)  y  = 0;
    if (x2 > dw) x2 = dw;
    if (y2 > dh) y2 = dh;
    if (x >= x2 || y >= y2)
        return;

    for (yy = y; yy < y2; yy++)
    {
        row = (unsigned char *) dc->data + ((unsigned long) yy * dc->pitch);

        if (dc->bpp == 32)
        {
            unsigned int *p = (unsigned int *) (row + ((unsigned long) x << 2));
            for (xx = x; xx < x2; xx++)
                *p++ = c;
        }
        else if (dc->bpp == 24)
        {
            unsigned char *p = row + ((unsigned long) x * 3);
            for (xx = x; xx < x2; xx++)
            {
                p[0] = b;
                p[1] = g;
                p[2] = r;
                p += 3;
            }
        }
    }
}

// Read a pixel as 0x00RRGGBB. Caller guarantees x,y are inside the dc.
static unsigned int paint_get_pixel(long x, long y)
{
    unsigned char *row = 
        (unsigned char *) dc00->data + ((unsigned long) y * dc00->pitch);

    if (dc00->bpp == 32)
        return (*(unsigned int *)(row + ((unsigned long) x << 2))) & 0x00FFFFFF;

    // 24 bpp
    {
        unsigned char *p = row + ((unsigned long) x * 3);
        return (unsigned int)( p[0] | (p[1] << 8) | (p[2] << 16) );
    }
}

// Width of the paintable area.
static long canvas_w(void)
{
    return (long) dc00->device_width;
}

// Height of the paintable area (above the toolbar).
static long canvas_h(void)
{
    long h = (long) current_cr_height;
    if (h > (long) dc00->device_height)
        h = (long) dc00->device_height;
    return h;
}

// ----------------------------------------------------
// Brush
// ----------------------------------------------------
static long brush_size(void)
{
    long s = BrushSizes[current_size_index];
    // The eraser is a bit bigger than the pen.
    if (current_tool == TOOL_ERASER)
        s = (s * 2) + 4;
    return s;
}

static unsigned int brush_color(void)
{
    if (current_tool == TOOL_ERASER)
        return COLOR_WHITE;
    return current_color;
}

// Square brush centred on x,y, clipped to the paintable area.
static void paint_stamp(long x, long y, long size, unsigned int color)
{
    long l = x - (size / 2);
    long t = y - (size / 2);
    long h = size;
    long ch = canvas_h();

    if (t < 0){
        h += t;
        t = 0;
    }
    if (t + h > ch)
        h = ch - t;

    paint_fill_rect(dc00, l, t, size, h, color);
}

// Continuous stroke between two points (Bresenham).
// The brush is stamped every size/2 steps, which is dense enough for a
// square brush and keeps big brushes cheap.
static void 
paint_stroke(
    long x0, long y0, 
    long x1, long y1, 
    long size, 
    unsigned int color )
{
    long dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    long dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);
    long sx = (x0 < x1) ? 1 : -1;
    long sy = (y0 < y1) ? 1 : -1;
    long err = dx - dy;
    long e2;
    long step = size / 2;
    long n = 0;

    if (step < 1)
        step = 1;

    while (1)
    {
        if ((n % step) == 0 || (x0 == x1 && y0 == y1))
            paint_stamp(x0, y0, size, color);

        if (x0 == x1 && y0 == y1)
            break;

        e2 = 2 * err;
        if (e2 > -dy){ err -= dy; x0 += sx; }
        if (e2 <  dx){ err += dx; y0 += sy; }
        n++;
    }
}

// ----------------------------------------------------
// Flood fill (scanline, explicit seed stack, no recursion, no malloc)
// If the stack overflows on a very complex region, some seeds are
// dropped and the fill may be incomplete; pressing again finishes it.
// ----------------------------------------------------
#define FILL_STACK_MAX  4096
struct seed_d { int x; int y; };
static struct seed_d fill_stack[FILL_STACK_MAX];
static int fill_sp = 0;

static void fill_push(int x, int y)
{
    if (fill_sp < FILL_STACK_MAX)
    {
        fill_stack[fill_sp].x = x;
        fill_stack[fill_sp].y = y;
        fill_sp++;
    }
}

static void flood_fill(long sx, long sy, unsigned int color)
{
    long w = canvas_w();
    long h = canvas_h();
    unsigned int target;
    long x, y, lx, rx, ny, i;
    int inrun;

    if (sx < 0 || sy < 0 || sx >= w || sy >= h)
        return;

    color &= 0x00FFFFFF;
    target = paint_get_pixel(sx, sy);
    if (target == color)
        return;

    fill_sp = 0;
    fill_push((int) sx, (int) sy);

    while (fill_sp > 0)
    {
        fill_sp--;
        x = fill_stack[fill_sp].x;
        y = fill_stack[fill_sp].y;

        if (paint_get_pixel(x, y) != target)
            continue;

        lx = x;
        while (lx > 0 && paint_get_pixel(lx - 1, y) == target)
            lx--;
        rx = x;
        while (rx < (w - 1) && paint_get_pixel(rx + 1, y) == target)
            rx++;

        paint_fill_rect(dc00, lx, y, (rx - lx + 1), 1, color);

        // Seeds for the rows above and below
        for (ny = y - 1; ny <= y + 1; ny += 2)
        {
            if (ny < 0 || ny >= h)
                continue;

            inrun = FALSE;
            for (i = lx; i <= rx; i++)
            {
                if (paint_get_pixel(i, ny) == target)
                {
                    if (inrun == FALSE){
                        fill_push((int) i, (int) ny);
                        inrun = TRUE;
                    }
                } else {
                    inrun = FALSE;
                }
            }
        }
    }
}

// ----------------------------------------------------
static void clear_canvas(void)
{
    if ((void*) dc00 == NULL)
        return;

    // Only the drawing area (above toolbar)
    paint_fill_rect(
        dc00, 
        0, 0, 
        canvas_w(), (long) current_cr_height, 
        COLOR_WHITE );

    is_drawing = FALSE;
    has_prev = FALSE;
}

// ====================================================
// Status line
// ====================================================

static char *sput(char *d, char *end, const char *s)
{
    while (*s && d < end)
        *d++ = *s++;
    return d;
}

static char *snum(char *d, char *end, unsigned long v)
{
    char tmp[16];
    int n = 0;

    if (v == 0)
        tmp[n++] = '0';
    while (v > 0 && n < 15){
        tmp[n++] = (char)('0' + (v % 10));
        v /= 10;
    }
    while (n > 0 && d < end)
        *d++ = tmp[--n];
    return d;
}

static char *shex(char *d, char *end, unsigned int v)
{
    int i;
    for (i = 5; i >= 0 && d < end; i--)
        *d++ = "0123456789ABCDEF"[(v >> (i * 4)) & 0xF];
    return d;
}

static void draw_status(void)
{
    char line[160];
    char *p = line;
    char *end = line + sizeof(line) - 1;
    unsigned long max_chars;
    unsigned long top;
    const char *tool_name = "Pen";

    if ((void*) dc00 == NULL)
        return;

    if (current_tool == TOOL_ERASER) tool_name = "Eraser";
    if (current_tool == TOOL_FILL)   tool_name = "Fill";

    p = sput(p, end, " ");
    p = sput(p, end, tool_name);
    p = sput(p, end, " | size ");
    p = snum(p, end, (unsigned long) brush_size());
    p = sput(p, end, " | #");
    p = shex(p, end, current_color);
    p = sput(p, end, " | P pen  E erase  F fill  C clear  [ ] size  K/R/G/B  Q quit");
    *p = '\0';

    // #bugbug:
    // libgui_drawstring_dc() does not clip, so never exceed the width.
    max_chars = (unsigned long) (canvas_w() / 8);
    if (max_chars < sizeof(line))
        line[max_chars] = '\0';

    top = wi.cr_height - FOOTER_H;

    paint_fill_rect(dc00, 0, (long) top, canvas_w(), FOOTER_H, LIBGUI_COLOR_PANEL_FACE);
    libgui_drawstring_dc(
        dc00, 
        0, top + 5, 
        0x00101010, LIBGUI_COLOR_PANEL_FACE, 
        0,      // ROP
        line 
    );
}

// ====================================================
// Toolbar
// ====================================================

static void draw_swatches(void)
{
    int i;
    long x, y, w, h;

    for (i = 0; i < SWATCH_COUNT; i++)
    {
        x = (long) Swatch[i].left;
        y = (long) Swatch[i].top;
        w = (long) Swatch[i].width;
        h = (long) Swatch[i].height;

        if (Palette[i] == (current_color & 0x00FFFFFF))
        {
            // Selected: black frame, white ring, colour
            paint_fill_rect(dc00, x,     y,     w,     h,     0x00000000);
            paint_fill_rect(dc00, x + 1, y + 1, w - 2, h - 2, 0x00FFFFFF);
            paint_fill_rect(dc00, x + 2, y + 2, w - 4, h - 4, 0x00000000);
            paint_fill_rect(dc00, x + 3, y + 3, w - 6, h - 6, Palette[i]);
        }
        else
        {
            paint_fill_rect(dc00, x,     y,     w,     h,     0x00404040);
            paint_fill_rect(dc00, x + 1, y + 1, w - 2, h - 2, Palette[i]);
        }
    }
}

// Tool buttons look "sunken" while their tool is active.
static void refresh_tool_buttons(void)
{
    int i;
    int active_id = -1;

    if (current_tool == TOOL_PEN)    active_id = BTN_PEN;
    if (current_tool == TOOL_ERASER) active_id = BTN_ERASER;
    if (current_tool == TOOL_FILL)   active_id = BTN_FILL;

    for (i = 0; i < BTN_COUNT; i++)
    {
        if ((void*) Uic[i] == NULL)
            continue;

        if (Btn[i].button_id == active_id)
            libgui_set_ui_component_status(Uic[i], BS_PRESSED);
        else
            libgui_set_ui_component_status(Uic[i], BS_RELEASED);

        libgui_redraw_ui_component(Uic[i], dc00);
    }
}

// Cheap refresh after a tool/colour/size change. No window-info query.
static void refresh_toolbar_state(void)
{
    refresh_tool_buttons();
    draw_swatches();
    draw_status();
}

static void select_tool(int tool)
{
    current_tool = tool;
    refresh_toolbar_state();
}

static void select_color(unsigned int color)
{
    current_color = (color & 0x00FFFFFF);
    // Picking a colour while erasing goes back to the pen.
    if (current_tool == TOOL_ERASER)
        current_tool = TOOL_PEN;
    refresh_toolbar_state();
}

static void change_size(int delta)
{
    int n = current_size_index + delta;
    if (n < 0) 
        n = 0;
    if (n >= BRUSH_COUNT) 
        n = BRUSH_COUNT - 1;
    current_size_index = n;
    draw_status();
}

// Lay out the toolbar and repaint it. The canvas is left intact
// so the user's drawing survives MSG_PAINT.
static void update_children(int fd)
{
    int i;
    unsigned long toolbar_top;
    unsigned long btn_w;
    unsigned long sw_w = 40;
    unsigned long sw_gap = 6;

    gws_get_window_info(fd, main_window, &wi);

    if ((void*) dc00 == NULL)
        return;

    toolbar_top = wi.cr_height - TOOLBAR_H;
    btn_w = (wi.cr_width - (GAP * (BTN_COUNT + 1))) / BTN_COUNT;

    // The current height for the area where the
    // pointer is allowed to paint.
    current_cr_height = toolbar_top;

    // Toolbar background strip (buttons + swatches area)
    paint_fill_rect(
        dc00, 
        0, (long) toolbar_top, 
        (long) wi.cr_width, (TOOLBAR_H - FOOTER_H), 
        COLOR_GRAY );

    // ---- Row 1: buttons ----
    for (i = 0; i < BTN_COUNT; i++)
    {
        Btn[i].left   = GAP + ((btn_w + GAP) * i);
        Btn[i].top    = toolbar_top + ROW1_Y;
        Btn[i].width  = btn_w;
        Btn[i].height = ROW1_H;
        Btn[i].absolute_left = wi.left + wi.cr_left + Btn[i].left;
        Btn[i].absolute_top  = wi.top  + wi.cr_top  + Btn[i].top;

        libgui_set_ui_component_position(Uic[i], Btn[i].left, Btn[i].top);
        libgui_set_ui_component_dimension(Uic[i], Btn[i].width, Btn[i].height);
        libgui_set_ui_component_flags(Uic[i], (0x0001 | 0x0002));
    }

    // ---- Row 2: swatches ----
    for (i = 0; i < SWATCH_COUNT; i++)
    {
        Swatch[i].button_id = SWATCH_BASE + i;
        Swatch[i].left   = GAP + ((sw_w + sw_gap) * i);
        Swatch[i].top    = toolbar_top + ROW2_Y;
        Swatch[i].width  = sw_w;
        Swatch[i].height = ROW2_H;
        Swatch[i].absolute_left = wi.left + wi.cr_left + Swatch[i].left;
        Swatch[i].absolute_top  = wi.top  + wi.cr_top  + Swatch[i].top;
    }

    refresh_toolbar_state();
}

// ====================================================
// Hit test and button handlers
// ====================================================

// Hit-test for toolbar buttons and swatches.
static int __hit_test_button(unsigned long rel_mx, unsigned long rel_my)
{
    int i;

    for (i = 0; i < BTN_COUNT; i++)
    {
        if ( rel_mx >= Btn[i].left &&
             rel_mx <  Btn[i].left + Btn[i].width &&
             rel_my >= Btn[i].top  &&
             rel_my <  Btn[i].top  + Btn[i].height )
            return Btn[i].button_id;
    }

    for (i = 0; i < SWATCH_COUNT; i++)
    {
        if ( rel_mx >= Swatch[i].left &&
             rel_mx <  Swatch[i].left + Swatch[i].width &&
             rel_my >= Swatch[i].top  &&
             rel_my <  Swatch[i].top  + Swatch[i].height )
            return Swatch[i].button_id;
    }

    return -1;
}

// For visual effects (only real buttons have a pressed look)
static void on_button_pressed(int button_id)
{
    if (button_id < 1 || button_id > BTN_COUNT)
        return;
    libgui_on_button_pressed(Uic[button_id - 1], dc00);
}

// For visual effects
static void on_button_released(int button_id)
{
    if (button_id < 1 || button_id > BTN_COUNT)
        return;
    libgui_on_button_released(Uic[button_id - 1], dc00);
}

static void on_button_clicked(int id)
{
    if (id < 0)
        return;

    // Palette
    if (id >= SWATCH_BASE && id < (SWATCH_BASE + SWATCH_COUNT))
    {
        select_color(Palette[id - SWATCH_BASE]);
        return;
    }

    switch (id)
    {
        case BTN_CLEAR:
            clear_canvas();
            refresh_toolbar_state();   // restore the tool highlight
            break;
        case BTN_PEN:     select_tool(TOOL_PEN);    break;
        case BTN_ERASER:  select_tool(TOOL_ERASER); break;
        case BTN_FILL:    select_tool(TOOL_FILL);   break;
        case BTN_SIZE_DN: change_size(-1); refresh_tool_buttons(); break;
        case BTN_SIZE_UP: change_size(+1); refresh_tool_buttons(); break;
        default: break;
    };
}

// ====================================================
// The procedure
// ====================================================
static int
paintProcedure(
    int fd,
    int event_window,
    int event_type,
    unsigned long long1,
    unsigned long long2 )
{
    int id = -1;
    long cw = 0;
    long ch = 0;

    if (fd < 0) return -1;
    if (event_window < 0) return -1;
    if (event_type < 0) return -1;

    switch (event_type){

    case 0:
        return 0;

    case MSG_PAINT:
        // Only refresh the toolbar – keep the drawing
        update_children(fd);
        return 0;
        break;

    case MSG_KEYDOWN:
        switch (long1){

        case 'C': case 'c':
            clear_canvas();
            refresh_toolbar_state();
            return 0;
            break;
        case 'P': case 'p':  select_tool(TOOL_PEN);    break;
        case 'E': case 'e':  select_tool(TOOL_ERASER); break;
        case 'F': case 'f':  select_tool(TOOL_FILL);   break;

        case 'K': case 'k':  select_color(0x00000000); break;
        case 'R': case 'r':  select_color(0x00FF0000); break;
        case 'G': case 'g':  select_color(0x0000A000); break;
        case 'B': case 'b':  select_color(0x000000FF); break;

        case '[': case '-':  change_size(-1); break;
        case ']': case '+': case '=':  change_size(+1); break;

        case 'Q': case 'q':
            isTimeToQuit = TRUE;  // Quit the application
            break;
        };
        return 0;
        break;

    case MSG_SYSKEYDOWN:
        // F1 = clear, F2-F5 = colours
        switch (long1)
        {
        case VK_F1: clear_canvas(); refresh_toolbar_state(); break;
        case VK_F2: select_color(0x00000000); break;
        case VK_F3: select_color(0x00FF0000); break;
        case VK_F4: select_color(0x0000A000); break;
        case VK_F5: select_color(0x000000FF); break;
        }
        return 0;
        break;

    case MSG_MOUSEMOVE:

        // long1/long2 carry the pointer position for the toolbar hit-test
        // and for drawing.
        id = __hit_test_button(long1, long2);
        __hover_button_id = id;

        if (is_drawing == TRUE)
        {
            cw = canvas_w();
            ch = canvas_h();

            if ((long) long1 < cw && (long) long2 < ch)
            {
                if (has_prev == TRUE)
                    paint_stroke(prev_x, prev_y, (long) long1, (long) long2,
                                 brush_size(), brush_color());
                else
                    paint_stamp((long) long1, (long) long2,
                                brush_size(), brush_color());

                prev_x = (long) long1;
                prev_y = (long) long2;
                has_prev = TRUE;
            }
            else
            {
                // Left the canvas: the stroke continues when we come back,
                // but without a connecting line.
                has_prev = FALSE;
            }
        }

        return 0;
        break;

    case MSG_MOUSEPRESSED:

        // In the case of pressed, long1 and long2 carry the button
        // numbers. The pointer position is in last_x/last_y
        // (saved by the input loop from RTLEventBuffer[4]/[5]).

        __pressed_button_id = -1;
        cw = canvas_w();
        ch = canvas_h();

        if ((long) last_x < cw && (long) last_y < ch)
        {
            // Inside the canvas
            if (current_tool == TOOL_FILL)
            {
                flood_fill((long) last_x, (long) last_y, current_color);
            }
            else
            {
                is_drawing = TRUE;
                paint_stamp((long) last_x, (long) last_y,
                            brush_size(), brush_color());
                prev_x = (long) last_x;
                prev_y = (long) last_y;
                has_prev = TRUE;
            }
        }
        else
        {
            // Inside the toolbar
            id = __hit_test_button(last_x, last_y);
            if (id < 0)
                id = __hover_button_id;
            __pressed_button_id = id;
            on_button_pressed(id);
        }

        return 0;
        break;

    case MSG_MOUSERELEASED:

        // Not drawing anymore
        is_drawing = FALSE;
        has_prev = FALSE;

        if (__pressed_button_id >= 0)
        {
            on_button_released(__pressed_button_id);

            // A click only counts if we are still over the same button
            if (__pressed_button_id == __hover_button_id)
                on_button_clicked(__pressed_button_id);
            else
                refresh_tool_buttons();   // restore highlight

            __pressed_button_id = -1;
        }

        return 0;
        break;

    case MSG_CLOSE:
        isTimeToQuit = TRUE;
        return 0;
        break;

    default:
        return -1;
    }

    return -1;
}

// ----------------------------------------------------
int main(int argc, char *argv[])
{
    const char *display_name = "display:name.0";
    int client_fd = -1;
    int i;

    isTimeToQuit = FALSE;

    Display = gws_open_display(display_name);
    if ((void*)Display == NULL) {
        printf("paint: Could not open display\n");
        return EXIT_FAILURE;
    }

    client_fd = Display->fd;
    if (client_fd <= 0) {
        printf("paint: Invalid fd\n");
        return EXIT_FAILURE;
    }

    unsigned long screen_w = gws_get_system_metrics(1);
    unsigned long screen_h = gws_get_system_metrics(2);

    // Library init
    if (libgui_initialize() < 0) {
        printf("paint: libgui_initialize failed\n");
        exit(1);
    }

    // Main window
    unsigned long win_w = screen_w -20;
    unsigned long win_h = screen_h -40;
    unsigned long win_x = 10;
    unsigned long win_y = 10;

    main_window = (int) gws_create_window(
            client_fd,
            WT_OVERLAPPED,
            WINDOW_STATUS_ACTIVE,
            WINDOW_STATE_NULL,
            "Paint",
            win_x, win_y, win_w, win_h,
            0,
            WS_APP,
            COLOR_WHITE, COLOR_GRAY);

    if (main_window < 0) {
        printf("paint: Failed to create main window\n");
        return EXIT_FAILURE;
    }

    // Window info
    gws_get_window_info(client_fd, main_window, &wi);

    // wproxy update
    unsigned long m[10];
    int mytid = gettid();
    m[0] = (unsigned long)(mytid & 0xFFFFFFFF);
    m[1] = wi.left;  
    m[2] = wi.top;
    m[3] = wi.width; 
    m[4] = wi.height;
    
    m[5] = wi.cr_left;  
    m[6] = wi.cr_top;
    m[7] = wi.cr_width; 
    m[8] = wi.cr_height;
    sc80(48, &m[0], &m[0], &m[0]);

    __sh_flags = (unsigned long) wi.sh_flags;

    // Device context for the client area
    dc00 = (struct dccanvas_d *) libgui_create_dc(
            wi.ca_canvas_base_address,
            wi.ca_canvas_width,
            wi.ca_canvas_height,
            wi.ca_canvas_bpp);
    if ((void*)dc00 == NULL) {
        printf("paint: dc00 failed\n");
        exit(1);
    }

    // The current height for the area where the
    // pointer is allowed to paint.
    current_cr_height = wi.cr_height - TOOLBAR_H;

    // Initial white canvas
    paint_fill_rect(dc00, 0, 0, (long) wi.cr_width, (long) wi.cr_height, COLOR_WHITE);

    // ---- Create toolbar buttons (positions are fixed up by update_children) ----
    unsigned long btn_w = (wi.cr_width - (GAP * (BTN_COUNT + 1))) / BTN_COUNT;
    unsigned long btn_y = current_cr_height + ROW1_Y;

    for (i = 0; i < BTN_COUNT; i++)
    {
        Btn[i].button_id = i + 1;
        Btn[i].left   = GAP + ((btn_w + GAP) * i);
        Btn[i].top    = btn_y;
        Btn[i].width  = btn_w;
        Btn[i].height = ROW1_H;

        Uic[i] = libgui_create_ui_component(
                dc00,
                UI_COMPONENT_BUTTON,
                Btn[i].left, Btn[i].top,
                Btn[i].width, Btn[i].height,
                BtnLabel[i], (0x0001 | 0x0002));
    }

    // First toolbar paint (also paints swatches and the status line)
    update_children(client_fd);

    gws_set_active(client_fd, main_window);

    // -------------------- Event loop --------------------
    // Same loop as before: one cheap check of the shared flags and one
    // event poll per iteration. The procedure does all the work.
    while (1)
    {
        if (isTimeToQuit == TRUE)
            break;

        // Shared-flags blit handling (identical to Power / Launch)
        if (__sh_flags != 0)
        {
            char *flags_ptr = (char *) __sh_flags;
            if (*flags_ptr & 0x0008)
            {
                *flags_ptr &= ~0x0008;
                update_children(client_fd);
            }
        }

        // Input-broker events
        if (rtl_get_event() == TRUE)
        {
            last_x = (unsigned long) RTLEventBuffer[4];
            last_y = (unsigned long) RTLEventBuffer[5];

            paintProcedure(
                client_fd,
                (int) RTLEventBuffer[0],
                (int) RTLEventBuffer[1],
                (unsigned long) RTLEventBuffer[2],
                (unsigned long) RTLEventBuffer[3] );

            RTLEventBuffer[1] = 0;
        }
    }

    if (isTimeToQuit == TRUE){
        printf("Paint: Close window\n");
        gws_destroy_window(client_fd, main_window);
    }

    if (client_fd > 0)
        close(client_fd);

    return EXIT_SUCCESS;
}

