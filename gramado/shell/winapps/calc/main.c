// main.c
// Gramado OS client-side GUI app: Calculator
//
// In the spirit of the simple desktop applications of the Windows 1.x / 2.x
// era: one small window, a display, and a grid of buttons.
//
//   MC  MR  M+  M-   C
//    7   8   9   /  CE
//    4   5   6   *   %
//    1   2   3   -  sqrt
//   +/-  0   .   +  1/x
//   [          =         ]
//
// Design notes:
//  - Integer fixed-point math (4 decimals). No floating point,
//    so it does not depend on the FPU/SSE state of the system.
//    Range: -99,999,999.9999 to +99,999,999.9999. Out of range = "Error".
//  - Same routines as the Power / Launch / Sysinfo examples:
//      on_button_pressed / on_button_released / on_button_clicked
//      hover tracking on MSG_MOUSEMOVE (blue frame)
//      press effect on MSG_MOUSEPRESSED, action on MSG_MOUSERELEASED
//  - Same event loop: shared-flags blit check, then rtl_get_event()
//    handing the event to the procedure. No display-server pump.
//  - The display is redrawn alone when a key is pressed. The whole
//    window is only repainted on MSG_PAINT / blit flag.
//
// Keyboard:
//   0-9 .        digits and decimal point (',' works too)
//   + - * /      operators
//   = Enter      equals (repeating = repeats the last operation)
//   C            clear all          E   clear entry (CE)
//   Backspace    delete last digit
//   N            change sign        %   percent
//   @            square root        R   reciprocal (1/x)
//   L M P O      memory: clear, recall, add, subtract
//   Q            quit
//
// #ps: Unlike the Power example, the keyboard action happens on
// key DOWN (the pressed look is released on key UP). A calculator must not
// lose a digit when the user types fast and two keys overlap.

// rtl
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <rtl/gramado.h>

// The client-side library
#include <gws.h>

// #test
// The client-side library
#include <libgui.h>

// ----------------------------------------------------
// Config
// ----------------------------------------------------

// Set to 1 to get messages on the console.
#define CALC_DEBUG  0
#if CALC_DEBUG
#define CALC_LOG(...)  printf(__VA_ARGS__)
#else
#define CALC_LOG(...)  ((void)0)
#endif

// Look
#define CALC_BG_COLOR        LIBGUI_COLOR_PANEL_FACE
#define DISPLAY_BG_COLOR     0x00FFFFFF
#define DISPLAY_BORDER_COLOR 0x00404040
#define DISPLAY_TEXT_COLOR   0x00101010
#define FRAME_THICKNESS      2
#define FRAME_HOVER_COLOR    0x000060C0
#define FRAME_NONE_COLOR     CALC_BG_COLOR

#define MARGIN       8
#define GAP          4      // between buttons. It must hold the hover frames.
#define DISPLAY_H    40
#define GRID_COLS    5
#define GRID_ROWS    6

// Fixed-point: 4 decimals.
#define SCALE        10000ULL
#define VAL_LIMIT    999999999999LL     //  99,999,999.9999
#define INT_LIMIT    99999999ULL
#define MAX_INT_DIGITS   8
#define MAX_FRAC_DIGITS  4

static int isTimeToQuit = FALSE;

// Global display pointer
struct gws_display_d *Display;

struct dccanvas_d *dc00;  // shared dc

static int client_fd   = -1;
static int main_window = -1;

static unsigned long __sh_flags = 0;

// ----------------------------------------------------
// The buttons
// ----------------------------------------------------
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

// What each button does. Digits are 0..9.
enum {
    A_D0 = 0, A_D1, A_D2, A_D3, A_D4, A_D5, A_D6, A_D7, A_D8, A_D9,
    A_DOT,
    A_ADD, A_SUB, A_MUL, A_DIV,
    A_EQ,
    A_C, A_CE,
    A_NEG, A_PCT, A_SQRT, A_INV,
    A_MC, A_MR, A_MPLUS, A_MMINUS
};

struct calc_item_d
{
    struct button_info_d info;
    struct ui_component_d *uic;
    const char *label;
    char key;              // keyboard shortcut (lowercase). 0 = none
    int action;
    int col;
    int row;
    int span;              // how many columns the button uses
};

// button_id = index + 1
static struct calc_item_d  ItemList[] = {
    // Row 0
    { {0}, NULL, "MC",   'l', A_MC,     0, 0, 1 },
    { {0}, NULL, "MR",   'm', A_MR,     1, 0, 1 },
    { {0}, NULL, "M+",   'p', A_MPLUS,  2, 0, 1 },
    { {0}, NULL, "M-",   'o', A_MMINUS, 3, 0, 1 },
    { {0}, NULL, "C",    'c', A_C,      4, 0, 1 },
    // Row 1
    { {0}, NULL, "7",    '7', A_D7,     0, 1, 1 },
    { {0}, NULL, "8",    '8', A_D8,     1, 1, 1 },
    { {0}, NULL, "9",    '9', A_D9,     2, 1, 1 },
    { {0}, NULL, "/",    '/', A_DIV,    3, 1, 1 },
    { {0}, NULL, "CE",   'e', A_CE,     4, 1, 1 },
    // Row 2
    { {0}, NULL, "4",    '4', A_D4,     0, 2, 1 },
    { {0}, NULL, "5",    '5', A_D5,     1, 2, 1 },
    { {0}, NULL, "6",    '6', A_D6,     2, 2, 1 },
    { {0}, NULL, "*",    '*', A_MUL,    3, 2, 1 },
    { {0}, NULL, "%",    '%', A_PCT,    4, 2, 1 },
    // Row 3
    { {0}, NULL, "1",    '1', A_D1,     0, 3, 1 },
    { {0}, NULL, "2",    '2', A_D2,     1, 3, 1 },
    { {0}, NULL, "3",    '3', A_D3,     2, 3, 1 },
    { {0}, NULL, "-",    '-', A_SUB,    3, 3, 1 },
    { {0}, NULL, "sqrt", '@', A_SQRT,   4, 3, 1 },
    // Row 4
    { {0}, NULL, "+/-",  'n', A_NEG,    0, 4, 1 },
    { {0}, NULL, "0",    '0', A_D0,     1, 4, 1 },
    { {0}, NULL, ".",    '.', A_DOT,    2, 4, 1 },
    { {0}, NULL, "+",    '+', A_ADD,    3, 4, 1 },
    { {0}, NULL, "1/x",  'r', A_INV,    4, 4, 1 },
    // Row 5
    { {0}, NULL, "=",    '=', A_EQ,     0, 5, 5 }
};
#define ITEM_COUNT  (int)(sizeof(ItemList)/sizeof(ItemList[0]))

// ----------------------------------------------------
// Interface state
// ----------------------------------------------------
static int __hover_button_id  = -1;   // Invalidate.
static int __mouse_pressed_id = -1;   // Pressed with the mouse
static unsigned char __key_down[ITEM_COUNT + 1];   // Pressed with the keyboard

// Position of the pointer at the time of the event.
// Filled by the input loop from the event buffer.
static unsigned long last_x = 0;
static unsigned long last_y = 0;

// The display rectangle (relative to the client area)
static unsigned long disp_x = MARGIN;
static unsigned long disp_y = MARGIN;
static unsigned long disp_w = 0;
static unsigned long disp_h = DISPLAY_H;

// ----------------------------------------------------
// Calculator state
// ----------------------------------------------------
static long long acc = 0;          // left operand
static long long cur = 0;          // the number on the display, when not typing
static long long mem = 0;          // memory
static char pending = 0;           // pending operator: + - * /  (0 = none)
static char last_op = 0;           // for repeating '='
static long long last_operand = 0;
static int typing = FALSE;         // the user is typing a number
static int operand_ready = TRUE;   // there is an operand for the pending op
static int calc_error = FALSE;

static char entry[24];             // the number being typed
static int  entry_len = 0;

// -----------------
static void on_button_pressed(int button_id);
static void on_button_released(int button_id);
static void on_button_clicked(int button_id);

static int  __hit_test_button(unsigned long rel_mx, unsigned long rel_my);
static void layout_items(struct gws_window_info_d *wi);
static void draw_display(void);
static void update_children(int fd);

// =====================================================
// Drawing helper
// =====================================================

// Fill a rectangle, clipped against the dc. ROP_COPY only.
static void 
calc_fill_rect(
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

    if ((void*) dc00 == NULL || (void*) dc00->data == NULL)
        return;
    if (w <= 0 || h <= 0)
        return;

    dw = (long) dc00->device_width;
    dh = (long) dc00->device_height;

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
        row = (unsigned char *) dc00->data + ((unsigned long) yy * dc00->pitch);

        if (dc00->bpp == 32)
        {
            unsigned int *p = (unsigned int *) (row + ((unsigned long) x << 2));
            for (xx = x; xx < x2; xx++)
                *p++ = c;
        }
        else if (dc00->bpp == 24)
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

// =====================================================
// Fixed-point math (4 decimals, 64-bit, no floating point)
// =====================================================

static unsigned long long u_abs(long long v)
{
    return (v < 0) ? (unsigned long long)(-v) : (unsigned long long) v;
}

// (a * b) / SCALE, rounded. Sets *err on overflow.
static long long mul_scaled(long long a, long long b, int *err)
{
    unsigned long long ua = u_abs(a);
    unsigned long long ub = u_abs(b);
    unsigned long long ah = ua / SCALE;
    unsigned long long al = ua % SCALE;
    unsigned long long bh = ub / SCALE;
    unsigned long long bl = ub % SCALE;
    unsigned long long hh, t;
    int neg = ((a < 0) != (b < 0));

    // ah*bh must stay inside the integer range
    if (ah != 0 && bh > (INT_LIMIT / ah)){
        *err = 1;
        return 0;
    }
    hh = ah * bh;

    // (ah + al/S) * (bh + bl/S) * S
    t = (hh * SCALE) + (ah * bl) + (al * bh) + (((al * bl) + (SCALE / 2)) / SCALE);

    if (t > (unsigned long long) VAL_LIMIT){
        *err = 1;
        return 0;
    }
    return neg ? -(long long) t : (long long) t;
}

// (a * SCALE) / b, rounded. Sets *err on overflow or division by zero.
static long long div_scaled(long long a, long long b, int *err)
{
    unsigned long long ua = u_abs(a);
    unsigned long long ub = u_abs(b);
    unsigned long long q;
    int neg = ((a < 0) != (b < 0));

    if (ub == 0){
        *err = 1;
        return 0;
    }

    // ua <= ~1e12, so ua*SCALE <= ~1e16. It fits in 64 bits.
    q = ((ua * SCALE) + (ub / 2)) / ub;

    if (q > (unsigned long long) VAL_LIMIT){
        *err = 1;
        return 0;
    }
    return neg ? -(long long) q : (long long) q;
}

// Integer square root (floor).
static unsigned long long isqrt_u64(unsigned long long n)
{
    unsigned long long r = 0;
    unsigned long long bit = 1ULL << 62;

    while (bit > n)
        bit >>= 2;

    while (bit != 0)
    {
        if (n >= (r + bit)){
            n -= (r + bit);
            r = (r >> 1) + bit;
        } else {
            r >>= 1;
        }
        bit >>= 2;
    }
    return r;
}

// value / 100, rounded.
static long long div100(long long v)
{
    return (v + ((v >= 0) ? 50 : -50)) / 100;
}

static long long apply_op(long long a, char op, long long b, int *err)
{
    long long r = 0;

    switch (op){
    case '+': r = a + b; break;
    case '-': r = a - b; break;
    case '*': return mul_scaled(a, b, err);
    case '/': return div_scaled(a, b, err);
    default:  return b;
    };

    if (r > VAL_LIMIT || r < -VAL_LIMIT)
        *err = 1;
    return r;
}

// =====================================================
// Numbers <-> text
// =====================================================

// Value to text. Up to 4 decimals, trailing zeros removed.
// out needs 20 bytes.
static void format_value(long long v, char *out)
{
    static const unsigned long long p10[4] = { 1000, 100, 10, 1 };
    char tmp[24];
    int n = 0;
    int i = 0;
    int k;
    unsigned long long u = u_abs(v);
    unsigned long long ip = u / SCALE;
    unsigned long long fp = u % SCALE;

    if (v < 0)
        out[i++] = '-';

    if (ip == 0)
        tmp[n++] = '0';
    while (ip > 0){
        tmp[n++] = (char)('0' + (ip % 10));
        ip /= 10;
    }
    while (n > 0)
        out[i++] = tmp[--n];

    if (fp != 0)
    {
        out[i++] = '.';
        for (k = 0; k < 4; k++)
            out[i++] = (char)('0' + ((fp / p10[k]) % 10));

        // Trim trailing zeros
        while (out[i - 1] == '0')
            i--;
    }
    out[i] = '\0';
}

// The text the user is typing, to a value.
static long long parse_entry(void)
{
    int i = 0;
    int neg = 0;
    int dot = 0;
    int fd = 0;
    unsigned long long ip = 0;
    unsigned long long fp = 0;
    unsigned long long v;
    char c;

    if (entry[0] == '-'){
        neg = 1;
        i = 1;
    }

    for ( ; entry[i] != '\0'; i++)
    {
        c = entry[i];
        if (c == '.'){
            dot = 1;
            continue;
        }
        if (dot == 0){
            ip = (ip * 10) + (unsigned long long)(c - '0');
        } else if (fd < MAX_FRAC_DIGITS){
            fp = (fp * 10) + (unsigned long long)(c - '0');
            fd++;
        }
    }
    while (fd < MAX_FRAC_DIGITS){
        fp *= 10;
        fd++;
    }

    v = (ip * SCALE) + fp;
    return neg ? -(long long) v : (long long) v;
}

// =====================================================
// The calculator
// =====================================================

static long long current_value(void)
{
    return (typing == TRUE) ? parse_entry() : cur;
}

static void set_error(void)
{
    calc_error = TRUE;
    typing = FALSE;
    pending = 0;
    last_op = 0;
    operand_ready = TRUE;
}

static void calc_reset(void)
{
    acc = 0;
    cur = 0;
    pending = 0;
    last_op = 0;
    last_operand = 0;
    typing = FALSE;
    operand_ready = TRUE;
    calc_error = FALSE;
    entry[0] = '\0';
    entry_len = 0;
}

// A result: it becomes the number on the display.
static void set_result(long long v)
{
    cur = v;
    typing = FALSE;
    operand_ready = TRUE;
}

static void start_entry(void)
{
    entry[0] = '0';
    entry[1] = '\0';
    entry_len = 1;
    typing = TRUE;
    operand_ready = TRUE;
}

// Count the digits of the text being typed.
static void entry_counts(int *int_digits, int *frac_digits, int *has_dot)
{
    int i;
    int d = 0;
    int f = 0;
    int dot = 0;

    for (i = 0; entry[i] != '\0'; i++)
    {
        if (entry[i] == '-')
            continue;
        if (entry[i] == '.'){
            dot = 1;
            continue;
        }
        if (dot == 0) d++; else f++;
    }
    *int_digits = d;
    *frac_digits = f;
    *has_dot = dot;
}

static void input_digit(int d)
{
    int id, fd, dot;

    if (typing != TRUE)
        start_entry();

    // "0" or "-0": the digit replaces the zero
    if (strcmp(entry, "0") == 0 || strcmp(entry, "-0") == 0)
    {
        if (d == 0)
            return;
        entry[entry_len - 1] = (char)('0' + d);
        return;
    }

    entry_counts(&id, &fd, &dot);
    if (dot == 0 && id >= MAX_INT_DIGITS)
        return;
    if (dot != 0 && fd >= MAX_FRAC_DIGITS)
        return;
    if (entry_len >= (int) sizeof(entry) - 2)
        return;

    entry[entry_len++] = (char)('0' + d);
    entry[entry_len] = '\0';
}

static void input_dot(void)
{
    int id, fd, dot;

    if (typing != TRUE)
        start_entry();

    entry_counts(&id, &fd, &dot);
    if (dot != 0)
        return;
    if (entry_len >= (int) sizeof(entry) - 2)
        return;

    entry[entry_len++] = '.';
    entry[entry_len] = '\0';
}

static void input_backspace(void)
{
    if (calc_error == TRUE || typing != TRUE)
        return;

    entry_len--;
    entry[entry_len] = '\0';

    if (entry_len <= 0 || strcmp(entry, "-") == 0)
        start_entry();
}

static void do_negate(void)
{
    int i;

    if (typing == TRUE)
    {
        if (entry[0] == '-'){
            for (i = 0; i < entry_len; i++)
                entry[i] = entry[i + 1];
            entry_len--;
        } else if (strcmp(entry, "0") != 0){
            for (i = entry_len; i >= 0; i--)
                entry[i + 1] = entry[i];
            entry[0] = '-';
            entry_len++;
        }
        return;
    }

    set_result(-cur);
}

static void do_operator(char op)
{
    long long x;
    int e = 0;

    x = current_value();

    if (pending != 0 && operand_ready == TRUE)
    {
        // Chain: 2 + 3 + ...  ->  5 +
        acc = apply_op(acc, pending, x, &e);
        if (e){
            set_error();
            return;
        }
    }
    else if (pending == 0)
    {
        acc = x;
    }
    // else: an operator after an operator. Just replace it.

    pending = op;
    last_op = 0;
    typing = FALSE;
    cur = acc;
    operand_ready = FALSE;
}

static void do_equals(void)
{
    long long x;
    long long r;
    int e = 0;

    x = current_value();

    if (pending != 0)
    {
        // "5 + =" uses the accumulator as the second operand
        if (operand_ready != TRUE)
            x = acc;

        r = apply_op(acc, pending, x, &e);
        if (e){
            set_error();
            return;
        }
        last_op = pending;
        last_operand = x;
        pending = 0;
        acc = r;
    }
    else if (last_op != 0)
    {
        // Repeat the last operation: 5 + 3 = = ...
        r = apply_op(x, last_op, last_operand, &e);
        if (e){
            set_error();
            return;
        }
        acc = r;
    }
    else
    {
        acc = x;
    }

    set_result(acc);
}

static void do_percent(void)
{
    long long x = current_value();
    long long r;
    int e = 0;

    if (pending == '+' || pending == '-')
    {
        // 200 + 10 %  ->  10% of 200
        r = mul_scaled(acc, x, &e);
        if (e){
            set_error();
            return;
        }
        r = div100(r);
    }
    else
    {
        r = div100(x);
    }
    set_result(r);
}

static void do_sqrt(void)
{
    long long x = current_value();

    if (x < 0){
        set_error();
        return;
    }
    // sqrt(x/S) * S = sqrt(x*S)
    set_result( (long long) isqrt_u64( ((unsigned long long) x) * SCALE ) );
}

static void do_inverse(void)
{
    long long x = current_value();
    int e = 0;
    long long r;

    r = div_scaled((long long) SCALE, x, &e);
    if (e){
        set_error();
        return;
    }
    set_result(r);
}

static void do_memory(int action)
{
    long long x;
    long long r;

    if (action == A_MC){
        mem = 0;
        return;
    }
    if (action == A_MR){
        set_result(mem);
        return;
    }

    x = current_value();
    r = (action == A_MPLUS) ? (mem + x) : (mem - x);
    if (r > VAL_LIMIT || r < -VAL_LIMIT){
        set_error();
        return;
    }
    mem = r;

    // The typed number is now committed
    set_result(x);
}

// Execute what a button does and update the display.
static void do_action(int action)
{
    // After an error only C and CE work.
    if (calc_error == TRUE && action != A_C && action != A_CE)
        return;

    if (action >= A_D0 && action <= A_D9){
        input_digit(action);
    } else {
        switch (action){
        case A_DOT:    input_dot();       break;
        case A_ADD:    do_operator('+');  break;
        case A_SUB:    do_operator('-');  break;
        case A_MUL:    do_operator('*');  break;
        case A_DIV:    do_operator('/');  break;
        case A_EQ:     do_equals();       break;
        case A_C:      calc_reset();      break;
        case A_CE:
            // Clear only the number on the display
            calc_error = FALSE;
            set_result(0);
            break;
        case A_NEG:    do_negate();       break;
        case A_PCT:    do_percent();      break;
        case A_SQRT:   do_sqrt();         break;
        case A_INV:    do_inverse();      break;
        case A_MC:
        case A_MR:
        case A_MPLUS:
        case A_MMINUS: do_memory(action); break;
        default: break;
        };
    }

    draw_display();
}

// =====================================================
// The display
// =====================================================
static void draw_display(void)
{
    char text[32];
    char ind[4];
    unsigned long len;
    unsigned long scale = 2;
    unsigned long tw;
    unsigned long tx;
    unsigned long ty;

    if ((void*) dc00 == NULL)
        return;
    if (disp_w < 16 || disp_h < 16)
        return;

// The box
    calc_fill_rect(disp_x, disp_y, disp_w, disp_h, DISPLAY_BORDER_COLOR);
    calc_fill_rect(disp_x + 1, disp_y + 1, disp_w - 2, disp_h - 2, DISPLAY_BG_COLOR);

// The text
    if (calc_error == TRUE)
        strcpy(text, "Error");
    else if (typing == TRUE)
        strcpy(text, entry);
    else
        format_value(cur, text);

    len = (unsigned long) strlen(text);
    if ((len * 8 * scale) + 12 > disp_w)
        scale = 1;

    tw = len * 8 * scale;
    tx = (disp_w > (tw + 6)) ? (disp_x + disp_w - tw - 6) : (disp_x + 2);
    ty = disp_y + ((disp_h - (8 * scale)) / 2);

    libgui_drawstringblock_dc(
        dc00, tx, ty, DISPLAY_TEXT_COLOR, text, scale );

// Indicators: memory and pending operator
    ind[0] = (mem != 0) ? 'M' : ' ';
    ind[1] = (pending != 0 && calc_error != TRUE) ? pending : ' ';
    ind[2] = '\0';

    libgui_drawstring_dc(
        dc00, disp_x + 4, disp_y + 4, 
        DISPLAY_TEXT_COLOR, DISPLAY_BG_COLOR, 0, ind );
}

// =====================================================
// Frames: hover (blue)
// =====================================================

// Redraw the frame around one button, given its id.
static void draw_item_frame(int button_id)
{
    struct calc_item_d *it;
    unsigned int color = FRAME_NONE_COLOR;
    long x, y, w, h, t = FRAME_THICKNESS;

    if (button_id < 1 || button_id > ITEM_COUNT)
        return;
    if ((void*) dc00 == NULL)
        return;

    it = &ItemList[button_id - 1];

    if (button_id == __hover_button_id)
        color = FRAME_HOVER_COLOR;

    x = (long) it->info.left   - t;
    y = (long) it->info.top    - t;
    w = (long) it->info.width  + (t * 2);
    h = (long) it->info.height + (t * 2);

    calc_fill_rect(x,         y,         w, t,         color);  // top
    calc_fill_rect(x,         y + h - t, w, t,         color);  // bottom
    calc_fill_rect(x,         y + t,     t, h - (t*2), color);  // left
    calc_fill_rect(x + w - t, y + t,     t, h - (t*2), color);  // right
}

static void set_hover(int button_id)
{
    int old = __hover_button_id;

    if (button_id == old)
        return;

    __hover_button_id = button_id;
    draw_item_frame(old);
    draw_item_frame(button_id);
}

// =====================================================
// Button handlers (same roles as in the Power example)
// =====================================================

// For visual effects
// #todo: We need to implement this kind of worker inside the library
static void on_button_pressed(int button_id)
{
    if (button_id < 1 || button_id > ITEM_COUNT)
        return;

    libgui_on_button_pressed(ItemList[button_id - 1].uic, dc00);
}

// For visual effects
// #todo: We need to implement this kind of worker inside the library
static void on_button_released(int button_id)
{
    if (button_id < 1 || button_id > ITEM_COUNT)
        return;

    libgui_on_button_released(ItemList[button_id - 1].uic, dc00);
}

// Handle click events for components
static void on_button_clicked(int button_id)
{
    if (button_id < 1 || button_id > ITEM_COUNT)
        return;

    CALC_LOG("calc: %s\n", ItemList[button_id - 1].label);
    do_action(ItemList[button_id - 1].action);
}

// Hit-test for the buttons
static int __hit_test_button(unsigned long rel_mx, unsigned long rel_my) 
{
    int i;
    struct button_info_d *b;

    for (i = 0; i < ITEM_COUNT; i++)
    {
        b = &ItemList[i].info;

        if ( rel_mx >= b->left && 
             rel_mx <  b->left + b->width &&
             rel_my >= b->top  && 
             rel_my <  b->top  + b->height )
        {
            return (int) b->button_id;
        }
    }

    return -1;
}

// Which button does this key activate? (-1 = none)
static int __key_to_button_id(unsigned long key)
{
    int i;
    char c = (char) key;

    if (key == VK_RETURN)
        c = '=';
    if (c == ',')
        c = '.';
    if (c >= 'A' && c <= 'Z')
        c = (char)(c + 32);
    if (c == 0)
        return -1;

    for (i = 0; i < ITEM_COUNT; i++)
    {
        if (ItemList[i].key == c)
            return ItemList[i].info.button_id;
    }

    return -1;
}

// =====================================================
// Layout and painting
// =====================================================

// Compute the position and dimension of the display and of every
// button from the client area. The grid fills what is left.
static void layout_items(struct gws_window_info_d *wi)
{
    int i;
    long cr_w = (long) wi->cr_width;
    long cr_h = (long) wi->cr_height;
    long grid_top;
    long avail_h;
    long cell_w;
    long cell_h;

    disp_x = MARGIN;
    disp_y = MARGIN;
    disp_w = (cr_w > (MARGIN * 2)) ? (unsigned long)(cr_w - (MARGIN * 2)) : 0;
    disp_h = DISPLAY_H;

    grid_top = (long) (disp_y + disp_h + MARGIN);
    avail_h  = cr_h - grid_top - MARGIN;

    cell_w = (cr_w - (MARGIN * 2) - (GAP * (GRID_COLS - 1))) / GRID_COLS;
    cell_h = (avail_h - (GAP * (GRID_ROWS - 1))) / GRID_ROWS;
    if (cell_w < 1) cell_w = 1;
    if (cell_h < 1) cell_h = 1;

    for (i = 0; i < ITEM_COUNT; i++)
    {
        struct calc_item_d *it = &ItemList[i];

        it->info.button_id = i + 1;

        // Relative values
        it->info.left   = (unsigned long)(MARGIN + (it->col * (cell_w + GAP)));
        it->info.top    = (unsigned long)(grid_top + (it->row * (cell_h + GAP)));
        it->info.width  = (unsigned long)((it->span * cell_w) + ((it->span - 1) * GAP));
        it->info.height = (unsigned long) cell_h;

        // Absolute values
        it->info.absolute_left = wi->left + wi->cr_left + it->info.left;
        it->info.absolute_top  = wi->top  + wi->cr_top  + it->info.top;
    }
}

static void update_children(int fd)
{
    struct gws_window_info_d  wi;
    int i;

    // Get window info
    gws_get_window_info(fd, main_window, &wi);

    if ((void*)dc00 == NULL)
        return;

    layout_items(&wi);

// The background for the client area
    calc_fill_rect(0, 0, wi.cr_width, wi.cr_height, CALC_BG_COLOR);

    draw_display();

// Buttons: position, dimension and redraw.
// The component keeps its status, so a pressed button stays pressed.
    for (i = 0; i < ITEM_COUNT; i++)
    {
        libgui_set_ui_component_position(
            ItemList[i].uic, 
            ItemList[i].info.left, 
            ItemList[i].info.top );
        libgui_set_ui_component_dimension(
            ItemList[i].uic,
            ItemList[i].info.width,
            ItemList[i].info.height );
        libgui_set_ui_component_flags(ItemList[i].uic, (0x0001 | 0x0002) );
        libgui_redraw_ui_component(ItemList[i].uic, dc00);
    }

// The hover frame goes on top
    draw_item_frame(__hover_button_id);
}

// ----------------------------------------------------
// Procedure: handles the events
// ----------------------------------------------------
static int 
calcProcedure(
    int fd, 
    int event_window, 
    int event_type, 
    unsigned long long1, 
    unsigned long long2 )
{
    int ButtonId = -1;
    int i;

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

    // Keyboard: the pressed look and the action happen on key down.
    // The look is released on key up.
    case MSG_KEYDOWN:

        // Quit the calculator
        if (long1 == 'Q' || long1 == 'q')
        {
            isTimeToQuit = TRUE;
            return 0;
        }

        // Backspace (8) and Delete (127) remove the last digit.
        if (long1 == 8 || long1 == 127)
        {
            input_backspace();
            draw_display();
            return 0;
        }

        ButtonId = __key_to_button_id(long1);
        // Ignore the auto-repeat: only the first key down counts.
        if (ButtonId > 0 && __key_down[ButtonId] == 0)
        {
            __key_down[ButtonId] = 1;
            on_button_pressed(ButtonId);   // Visual effect
            on_button_clicked(ButtonId);   // Action
        }
        return 0;
        break;

    case MSG_KEYUP:
        // The key up can report a different character than the key down
        // (for example when Shift is released first), so every button
        // that is held by the keyboard gets released.
        for (i = 1; i <= ITEM_COUNT; i++)
        {
            if (__key_down[i] != 0)
            {
                __key_down[i] = 0;
                on_button_released(i);   // Visual effect
            }
        }
        return 0;
        break;

    case MSG_SYSKEYDOWN:
        return 0;
        break;

    case GWS_MouseClicked:
        break;

    case MSG_MOUSEMOVE:
        // long1/long2: pointer position, relative to the client area.
        ButtonId = (int) __hit_test_button(long1, long2);
        set_hover(ButtonId);   // Frame effect
        return 0;
        break;

    case MSG_MOUSEPRESSED:
        // The pointer position is in last_x/last_y
        // (saved by the input loop from RTLEventBuffer[4]/[5]).
        ButtonId = (int) __hit_test_button(last_x, last_y);
        if (ButtonId < 0)
            ButtonId = __hover_button_id;

        __mouse_pressed_id = ButtonId;
        if (ButtonId > 0)
            on_button_pressed(ButtonId);  // Visual effect
        return 0;
        break;

    case MSG_MOUSERELEASED:
        if (__mouse_pressed_id > 0)
        {
            on_button_released(__mouse_pressed_id);  // Visual effect

            // A click only counts if the pointer is still
            // over the button that was pressed.
            ButtonId = (int) __hit_test_button(last_x, last_y);
            if (ButtonId < 0)
                ButtonId = __hover_button_id;

            if (ButtonId == __mouse_pressed_id)
                on_button_clicked(ButtonId);  // Action

            __mouse_pressed_id = -1;
        }
        return 0;
        break;

    case MSG_CLOSE:
        isTimeToQuit = TRUE;
        return 0;
        break;

    // Unknown event
    default:
        return -1;
        break;
    };

// Fail
    return (int) -1;
}

int main(int argc, char *argv[])
{
    const char *display_name = "display:name.0";
    int i;

    isTimeToQuit = FALSE;

    // Connect to display server
    Display = gws_open_display(display_name);
    if ((void*) Display == NULL) {
        printf("calc: Could not open display\n");
        return EXIT_FAILURE;
    }

    client_fd = Display->fd;
    if (client_fd <= 0) {
        printf("calc: Invalid fd\n");
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
        printf("calc: on libgui_initialize()\n");
        exit(1);
    }

// =========================================
// Main window
// A small fixed-size window, centered.

    unsigned long mw_style = WS_APP;

    unsigned long win_w = 330;
    unsigned long win_h = 410;
    if (win_w > screen_w) win_w = screen_w;
    if (win_h > screen_h) win_h = screen_h;

    unsigned long win_x = (screen_w - win_w) / 2;
    unsigned long win_y = (screen_h - win_h) / 2;

    main_window = 
        (int) gws_create_window(
                client_fd,
                WT_OVERLAPPED,
                WINDOW_STATUS_ACTIVE,  //status
                WINDOW_STATE_NULL,  //state
                "Calculator",
                win_x, win_y, win_w, win_h,
                0,
                mw_style,  // style
                COLOR_WHITE, COLOR_GRAY );

    if (main_window < 0) {
        printf("calc: Failed to create main window\n");
        return EXIT_FAILURE;
    }

// After creating main_window,
// get information about it.
    struct gws_window_info_d wi;
    gws_get_window_info(
        client_fd,
        main_window,
        (struct gws_window_info_d *) &wi );

// ============================================================
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
// Getting the flag earlier. This way we can use it in the loop.

    __sh_flags = (unsigned long) wi.sh_flags;

// ============================================================
// Create a device context structure
// based on the information we got with the server.
// This is a dc to draw into the client area.

    dc00 = (struct dccanvas_d *) libgui_create_dc(
        wi.ca_canvas_base_address,
        wi.ca_canvas_width,
        wi.ca_canvas_height,
        wi.ca_canvas_bpp
    );
    if ((void*)dc00 == NULL){
        printf("calc: on dc00\n");
        exit(1);
    }

// The calculator starts clean
    calc_reset();
    mem = 0;
    memset(__key_down, 0, sizeof(__key_down));

// bg for the client area
    calc_fill_rect(0, 0, wi.cr_width, wi.cr_height, CALC_BG_COLOR);

// ============================================================
// Create the buttons.
// The layout gives the position and dimension of each one.

    layout_items(&wi);

    for (i = 0; i < ITEM_COUNT; i++)
    {
        ItemList[i].uic = libgui_create_ui_component (
            dc00, 
            UI_COMPONENT_BUTTON,   // type 1 = button 
            ItemList[i].info.left, 
            ItemList[i].info.top, 
            ItemList[i].info.width, 
            ItemList[i].info.height,
            ItemList[i].label,
            (0x0001 | 0x0002)
        );
        if ((void*) ItemList[i].uic == NULL){
            printf("calc: on button %d\n", i);
            exit(1);
        }
    }

    draw_display();

// Main window: Activate and show.
    gws_set_active( client_fd, main_window );

//
// Event loop
//

// #ps:
// The purpose of this loop is only get system events,
// not to get events from the display server.
// We do this for performance reasons.

    while (1){

        if (isTimeToQuit == TRUE)
            break;

        // Shared flags handling (same as Power app)
        if (__sh_flags != 0)
        {
            char *flags_ptr = (char *) __sh_flags;
            if (*flags_ptr & 0x0008)
            {
                // Clear BLIT bit
                *flags_ptr &= ~0x0008;
                // Redraw
                update_children(client_fd);
            }
        }

        // Pump events from Input Broker (system events)
        if (rtl_get_event() == TRUE)
        {
            last_x = (unsigned long) RTLEventBuffer[4];
            last_y = (unsigned long) RTLEventBuffer[5];

            // IN: wid, event type, VK, scancode.
            calcProcedure(
                client_fd,
                (int) RTLEventBuffer[0],
                (int) RTLEventBuffer[1],
                (unsigned long) RTLEventBuffer[2],
                (unsigned long) RTLEventBuffer[3] );
            RTLEventBuffer[1] = 0;  // Clear after dispatch
        }
    };

    if (isTimeToQuit == TRUE){
        printf("Calc: Close window\n");
        gws_destroy_window(client_fd, main_window);
    }

    if (client_fd > 0)
        close(client_fd);

    return EXIT_SUCCESS;
}

